if exist "%ZENO_ROOT%\out" rmdir "%ZENO_ROOT%\out" /S /Q
mkdir "%ZENO_ROOT%\out"

mkdir "%ZENO_ROOT%\out\Implementations"

cd "%ZENO_ROOT%\ZenCommon"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\libs\ZenCoreCLR"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\ZenEngine"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\ZenEngine"
call build\lib_rbuild_win64.bat

cd "%ZENO_ROOT%\Elements\ZenCsScriptWrapper"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\Elements\ZenDebugWrapper"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\Elements\ZenElementsExecuterWrapper"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\Elements\ZenStart"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\Elements\ZenStop"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\Elements\ZenSleep"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\Elements\ZenPython"
call build\rbuild_win64_py36.bat
call build\rbuild_win64_py37.bat
call build\rbuild_win64_py38.bat


cd "%ZENO_ROOT%\Elements\ZenSqlServerWrapper"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\Elements\ZenCounter"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\Elements\ZenGUI"
call build\rbuild_win64.bat

cd "%ZENO_ROOT%\Elements\ZenRespond"
call build\rbuild_win64.bat