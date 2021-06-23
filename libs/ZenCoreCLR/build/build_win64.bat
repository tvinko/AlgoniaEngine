if exist bin\Debug rd /s /q bin\Debug
mkdir  bin\Debug

if not defined DevEnvDir (
    call vcvarsall.bat x64
)

set includedirs=/I"%ZENO_ROOT%\ZenCommon" /I"%ZENO_ROOT%\libs\logging" /I"%ZENO_ROOT%\libs\os_call\src" /I"%ZENO_ROOT%\libs\pthread\src"
set libdirs=/LIBPATH:"%ZENO_ROOT%\libs\ZenCommon\lib_msvc\1.0.0.0" /LIBPATH:"%ZENO_ROOT%\libs\pthread\lib\1.0.0.0"
set srcfiles=ZenCoreCLR.cpp
set libs="ZenCommon.lib" "libpthreadGC2.a"

set compilerflags=/Fo"bin\Debug/" %includedirs% /GS /W3 /Zc:wchar_t  /ZI /Gm /Od /sdl /Fd"bin\Debug\vc141.pdb" /Zc:inline /fp:precise /D "_CRT_SECURE_NO_WARNINGS" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "ZENCORECLR_EXPORTS" /D "_WINDLL" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /RTC1 /Gd /MDd   /Fp"bin\Debug\ZenCoreCLR.pch" 
set linkerflags=/OUT:"bin\Debug\ZenCoreCLR.dll"  %libdirs% /MANIFEST /NXCOMPAT /PDB:"bin\Debug\ZenCoreCLR.pdb" /DYNAMICBASE %libs% "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /IMPLIB:"bin\Debug\ZenCoreCLR.lib" /DEBUG /DLL /MACHINE:X64 /INCREMENTAL  /SUBSYSTEM:WINDOWS /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"bin\Debug\ZenCoreCLR.dll.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO  /TLBID:1  

cl.exe %compilerflags% %srcfiles% /link %linkerflags%

copy bin\\Debug\\ZenCoreCLR.lib  "%ZENO_ROOT%\libs\ZenCoreCLR\lib\1.0.0.0"
copy bin\\Debug\\ZenCoreCLR.dll  "%ZENO_ROOT%\libs\ZenCoreCLR\lib\1.0.0.0"