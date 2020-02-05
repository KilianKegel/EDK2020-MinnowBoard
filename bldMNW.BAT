@echo off
cd %~p0\edk2
build -a IA32 -a X64 -n 5 -t VS2015x86 -b %1 -p PlatformPkgX64.dsc
:EOF