#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#undef NULL
#include <uefi.h>

#define BSIZE 4096

extern char* strefierror(EFI_STATUS errcode);                           // Torito C extention according to strerror()

typedef struct _NVRAMCOMMANDLINE {
    int rejectStart;            //  1 -> suppress start of driver, even if registered with EFI_CALLER_ID_GUID. 0 -> start driver and pass command line to it
    char CommandLine[0];		/*  assigned command line includeing filename*/
}NVRAMCOMMANDLINE;

int main(int argc, char** argv) {

    EFI_SYSTEM_TABLE* SystemTable = (EFI_SYSTEM_TABLE*)(argv[-1]);      //SystemTable is passed in argv[-1]
//    EFI_HANDLE ImageHandle = (void*)(argv[-2]);                         //ImageHandle is passed in argv[-2]
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_GUID guid;
    int n;
    int nRet = 1;
    char fPrint = 1;
    size_t len = argc > 1 ? 1 + strlen(argv[1]) : 0;
    wchar_t* pVarName        = malloc(BSIZE);
    wchar_t* pVarName2       = malloc(BSIZE);
    NVRAMCOMMANDLINE* pNvram = malloc(BSIZE);
    size_t datasize = BSIZE;
    size_t VarNameSize = BSIZE;

    memset(&guid, 0, sizeof(guid));
    
    swprintf(pVarName2, len, L"%S", argv[1]);
    printf(__FILE__"(%d)""%S\n", __LINE__, pVarName2);
    wcscpy(pVarName, L"");

    do {
        VarNameSize = BSIZE;
        Status = SystemTable->RuntimeServices->GetNextVariableName(
            &VarNameSize,
            pVarName,
            &guid
        );

        if ((Status != EFI_SUCCESS)) {
            break;
            fprintf(stderr, __FILE__"%d): ""%s\n", __LINE__, strefierror(Status));
        }
        
        if (len) {
            fPrint = !wcsncmp(pVarName2, pVarName,len-1);
            //printf(__FILE__"(%d): ""%S vs. %S, len == %d, fPrint == %d\n", __LINE__, pVarName2, pVarName, len, fPrint);
        }
        else {
            fPrint = 1;
        }

        if (Status == EFI_SUCCESS && fPrint == 1) {
            pVarName[VarNameSize] = '\0';
            printf("%S : ", pVarName);
            printf("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
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
                pVarName    /* IN CHAR16 * VariableName */,
                &guid       /* IN EFI_GUID * VendorGuid */,
                NULL        /* OUT UINT32 * Attributes OPTIONAL*/,
                &datasize    /* IN OUT UINTN * DataSize */,
                pNvram       /* OUT VOID * Data OPTIONAL*/
            );
            
            //fprintf(stderr, __FILE__"%d): ""%s\n", __LINE__, strefierror(Status));
            
            if (EFI_SUCCESS == Status) {
                printf("%s -> CommandLine: \"%s\"\n", pNvram->rejectStart == 1 ? "driver start rejected" : "driver started during POST", pNvram->CommandLine);
            }

        }
    } while (Status == EFI_SUCCESS);

    return nRet;
}


