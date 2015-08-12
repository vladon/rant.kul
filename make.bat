@ECHO off

if "%1"=="" GOTO blank
if "%1"=="libs" GOTO libs
else GOTO eof

:libs
SET PWD=%CD%
CALL :get
GOTO eof

:get
SET GIT_VER=master
IF NOT EXIST %PWD%\ext\sparsehash\%GIT_VER% (
	git clone http://github.com/dekken/google.sparsehash --branch %GIT_VER% %PWD%/ext/sparsehash/%GIT_VER%
)
IF NOT EXIST %PWD%\ext\pugixml\%GIT_VER% (
	git clone https://github.com/dekken/pugixml --branch %GIT_VER% %PWD%/ext/pugixml/%GIT_VER%
)
GOTO eof

:blank
SET OWD=%PWD%
SET PWD=%CD%\ext\kul\%GIT_VER%

CALL :get

RD /S/Q %PWD%\ext\pugixml\%GIT_VER%\bin
MKDIR %PWD%\ext\pugixml\%GIT_VER%\bin
%VSCC% %CXXFLAGS%  %INCLUDES% /c /Fo"%PWD%\ext\pugixml\%GIT_VER%\bin\pugixml.o" "%PWD%\ext\pugixml\%GIT_VER%\src\pugixml.cpp"
%VSCAR% /OUT:"%PWD%\ext\pugixml\%GIT_VER%\bin\pugixml.lib" /NOLOGO /LTCG %PWD%\ext\pugixml\%GIT_VER%\bin\pugixml.o

RD /S/Q %PWD%\bin
MKDIR %PWD%\bin
FOR /f %%f in ('dir /b %PWD%\os\win\src') do %VSCC% %CXXFLAGS% %INCLUDES% /c /Fo"%PWD%\bin\%%f.o" "%PWD%\os\win\src\%%f" 
FOR /f %%f in ('dir /b %PWD%\src') do %VSCC% %CXXFLAGS% %INCLUDES% /c /Fo"%PWD%\bin\%%f.o" "%PWD%\src\%%f" 
FOR /f %%f in ('dir /b %PWD%\bin') do SET OBJECTS=!OBJECTS! %PWD%\bin\%%f
%VSCAR% /OUT:"%PWD%\bin\kul.lib" /NOLOGO /LTCG %OBJECTS%

SET OBJECTS=
SET PWD=%OWD%
GOTO eof

:errlvl
if %errorlevel% neq 0 exit /b %errorlevel%
GOTO eof

:eof
