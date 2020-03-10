#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#undef NULL
#include <uefi.h>

extern char* strefierror(EFI_STATUS errcode);                           // Torito C extention according to strerror()

typedef struct _NVRAMCOMMANDLINE {
    int rejectStart;            //  1 -> suppress start of driver, even if registered with EFI_CALLER_ID_GUID. 0 -> start driver and pass command line to it
    char CommandLine[0];		/*  assigned command line includeing filename*/
}NVRAMCOMMANDLINE;

int main(int argc, char **argv) {

    EFI_SYSTEM_TABLE* SystemTable = (EFI_SYSTEM_TABLE*)(argv[-1]);      //SystemTable is passed in argv[-1]
//    EFI_HANDLE ImageHandle = (void*)(argv[-2]);                         //ImageHandle is passed in argv[-2]
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_GUID guid;
    int n,i;
    int nRet = 1;
    size_t len = argc > 2 ? 1 + strlen(argv[2]) : 0;
    NVRAMCOMMANDLINE* pBuf = malloc(sizeof(NVRAMCOMMANDLINE) + len);
    int icmd = 0, idis = 0, iclr = 0, iena = 0, iguid = 0;

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
            if (0 == strcmp("/clr", argv[i]))
                iclr = i;
            if (0 == strcmp("/guid", argv[i]))
                iguid = i;
        }

        //
        // check command line parameters
        //
        if (0 == iguid) {
            fprintf(stderr, "MISSING parameter: /guid 12345678-AAAA-BBBB-CCDD-112233445566");
            break;
        }
        if (0 == icmd && 0 == idis && 0 == iclr && 0 == iena)) {
            fprintf(stderr, "MISSING parameter: /cmd \"drivername parm1 parm2\"");
            break;
        }

        memset(&guid, 0, sizeof(guid));

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
        //
        // check command line
        //
        if (11 != n) {
            fprintf(stderr, "Invalid GUID format\nUse: 11111111-2222-3333-4455-66778899AABB\n");
            break;
        }

        if (argc < 3) {
            fprintf(stderr, "\nUse: SETLDOPT 11111111-2222-3333-4455-66778899AABB \"drivername parm1 parm2 ...\"\n");
            break;
        }

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

        printf("sizeof(NVRAMCOMMANDLINE) %d\n", (int)sizeof(NVRAMCOMMANDLINE));
        
        pBuf->rejectStart = 0;
        strcpy(pBuf->CommandLine, argv[2]);

        Status = SystemTable->RuntimeServices->SetVariable(
            L"CdeLoadOption",
            &guid, 
            EFI_VARIABLE_NON_VOLATILE + EFI_VARIABLE_BOOTSERVICE_ACCESS + EFI_VARIABLE_RUNTIME_ACCESS,
            sizeof(NVRAMCOMMANDLINE) + len,
            pBuf
        );

        if (Status != EFI_SUCCESS)
            fprintf(stderr, "%s\n", strefierror(Status));

    } while (0);
    
    return nRet;
}


