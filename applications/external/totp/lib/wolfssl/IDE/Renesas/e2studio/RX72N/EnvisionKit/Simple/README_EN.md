wolfSSL simple application project for RX72N EnvisionKit board
======
## 1. Overview
-----

We provide a simple program for evaluating wolfSSL targeting the RX72N evaluation board, which has RX72 MCU on it. The sample program runs in a bare metal environment that does not use a real-time OS and uses e2 studio as an IDE. This document describes the procedure from build to execution of the sample program. 

The sample provided is a single application that can evaluate the following four functions:

- CryptoTest: A program that automatically tests various cryptographic operation functions
- Benchmark: A program that measures the execution speed of various cryptographic operations 
- TlsClient: A program that performs TLS communication with the opposite TLS server application
- TlsServer: A program that performs TLS communication with the opposite TLS client application

Since the H/W settings and S/W settings for operating the evaluation board have already been prepared, the minimum settings are required to operate the sample application. In addition, the RootCA certificate and server-side certificate required for TLS communication have already been set for sample use only. 

The following sections will walk you through the steps leading up to running the sample application.
## 2. Target H/W, components and libraries
-----

This sample program uses the following hardware and software libraries. If a new version of the software component is available at the time of use, please update it as appropriate.

|item|name & version|
|:--|:--|
|Board|RX72N EnvisionKit|
|Device|R5F572NNHxFB|
|IDE| Renesas e2Studio Version:2022-07 |
|Emulator| E2 Emulator Lite |
|Toolchain|CCRX v3.03.00|
|TSIP| TSIP v1.15|


The project of this sample program has a configuration file that uses the following FIT components.
However, the FIT components themselves are not included in the distribution of this sample application. You need to download it by operating in the IDE. Some FIT components (TSIPs) cannot be downloaded directly from within the IDE and may need to be downloaded separately from the Renesas download site.


|FIT component|version|name|
|:--|:--|:--|
|Board Support Packages|7.20|r_bsp|
|CMT driver|5.20|r_cmt_rx|
|Ethernet Driver|1.22|r_ether_rx|
|Generic system timer for RX MCUs|1.01|r_sys_time_rx|
|TCP/IP protocol stack[M3S-T4-Tiny] - RX Ethernet Driver Interface|1.09|r_t4_driver_rx|
|TCP/IP protocol stack[M3S-T4-Tiny] for Renesas MCUs|2.10|r_t4_rx|
|TSIP(Trusted Secure IP) driver|1.15.l|r_tsip_rx|

## 3. Importing sample application project into e2Studio
----

There is no need to create a new sample program. Since the project file is already prepared, please import the project from the IDE by following the steps below. 

+ e2studio "File" menu> "Open project from file system ..."> "Directory (R) ..." Click the import source button and select the folder from which the project will be imported. Select the folder (Renesas/e2studio/{MCU}/{board-name-folder}/Simple) where this README file exists. 
+ Three projects that can be imported are listed, but check only the three projects "test" and "wolfssl" and click the "Finish" button. 

You should see the **test** and **wolfssl** 2 projects you imported into the project explorer. 

## 4. FIT module download and smart configurator file generation
----

You will need to get the required FIT modules yourself. Follow the steps below to get them.

1. Open the test project in Project Explorer and double-click the **test.scfg** file to open the Smart Configurator Perspective. 

2. Select the "Components" tab on the software component settings pane. Then click the "Add Component" button at the top right of the pane. The software component selection dialog will pop up. Click "Download the latest version of FIT driver and middleware" at the bottom of the dialog to get the modules. You can check the download destination folder by pressing "Basic settings...".

3. The latest version of the TSIP component may not be automatically obtained due to the delay in Renesas' support by the method in step 2 above. In that case, you can download it manually from the Renesas website. Unzip the downloaded component and store the files contained in the FIT Modules folder in the download destination folder of step 2.

4. Select the required FIT components shown from the list and press the "Finish" button. Repeat this operation until you have the required FIT components. 

5. Select the Components tab on the Software Component Settings pane and select the r_t4_rx component. In the settings pane on the right, specify the IP address of this board as the value of the "# IP address for ch0, when DHCP disable." Property (e.g. 192.168.1.9).

6. Press the "Generate Code" button at the top right of the software component settings pane to have the smart configurator generate the source files. A src/smc_gen folder will be created under the smc project to store source files, libraries, etc.

