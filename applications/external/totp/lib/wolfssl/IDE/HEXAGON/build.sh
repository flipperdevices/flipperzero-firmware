#!/bin/bash
if [ -z $1 ]; then
	echo "./build <Debug | Release>"
	exit 1
fi

printf "Erasing previous hexagon_$1_dynamic_toolv83_v65 and UbuntuARM_$1_aarch64\n"
rm -rf hexagon_$1_dynamic_toolv83_v65 &> /dev/null
rm -rf UbuntuARM_$1_aarch64 &> /dev/null
cd ./DSP &> /dev/null
rm -rf hexagon_$1_dynamic_toolv83_v65 &> /dev/null

printf "Building hexagon_$1_dynamic_toolv83_v65 ..."
make V=hexagon_$1_dynamic_toolv83_v65 &> /dev/null
if [ $? != 0 ]; then
     printf "failed\n"
else
     printf "done\n"
fi

cd ../ &> /dev/null
printf "Building UbuntuARM_$1_aarch64 ..."
make V=UbuntuARM_$1_aarch64 &> /dev/null
if [ $? != 0 ]; then
     printf "failed\n"
else
     printf "done\n"
fi

printf "Trying to push libwolfssl_Dsp_skel.so, libwolfssl.so, benchmark, eccverify, eccbenchmark\n"
adb push DSP/hexagon_$1_dynamic_toolv83_v65/ship/libwolfssl_dsp_skel.so /data/rfsa/adsp/
adb push UbuntuARM_$1_aarch64/ship/libwolfssl.so /data/
adb push UbuntuARM_$1_aarch64/ship/benchmark /data/
adb push UbuntuARM_$1_aarch64/ship/eccverify /data/
adb push UbuntuARM_$1_aarch64/ship/eccbenchmark /data/
#adb push UbuntuARM_$1_aarch64/ship/testwolfcrypt /data/wolfcrypt/test/
printf "done\n"
exit 0
