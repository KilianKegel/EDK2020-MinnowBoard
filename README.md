# EDK2020-MinnowBoard featuring [CdePkg (C Development Environment Package)](https://github.com/KilianKegel/CdePkg#cdepkg)
Get the MinnowBoard running with EDK2020 and VS2019 
* [Introduction](https://github.com/KilianKegel/EDK2020-MinnowBoard#introduction-edk2020-minnowboard)
* [Goal](https://github.com/KilianKegel/EDK2020-MinnowBoard#goal)
* [HowTo/install/build](https://github.com/KilianKegel/EDK2020-MinnowBoard#howto-installbuildedit)
	* [Installation](https://github.com/KilianKegel/EDK2020-MinnowBoard#installation)
	* [Build](https://github.com/KilianKegel/EDK2020-MinnowBoard#build)
	* [Edit](https://github.com/KilianKegel/EDK2020-MinnowBoard#edit)
* [Related Projects](https://github.com/KilianKegel/EDK2020-MinnowBoard#related-projects)
* [Known Bugs](https://github.com/KilianKegel/EDK2020-MinnowBoard#known-bugs)
* [Revision History](https://github.com/KilianKegel/EDK2020-MinnowBoard#revision-history)


![pci1](https://github.com/KilianKegel/pictures/blob/master/minnowboardgray.png)

https://github.com/MinnowBoard-org/

## Introduction **EDK2020-Minnowboard**
**This **EDK2020-Minnowboard** introduces the [**_CdePkg_**](https://github.com/KilianKegel/CdePkg#cdepkg) and
the [**_CdeValidationPkg_**](https://github.com/KilianKegel/CdeValidationPkg#cdevalidationpkg) to the
UEFI/Tianocore open source community.**

**EDK2020-Minnowboard** is the Git-*Super-Project* in the [EDK2020-MinnowBoard](https://github.com/KilianKegel/EDK2020-MinnowBoard) project to build the MinnowBoard UEFI BIOS.
It contains the [**tianocore edk2**](https://github.com/tianocore/edk2) as a Git-*Sub-Project*.
[**tianocore edk2**](https://github.com/tianocore/edk2) can also be used stand alone in Emulation Mode (EmulatorPkg)
(NOTE: In emulation mode timing calulation assumes to run a 1GHz platform).


The MinnowBoard familiy is an *open source* Personal Computer hardware originally created by a company called
*ADI Engineering*, that now belongs to [Silicom](https://www.silicom-usa.com/)

Intel provides [binary modules](https://firmware.intel.com/projects/minnowboard-max) and maintains the [build environment](https://github.com/tianocore/edk2-platforms/blob/master/Platform/Intel/Vlv2TbltDevicePkg/Readme.md)
to get the MinnowBoard running with the open source [UEFI BIOS Tianocore\EDK2](https://github.com/tianocore/edk2.git)

The MinnowBoard is the only free available hardware/PC platform to get UEFI Tianocore BIOS running.

**EDK2020-MinnowBoard** is the Git-*Super-Project* to build the MinnowBoard UEFI BIOS and demonstrates
[**_CdePkg_**](https://github.com/KilianKegel/CdePkg#cdepkg) and [**_CdeValidationPkg_**](https://github.com/KilianKegel/CdeValidationPkg#cdevalidationpkg)
on real hardware.

## Goal
1. 	**The main aspect is, to introduce the [**_CdePkg_**](https://github.com/KilianKegel/CdePkg#cdepkg) and
	the [**_CdeValidationPkg_**](https://github.com/KilianKegel/CdeValidationPkg#cdevalidationpkg) to the
	UEFI/Tianocore open source community.**

2. get the MinnowBoard and the EDK2 Emulation (EmulatorPkg) running with the latest released EDK2020 and the latest Visual Studio VS2019 build environment


**CdePkg** is considered an improvement over traditional UEFI BIOS development, since it introduces
* stable, precise, chipset (ACPI timer) independent C library conform [`clock()`](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/clock?view=vs-2019) for POST and UEFI Shell in millisecond resolution
* unlimited (buffer less) [`printf()`](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/printf-printf-l-wprintf-wprintf-l?view=vs-2019)-family and [`scanf()`](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/scanf-scanf-l-wscanf-wscanf-l?view=vs-2019)-family implementation
* Standard C conform format specifiers for [`printf()`](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/printf-printf-l-wprintf-wprintf-l?view=vs-2019)-family and [`scanf()`](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/scanf-scanf-l-wscanf-wscanf-l?view=vs-2019)-family
* a library, which satisfies all the Microsoft C compiler intrinsic requirements with one and the same library (C++ not supported)
* *Hosted Environment* instead of *Freestanding Environment* for POST driver

This improvements shall enable UEFI and [modernFW](https://github.com/intel/ModernFW#modernfw-project)
to provide a Standard C interface for any "C"-(open)source-based extention.

NOTE: Visual Studio is here only used for editing the project. The build process is still pure EDK!
      (the startup error message of VS2019 can be ignored)

## HowTo install/build/Edit
### Installation
1. install a build machine according to https://github.com/KilianKegel/HowTo-setup-an-UEFI-Development-PC#howto-setup-an-uefi-development-pc

2. clone the EDK2020-MinnowBoard project `--recursive`<br>
	`git.exe clone --progress --recursive -v "https://github.com/KilianKegel/EDK2020-MinnowBoard.git A:\EDK2020-MinnowBoard`<br>

### Build
3. To build the EDK emulation:<br>
`launch.bat` to setup EDK2 / emulation (EmulatorPkg) build environment<br>
`rd /s /q build` in the `edk2`directory to clean previous EDK2 / emulation build<br>
`bldEMU.bat (Debug) to start the build process in the EDK2 directory<br>
`dbgemu.bat` to start the UEFI BIOS EMULATION<br>

4. To build the MinnowBoard:<br>
`launchbat` to setup MinnowBoard (`Vlv2TbltDevicePkg`) build environment<br>
`rd /s /q build` in the `.\`directory to clean previous `Vlv2TbltDevicePkg` build<br>
`bldMNW.bat DEBUG/RELEASE` <br>

### Edit

EDK2020-MinnowBoard is the Git-[Superproject **EDK2020-MinnowBoard**](https://github.com/KilianKegel/EDK2020-MinnowBoard) to build the MinnowBoard UEFI BIOS.

It includes the *edk2* which is the Git-[Subproject **edk2-vUDK2018**](https://github.com/KilianKegel/edk2-vUDK2018), that in turn
includes the *Cde*-directories as GIT-[Subproject **CdePkg**](https://github.com/KilianKegel/CdePkg) and 
GIT-[Subproject **CdeValidationPkg**](https://github.com/KilianKegel/CdeValidationPkg).

## Related Projects
| related project|annotation|
|:-|:-|
|[Torito C Library](https://github.com/KilianKegel/torito-C-Library#torito-c-library)|C Library for UEFI Shell only. All projects below are built on or derived from *Torito C Library*|
|[Visual ANSI C for UEFI Shell](https://github.com/KilianKegel/Visual-ANSI-C-for-UEFI-Shell#visual-ansi-c-for-uefi-shell)|Visual Studio for UEFI Shell for beginners.|
|[Visual DOS Tools for UEFI Shell](https://github.com/KilianKegel/Visual-DOS-Tools-for-UEFI-Shell#visual-dos-tools-for-uefi-shell)|more command implementation|
|[Visual HWTools for UEFI Shell](https://github.com/KilianKegel/Visual-HWTools-for-UEFI-Shell#visual-hwtools-for-uefi-shell)|HWTools: PCI- and GPIOSpy for Baytrail. MemSpy for all.|
|[CdePkg](https://github.com/KilianKegel/CdePkg#cdepkg)|*Torito C Library* redone for UEFI POST usage|
|[CdeValidationPkg](https://github.com/KilianKegel/CdeValidationPkg#cdevalidationpkg)|Unit tests for *CdePkg*|

## Known Bugs

## Revision History
### 20200205
* introduce EDK2020
### 20190906/improve tool chain check
* add IASL version check
* add PYTHON version check
* add NASM version check
### 20190901
* add Dediprog flash update
* [BIOS binary](https://github.com/KilianKegel/EDK2020-MinnowBoard/blob/master/edk2-platforms/Vlv2TbltDevicePkg/Stitch/MNW2MAX1.X64.0101.R01.1909012230.bin)

### 20190830
* CdeValPkg branch merge

### 20190830\OpenSSL
* add OpenSSL_1_1_0-stable to CryptoPkg\Library\OpensslLib

### 20190830
* improved build and startup batches
* [BIOS binary](https://github.com/KilianKegel/EDK2020-MinnowBoard/blob/master/edk2-platforms/Vlv2TbltDevicePkg/MNW2MAX1.X64.0101.R01.1908292234.bin)

### 20190829
* convert EDK2 directory to submodule, to demonstrate *CdePkg* separated from entire MinnowBoard environment
* update EDK2 to *20190828/OpenSSL*
* integrate *Release 1.01 Firmware Download (Aug 2019)* from https://firmware.intel.com/projects/minnowboard-max

### 20190822
* place CDE*.SLN solution files into EDK2 directory, to be able to use Visual Studio solutions
  EDK2 directory without MinnowBoard build environment
* improve CdeValidationPkg

### 20190811
* add validation driver/executables for all `ctype.h`- and `wctype.h`-related functions:
    (ctypeALLPei, ctypeALLDxe, wctypeALLPei, wctypeALLDxe)
* remove previously used single function test drivers

### 20190802/1
* WinNTx86-64 (Microsoft C Library)
* WinNTx86-64 (Torito C Library)

### 20190730
* add all <em>CTYPE.H()</em> -functions to PEI post phase

###	20190728
* add all <em>CTYPE.H</em>-functions for DXE

### 20190727/2
* add <em>clockPei()</em>

### 20190726
* initial version of branch CdeValPkg
* add MFNBAR as bare/naked parameter for CDEMOFINE macro
* add clockDxe commandline to CdeLoadOptions.h
* update CdeSrcPkg/b81394c620206ebbc300216652cd43d7f4ac94e3


### 20190709
* add initial PEI Support, rudimentary functional range
* [BIOS binary](https://github.com/KilianKegel/EDK2020-MinnowBoard/blob/master/edk2-platforms/Vlv2TbltDevicePkg/Stitch/MNW2MAX1.X64.0100.R01.1907070918.bin)

### 20190626
* add VS2019 support (for both MinnowBoard- and Emulationbuild)

### 20190605
* removed CdePkg from main tree
* add submodules CdePkg and CdeSrcPkg
* [BIOS binary](https://github.com/KilianKegel/EDK2020-MinnowBoard/blob/master/edk2-platforms/Vlv2TbltDevicePkg/Stitch/MNW2MAX1.X64.0100.R01.1906052251.bin)

### 20190513
* CdePkg: initial revision - standard C Library and standard headerfile usage

### 20190225
* enable emulation build and debug
* improve MinnowBoard build

### 20190224
* added missing files from UDK2017 to build MinnowBoard in UDK2018
* fixed: build MinnowBoard

### 20190222 - initial checkin
* downloaded the entire source code (see HowTo)
* configured as a VS2017 solution - just for editing the source
* not able to build
