@echo off
cd %~p0\edk2
build -p EmulatorPkg.dsc -t VS2015x86 -a IA32
:EOF