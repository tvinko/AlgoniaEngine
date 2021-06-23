if exist bin\Release rd /s /q bin\Release
mkdir  bin\Release

copy "%ZENO_ROOT%\libs\pthread\lib\1.0.0.0\pthreadGC2.dll" "%ZENO_ROOT%\ZenEngine\bin\Release"
copy "%ZENO_ROOT%\out\ZenCoreCLR.dll" bin\Release "%ZENO_ROOT%\ZenEngine\bin\Release"
copy "%ZENO_ROOT%\out\ZenCommon.dll" bin\Release "%ZENO_ROOT%\ZenEngine\bin\Release"

copy "%ZENO_ROOT%\libs\Python36\python36.dll" "%ZENO_ROOT%\ZenEngine\bin\Release"
copy "%ZENO_ROOT%\libs\Python36\python36_d.dll" "%ZENO_ROOT%\ZenEngine\bin\Release"
copy "%ZENO_ROOT%\libs\Python37\python37.dll" "%ZENO_ROOT%\ZenEngine\bin\Release"
copy "%ZENO_ROOT%\libs\Python37\python37_d.dll" "%ZENO_ROOT%\ZenEngine\bin\Release"
copy "%ZENO_ROOT%\libs\Python38\python37.dll" "%ZENO_ROOT%\ZenEngine\bin\Release"
copy "%ZENO_ROOT%\libs\Python38\python37_d.dll" "%ZENO_ROOT%\ZenEngine\bin\Release"


if not defined DevEnvDir (
    call vcvarsall.bat x64
)


set includedirs=/I"%ZENO_ROOT%" /I"%ZENO_ROOT%\libs\os_call\src" /I"%ZENO_ROOT%\libs\dirent\src" /I"%ZENO_ROOT%\libs\pthread\src" /I"%ZENO_ROOT%\libs\cJSON\src" /I"%ZENO_ROOT%\libs\ini\src" /I"%ZENO_ROOT%\ZenCommon"
set libdirs=/LIBPATH:"%ZENO_ROOT%\libs\pthread\lib\1.0.0.0" /LIBPATH:"%ZENO_ROOT%\out"
set srcfiles=AlgoniaEngine.c "%ZENO_ROOT%\libs\ini\src\ini.c"
set libs="ZenCommon.lib" "libpthreadGC2.a"

set compilerflags=/Fo"bin/Release/" %includedirs% /GS /W4 /Zc:wchar_t /Gm /O2 /sdl /Zc:inline /fp:precise /D "_CRT_SECURE_NO_WARNINGS" /D "HAVE_STRUCT_TIMESPEC" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /MD /Fp"bin\Release\ZenEngine.pch"


set linkerflags=/OUT:"bin\Release\AlgoniaEngine.dll" %libdirs% /MANIFEST /NXCOMPAT /DYNAMICBASE %libs% "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /DEBUG:NONE /DLL /MACHINE:X64 /INCREMENTAL /SUBSYSTEM:WINDOWS /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ERRORREPORT:PROMPT /NOLOGO /TLBID:1 

cl.exe %compilerflags% %srcfiles% /link %linkerflags%
copy bin\Release\pthreadGC2.dll "%ZENO_ROOT%\out\" /y
copy bin\Release\AlgoniaEngine.dll "%ZENO_ROOT%\out\" /y