## 5. Select the behavior of the test application
----

The test project is a single sample application, but you can specify to perform one of the following by switching the settings:

- CryptoTest: A program that automatically tests various encryption operation functions
- Benchmark: A program that measures the execution speed of various cryptographic operations
- TlsClient: A program that performs TLS communication with the opposite TLS server application
- TlsServer: A program that performs TLS communication with the opposite TLS client application
- 
Open the <board-name-folder\>/test/src/wolfssl_simple_demo.h file and enable one of the following definitions:

- #define CRYPT_TEST
- #define BENCHMARK
- #define SIMPLE_TCP_CLIENT
- #define SIMPLE_TLS_CLIENT
- #define SIMPLE_TLS_TSIP_CLIENT
- #define SIMPLE_TCP_SERVER
- #define SIMPLE_TLS_SERVER

Then build the test application.


## 6. Build and run the test application
-----

Now that the test application is ready to build.

1. Build the wolfssl project on the project explorer, then the test project.

2. After a successful build, connect the target board to the emulator and supply external power.

3. Select "Run" menu> "Debug" to open the debug perspective.

4. The test application outputs the operating status to the standard output. Keep the "Renesas Debug Virtual Console" open for viewing this standard output.

5. Press the run button to run the test application.

6. CryptoTest, Benchmark, TLS_Client or TLS_Server After displaying the execution result according to the selected behavior, it enters an infinite loop, so if you think that the output has stopped, stop debugging. 

