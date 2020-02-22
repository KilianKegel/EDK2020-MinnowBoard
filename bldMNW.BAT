@echo off
rem echo %~p0
rem goto EOF

if exist %~p0Build\Vlv2TbltDevicePkgX64\%1_VS2015x86\FV\VLV.old (
    del %~p0Build\Vlv2TbltDevicePkgX64\%1_VS2015x86\FV\VLV.old
)
if exist %~p0Build\Vlv2TbltDevicePkgX64\%1_VS2015x86\FV\VLV.fd (
    ren %~p0Build\Vlv2TbltDevicePkgX64\%1_VS2015x86\FV\VLV.fd vlv.old
)
cd %~p0\edk2
call build -a IA32 -a X64 -n 5 -t VS2015x86 -b %1 -p PlatformPkgX64.dsc
if exist %~p0Build\Vlv2TbltDevicePkgX64\%1_VS2015x86\FV\VLV.fd (
    echo copy /y /b %~p0Build\Vlv2TbltDevicePkgX64\%1_VS2015x86\FV\VLV.fd %~p0UEFIBinaries\%1.bin
    copy /y /b %~p0Build\Vlv2TbltDevicePkgX64\%1_VS2015x86\FV\VLV.fd %~p0UEFIBinaries\%1.bin

) else (
    if exist %~p0Build\Vlv2TbltDevicePkgX64\%1_VS2015x86\FV\vlv.old (
        ren %~p0Build\Vlv2TbltDevicePkgX64\%1_VS2015x86\FV\vlv.old VLV.fd
    )
)
:EOF