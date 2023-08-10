## wolfSSL IoT-SAFE Example


### Description

This demo example will run an example TLS 1.3 client using wolfSSL, using an IoT-SAFE applet supporting the [IoT.05-v1-IoT standard](https://www.gsma.com/iot/wp-content/uploads/2019/12/IoT.05-v1-IoT-Security-Applet-Interface-Description.pdf).


### Evaluation Platform

* Any platform with POSIX-compliant OS such as RaspberryPi
* Any ICC form-factor with IoT-SAFE applet (see below section for the specific IoT-SAFE configuration)
* Any serial modem for interfacing between the platform and IoT-SAFE


### TLS 1.3

The TLS 1.3 configuration used in this example will use:
* The mutual authentication between client and server is done with ECC certificate.
* The key exchange performed is done with ECDHE.
* The application traffic cipher used is any AEAD.

No PSK authentication, early data, session resumption are used in this demo.


### IoT-SAFE Configuration

The below preprocessor macros can be found in the `client-tls13.c`.

The applet that has been tested with this demo has the current configuration:

| Key slot | Name                 | Description                                                  |
| -------- | -------------------- | ------------------------------------------------------------ |
| 0x01     | `PRIVKEY_ID`         | pre-provisioned with the client ECC private key              |
| 0x02     | `CRT_CLIENT_FILE_ID` | pre-provisioned with the client ECC public key certificate   |
| 0x03     | `CRT_SERVER_FILE_ID` | pre-provisioned with the server ECC public key certificate   |
| 0x04     | `ECDH_KEYPAIR_ID`    | used to generate the ECDH key pair that will be used during the TLS session |
| 0x05     | `PEER_PUBKEY_ID`     | used to store the ECDH public key received from the peer during the TLS session |

The following file is used to read the client's certificate and will be used to authenticate the client:

| File Slot | Name                 | Description                                                |
| --------- | -------------------- | ---------------------------------------------------------- |
| 0x02      | `CRT_CLIENT_FILE_ID` | pre-provisioned with the client ECC public key certificate |

The following file is used to read the server's certificate and will be used to authenticate the server by trusting the server certificate (trust here means no CA chain verification is performed, only comparing the server certificate sent from the server with the one stored in the IoT-SAFE):

| File Slot | Name                 | Description                                                |
| --------- | -------------------- | ---------------------------------------------------------- |
| 0x03      | `CRT_SERVER_FILE_ID` | pre-provisioned with the server ECC public key certificate |

How the applet configuration (such as putting the client key pair with the corresponding client certificate and the server certificate the client can trust) is performed during its initial and on-field lifetime depends on the infrastructure and is out-of-scope of this demo.


### IoT-SAFE Interface

The below code explanations can be found in the `client-tls13.c`.

In this demo, the client is the IoT-SAFE capable endpoint.

First, it creates a wolfSSL context `ctx` with TLS 1.3.

```c
wolfSSL_CTX_new(wolfTLSv1_3_client_method());
```

In order to activate IoT-SAFE support in this context, the following function is
called:

```c
wolfSSL_CTX_iotsafe_enable(ctx);
```

Extracting the client and server certificate can be done by the following functions:

```c
wolfIoTSafe_GetCert(
                    CRT_CLIENT_FILE_ID,
                    cert_buffer,
                    sizeof(cert_buffer));
```

```c
wolfIoTSafe_GetCert(
                    CRT_SERVER_FILE_ID,
                    cert_buffer,
                    sizeof(cert_buffer));
```

in which the extracted certificate inside the `cer_buffer` can be later loaded to the `ctx`.

Additionally, after the TLS session `ssl` creation, shown below:

```c
ssl = wolfSSL_new(ctx);
```

the client associates the pre-provisioned keys and the available slots in the
IoT-SAFE applet to the current session:


```c
wolfSSL_iotsafe_on(ssl, PRIVKEY_ID, ECDH_KEYPAIR_ID, PEER_PUBKEY_ID, PEER_CERT_ID);
```


### Compiling

First, user needs to build wolfSSL with the following options:
```
./configure CFLAGS="-DWOLFSSL_TRUST_PEER_CERT" --enable-tls13 --enable-pkcallbacks --enable-debug --enable-iotsafe --enable-hkdf
```

Additionally, user can pass `CFLAGS="-DDEBUG_WOLFSSL -DWOLFSSL_DEBUG_TLS -DDEBUG_IOTSAFE"` if more debugging information is to be used. This can clutter the demo stdout more than `--enable-debug` does, but this is very useful to see the overall TLS 1.3 handshaking process with IoT-SAFE.

Hence, the full wolfSSL build for the demo is:
```
./configure CFLAGS="-DWOLFSSL_TRUST_PEER_CERT -DDEBUG_WOLFSSL -DWOLFSSL_DEBUG_TLS -DDEBUG_IOTSAFE" --enable-tls13 --enable-pkcallbacks --enable-debug --enable-iotsafe
```

`-DWOLFSSL_TRUST_PEER_CERT` is needed for `wolfSSL_CTX_trust_peer_buffer` in `IDE/iotsafe-raspberrypi/client-tls13.c`

### Running

After building wolfSSL, from this directory, run `make` and a help usage will be shown.

Run below to build a minimal demo:
```
make all
```

Run below to enable printing UART IO:
```
make all ENABLE_DEBUG_UART_IO_EXTRA=on|off
```

Run the built `./main.bin` to print the help usage.

An example to run the demo connecting to a server:
```
./main.bin -ip <ipaddress> -h <full-hostname> -p <port> -t 25 -d /dev/ttyUSB0|/dev/tty/ACM0
```