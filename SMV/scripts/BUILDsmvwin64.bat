@echo off
Title Building Smokeview for 64 bit Windows

Rem  Windows batch file to build a release Smokeview for Windows 64.

Rem setup environment variables (defining where repository resides etc) 

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
echo.
echo Using GIT revision %smv_revision% to build a 64 bit Windows Smokeview

%svn_drive%
cd %svn_root%\SMV\Build\intel_win_64
call make_smv -r

echo.
echo compilation complete
pause
