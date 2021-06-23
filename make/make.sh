#############      COMPLETE SYSTEM COMPILE SCRIPT (Engine & Elements)     ########################
#     create /home/zenodys									                                     #
#     copy project dir to /home/zenodys								                             #
#     compile inside docker container (choose image based on OS): 				                 #
#         run sudo docker run -v /home/zenodys:/home/zenodys  -it ubuntu bash			         #
#         run sudo docker run -v /home/zenodys:/home/zenodys  -it sdhibit/rpi-raspbian bash	     #
#         docker run -v /C/Algonia/:/home/algonia  -it ubuntu bash                               #
#         docker run -v /C/Algonia/:/home/algonia  -it tvinko/py:3.7 #-slim-buster bash          #  
#		  docker run -v /C/Algonia/:/home/algonia  -it python:3.7-slim-buster bash  	   		 #
#         docker run -v /C/Algonia/:/home/algonia  -it tvinko/py:37 bash 		 				 # 	 
#		  /usr/local/bin/python3.7-config  --cflags									             #
#												                                                 #
##################################################################################################

# docker build --tag algonia:py36 .

# ./configure   --enable-optimizations  --enable-shared
# export DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=true
# libgdiplus needed for ML object recognition

#docker commit 9a292e867464  python:3.8-slim-buster

#apt-get update -y && apt-get upgrade -y  && apt-get install -y wget curl build-essential libssl-dev zlib1g-dev libncurses5-dev libncursesw5-dev libreadline-dev libsqlite3-dev libgdbm-dev libdb5.3-dev libbz2-dev libexpat1-dev liblzma-dev tk-dev libffi-dev -y 
#apt-get install libgdiplus

#docker run -v /C/Algonia/algoniavs/AlgoniaEngine/templates/:/home/algonia/templates -v /C/Algonia/algoniavs/AlgoniaEngine/libs/:/home/algonia/libs  algonia:py36
#docker run -v /C/tmp/algoniavs/AlgoniaEngine/templates/:/home/algonia/templates -v /C/tmp/algoniavs/AlgoniaEngine/libs/:/home/algonia/libs -e TEMPLATE_ROOT=CompileTest  algonia:py36

###### Permissions to start.sh must be added to docker for macos ###########
# chmod -R 777 /Users/tomaz/Documents/algonia/algoniavs/AlgoniaEngine/templates/test/start.sh
# Inside start.sh : chmod -R 777 /home/algonia/;
##############################################################################

if [ "$1" = "Py36" ]; 
then  
	cd ../Elements/ZenPython/ && make -f makefile_linux_py36;
	mv ZenPython.so ZenPython36.so;	
	cd ../../..;
    cp zenunmanaged/Elements/ZenPython/ZenPython36.so zenunmanaged/make/out/Implementations/;		
elif [ "$1" = "Py37" ]; 
then  
	cd ../Elements/ZenPython/ && make -f makefile_linux_py37;
	mv ZenPython.so ZenPython37.so;	
	cd ../../..;
    cp zenunmanaged/Elements/ZenPython/ZenPython37.so zenunmanaged/make/out/Implementations/;		
elif [ "$1" = "Py38" ]; 
then  
	cd ../Elements/ZenPython/ && make -f makefile_linux_py38;
	mv ZenPython.so ZenPython38.so;	
	cd ../../..;
    cp zenunmanaged/Elements/ZenPython/ZenPython38.so zenunmanaged/make/out/Implementations/;		
else
	cd ../ZenCommon/ && make;
	cd ../ZenEngine/ && make;
	cd ../libs/ZenCoreCLR && make;
	cd ../../Elements/ZenCsScriptWrapper/;
	cd ../ZenDebugWrapper/ && make;
	cd ../ZenStart/ && make;
	cd ../ZenStop/ && make;
	cd ../ZenSleep/ && make;
	cd ../ZenCounter/ && make;
	cd ../ZenElementsExecuterWrapper/;
	cd ../ZenSqlServerWrapper/ && make;
	cd ../ZenCsScriptWrapper/ && make;
	cd ../ZenElementsExecuterWrapper/ && make;
	cd ../ZenRespond/ && make;
	
	cd ../../..;

	[ -d zenunmanaged/make/out ] && rm -r zenunmanaged/make/out; mkdir zenunmanaged/make/out;[ -d zenunmanaged/make/out/Implementations ] && rm -r zenunmanaged/make/out/Implementations; mkdir zenunmanaged/make/out/Implementations;
	
	cp zenunmanaged/libs/ZenCoreCLR/libZenCoreCLR.so /usr/lib;
	cp zenunmanaged/ZenCommon/libZenCommon.so /usr/lib;
	cp zenunmanaged/libs/ZenCoreCLR/libZenCoreCLR.so zenunmanaged/make/out/;
	cp zenunmanaged/ZenCommon/libZenCommon.so zenunmanaged/make/out/;
	
	cp zenunmanaged/Elements/ZenRespond/ZenRespond.so zenunmanaged/make/out/Implementations/;
	cp zenunmanaged/Elements/ZenCsScriptWrapper/ZenCsScriptWrapper.so zenunmanaged/make/out/Implementations/;
	cp zenunmanaged/Elements/ZenDebugWrapper/ZenDebugWrapper.so zenunmanaged/make/out/Implementations/;
	cp zenunmanaged/Elements/ZenStart/ZenStart.so zenunmanaged/make/out/Implementations/;
	cp zenunmanaged/Elements/ZenStop/ZenStop.so zenunmanaged/make/out/Implementations/;
	cp zenunmanaged/Elements/ZenSleep/ZenSleep.so zenunmanaged/make/out/Implementations/;
	cp zenunmanaged/Elements/ZenCounter/ZenCounter.so zenunmanaged/make/out/Implementations/;
	cp zenunmanaged/Elements/ZenElementsExecuterWrapper/ZenElementsExecuterWrapper.so zenunmanaged/make/out/Implementations/;
	cp zenunmanaged/Elements/ZenSqlServerWrapper/ZenSqlServerWrapper.so zenunmanaged/make/out/Implementations/ && \
	cp zenunmanaged/ZenEngine/AlgoniaEngine zenunmanaged/make/out/
fi