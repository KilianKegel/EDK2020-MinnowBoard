@echo off
echo clean up...
if exist x64 del /s x64\*.pdb
if exist x64 del /s x64\*.map
if exist x64 del /s x64\*.ilk
rem for /D %%d in (LstLdOpt SetLdOpt) do if exist %%d\x64 rd /s /q %%d\x64

for /F "tokens=1-4 delims==," %%a in ('type *.sln ^| find ".vcxproj"') do (
    for  %%o in (%%c) do (
        echo %%~dpo
        if exist %%~dpo\x64 rd /s /q %%~dpo\x64
    )
)
rem pause

