if exist bin\Debug rd /s /q bin\Debug
mkdir  bin\Debug

copy "%ZENO_ROOT%\libs\pthread\lib\1.0.0.0\pthreadGC2.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\ZenCoreCLR\lib\1.0.0.0\ZenCoreCLR.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"
copy "%ZENO_ROOT%\libs\ZenCommon\lib_msvc\1.0.0.0\ZenCommon.dll" "%ZENO_ROOT%\ZenEngine\bin\Debug"


call vcvarsall.bat x64

set includedirs=/I"%ZENO_ROOT%" /I"%ZENO_ROOT%\libs\os_call\src" /I"%ZENO_ROOT%\libs\dirent\src" /I"%ZENO_ROOT%\libs\pthread\src" /I"%ZENO_ROOT%\libs\cJSON\src" /I"%ZENO_ROOT%\libs\ini\src" /I"%ZENO_ROOT%\ZenCommon"
set libdirs=/LIBPATH:"%ZENO_ROOT%\libs\pthread\lib\1.0.0.0" /LIBPATH:"%ZENO_ROOT%\libs\ZenCommon\lib_msvc\1.0.0.0"
set srcfiles=AlgoniaEngine.c "%ZENO_ROOT%\libs\ini\src\ini.c"
set libs="ZenCommon.lib" "libpthreadGC2.a"

set compilerflags=/Fo"bin/Debug/" %includedirs% /GS /W3 /Zc:wchar_t /ZI /Gm /Od /sdl /Fd"bin\Debug\vc141.pdb" /Zc:inline /fp:precise /D "_CRT_SECURE_NO_WARNINGS" /D "HAVE_STRUCT_TIMESPEC" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /MDd /Fp"bin\Debug\ZenEngine.pch"
set linkerflags= /OUT:"bin\Debug\AlgoniaEngine.dll" /MANIFEST /NXCOMPAT /PDB:"bin\Debug\AlgoniaEngine.pdb" /DYNAMICBASE %libs% "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" %libdirs% /DEBUG /DLL /MACHINE:X64 /INCREMENTAL  /SUBSYSTEM:WINDOWS /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"bin\Debug\ZenEngine.dll.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO /TLBID:1 

cl.exe %compilerflags% %srcfiles% /link %linkerflags%

copy bin\Debug\AlgoniaEngine.dll "%ZENO_ROOT%\libs\ZenEngine\" /y
copy bin\Debug\AlgoniaEngine.lib "%ZENO_ROOT%\libs\ZenEngine\" /y
