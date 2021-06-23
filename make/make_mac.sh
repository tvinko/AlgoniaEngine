# for all coreclr libs  cp -R  /usr/local/share/dotnet/shared/Microsoft.NETCore.App/3.1.8/* .

cd zenunmanaged/ZenCommon/ && make && \
cd ../ZenEngine/ && make && \
cd ../libs/ZenCoreCLR && make && \
cd ../../Elements/ZenCsScriptWrapper/ && make && \
cd ../ZenDebugWrapper/ && make && \
cd ../ZenStart/ && make && \
cd ../ZenStop/ && make && \
cd ../ZenSleep/ && make && \
cd ../ZenCounter/ && make && \
cd ../ZenElementsExecuterWrapper/ && make && \
cd ../ZenRespond/ && make && \
cd ../ZenPython/ && make -f makefile_mac_py36 && \
mv ZenPython.so ZenPython36.so && \
cd ../ZenPython/ && make -f makefile_mac_py37 && \
mv ZenPython.so ZenPython37.so && \
cd ../ZenPython/ && make -f makefile_mac_py38 && \
mv ZenPython.so ZenPython38.so && \ 
cd ../ZenSqlServerWrapper/ && make && \

#cd ../ZenGUI/algonia-webview && cargo build  && \
#cd ../../ZenGUI/ && make && \

cd ../../.. && \
cp zenunmanaged/libs/ZenCoreCLR/libZenCoreCLR.so /usr/local/lib && \
cp zenunmanaged/libs/ultralight/macos/bin/libAppCore.dylib /usr/local/lib && \
cp zenunmanaged/libs/ultralight/macos/bin/libUltralight.dylib /usr/local/lib && \
cp zenunmanaged/libs/ultralight/macos/bin/libUltralightCore.dylib /usr/local/lib && \
cp zenunmanaged/libs/ultralight/macos/bin/libWebCore.dylib /usr/local/lib && \


cp zenunmanaged/ZenCommon/libZenCommon.so /usr/local/lib && \
cp zenunmanaged/libs/ZenCoreCLR/libZenCoreCLR.so . && \
cp zenunmanaged/ZenCommon/libZenCommon.so . && \
#cp zenunmanaged/Elements/ZenGUI/algonia-webview/target/debug/libweb_view_core.dylib . && \

mkdir out && \
cp zenunmanaged/Elements/ZenCsScriptWrapper/ZenCsScriptWrapper.so out/ && \
cp zenunmanaged/Elements/ZenDebugWrapper/ZenDebugWrapper.so out/ && \
cp zenunmanaged/Elements/ZenStart/ZenStart.so out/ && \
cp zenunmanaged/Elements/ZenStop/ZenStop.so out/ && \
#cp zenunmanaged/Elements/ZenGUI/ZenGUI.so out/ && \
cp zenunmanaged/Elements/ZenSleep/ZenSleep.so out/ && \
cp zenunmanaged/Elements/ZenRespond/ZenRespond.so out/ && \
cp zenunmanaged/Elements/ZenCounter/ZenCounter.so out/ && \
cp zenunmanaged/Elements/ZenElementsExecuterWrapper/ZenElementsExecuterWrapper.so out/ && \
cp zenunmanaged/Elements/ZenPython/ZenPython36.so out/ && \
cp zenunmanaged/Elements/ZenPython/ZenPython37.so out/ && \
cp zenunmanaged/Elements/ZenPython/ZenPython38.so out/ && \
cp zenunmanaged/Elements/ZenSqlServerWrapper/ZenSqlServerWrapper.so out/ && \
cp zenunmanaged/ZenEngine/AlgoniaEngine .