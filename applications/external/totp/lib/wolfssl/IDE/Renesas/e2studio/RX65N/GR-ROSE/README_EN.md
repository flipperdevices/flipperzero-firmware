wolfSSL sample application project for GR-ROSE evaluation board
======

<br>

## 1. Overview
-----

We provide a sample program for evaluating wolfSSL targeting the GR-ROSE evaluation board, which has RX65N MCU on it. The sample program runs in a bare metal environment that does not use a real-time OS and uses e2 studio as an IDE. This document describes the procedure from build to execution of the sample program. 

The sample provided is a single application that can evaluate the following three functions:

- CryptoTest: A program that automatically tests various cryptographic operation functions
- Benchmark: A program that measures the execution speed of various cryptographic operations 
- TlsClient: A program that performs TLS communication with the opposite TLS server application
<br><br>

Since the H/W settings and S/W settings for operating the evaluation board have already been prepared, the minimum settings are required to operate the sample application. In addition, the RootCA certificate and server-side certificate required for TLS communication have already been set for sample use only. 

The following sections will walk you through the steps leading up to running the sample application.
<br><br>

## 2. Target H/W, components and libraries
-----

This sample program uses the following hardware and software libraries. If a new version of the software component is available at the time of use, please update it as appropriate. 

|item|name & version|
|:--|:--|
|Board|GR-ROSE|
|Device|R5F565NEHxFP|
|IDE| Renesas e2Studio Version:2022-01 |
|Emulator| E1, E2 Emulator Lite |
|Toolchain|CCRX v3.04.00|
|TSIP| TSIP v1.17|
<br>

The project of this sample program has a configuration file that uses the following FIT components.
However, the FIT components themselves are not included in the distribution of this sample application. You need to download it by operating in the IDE. Some FIT components (TSIPs) cannot be downloaded directly from within the IDE and may need to be downloaded separately from the Renesas download site.
<br>

|FIT component|version|name|
|:--|:--|:--|
|Board Support Packages|6.21|r_bsp|
|CMT driver|4.90|r_cmt_rx|
|Ethernet Driver|1.21|r_ether_rx|1.21|r_ether_rx|
|Generic system timer for RX MCUs|1.01|r_sys_time_rx|
|TCP/IP protocol stack[M3S-T4-Tiny] - RX Ethernet Driver Interface|1.09|r_t4_driver_rx|
|TCP/IP protocol stack[M3S-T4-Tiny] for Renesas MCUs|2.10|r_t4_rx|
|TSIP(Trusted Secure IP) driver|1.17.l|r_tsip_rx|
<br>

Note) As of April 2022, TIPS v1.15 does not seem to be able to be added as a FIT component by adding a component in the Smart Configurator Perspective. Add it manually along the method described later. 
<br><br>

## 3. Importing sample application project into e2Studio
----

There is no need to create a new sample program. Since the project file is already prepared, please import the project from the IDE by following the steps below. 

+ e2studio "File" menu> "Open project from file system ..."> "Directory (R) ..." Click the import source button and select the folder from which the project will be imported. Select the folder (Renesas/e2studio/{MCU}/{board-name-folder}) where this README file exists. 
+ Four projects that can be imported are listed, but check only the three projects "smc", "test" and "wolfssl" and click the "Finish" button. 

You should see the **smc**, **test**, and **wolfssl** 3 projects you imported into the project explorer. 
<br><br>

## 4. FIT module download and smart configurator file generation
----

You will need to get the required FIT modules yourself. Follow the steps below to get them.

1. Open the smc project in Project Explorer and double-click the **smc.scfg** file to open the Smart Configurator Perspective. 

2. Select the "Components" tab on the software component settings pane. Then click the "Add Component" button at the top right of the pane. The software component selection dialog will pop up. Click "Download the latest version of FIT driver and middleware" at the bottom of the dialog to get the modules. You can check the download destination folder by pressing "Basic settings...".

3. The latest version of the TSIP component may not be automatically obtained due to the delay in Renesas' support by the method in step 2 above. In that case, you can download it manually from the Renesas website. Unzip the downloaded component and store the files contained in the FIT Modules folder in the download destination folder of step 2.

4. Select the required FIT components shown from the list and press the "Finish" button. Repeat this operation until you have the required FIT components. 

5. Select the Components tab on the Software Component Settings pane and select the r_t4_rx component. In the settings pane on the right, specify the IP address of this board as the value of the "# IP address for ch0, when DHCP disable." Property (e.g. 192.168.1.9).

