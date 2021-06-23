cd "%ZENO_ROOT%\ZenCommon"
call build\build_win64.bat

cd "%ZENO_ROOT%\libs\ZenCoreCLR"
call build\build_win64.bat

cd "%ZENO_ROOT%\ZenEngine"
call build\build_win64.bat

cd "%ZENO_ROOT%\Elements\ZenCsScriptWrapper"
call build\build_win64.bat

cd "%ZENO_ROOT%\Elements\ZenDebugWrapper"
call build\build_win64.bat

cd "%ZENO_ROOT%\Elements\ZenElementsExecuterWrapper"
call build\build_win64.bat

cd "%ZENO_ROOT%\Elements\ZenStart"
call build\build_win64.bat

cd "%ZENO_ROOT%\Elements\ZenStop"
call build\build_win64.bat

cd "%ZENO_ROOT%\Elements\ZenSleep"
call build\build_win64.bat

cd "%ZENO_ROOT%\Elements\ZenNodeJS"
call build\build_win64.bat

cd "%ZENO_ROOT%\Elements\ZenPython"
call build\build_win64_py36.bat
call build\build_win64_py37.bat
call build\build_win64_py38.bat

cd "%ZENO_ROOT%\Elements\ZenSqlServerWrapper"
call build\build_win64.bat

cd "%ZENO_ROOT%\Elements\ZenCounter"
call build\build_win64.bat

cd "%ZENO_ROOT%\Elements\ZenGUI"
call build\build_win64.bat