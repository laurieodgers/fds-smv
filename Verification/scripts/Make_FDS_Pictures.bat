@echo off

if "x%1" == "x" goto else1
set SMOKEVIEW=%1
:else1
set SMOKEVIEW=smokeview
:endif1

cd ..
set BASEDIR=%CD%\

echo.
echo erasing User Guide scripted figures generated previously
erase ..\Manuals\FDS_User_Guide\SCRIPT_FIGURES\*.png 1> Nul 2>&1

echo.
echo erasing Verification scripted figures generated previously
erase ..\Manuals\FDS_Verification_Guide\SCRIPT_FIGURES\*.png 1> Nul 2>&1

cd ..
set SVNROOT=%CD%

cd %BASEDIR%
set RUNSMV=call "%BASEDIR%\scripts\runsmv.bat"

call ..\Utilities\Data_Processing\sh2bat FDS_Pictures.sh FDS_Pictures.bat

call FDS_Pictures.bat

cd %BASEDIR%
erase FDS_Pictures.bat