6. Press the "Generate Code" button at the top right of the software component settings pane to have the smart configurator generate the source files. A src/smc_gen folder will be created under the smc project to store source files, libraries, etc.

<br><br>

## 5. Adding smc_gen files to the test project
----

The test project is a sample application project. When you open the test project in the project explorer, there is a src folder, which contains the source files of the sample application. Add the source files by copying and pasting the smc_gen folder generated in the step5, directly under this src folder.

Make sure you have a folder hierarchy like this:

```
<board-name-folder>/test/src/smc_gen
```
<br>

## 6. Select the behavior of the test application
----

The test project is a single sample application, but you can specify to perform one of the following by switching the settings:

- CryptoTest: A program that automatically tests various encryption operation functions
- Benchmark: A program that measures the execution speed of various cryptographic operations
- TlsClient: A program that performs TLS communication with the opposite TLS server application

Open the <board-name-folder\>/test/src/wolfssl_demo.h file and enable one of the following definitions:

- #define CRYPT_TEST
- #define BENCHMARK
- #define TLS_CLIENT

Then build the test application.
<br><br>


## 7. Build and run the test application
-----

Now that the test application is ready to build. 

1. Build the wolfssl project on the project explorer, then the test project.

2. After a successful build, connect the target board to the emulator and supply external power.

3. Select "Run" menu> "Debug" to open the debug perspective.

4. The test application outputs the operating status to the standard output. Keep the "Renesas Debug Virtual Console" open for viewing this standard output.

5. Press the run button to run the test application.

6. CryptoTest, Benchmark or TLS_Client After displaying the execution result according to the selected behavior, it enters an infinite loop, so if you think that the output has stopped, stop debugging. 
<br><br>

## 8. Running test application as TLS_Client
-----
<br>

### 8.1 TLS version supported by the test application 

<br>
You can use the TLS1.3 protocol in addition to the existing TLS1.2. The following macro is defined to {board-name-folder}/common/user_settings.h.
<br><br>

```
#define WOLFSSL_TLS13
```

This definition sets it to use the TLS 1.3 protocol. If you use TLS1.2, comment out this macro definition and rebuild your test application.

<br>

### 8.2 Type of certificates to use

<br>

The test and server applications allow you to select the type of certificate (RootCA certificate, server certificate, client certificate) to use for TLS communication. You can select either an RSA certificate whose public key included in the certificate is an RSA public key or an ECDSA certificate containing an ECC public key. By default, the ECDSA certificate is used.
<br>

The following macro is defined to {board-name-folder}/common/user_settings.h by default. If you want to use the RSA certificates, comment out the above definition and rebuild the test application.

<br>

```
#define USE_ECC_CERT
```
<br>

### 8.3 Cipher suites

<br>

In the test application, the TLS version and certificate type determine the cipher suites used by the test application. The table below shows the cipher suites that can be used.
<br>

|Tls version |Certificate type|Cipher suites|
|:--|:--|:--|
|TLS1.3|RSA/ECDSA certificate|  |
|||TLS_AES_128_GCM_SHA256|
|||TLS_AES_128_CCM_SHA256| 
|TLS1.2|RSA certificate|
|||TLS_RSA_WITH_AES_128_CBC_SHA|
|||TLS_RSA_WITH_AES_256_CBC_SHA|
|||TLS_RSA_WITH_AES_128_CBC_SHA256|
|||TLS_RSA_WITH_AES_256_CBC_SHA256|
|||TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256|
|||TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256|
||ECDSA certificate||
|||TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256|
|||TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256|


<br>

### 8.4 Build and run a server application

<br>
To operate as TLS_Client, an opposite application for TLS communication is required. A wolfSSL package provides a server sample application that you can use for this purpose. This program is generated by building wolfssl. You can build wolfSSL on Linux (including MacOS and WSL) with gcc installed or build using Visual Studio. The following introduces the build on WSL.
<br><br>

Configuration options need to be changed depending on the certificate type used.

#### 8.4.1 Configuration when using ECDSA certificates 

<br>

```
$ cd <wolfssl-folder>
$ ./autogen.sh
$ ./configure --enable-ecc --enable-dsa --enable-aesccm CFLAGS="-DWOLFSSL_STATIC_RSA -DHAVE_AES_CBC -DHAVE_DSA -DHAVE_ALL_CURVES -DHAVE_ECC -DNO_RSA"
$ make
```
Note: Do not forget to specify "-DNO_RSA"



