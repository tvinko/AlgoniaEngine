if exist bin\Release rd /s /q bin\Release
mkdir  bin\Release

if not defined DevEnvDir (
    call vcvarsall.bat x64
)

set includedirs=/I"%ZENO_ROOT%\ZenCommon" /I"%ZENO_ROOT%\libs\logging" /I"%ZENO_ROOT%\libs\os_call\src" /I"%ZENO_ROOT%\libs\pthread\src"
set libdirs=/LIBPATH:"%ZENO_ROOT%\libs\ZenCommon\lib_msvc\1.0.0.0" /LIBPATH:"%ZENO_ROOT%\libs\pthread\lib\1.0.0.0"
set srcfiles=ZenCoreCLR.cpp
set libs="ZenCommon.lib" "libpthreadGC2.a"

set compilerflags=/Fo"bin/Release/" %includedirs% /GS /W4 /Zc:wchar_t /Gm /O2 /sdl /Zc:inline /fp:precise /D "_CRT_SECURE_NO_WARNINGS" /D "_WINDOWS" /D "_USRDLL" /D "ZENCORECLR_EXPORTS" /D "_WINDLL" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /Gd /MD /Fp"bin\Release\ZenCoreCLR.pch" 
set linkerflags=/OUT:"bin\Release\ZenCoreCLR.dll" %libdirs% /MANIFEST /NXCOMPAT /DYNAMICBASE %libs% "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /IMPLIB:"bin\Release\ZenCoreCLR.lib" /DEBUG:NONE /DLL /MACHINE:X64 /INCREMENTAL /SUBSYSTEM:WINDOWS /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"bin\Release\ZenCoreCLR.dll.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO  /TLBID:1  

cl.exe %compilerflags% %srcfiles% /link %linkerflags%

copy bin\Release\ZenCoreCLR.lib "%ZENO_ROOT%\libs\ZenCoreCLR\lib\Release\" /y
copy bin\Release\ZenCoreCLR.dll "%ZENO_ROOT%\libs\ZenCoreCLR\lib\Release\" /y
copy bin\Release\ZenCoreCLR.dll "%ZENO_ROOT%\out\" /y