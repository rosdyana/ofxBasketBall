@ECHO OFF
SETLOCAL

PUSHD %~dp0

AStyle.exe --version 1>&2 2>NUL

IF %ERRORLEVEL% NEQ 0 (
  ECHO.
  ECHO ERROR: Astyle wasn't found!
  ECHO Visit http://astyle.sourceforge.net/ for download and details.
  GOTO END
)
AStyle.exe --style=kr^
 --indent=spaces=4 --indent-switches --indent-namespaces --indent-col1-comments^
 --suffix=none^
 --add-brackets^
 --pad-header --pad-oper --unpad-paren^
 --align-pointer=name^
 --preserve-date^
 --recursive ..\..\src\*.cpp ..\..\src\*.h ..\..\src-app\*.cpp ..\..\src-app\*.h
IF %ERRORLEVEL% NEQ 0 (
  ECHO.
  ECHO ERROR: Something went wrong!
  GOTO END
)
:END
POPD
PAUSE
ENDLOCAL
EXIT /B