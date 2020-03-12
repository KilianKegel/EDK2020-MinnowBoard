#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#undef NULL
#include <uefi.h>

extern char* strefierror(EFI_STATUS errcode);                           // Torito C extention according to strerror()

#define BSIZE 4096

typedef struct _NVRAMCOMMANDLINE {
    int rejectStart;            //  1 -> suppress start of driver, even if registered with EFI_CALLER_ID_GUID. 0 -> start driver and pass command line to it
    char CommandLine[0];		/*  assigned command line includeing filename*/
}NVRAMCOMMANDLINE;

int main(int argc, char** argv) {

    size_t len = argc > 2 ? 1 + strlen(argv[2]) : 0;
    EFI_SYSTEM_TABLE* SystemTable = (EFI_SYSTEM_TABLE*)(argv[-1]);      //SystemTable is passed in argv[-1]
//    EFI_HANDLE ImageHandle = (void*)(argv[-2]);                         //ImageHandle is passed in argv[-2]
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_GUID guid;
    UINT32 Attributes = EFI_VARIABLE_NON_VOLATILE + EFI_VARIABLE_BOOTSERVICE_ACCESS + EFI_VARIABLE_RUNTIME_ACCESS, * pAttributes = &Attributes;
    UINTN datasize = BSIZE;
    NVRAMCOMMANDLINE* pNvram = malloc(BSIZE);
    NVRAMCOMMANDLINE* pBuf = malloc(sizeof(NVRAMCOMMANDLINE) + len);
    int n, i;
    int nRet = 1;
    int icmd = 0, idis = 0, idel = 0, iena = 0, iguid = 0, ilst = 0;

    do {

        //
        // check command line parameters
        //
        for (i = 1; i < argc; i++) {
            if (0 == strcmp("/cmd", argv[i]))
                icmd = i;
            if (0 == strcmp("/dis", argv[i]))
                idis = i;
            if (0 == strcmp("/ena", argv[i]))
                iena = i;
            if (0 == strcmp("/del", argv[i]))
                idel = i;
            if (0 == strcmp("/guid", argv[i]))
                iguid = i;
            if (0 == strcmp("/list", argv[i]))
                ilst = i;
        }

        if (0 != ilst) {
            char fPrint = 0;
            wchar_t* pVarName = malloc(BSIZE);
            size_t VarNameSize = BSIZE;
            int num = 0, found = 0;

            memset(&guid, 0, sizeof(guid));

            wcscpy(pVarName, L"");
            do {


                VarNameSize = BSIZE;

                Status = SystemTable->RuntimeServices->GetNextVariableName(
                    &VarNameSize,
                    pVarName,
                    &guid
                );

                if ((Status == EFI_NOT_FOUND)) {
                    if (0 == found) {
                        printf("No \"CdeLoadOption\" variable found in NVRAM\n");
                    }
                    break;
                }

                if ((Status != EFI_SUCCESS)) {
                    fprintf(stderr, __FILE__"(%d): ""num %d -> %s\n", __LINE__, num, strefierror(Status));
                    break;
                }

                num++;

                //if (len) {
                fPrint = !wcscmp(pVarName, L"CdeLoadOption");
                found = found + fPrint;
                //    //printf(__FILE__"(%d): ""%S vs. %S, len == %d, fPrint == %d\n", __LINE__, pVarName2, pVarName, len, fPrint);
                //}
                //else {
                //    fPrint = 1;
                //}

                if (Status == EFI_SUCCESS && fPrint == 1) {
                    pVarName[VarNameSize] = '\0';
                    printf("%S : ", pVarName);
                    printf("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X ->",
                        guid.Data1,
                        guid.Data2,
                        guid.Data3,
                        guid.Data4[0],
                        guid.Data4[1],
                        guid.Data4[2],
                        guid.Data4[3],
                        guid.Data4[4],
                        guid.Data4[5],
                        guid.Data4[6],
                        guid.Data4[7]);

                    datasize = BSIZE;
                    Status = SystemTable->RuntimeServices->GetVariable(
                        L"CdeLoadOption"    /* IN CHAR16 * VariableName */,
                        &guid               /* IN EFI_GUID * VendorGuid */,
                        NULL                /* OUT UINT32 * Attributes OPTIONAL*/,
                        &datasize           /* IN OUT UINTN * DataSize */,
                        pNvram              /* OUT VOID * Data OPTIONAL*/
                    );

                    //fprintf(stderr, __FILE__"%d): ""%s\n", __LINE__, strefierror(Status));

                    if (EFI_SUCCESS == Status) {
                        printf("\"%s\" %s\n", pNvram->CommandLine, pNvram->rejectStart ? "Start rejected" : "");
                    }

                }
            } while (Status == EFI_SUCCESS);

            if (num == 0) {
                fprintf(stderr, __FILE__"(%d): ""\"CdeLoadOption\" NVRAM variable not found\n", __LINE__);
            }
            break;
        }
        //
        // check command line parameters
        //
        if (0 == iguid) {
            fprintf(stderr, "MISSING parameter: /guid 12345678-AAAA-BBBB-CCDD-112233445566\n");
            break;
        }

        if (0 == icmd && 0 == idis && 0 == idel && 0 == iena) {
            fprintf(stderr, "MISSING parameter: /cmd \"drivername parm1 parm2\"\n");
            fprintf(stderr, "                   /dis\n");
            fprintf(stderr, "                   /ena\n");
            fprintf(stderr, "                   /del\n");
            break;
        }

        memset(&guid, 0, sizeof(guid));

        //
        // get and test the guid passed after /guid command line parameter
        //
        n = sscanf(argv[iguid + 1], "%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
            &guid.Data1,
            &guid.Data2,
            &guid.Data3,
            &guid.Data4[0],
            &guid.Data4[1],
            &guid.Data4[2],
            &guid.Data4[3],
            &guid.Data4[4],
            &guid.Data4[5],
            &guid.Data4[6],
            &guid.Data4[7]);
        if (11 != n) {
            fprintf(stderr, "Invalid GUID format\nUse: 11111111-2222-3333-4455-66778899AABB\n");
            break;
        }

        //
        // /del - delete NVRAM variable
        //
        if (idel) {
            Status = SystemTable->RuntimeServices->SetVariable(
                L"CdeLoadOption",
                &guid,
                EFI_VARIABLE_NON_VOLATILE + EFI_VARIABLE_BOOTSERVICE_ACCESS + EFI_VARIABLE_RUNTIME_ACCESS,
                0,
                NULL
            );

            if (Status != EFI_SUCCESS)
                fprintf(stderr, __FILE__"(%d): ""%s\n", __LINE__, strefierror(Status));
            break;
        }

        //
        // /dis - disable (set rejectStart flag)
        //
        if (idis) {

            datasize = BSIZE;
            pNvram = realloc(pNvram, BSIZE);

            Status = SystemTable->RuntimeServices->GetVariable(
                L"CdeLoadOption"   /* IN CHAR16 * VariableName */,
                &guid               /* IN EFI_GUID * VendorGuid */,
                pAttributes         /* OUT UINT32 * Attributes OPTIONAL*/,
                &datasize           /* IN OUT UINTN * DataSize */,
                pNvram              /* OUT VOID * Data OPTIONAL*/
            );

            if (Status != EFI_SUCCESS) {
                fprintf(stderr, __FILE__"(%d): ""%s\n", __LINE__, strefierror(Status));
                break;
            }

            pNvram->rejectStart = 1;    // set disable flag

            Status = SystemTable->RuntimeServices->SetVariable(
                L"CdeLoadOption"    /* IN CHAR16* VariableName */,
                &guid               /* IN EFI_GUID* VendorGuid */,
                *pAttributes         /* IN UINT32 Attributes */,
                datasize            /* IN UINTN DataSize */,
                pNvram              /* IN VOID* Data */
            );
            if (Status != EFI_SUCCESS) {
                fprintf(stderr, __FILE__"(%d): ""%s\n", __LINE__, strefierror(Status));
                break;
            }
            printf("CdeLoadOption : %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X successfully DISABLED\n", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
            break;

        }

        //
        // /ena - enable (clear rejectStart flag)
        //
        if (iena) {

            datasize = BSIZE;
            pNvram = realloc(pNvram, BSIZE);

            Status = SystemTable->RuntimeServices->GetVariable(
                L"CdeLoadOption"   /* IN CHAR16 * VariableName */,
                &guid               /* IN EFI_GUID * VendorGuid */,
                pAttributes         /* OUT UINT32 * Attributes OPTIONAL*/,
                &datasize           /* IN OUT UINTN * DataSize */,
                pNvram              /* OUT VOID * Data OPTIONAL*/
            );

            if (Status != EFI_SUCCESS) {
                fprintf(stderr, __FILE__"(%d): ""%s\n", __LINE__, strefierror(Status));
                break;
            }

            pNvram->rejectStart = 0;    // set disable flag

            Status = SystemTable->RuntimeServices->SetVariable(
                L"CdeLoadOption"    /* IN CHAR16* VariableName */,
                &guid               /* IN EFI_GUID* VendorGuid */,
                *pAttributes        /* IN UINT32 Attributes */,
                datasize            /* IN UINTN DataSize */,
                pNvram              /* IN VOID* Data */
            );

            if (Status != EFI_SUCCESS) {
                fprintf(stderr, __FILE__"(%d): ""%s\n", __LINE__, strefierror(Status));
                break;
            }
            printf("CdeLoadOption : %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X successfully ENABLED\n", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
            break;
        }

        //
        // /cmd the command line passed to the driver "drivername parm1 parm2"
        //
        if (icmd) {

            pBuf->rejectStart = (0 != idis);

            strcpy(pBuf->CommandLine, argv[icmd + 1]);

            Status = SystemTable->RuntimeServices->SetVariable(
                L"CdeLoadOption",
                &guid,
                EFI_VARIABLE_NON_VOLATILE + EFI_VARIABLE_BOOTSERVICE_ACCESS + EFI_VARIABLE_RUNTIME_ACCESS,
                sizeof(NVRAMCOMMANDLINE) + len,
                pBuf
            );

            if (Status != EFI_SUCCESS) {
                fprintf(stderr, __FILE__"(%d): ""%s\n", __LINE__, strefierror(Status));
                break;
            }

            datasize = BSIZE;

            Status = SystemTable->RuntimeServices->GetVariable(
                L"CdeLoadOption"    /* IN CHAR16 * VariableName */,
                &guid               /* IN EFI_GUID * VendorGuid */,
                NULL                /* OUT UINT32 * Attributes OPTIONAL*/,
                &datasize           /* IN OUT UINTN * DataSize */,
                pNvram              /* OUT VOID * Data OPTIONAL*/
            );

            //fprintf(stderr, __FILE__"%d): ""%s\n", __LINE__, strefierror(Status));

            if (EFI_SUCCESS != Status) {
                fprintf(stderr, __FILE__"(%d): ""fail to read NVRAM variable \"CdeLoadOption\" : %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X -> %s\n", __LINE__, guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7], strefierror(Status));
                break;
            }
            printf("CdeLoadOption : %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X command line \"%s\" successfully createdn\n", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7], pNvram->CommandLine);

        }
    } while (0);

    return nRet;
}


