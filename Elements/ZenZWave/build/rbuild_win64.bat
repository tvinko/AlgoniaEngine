call vcvarsall.bat x64

set elementName=ZenDebugWrapper
set elementExports=ZENDEBUGWRAPPER_EXPORTS

set includedirs=/I""%ZENO_ROOT%"\ZenCommon" /I""%ZENO_ROOT%"\libs\ZenCoreCLR" /I""%ZENO_ROOT%"\libs\pthread\src" /I""%ZENO_ROOT%"\libs\zip\src"
set libdirs=/LIBPATH:""%ZENO_ROOT%"\libs\ZenCommon\lib_msvc\Release" /LIBPATH:""%ZENO_ROOT%"\libs\ZenCoreCLR\lib\Release" /LIBPATH:""%ZENO_ROOT%"\libs\pthread\lib\1.0.0.0"
set srcfiles=%elementName%.c "%ZENO_ROOT%"\libs\zip\src\zip.c

set libs="ZenCommon.lib" "ZenCoreCLR.lib" "libpthreadGC2.a"

set compilerflags=/Fo"bin\Release/" %includedirs% /GS /W4 /Zc:wchar_t /Gm /O2 /sdl /Zc:inline /fp:precise /D "_CRT_SECURE_NO_WARNINGS" /D "_WINDOWS" /D "_USRDLL" /D "%elementExports%" /D "_WINDLL" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope  /Gd /MD   /Fp"bin\Release\%elementName%.pch" 
set linkerflags=/OUT:"bin\Release\%elementName%.dll"  %libdirs% /MANIFEST /NXCOMPAT /DYNAMICBASE %libs% "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /IMPLIB:"bin\Release\%elementName%.lib" /DEBUG:NONE /DLL /MACHINE:X64 /INCREMENTAL  /SUBSYSTEM:WINDOWS /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"bin\Release\%elementName%.dll.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO  /TLBID:1  

cl.exe %compilerflags% %srcfiles% /link %linkerflags%