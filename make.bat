@ECHO off

if "%1"=="" GOTO blank
if "%1"=="libs" GOTO libs
else GOTO eof

:libs
SET PWD=%CD%
CALL :get
GOTO eof

:get
IF NOT EXIST %PWD%\ext\sparsehash\master (
	git clone http://github.com/dekken/sparsehash ext/sparsehash/master
)
IF NOT EXIST %PWD%\ext\pugixml\master (
	git clone https://github.com/dekken/pugixml ext/pugixml/master
)
GOTO eof

:blank
SET OWD=%PWD%
SET PWD=%CD%\ext\kul\%GIT_VER%

CALL :get

RD /S/Q %PWD%\ext\pugixml\master\bin
MKDIR %PWD%\ext\pugixml\master\bin
%VSCC% %CXXFLAGS%  %INCLUDES% /c /Fo"%PWD%\ext\pugixml\master\bin\pugixml.o" "%PWD%\ext\pugixml\master\src\pugixml.cpp"
%VSCAR% /OUT:"%PWD%\ext\pugixml\master\bin\pugixml.lib" /NOLOGO /LTCG %PWD%\ext\pugixml\master\bin\pugixml.o

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
