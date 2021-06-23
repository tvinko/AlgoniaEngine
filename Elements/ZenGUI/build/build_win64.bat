if exist bin\Debug rd /s /q bin\Debug
mkdir  bin\Debug

if not defined DevEnvDir (
    call vcvarsall.bat x64
)

cd algonia-webview
cargo build
cd ..
set elementName=ZenGUI
set elementExports=ZENGUI_EXPORTS

set includedirs=/I"%ZENO_ROOT%\libs\logging" /I"%ZENO_ROOT%\ZenCommon" /I"%ZENO_ROOT%\libs\ZenCoreCLR" /I"%ZENO_ROOT%\libs\pthread\src" /I"%ZENO_ROOT%\libs\cJSON\src"
set libdirs=/LIBPATH:"algonia-webview\target\debug" /LIBPATH:"%ZENO_ROOT%\libs\ZenCommon\lib_msvc\1.0.0.0" /LIBPATH:"%ZENO_ROOT%\libs\ZenCoreCLR\lib\1.0.0.0" /LIBPATH:"%ZENO_ROOT%\libs\pthread\lib\1.0.0.0"
set srcfiles=%elementName%.c

set libs="web_view_core.dll.lib" "ZenCommon.lib" "ZenCoreCLR.lib" "libpthreadGC2.a"

set compilerflags=/Fo"bin\Debug/" %includedirs% /GS /W3 /Zc:wchar_t  /ZI /Gm /Od /sdl /Fd"bin\Debug\vc141.pdb" /Zc:inline /fp:precise /D "_MBCS" /D "DEVICE=DESKTOP" /D "NOMINMAX=1" /D "_CRT_SECURE_NO_WARNINGS" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "%elementExports%" /D "_WINDLL" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /RTC1 /Gd /MDd   /Fp"bin\Debug\ZenCsScriptWrapper.pch"  /EHsc
set linkerflags=/OUT:"bin\Debug\%elementName%.dll"  %libdirs% /MANIFEST /NXCOMPAT /PDB:"bin\Debug\%elementName%.pdb" /DYNAMICBASE %libs% "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /IMPLIB:"bin\Debug\%elementName%.lib" /DEBUG /DLL /MACHINE:X64 /INCREMENTAL  /SUBSYSTEM:WINDOWS /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"bin\Debug\%elementName%.dll.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO  /TLBID:1  

cl.exe %compilerflags% %srcfiles% /link %linkerflags%
copy bin\\Debug\\%elementName%.dll  "%ZENO_PROJ%/Implementations/"
copy algonia-webview\target\debug\web_view_core.dll  "%ZENO_ROOT%/ZenEngine/"