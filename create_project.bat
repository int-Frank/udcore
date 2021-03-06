@ECHO OFF

ECHO Select the type of project you would like to create:
ECHO 1. Visual Studio 2015 Solution
ECHO 2. Visual Studio 2015 Solution (Emscripten)
ECHO 3. Visual Studio 2010 Solution
ECHO 4. CodeLite (Linux)
ECHO 5. GNU Makefile (Linux)

CHOICE /N /C:12345 /M "[1-5]:"

IF ERRORLEVEL ==5 GOTO FIVE
IF ERRORLEVEL ==4 GOTO FOUR
IF ERRORLEVEL ==3 GOTO THREE
IF ERRORLEVEL ==2 GOTO TWO
IF ERRORLEVEL ==1 GOTO ONE
GOTO END

:FIVE
 ECHO Creating GNU Makefile...
 bin\premake-bin\premake5.exe gmake --os=linux
 GOTO END
:FOUR
 ECHO Creating CodeLite Project...
 bin\premake-bin\premake5.exe codelite --os=linux
 GOTO END
:THREE
 ECHO Creating VS2010 Project...
 bin\premake-bin\premake5.exe vs2010
 GOTO END
:TWO
 ECHO Creating VS2013 Project...
 bin\premake-bin\premake5.exe vs2015 --os=emscripten
 GOTO END
:ONE
 ECHO Creating VS2015 Project...
 bin\premake-bin\premake5.exe vs2015
 GOTO END

:END
