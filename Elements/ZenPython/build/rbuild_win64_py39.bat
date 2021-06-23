if exist bin\Release rd /s /q bin\Release
mkdir  bin\Release

if not defined DevEnvDir (
    call vcvarsall.bat x64
)

set elementName=ZenPython
set elementExports=ZENPYTHON_EXPORTS

set includedirs=/I"%ZENO_ROOT%\libs\logging" /I"%ZENO_ROOT%\libs\dirent\src" /I"%ZENO_ROOT%\ZenCommon" /I"%ZENO_ROOT%\libs\ZenCoreCLR" /I"%ZENO_ROOT%\libs\pthread\src" /I"%ZENO_ROOT%\libs\Python39\include"
set libdirs=/LIBPATH:"%ZENO_ROOT%\libs\ZenCommon\lib_msvc\Release" /LIBPATH:"%ZENO_ROOT%\libs\ZenCoreCLR\lib\Release" /LIBPATH:"%ZENO_ROOT%\libs\pthread\lib\1.0.0.0" /LIBPATH:"%ZENO_ROOT%\libs\Python39\libs"
set srcfiles=%elementName%.c

set libs="ZenCommon.lib" "libpthreadGC2.a" "ZenCoreCLR.lib" "python39.lib"

set compilerflags=/Fo"bin\Release/" %includedirs% /GS /W4 /Zc:wchar_t /Gm /O2 /sdl /Zc:inline /fp:precise /D "_CRT_SECURE_NO_WARNINGS" /D "_WINDOWS" /D "_USRDLL" /D "%elementExports%" /D "_WINDLL" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope  /Gd /MD   /Fp"bin\Release\%elementName%.pch" 
set linkerflags=/OUT:"bin\Release\%elementName%.dll"  %libdirs% /MANIFEST /NXCOMPAT /DYNAMICBASE %libs% "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /IMPLIB:"bin\Release\%elementName%.lib" /DEBUG:NONE /DLL /MACHINE:X64 /INCREMENTAL  /SUBSYSTEM:WINDOWS /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"bin\Release\%elementName%.dll.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO  /TLBID:1  

cl.exe %compilerflags% %srcfiles% /link %linkerflags%

copy bin\Release\%elementName%.dll  "%ZENO_ROOT%\out\Implementations\ZenPython39.dll" /y