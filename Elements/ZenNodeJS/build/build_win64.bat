rem build instructions : https://github.com/nodejs/node/blob/master/BUILDING.md
rem install Boxstarter for automatic build:
rem Set-ExecutionPolicy Unrestricted -Force
rem iex ((New-Object System.Net.WebClient).DownloadString('https://boxstarter.org/bootstrapper.ps1'))
rem get-boxstarter -Force
rem Install-BoxstarterPackage https://raw.githubusercontent.com/nodejs/node/master/tools/bootstrap/windows_boxstarter -DisableReboots
rem ---------------------------------------------------------------------------
rem goto https://nodejs.org/en/download/ and download source code
rem copy it to the ZenNodeJS/node/node_version folder
rem run .\vcbuild

if exist bin\Debug rd /s /q bin\Debug
mkdir  bin\Debug

set node_version=node/node-v14.15.5
set elementName=ZenNodeJS
set elementExports=ZENPYTHON_EXPORTS
set srcfiles=%elementName%.cpp

if not defined DevEnvDir (
    rem call %node_version%/vcbuild.bat x64
    call vcvarsall.bat x64
)

rem set compilerflags=/Yu"node_pch.h" /MP /ifcOutput "out\Debug\obj\ZenNodeJS\" /GS /W3 /wd"4351" /wd"4355" /wd"4800" /wd"4251" /wd"4275" /wd"4244" /wd"4267" /Zc:wchar_t /I"%node_version%/src" /I"%node_version%/tools\msvs\genfiles" /I"%node_version%/deps\v8\include" /I"%node_version%/deps\cares\include" /I"%node_version%/deps\uv\include" /I"%node_version%/deps\uvwasi\include" /I"." /I"%node_version%/deps\histogram\src" /I"%node_version%/deps\icu-small\source\i18n" /I"%node_version%/deps\icu-small\source\common" /I"%node_version%/deps\zlib" /I"%node_version%/deps\llhttp\include" /I"%node_version%/deps\nghttp2\lib\includes" /I"%node_version%/deps\brotli\c\include" /I"%node_version%/deps\openssl\openssl\include" /Z7 /Gm- /Od /Fd"out\Debug\obj\ZenNodeJS\vc142.pdb" /FI"node_pch.h" /Zc:inline /fp:precise /D "V8_DEPRECATION_WARNINGS" /D "V8_IMMINENT_DEPRECATION_WARNINGS" /D "_GLIBCXX_USE_CXX11_ABI=1" /D "WIN32" /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "_HAS_EXCEPTIONS=0" /D "BUILDING_V8_SHARED=1" /D "BUILDING_UV_SHARED=1" /D "OPENSSL_NO_PINSHARED" /D "OPENSSL_THREADS" /D "FD_SETSIZE=1024" /D "NODE_PLATFORM=\"win32\"" /D "NOMINMAX" /D "_UNICODE=1" /D "NODE_USE_V8_PLATFORM=1" /D "NODE_HAVE_I18N_SUPPORT=1" /D "HAVE_OPENSSL=1" /D "UCONFIG_NO_SERVICE=1" /D "U_ENABLE_DYLOAD=0" /D "U_STATIC_IMPLEMENTATION=1" /D "U_HAVE_STD_STRING=1" /D "UCONFIG_NO_BREAK_ITERATION=0" /D "NGHTTP2_STATICLIB" /D "DEBUG" /D "_DEBUG" /D "V8_ENABLE_CHECKS" /errorReport:prompt /GF /WX- /Zc:forScope /RTC1 /Gd /Oy- /MTd /FC /Fa"out\Debug\obj\ZenNodeJS\" /nologo /Fo"out\Debug\obj\ZenNodeJS\" /Fp"out\Debug\obj\ZenNodeJS\ZenNodeJS.pch" /diagnostics:column
rem set linkerflags=/OUT:"out\Debug\ZenNodeJS.exe" /MANIFEST /NXCOMPAT /PDB:"out\Debug\ZenNodeJS.pdb" /DYNAMICBASE "Dbghelp.lib" "winmm.lib" "Ws2_32.lib" "iphlpapi.lib" "psapi.lib" "shell32.lib" "userenv.lib" "AdvAPI32.lib" "User32.lib" "ws2_32.lib" "gdi32.lib" "advapi32.lib" "crypt32.lib" "user32.lib" /DEF:"out\Debug\obj\global_intermediate\openssl.def" /DEBUG /MACHINE:X64 /SAFESEH:NO /INCREMENTAL /PGD:"out\Debug\ZenNodeJS.pgd" /SUBSYSTEM:CONSOLE /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"out\Debug\obj\ZenNodeJS\ZenNodeJS.exe.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO /TLBID:1 

rem cl.exe %compilerflags% %srcfiles% /link %linkerflags%


set includedirs=/I"%ZENO_ROOT%" /I"%ZENO_ROOT%\Elements\ZenNodeJS\node\node-v14.15.5\src\" /I"%ZENO_ROOT%\Elements\ZenNodeJS\node\node-v14.15.5\deps\uv\include"
set libdirs=/LIBPATH:"%ZENO_ROOT%\libs\pthread\lib\1.0.0.0" /LIBPATH:"%ZENO_ROOT%\libs\ZenCommon\lib_msvc\1.0.0.0"
set srcfiles=ZenNodeJS.cpp
set libs=""

set compilerflags=/Fo"bin/Debug/" %includedirs% /GS /W3 /Zc:wchar_t /ZI /Gm /Od /sdl /Fd"bin\Debug\vc141.pdb" /Zc:inline /fp:precise /D "_CRT_SECURE_NO_WARNINGS" /D "HAVE_STRUCT_TIMESPEC" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /MDd /Fp"bin\Debug\ZenNodeJS.pch"
set linkerflags= /OUT:"bin\Debug\ZenNodeJS.exe" /MANIFEST /NXCOMPAT /PDB:"bin\Debug\ZenNodeJS.pdb" /DYNAMICBASE %libs% "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" %libdirs% /MACHINE:X64 /INCREMENTAL /SUBSYSTEM:CONSOLE /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"bin\Debug\ZenNodeJS.exe.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO /TLBID:1 

rem cl.exe %compilerflags% %srcfiles% /link %linkerflags%
cl.exe %compilerflags% %srcfiles%


rem copy bin\\Debug\\%elementName%.dll  "%ZENO_ROOT%/ZenEngine/Implementations/ZenNodeJS.dll"