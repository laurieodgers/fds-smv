@echo off

::  Windows batch file to build Linux version of smokezip

:: setup environment variables (defining where repository resides etc) 

set envfile="%userprofile%"\fds_smv_env.bat
IF EXIST %envfile% GOTO endif_envexist
echo ***Fatal error.  The environment setup file %envfile% does not exist. 
echo Create a file named %envfile% and use SMV/scripts/fds_smv_env_template.bat
echo as an example.
echo.
echo Aborting now...
pause>NUL
goto:eof

:endif_envexist

call %envfile%
echo Building Linux version of fds2ascii

%svn_drive%
cd %svn_root%\smv\scripts

set scriptdir=FDS-SMV/SMV/scripts

plink %svn_logon% %scriptdir%/ssh_command.csh %linux_hostname% %scriptdir% MAKEf2alinux.csh %smv_revision%

echo.
echo compilation complete
pause
