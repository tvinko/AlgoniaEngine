if exist bin\Release rd /s /q bin\Release
mkdir  bin\Release

if not defined DevEnvDir (
    call vcvarsall.bat x64
)

set includedirs=/I"%ZENO_ROOT%" /I"%ZENO_ROOT%\libs\logging" /I"%ZENO_ROOT%\libs\dirent\src" /I"%ZENO_ROOT%\libs\pthread\src"  /I"%ZENO_ROOT%\libs\cJSON\src" /I"%ZENO_ROOT%\libs\b64\src"
set libdirs=/LIBPATH:"%ZENO_ROOT%\libs\pthread\lib\1.0.0.0" 
set srcfiles=ZenCommon.c "%ZENO_ROOT%\libs\logging\log.c" "%ZENO_ROOT%\libs\cJSON\src\cJSON.c" "%ZENO_ROOT%\libs\b64\src\decode.c" "%ZENO_ROOT%\libs\b64\src\encode.c" 
set libs="libpthreadGC2.a"

set compilerflags=/Fo"bin/Release/" %includedirs% /GS /W4 /Zc:wchar_t /Gm /O2 /sdl /Zc:inline /fp:precise /D "_CRT_SECURE_NO_WARNINGS" /D "_WINDOWS" /D "_USRDLL" /D "ZENCOMMON_EXPORTS" /D "_WINDLL" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /Gd /MD /Fp"bin\Release\ZenCommon.pch" 
set linkerflags=/OUT:"bin\Release\ZenCommon.dll" %libdirs% /MANIFEST /NXCOMPAT /DYNAMICBASE %libs% "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /IMPLIB:"bin\Release\ZenCommon.lib" /DEBUG:NONE /DLL /MACHINE:X64 /INCREMENTAL /SUBSYSTEM:WINDOWS /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"bin\Release\ZenCommon.dll.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO  /TLBID:1  

cl.exe %compilerflags% %srcfiles%  /link %linkerflags%

copy bin\Release\ZenCommon.dll "%ZENO_ROOT%\libs\ZenCommon\lib_msvc\Release\" /y
copy bin\Release\ZenCommon.lib "%ZENO_ROOT%\libs\ZenCommon\lib_msvc\Release\" /y
copy bin\Release\ZenCommon.dll "%ZENO_ROOT%\out\" /y
copy bin\Release\ZenCommon.lib "%ZENO_ROOT%\out\" /y