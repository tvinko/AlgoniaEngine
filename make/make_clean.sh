# for all coreclr libs  cp -R  /usr/local/share/dotnet/shared/Microsoft.NETCore.App/3.1.8/* .

cd zenunmanaged/ZenCommon/ && make clean && \
cd ../ZenEngine/ && make clean && \
cd ../libs/ZenCoreCLR && make clean && \
cd ../../Elements/ZenCsScriptWrapper/ && make clean && \
cd ../ZenDebugWrapper/ && make clean && \
cd ../ZenStart/ && make clean && \
cd ../ZenStop/ && make clean && \
cd ../ZenSleep/ && make clean && \
cd ../ZenCounter/ && make clean && \
cd ../ZenElementsExecuterWrapper/ && make clean && \
cd ../ZenPython/ && make -f makefile_mac_py36 clean && \
cd ../ZenPython/ && make -f makefile_mac_py37 clean && \
cd ../ZenPython/ && make -f makefile_mac_py38 clean && \
cd ../ZenSqlServerWrapper/ && make clean
#cd ../ZenGUI/ && make clean