<br>

With the above build, <wolfssl-folder\>/examples/server/server
Is generated. This executable acts as a TLS server application. If you execute it with the following options, it will be in the listening state for the connection from TLS_Client. 
<br><br>

```
$ examples/server/server -b -d -i
```
<br>

#### 8.4.2 Configuration when using RSA certificates

<br>

```
$ cd <wolfssl-folder>
$ ./autogen.sh
$ ./configure --enable-ecc --enable-dsa --enable-aesccm CFLAGS="-DWOLFSSL_STATIC_RSA -DHAVE_AES_CBC -DHAVE_DSA -DHAVE_ALL_CURVES -DHAVE_ECC"
$ make
```

<br>

#### 8.4.3 Run the server application

<br>

With the above build, {wolfssl-folder}/examples/server/server
is generated. This executable program acts as a server application. If you execute it with the following options, it will be in the standby state for connection from the test application. Specify "-v4" when using TLS1.3 as the TLS version to be used, and specify "-v3" when using TLS1.2.
<br> <br>

```
$ examples / server / server -b -v4 -i
```

<br>
For the test application, specify the IP address assigned to the server application.

Open {board-name-folder}/test/src/wolf_client.c to specify ip address of the server.

<br>

```
...
#define SIMPLE_TLSSEVER_IP  192.168.53.9
...
```
<br>
Save the file and rebuild the test application. When you run the test application, it makes a TLS connection with the server application, then exchanges a simple string and displays the following on the standard output. The cipher suites displayed depends on the combination of TLS version and certificate type.
<br><br>

```
cipher : ECDHE-RSA-AES128-GCM-SHA256
Received: I hear you fa shizzle!

cipher : ECDHE-RSA-AES128-SHA256
Received: I hear you fa shizzle!

cipher : AES128-SHA256
Received: I hear you fa shizzle!

cipher : AES256-SHA256
Received: I hear you fa shizzle!

cipher : ECDHE-RSA-AES128-SHA256
Received: I hear you fa shizzle!

cipher : ECDHE-RSA-AES128-GCM-SHA256
Received: I hear you fa shizzle!
```
<br>



## 9. What you need to do to use the RootCA certificate prepared by the user
----

In this sample program, the RootCA certificate required to operate as TLS_Client and the server certificate used by the sample TLS server application are certificates that can be used only for evaluation.
If you want to use it for purposes beyond functional evaluation, you need to prepare the RootCA certificate yourself. with this,
  1. Provisioning key
  2. RSA key pair required for RootCA certificate validation
  3. The signature generated by the RootCA certificate with the private key in 2 above.

will become necessary. Please refer to the manual provided by Renesas for how to generate them. 

<br>


## 10. Requirements for client authentication
----

wolfSSL supports client authentication as follows:
-	Both ECDSA certificates and RSA certificates are handled by TSIP.


(1)	Loading client certificate

Use wolfSSL_use_certificate_buffer or wolfSSL_CTX_use_certificate_buffer to load client certificate.

(2)	Loading client private key/public key

Type of the client certificate decides the keys to be loaded. 

a)	ECDSA certificate:<br>
	Load private key using tsip_use_PrivateKey_buffer.

b)	RSA certificate:<br>
Load private key using tsip_use_PrivateKey_buffer.
Load public key using tsip_use_PublicKey_buffer.
Note. In case of RSA certificate, the public key will be used for internal verification of signature process.   

(3)	How to generate encrypted keys

The keys (private and public keys) to be loaded should be encrypted-key format. Those keys could be generated with Renesas Secure Flash Programmer or SecurityKeyManagementTool.  Refer the section 7.5 and 7.6 of the application note named “RX Family TSIP Module Firmware Integration technology” how to operate above key wrapping tool.

(4)	Macro to be defined
Define “WOLF_PRIVATE_KEY_ID” in your user_settings.h. 

<br>

## 11. Limitations
----
<br>
wolfSSL, which supports TSIPv1.17, has the following functional restrictions.
<br><br>

1. Handshake message packets exchanged with the server during the TLS handshake are stored in plaintext in memory. This is used to calculate the hash of handshake messages. The content will be deleted at the end of the session.

1. Session resumption and early data using TSIP are not supported.

The above limitations are expected to be improved by TSIP or wolfSSL from the next version onwards.