## 7. Running test application as Client
-----
### 7.1 Run Client as TCP
When it enables SIMPLE_TCP_CLIENT, we can run TCP clint on the board. We are able to use [this TCP server allication](https://github.com/wolfSSL/wolfssl-examples/blob/master/tls/server-tcp.c) as the opposite TCP server. To compile and run the server,
```
$ gcc server-tcp.c -o server-tcp
$ ./server-tcp
```

If needed, it updates server IP address and port. Those are defined by `SIMPLE_TCPSERVER_IP` and `SIMPLE_TCPSERVER_PORT` in `test/src/simple_tcp.c`
You will see messages on Reneas Virtual Console when its communication succeeds.

```
Received : I hear ya fa shizzle!
```

Also you will see messages on the server console.
```
$ ./server-tcp
Waiting for a connection...
Client connected successfully
Client: Hello Server

Waiting for a connection...
```
### 7.2 Run Client as TLS (Software Only)
When it enables SIMPLE_TLS_CLIENT, we can run TLS client on the board.If needed, it updates server IP address and port. Those are defined by `SIMPLE_TLSSERVER_IP` and `SIMPLE_TLSSERVER_PORT` in `test/src/simple_tls_tsip.c`. After changing it, you need to re-compile test project.


As the opposite TLS server, we can use an example server in wolfSSL source tree. To configure and build it,

```
$ cd <wolfssl-folder>
$ ./autogen.sh
$ ./configure
$ make
```

To run an example TLS server, we need to specify ECC certificate because our test client expctes to use ECC certificate for TLS communication. The setting is done in `user_settings.h` as the following macro definition.
```
#define USE_ECC_CERT
```

Therefore, for an example server, it needs to specify ECC certificate corresponding to client use.
```
./examples/server/server -v 4 -b -i -c ./certs/server-ecc.pem -k ./certs/ecc-key.pem -A ./certs/client-ecc-cert.pem
```

You will see messages on Renesas Debug Virtual Console
```
cipher : TLS13-AES128-GCM-SHA256
Received: I hear you fa shizzle!
```

On an example server side, you will see messages.
```
SSL version is TLSv1.3
SSL cipher suite is TLS_AES_128_GCM_SHA256
SSL curve name is SECP256R1
Client message: Hello Server
```

Note `-v 4` option in server execution command line means to use TLS 1.3. If you want to use smaller version of TLS than 1.3, `-v 4` needs to be removed from an exmeple server command options.

### 7.3 Run Client using TSIP driver
When it enables SIMPLE_TLS_TSIP_CLIENT, we can run TLS client including TSIP driver on the baord. 
In this case, we disable `#define WOLFSSL_TLS13` and `#define USE_ECC_CERT`. As a result, TLS communication between Client and Server use TLS 1.2 and RSA certificate.

The table below shows the cipher suites that TSIP supports.

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


As the opposite TLS server, we can use an example server in wolfSSL. To configure and run the TLS server

```
$ cd <wolfssl-folder>
$ ./autogen.sh
$ ./configure CFLAGS="-DWOLFSSL_STATIC_RSA"
```

The server starts with the following command:

```
$ ./examples/server/server -b -i
```


When you run the test application, it makes a TLS connection with the server application by TLS1.2, then exchanges a simple string and displays the following on the standard output. 
The cipher suites displayed depends on the combination of TLS version and certificate type.

```
cipher : ECDHE-RSA-AES128-GCM-SHA256
Received: I hear you fa shizzle!

cipher : ECDHE-RSA-AES128-SHA256
Received: I hear you fa shizzle!

cipher : AES128-SHA
Received: I hear you fa shizzle!

cipher : AES128-SHA256
Received: I hear you fa shizzle!

cipher : AES256-SHA
Received: I hear you fa shizzle!

cipher : AES256-SHA256
Received: I hear you fa shizzle!

```

When you want to run test project by TLS 1.3, it enables `#define WOLFSSL_TLS13` and then re-compile wolfssl and test project. It needs to add `-v 4` option to the server command above.


## 8. Running test application as Server

### 8.1 Run Server as TCP Server
When it enables `SIMPLE_TCP_SERVER`, we can run TCP server on the board. We are able to use [this TCP client](https://github.com/wolfSSL/wolfssl-examples/blob/master/tls/client-tcp.c) as the opposite TCP client. To compile and run the server,

```
$ gcc client-tcp.c -o client-tcp
```

After lunching TCP server on the board, it waits TCP Client connection. You will see messages if its communication succeeds.
```
[TCP Client terminal]
$ ./client-tcp 192.168.10.33
Message for server: hello
Server: hello

[Renesas Debug Virtual Console]
Received: hello
```
### 8.2 Run Server as TLS Server
When it enables `SIMPLE_TLS_SERVER`, we can run TLS server on the board. We are able to use an example client included in wolfSSL. We can follow steps at [8.2 Run Client as TLS (Software Only)](#8.2-Run-Client-as-TLS-(Software-Only)) to compile the example client.

After re-building and lunching TLS server on the board, it waits TLS Client connection. You will see messages if its communication succeeds.

```
[TLS Client terminal]
$ ./examples/client/client -v 4 -h 192.168.10.33 -p 11111 -A ./certs/ca-ecc-cert.pem
SSL version is TLSv1.3
SSL cipher suite is TLS_AES_128_GCM_SHA256
SSL curve name is SECP256R1
hello wolfssl!


[Renesas Debug Virtual Console]
Received: hello wolfssl!
```
## 9. What you need to do to use the RootCA certificate prepared by the user
----

In this sample program, the RootCA certificate required to operate as TLS_Client and the server certificate used by the sample TLS server application are certificates that can be used only for evaluation.
If you want to use it for purposes beyond functional evaluation, you need to prepare the RootCA certificate yourself. with this,
  1. Provisioning key
  2. RSA key pair required for RootCA certificate validation
  3. The signature generated by the RootCA certificate with the private key in 2 above.

will become necessary. Please refer to the manual provided by Renesas for how to generate them. 

## 10. Limitations
----
wolfSSL, which supports TSIPv1.15, has the following functional restrictions.

1. Handshake message packets exchanged with the server during the TLS handshake are stored in plaintext in memory. This is used to calculate the hash of handshake messages. The content will be deleted at the end of the session.

2. TLS1.2 does not support the client authentication function using TSIP.
Use wolfSSL_CTX_use_certificate_buffer or wolfSSL_CTX_use_certificate_chain_buffer_format to load the client certificate and wolfSSL_CTX_use_PrivateKey_buffer to load the private key. It is processed by software.

1. In TLS 1.3, the client authentication function using TSIP is supported only for ECDSA client certificates. In the case of RSA certificates, it will be processed by software.

1. In TLS1.3, among the server authentication functions using TSIP, "Certificate Verify" message is processed by software.

1. Session resumption and early data using TSIP are not supported.

The above limitations 1 through 4 are expected to be improved by TSIP from the next version onwards.

# 11. Support

For support inquiries and questions, please email support@wolfssl.com. Feel free to reach out to info@wolfssl.jp as well.
