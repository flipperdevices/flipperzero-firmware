## wolfSSL IoT-Safe Example


### Evaluation Platform

  * ST [P-L496G-CELL02](https://www.st.com/en/evaluation-tools/p-l496g-cell02.html)

Including:
    * STM32L496AGI6-based low-power discovery mother board
    * STM Quectel BG96 modem, plugged into the 'STMod+' connector
    * IoT-Safe capable SIM card

Note: The BG96 was tested using firmware `BG96MAR02A08M1G_01.012.01.012`. If having issues with the demo make sure your BG96 firmware is updated.

### Description

This example firmware will run an example TLS 1.2 server using wolfSSL, and a 
TLS 1.2 client, on the same host, using an IoT-safe applet supporting the
[IoT.05-v1-IoT standard](https://www.gsma.com/iot/wp-content/uploads/2019/12/IoT.05-v1-IoT-Security-Applet-Interface-Description.pdf).

The client and server routines alternate their execution in a single-threaded,
cooperative loop.

Client and server communicate to each other using memory buffers to establish a 
TLS session without the use of TCP/IP sockets.

### IoT-Safe interface

In this example, the client is the IoT-safe capable endpoint. First, it creates
a wolfSSL context `cli_ctx` normally:

```c
wolfSSL_CTX_iotsafe_enable(cli_ctx);
```

In order to activate IoT-safe support in this context, the following function is
called:

```c
printf("Client: Enabling IoT Safe in CTX\n");
wolfSSL_CTX_iotsafe_enable(cli_ctx);
```


Additionally, after the SSL session creation, shown below:

```c
printf("Creating new SSL\n");
cli_ssl = wolfSSL_new(cli_ctx);
```

the client associates the pre-provisioned keys and the available slots in the
IoT safe applet to the current session:


```c
wolfSSL_iotsafe_on(cli_ssl, PRIVKEY_ID, ECDH_KEYPAIR_ID, PEER_PUBKEY_ID, PEER_CERT_ID);
```

The applet that has been tested with this demo has the current configuration:

   Key slot | Name | Description 
   -------|--------|------------------
   0x02 | `PRIVKEY_ID` | pre-provisioned with client ECC key
   0x03 | `ECDH_KEYPAIR_ID` | can store a keypair generated in the applet, used for shared key derivation
   0x04 | `PEER_PUBKEY_ID` | used to store the server's public key for key derivation
   0x05 | `PEER_CERT_ID` | used to store the server's public key to authenticate the peer


The following file is used to read the client's certificate:
   
  File Slot | Name | Description
  ----------|------|------------
  0x03 | `CRT_FILE_ID` | pre-provisioned with client certificate


### Compiling and running

From this directory, run 'make', then use your favorite flash programming
software to upload the firmware `image.bin` to the target board.

1) Using the STM32CubeProgrammer open the `image.elf` and program to flash.
2) Using ST-Link virtual serial port connect at 115220
3) Hit reset button. 
4) The output should look similar to below:

```
wolfSSL IoT-SAFE demo
Press a key to continue...
.
Initializing modem...
Modem booting...
Modem is on.
System up and running
Initializing wolfSSL...
Initializing modem port
Turning on VDDIO2
Initializing IoTSafe I/O...
Initializing RNG...
Getting RND...
Random bytes: 08ECF538192218569876EAB9D690306C
Starting memory-tls test...
=== SERVER step 0 ===
Setting TLSv1.3 for SECP256R1 key share
=== CLIENT step 0 ===
Client: Creating new CTX
Client: Enabling IoT Safe in CTX
Loading CA
Loaded Server certificate from IoT-Safe, size = 676
Server certificate successfully imported.
Loaded Client certificate from IoT-Safe, size = 867
Client certificate successfully imported.
Creating new SSL object
Setting TLS options: turn on IoT-safe for this socket
Setting TLSv1.3 for SECP256R1 key share
Connecting to server...
=== Cli->Srv: 162
=== SERVER step 1 ===
=== Srv RX: 5
=== Srv RX: 157
=== Srv-Cli: 128
=== Srv-Cli: 28
=== Srv-Cli: 43
=== Srv-Cli: 712
=== Srv-Cli: 100
=== Srv-Cli: 58
=== CLIENT step 1 ===
Connecting to server...
=== Cli RX: 5
=== Cli RX: 123
=== Cli RX: 5
=== Cli RX: 23
=== Cli RX: 5
=== Cli RX: 38
=== Cli RX: 5
=== Cli RX: 707
=== Cli RX: 5
=== Cli RX: 95
=== Cli RX: 5
=== Cli RX: 53
=== Cli->Srv: 902
=== Cli->Srv: 101
=== Cli->Srv: 58
Client connected!
Sending message: hello iot-safe wolfSSL
=== Cli->Srv: 44
wolfSSL client test success!
=== SERVER step 1 ===
=== Srv RX: 5
=== Srv RX: 897
=== Srv RX: 5
=== Srv RX: 96
=== Srv RX: 5
=== Srv RX: 53
wolfSSL accept success!
=== Srv RX: 5
=== Srv RX: 39
++++++ Server received msg from client: 'hello iot-safe wolfSSL'
IoT-Safe TEST SUCCESSFUL
```

## Support

For questions please email support@wolfssl.com
