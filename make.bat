@ECHO off

if "%1"=="" GOTO blank
if "%1"=="libs" GOTO libs
else GOTO eof

:libs
SET PWD=%CD%
CALL :get
GOTO eof

:get
SET KUL_VER=master
SET YAML_VER=master
SET HASH_VER=master
IF NOT EXIST %PWD%\ext\sparsehash\%HASH_VER% (
	git clone http://github.com/dekken/google.sparsehash.git --branch %HASH_VER% %PWD%/ext/sparsehash/%HASH_VER%
)
IF NOT EXIST %PWD%\ext\yaml\%YAML_VER% (
	git clone https://github.com/dekken/yaml.cpp.git --branch %YAML_VER% %PWD%/ext/yaml/%YAML_VER%
)
GOTO eof

:blank
SET OWD=%PWD%
SET PWD=%CD%\ext\kul\%KUL_VER%

CALL :get

RD /S/Q %PWD%\ext\yaml\%YAML_VER%\bin
MKDIR %PWD%\ext\yaml\%YAML_VER%\bin
%VSCC% %CXXFLAGS%  %INCLUDES% /c /Fo"%PWD%\ext\yaml\%YAML_VER%\bin\yaml.o" "%PWD%\ext\yaml\%YAML_VER%\src\yaml.cpp"
%VSCAR% /OUT:"%PWD%\ext\yaml\%YAML_VER%\bin\yaml.lib" /NOLOGO /LTCG %PWD%\ext\yaml\%YAML_VER%\bin\yaml.o

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
