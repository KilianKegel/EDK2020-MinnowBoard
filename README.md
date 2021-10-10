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


![pci1](https://github.com/KilianKegel/pictures/blob/master/TurbotBoardWFansink-SMALL.jpg)

https://github.com/MinnowBoard-org/

## Introduction **EDK2020-Minnowboard**
**This **EDK2020-Minnowboard** introduces the [**_CdePkg_**](https://github.com/KilianKegel/CdePkg#cdepkg) and
the [**_CdePkgValidation_**](https://github.com/KilianKegel/CdePkgValidation#CdePkgValidation) to the
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
[**_CdePkg_**](https://github.com/KilianKegel/CdePkg#cdepkg) and [**_CdePkgValidation_**](https://github.com/KilianKegel/CdePkgValidation#CdePkgValidation)
on real hardware.

## Goal
1. 	**The main aspect is, to introduce the [**_CdePkg_**](https://github.com/KilianKegel/CdePkg#cdepkg) and
	the [**_CdePkgValidation_**](https://github.com/KilianKegel/CdePkgValidation#CdePkgValidation) to the
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
    * `git.exe clone --progress --recursive -v "https://github.com/KilianKegel/EDK2020-MinnowBoard.git`<br>

### Build
3. To build the EDK emulation:<br>
    * `launch.bat` to setup EDK2 / emulation (EmulatorPkg) build environment<br>
    * `rd /s /q build` in the `edk2`directory to clean previous EDK2 / emulation build<br>
    * `bldEMU.bat` to start the emulation build process in the EDK2 directory<br>
    * `dbgemu.bat` to start the UEFI BIOS EMULATION<br>

4. To build the MinnowBoard:<br>
    * `launchbat` to setup MinnowBoard (`Vlv2TbltDevicePkg`) build environment<br>
    * `rd /s /q build` in the `.\`directory to clean previous `Vlv2TbltDevicePkg` build<br>
    * `bldMNW.bat DEBUG/RELEASE` <br>

### Edit

EDK2020-MinnowBoard is the Git-[Superproject **EDK2020-MinnowBoard**](https://github.com/KilianKegel/EDK2020-MinnowBoard) to build the MinnowBoard UEFI BIOS.

It includes the *edk2* which is the Git-[Subproject **edk2-vUDK2018**](https://github.com/KilianKegel/edk2-vUDK2018), that in turn
includes the *Cde*-directories as GIT-[Subproject **CdePkg**](https://github.com/KilianKegel/CdePkg) and 
GIT-[Subproject **CdePkgValidation**](https://github.com/KilianKegel/CdePkgValidation).

## Related Projects
| related project|annotation|
|:-|:-|
|[Torito C Library](https://github.com/KilianKegel/torito-C-Library#torito-c-library)|C Library for UEFI Shell only. All projects below are built on or derived from *Torito C Library*|
|[Visual ANSI C for UEFI Shell](https://github.com/KilianKegel/Visual-ANSI-C-for-UEFI-Shell#visual-ansi-c-for-uefi-shell)|Visual Studio for UEFI Shell for beginners.|
|[Visual DOS Tools for UEFI Shell](https://github.com/KilianKegel/Visual-DOS-Tools-for-UEFI-Shell#visual-dos-tools-for-uefi-shell)|more command implementation|
|[Visual HWTools for UEFI Shell](https://github.com/KilianKegel/Visual-HWTools-for-UEFI-Shell#visual-hwtools-for-uefi-shell)|HWTools: PCI- and GPIOSpy for Baytrail. MemSpy for all.|
|[CdePkg](https://github.com/KilianKegel/CdePkg#cdepkg)|*Torito C Library* redone for UEFI POST usage|
|[CdePkgValidation](https://github.com/KilianKegel/CdePkgValidation#CdePkgValidation)|Unit tests for *CdePkg*|

## Known Bugs 

## Revision History
### 20211010
* add Microsoft C Library functions for UEFIShell 64Bit applications only
  - `_mkdir()`
  - `_stat64i32()` that is the Microsoft version of POSIX `stat()`

  add POSIX C Library functions
  - `strnlen()`, `wcsnlen()`
### 20210912
* initial version of *TORO C LIBRARY* (`toroC64.lib` and `toroC32.lib`)
* *TORITO C LIBRARY* is _discontinued_ from now on
* add Microsoft C Library functions for UEFIShell 64Bit applications only
  - `getc()`
  - `_findfirst()`
  - `_findnext()`
  - `_findclose()`

### 20210821
* rename library file to `toroC64.lib` and `toroC32.lib`
* add SMM support
* fixed bugs related to length modifiers for string format specifiers
    - `wprintf()` with `%ls`,  `%hs` and Microsoft specific  `%S`
    - `printf()` with `%ls`,  `%hs` and Microsoft specific  `%S`
    - the string `(null)` was reported wrongly in wide format on `NULL` pointer (e.g. `wprintf()`or `%ls` modifier)
* add Standard C Library functions 
  - `mblen()`
  - `mbstowcs()`
  - `mbtowc()`
  - `wcstombs()`
  - `wctomb()`
  - `wctob()`
  - `wmemchr()`
  - `vfwprintf()`
* adjust `vfprintf()` and `vfwprintf()` to redirect `stdout` and `stderr` to `ReportStatusCode` interface
* change TimeStampCounter (TSC) calibration from RTC MC146818 to PIT i8254 base, since RTC is faulty on AMD systems.
* introduce new `IMAGE_ENTRY_POINT`:
  - `_cdeCRT0UefiPei` and respectively `_cdeCRT0UefiPeiEDK` for EDK build
  - `_cdeCRT0UefiDxe` and respectively `_cdeCRT0UefiDxeEDK` for EDK build
  - `_cdeCRT0UefiSmm` and respectively `_cdeCRT0UefiSmmEDK` for EDK build
  - `_cdeCRT0UefiShell` and respectively `_cdeCRT0UefiShellEDK` for EDK build
* adjust EMULATION time base to 2GHz

### 20210626
* add dedicated FATAL ERROR message to DXE+PEI CRT0 if CdeServices protocol  fails to
  be located, due to DEPEX or APRIORI issue

### 20210624
* enable Tianocore DEBUG macro for CDE-based/injected drivers to allow usage of
  DEBUG traces with RELEASE BIOS versions
* add support for BIOS vendor (A)
    * adjust include path
    * provide component description and configuration files

### 20210429
* remove .xdata and .pdata sections ($unwind$ and $pdata$ symbols) from .OBJ before binding the library
  
### 20210427
* source tree redesign
* torito C library redesign to coexist with / replace EDK2 driver functions
* configure CdePkg to support original tianocore DEBUG traces
* extend Microsft intrinsic __acrt_iob_func() to support variable length I/O buffer count
* improve EDK2 emulator support:
  1. check privileg level before _enable() and _disable()
  2. provide BREAKPOINT entries __cdeImageEntryPointPeiINT3(), __cdeImageEntryPointDxeINT3()
* remove dependancy from 'LoadOptions'
  CdePkg based drivers can start w/o LoadOptions and just get argv[0], argc == 1, if the LoadfOptions driver is not present in the system
* add missing UEFI SHELL Emulation support
* minor face lifts
  1. move forward to better module naming scheme with prefix '__cde'

### 20201205
* update EDK2 source to [`edk2-stable202011`](https://github.com/tianocore/edk2/releases/tag/edk2-stable202011)
* NOTE: to run DEBUG build succesfully ASSERTS must be [disabled](https://github.com/KilianKegel/EDK2020-MinnowBoard/blob/master/overrides/edk2-platforms/PlatformPkgX64.dsc#L498)

### 20201124
* fix EDK emulation

### 20200916
* update EDK2 source to `edk2-stable202008`
    * add to FmpMinnowMaxSystem.dsc, FmpBlueSampleDevice.dsc, FmpGreenSampleDevice.dsc, FmpRedSampleDevice.dsc `[LibraryClasses.common]` to prevent build failure: 
        * `  FmpDependencyLib|FmpDevicePkg\Library\FmpDependencyLib\FmpDependencyLib.inf`
        * `  FmpDependencyCheckLib|FmpDevicePkg\Library\FmpDependencyCheckLibNull\FmpDependencyCheckLibNull.inf`
        * `  FmpDependencyDeviceLib|FmpDevicePkg\Library\FmpDependencyDeviceLibNull\FmpDependencyDeviceLibNull.inf`

### 20200507
* add diagnostic driver CdeDiagTSCSync, that demonstrates syncrone timing of RTC and TSC-based <time.h> functions
* remove diagnostic driver CdeDiagPCI (PCI is available in BDS phase only)
* improved build configurations for all solution projects
* update all UEFI shell batch files to configure POST LoadOptions/command lines
    * [`setall.nsh`](https://github.com/KilianKegel/CdePkg/blob/master/LoadOptionShell/setall.nsh), to set command line defaults for all CdePkg drivers
    * [`enaclock.nsh`](https://github.com/KilianKegel/CdePkg/blob/master/LoadOptionShell/enaclock.nsh), to set command line defaults for CdeDiagTSCDiag and clock only
    * [`disall.nsh`](https://github.com/KilianKegel/CdePkg/blob/master/LoadOptionShell/disall.nsh), to prevent known CdePkg drivers from beeing started
    * [`delall.nsh`](https://github.com/KilianKegel/CdePkg/blob/master/LoadOptionShell/delall.nsh), to delete all CdePkg related command lines from flash
    * NOTE: 
        1. run `setall.nsh` first
        2. `disall.nsh`to prevent CdePkg drivers from beeing started if you don't want run all drivers
        3. adjust `enaclock.nsh` to your needs
* NOTE: This release is focused on real HW (MinnowBoard). Emulation mode doesn't allow hardware access (GPIO, RTC)
  For Emulation Build the command lines are still stored in the [`CdeLoadOptions.h`](https://github.com/KilianKegel/CdePkg/blob/master/Include/CdeLoadOptions.h)
  
### 20200409
* add diagnostic drivers (CdeDiagGPIO, CdeDiagRTC, CdeDiagPCI, CdeDiagEFIVAR(iable)) for demonstration purpose
  (CdeDiagPCI under construction)
### 20200315
* implement command line support based on NVRAM variables for the MinnowBoard
* create/provide `LoadOption` tool to **CdePkg** (UEFIBinaries\LoadOption.efi) to create command lines stored in NVRAM variables

### 20200306 Pre and Post MemoryInit
* integrate PreMemInit branch into master trunk
    * reorganize flash layout to get 192kByte in the RECOVERY volume to hold additional PRE-memory PEI drivers
* remove UEFI SHELL from bios flash binary
* update EDK2 to edk2-stable202002

### 20200212
* implement new **CdePkg** POST driver command line model:
    1. each **CdePkg** driver listed in [`CdeLoadOptions.h`](https://github.com/KilianKegel/CdePkg/blob/master/Include/CdeLoadOptions.h) gets the command line parameters passed
    2. each **CdePkg** driver listed in [`CdeLoadOptions.h`](https://github.com/KilianKegel/CdePkg/blob/master/Include/CdeLoadOptions.h) can be suppressed from beeing started by (`rejectStart=1`)
    3. each **CdePkg** driver **NOT** listed in [`CdeLoadOptions.h`](https://github.com/KilianKegel/CdePkg/blob/master/Include/CdeLoadOptions.h) is started with `argv[0] == "unknownCdeDriverDxe"`
* add **MdePkg* to **CdePkg** convertion sample [Ip4Dxe](https://github.com/KilianKegel/CdePkg/blob/master/footnotes/IP4DriverCDiff.html)
### 20200206
* add overrides directory to PACKAGES_PATH
* patch WinHost.c to enable debugging in emulation mode
### 20200205
* introduce EDK2020
### 20190906/improve tool chain check
* add IASL version check
* add PYTHON version check
* add NASM version check
### 20190901
* add Dediprog flash update

### 20190830
* CdeValPkg branch merge

### 20190830\OpenSSL
* add OpenSSL_1_1_0-stable to CryptoPkg\Library\OpensslLib

### 20190830
* improved build and startup batches

### 20190829
* convert EDK2 directory to submodule, to demonstrate *CdePkg* separated from entire MinnowBoard environment
* update EDK2 to *20190828/OpenSSL*
* integrate *Release 1.01 Firmware Download (Aug 2019)* from https://firmware.intel.com/projects/minnowboard-max

### 20190822
* place CDE*.SLN solution files into EDK2 directory, to be able to use Visual Studio solutions
  EDK2 directory without MinnowBoard build environment
* improve CdePkgValidation

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

### 20190626
* add VS2019 support (for both MinnowBoard- and Emulationbuild)

### 20190605
* removed CdePkg from main tree
* add submodules CdePkg and CdeSrcPkg

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
