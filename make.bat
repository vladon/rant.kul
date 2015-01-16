


if "%1"=="" GOTO blank
if "%1"=="init" GOTO init
else GOTO eof

:init
SET PWD=%CD%
CALL :get
GOTO eof

:get
IF NOT EXIST %PWD%\ext\sparsehash\trunk (
	svn co http://sparsehash.googlecode.com/svn/trunk %PWD%\ext\sparsehash\trunk
)
IF NOT EXIST %PWD%\ext\pugixml\trunk (
	svn co http://pugixml.googlecode.com/svn/tags/latest %PWD%\ext\pugixml\trunk
)
GOTO eof

:blank
SET OWD=%PWD%
SET PWD=%CD%\ext\kul\master

CALL :get

IF NOT EXIST %PWD%\ext\pugixml\trunk\bin mkdir %PWD%\ext\pugixml\trunk\bin
%VSCC% %CXXFLAGS%  %INCLUDES% /c /Fo"%PWD%\ext\pugixml\trunk\bin\pugixml.o" "%PWD%\ext\pugixml\trunk\src\pugixml.cpp"
%VSCAR% /OUT:"%PWD%\ext\pugixml\trunk\bin\pugixml.lib" /NOLOGO /LTCG %PWD%\ext\pugixml\trunk\bin\pugixml.o

IF NOT EXIST %PWD%\bin mkdir %PWD%\bin
FOR /f %%f in ('dir /b %PWD%\src') do %VSCC% %CXXFLAGS% %INCLUDES% /c /Fo"%PWD%\bin\%%f.o" "%PWD%\src\%%f" 

FOR /f %%f in ('dir /b %PWD%\bin') do SET OBJECTS=!OBJECTS! %PWD%\bin\%%f

%VSCAR% /OUT:"%PWD%\bin\kul.lib" /NOLOGO /LTCG %OBJECTS%

SET OBJECTS=
SET PWD=%OWD%
GOTO eof

:eof
