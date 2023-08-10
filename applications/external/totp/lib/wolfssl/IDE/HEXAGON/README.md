# Building wolfSSL with DSP Use

## Intro
This directory is to help with building wolfSSL for use with DSP. It assumes that the Hexagon SDK has been setup on the machine and that the environment variables have been set by calling (source ~/Qualcomm/Hexagon_SDK/3.4.3/setup_sdk_env.source). Currently offloading ECC 256 verify operations to the DSP is supported. When WOLFSSL_DSP is defined ECC verify operations are offloaded to the aDSP by default. When not in SINGLE_THREADED mode a call back function must be set for getting the handle or a handle must be set in the ecc_key structure for the operation to make use of multiple threads when offloading to the DSP. This is because creating new handles for new threads must be done.


## Building
The directory is divided up into a build for the CPU portion in IDE/HEXAGON and a build for use on the DSP located in IDE/HEXAGON/DSP. Each section has their own Makefile. The Makefile default to an Ubuntu + hexagon v65 release build but can be changed by using V=<build type>. An example of building both would be:

```
cd IDE/HEXAGON
make V=UbuntuARM_Release_aarch64
cd DSP
make V=hexagon_Release_dynamic_toolv83_v65
```

The results from each build will be placed into the ship directories of each, for example ./UbuntuARM_Release_aarch64/ship/* and ./DSP/hexagon_Release_dynamic_toolv83_v65/ship/*.
The Makefile creates a DSP library libwolfssl_dsp_skel.so, library libwolfssl.so, executable benchmark, example ecc-verify, example ecc-verify-benchmark and executable testwolfcrypt.
These then need pushed to the device in order to run. An example of pushing the results to the device would be:

```
cd IDE/HEXAGON
adb push DSP/hexagon_Release_dynamic_toolv83_v65/ship/libwolfssl_dsp_skel.so /data/rfsa/adsp/
adb push UbuntuARM_Release_aarch64/ship/libwolfssl.so /data/
adb push UbuntuARM_Release_aarch64/ship/benchmark /data/
adb push UbuntuARM_Release_aarch64/ship/eccverify /data/
adb push UbuntuARM_Release_aarch64/ship/eccbenchmark /data/
```

To change the settings wolfSSL is built with macros can be set in IDE/HEXAGON/user_settings.h. It contains a default setting at this point that was used for collecting benchmark values. The macro necessary to turn on use of the DSP is WOLFSSL_DSP.


The script IDE/HEXAGON/build.sh was added to help speed up building and testing. An example of using the script would be:

```
cd IDE/HEXAGON
./build.sh Release
```

This will delete the previous build and rebuild for Release mode. Then it will try to push the resulting library and some of the executables to the device.


For increased performance uncomment the -O3 flag in IDE/HEXAGON/Makefile and IDE/HEXAGON/DSP/Makefile.

## Use

A default handle is created with the call to wolfCrypt_Init() and is set to use the aDSP. A default mutex is locked for each use of the handle to make the library stable when multiple threads are calling to DSP supported operations.
To use wolfSSL with a user created handle it can be done by calling wc_ecc_set_handle or by setting a callback function using wolfSSL_SetHandleCb(). This should be set in the case of multithreaded applications to account for having a handle for each thread being used.


#### wolfSSL_SetHandleCb
The API wolfSSL_SetHandleCb takes a function pointer of type "int (*wolfSSL_DSP_Handle_cb)(remote_handle64 *handle, int finished void *ctx);". This callback is executed right before the operation is handed off to the DSP (finished set to 0) and right after done with the handle (finished set to 1). With ECC this would be after the ECC verify function has been called but before the information is passed on to the DSP and once again with the finished flag set after the result is returned.

The callback set should return 0 on successfully setting the input handle. The ctx argument is for future custom context to be passed in and is currently not used.

#### Expected Performance
This is the expected results from running ./eccbenchmark using the -O3 flag

benchmarking using default (locks on handle for aDSP)
5000 verifies on 1 threads took 17.481616 seconds
10000 verifies on 2 threads took 35.324308 seconds

benchmarking using software (+NEON if built in)
5000 verifies on 1 threads took 1.398336 seconds
10000 verifies on 2 threads took 1.383992 seconds

benchmarking using threads on aDSP
5000 verifies on 1 threads took 17.616811 seconds
10000 verifies on 2 threads took 19.215413 seconds
15000 verifies on 3 threads took 20.410200 seconds
20000 verifies on 4 threads took 23.261446 seconds

benchmarking 1 thread on cDSP
5000 verifies on 1 threads took 18.560995 seconds 

