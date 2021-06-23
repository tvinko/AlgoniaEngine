if exist bin\Debug rd /s /q bin\Debug
mkdir  bin\Debug

copy "%ZENO_ROOT%\libs\pthread\lib\1.0.0.0\pthreadGC2.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\ZenCoreCLR\lib\1.0.0.0\ZenCoreCLR.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\ZenCommon\lib_msvc\1.0.0.0\ZenCommon.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\Python36\python36.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\Python36\python36_d.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\Python37\python37.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\Python37\python37_d.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\Python38\python37.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\Python38\python37_d.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\czmq\x64-windows\dlls" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\zmq\x64-windows\dlls" "%ZENO_ROOT%\ZenEngine\bin\Debug"
rem robocopy "%ZENO_ROOT%\libs\ultralight\SDK\bin" "%ZENO_ROOT%\ZenEngine\bin\Debug" /E
rem copy "%ZENO_ROOT%\libs\sciter\bin\sciter.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"

if not defined DevEnvDir (
    call vcvarsall.bat x64
)

set includedirs=/I"%ZENO_ROOT%\libs\logging"  /I"%ZENO_ROOT%" /I"%ZENO_ROOT%\libs\os_call\src" /I"%ZENO_ROOT%\libs\dirent\src" /I"%ZENO_ROOT%\libs\pthread\src" /I"%ZENO_ROOT%\libs\cJSON\src" /I"%ZENO_ROOT%\libs\ini\src" /I"%ZENO_ROOT%\ZenCommon"
set libdirs=/LIBPATH:"%ZENO_ROOT%\libs\pthread\lib\1.0.0.0" /LIBPATH:"%ZENO_ROOT%\libs\ZenCommon\lib_msvc\1.0.0.0"
set srcfiles=AlgoniaEngine.c "%ZENO_ROOT%\libs\ini\src\ini.c"
set libs="ZenCommon.lib" "libpthreadGC2.a"

set compilerflags=/Fo"bin/Debug/" %includedirs% /GS /W3 /Zc:wchar_t /ZI /Gm /Od /sdl /Fd"bin\Debug\vc141.pdb" /Zc:inline /fp:precise /D "_CRT_SECURE_NO_WARNINGS" /D "HAVE_STRUCT_TIMESPEC" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /MDd /Fp"bin\Debug\ZenEngine.pch"
set linkerflags= /OUT:"bin\Debug\AlgoniaEngine.exe" /MANIFEST /NXCOMPAT /PDB:"bin\Debug\ZenEngine.pdb" /DYNAMICBASE %libs% "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" %libdirs% /MACHINE:X64 /INCREMENTAL /SUBSYSTEM:CONSOLE /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"bin\Debug\AlgoniaEngine.exe.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO /TLBID:1 

cl.exe %compilerflags% %srcfiles% /link %linkerflags%