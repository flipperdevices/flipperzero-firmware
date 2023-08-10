# tenAsys INtime RTOS Port

## Overview

This port is for the tenAsys INtime RTOS available [here](http://www.tenasys.com/intime).

To enable use the define `INTIME_RTOS`.

## Usage

The wolfExamples.sln is a Visual Studio 2015 project. You must have the INtime SDK installed and an INtime RTOS agent running.

The default configuration is set inside the `IDE/INTIME-RTOS/user_settings.h` file.

The example application provides a simple menu interface to select difference application functions to test.

```
wolfExamples started
wolfExamples finished initialization

                                MENU

        t. WolfCrypt Test
        b. WolfCrypt Benchmark
        c. WolfSSL Client Example
        s. WolfSSL Server Example
        l. WolfSSL Localhost Client/Server Example
Please select one of the above options:
```

### `t`wolfCrypt Test

Performs testing of all crypto algorithms.

```
Crypt Test
error    test passed!
base64   test passed!
base64   test passed!
MD5      test passed!
SHA      test passed!
SHA-256  test passed!
SHA-384  test passed!
SHA-512  test passed!
Hash     test passed!
HMAC-MD5 test passed!
HMAC-SHA test passed!
HMAC-SHA256 test passed!
HMAC-SHA384 test passed!
HMAC-SHA512 test passed!
HMAC-KDF    test passed!
X963-KDF    test passed!
GMAC     test passed!
Chacha   test passed!
POLY1305 test passed!
ChaCha20-Poly1305 AEAD test passed!
DES      test passed!
DES3     test passed!
AES      test passed!
AES-GCM  test passed!
AES-CCM  test passed!
AES Key Wrap test passed!
RANDOM   test passed!
RSA      test passed!
DH       test passed!
DSA      test passed!
SRP      test passed!
PWDBASED test passed!
openSSL extra test
OPENSSL  test passed!
ECC      test passed!
ECC Enc  test passed!
ECC buffer test passed!
CURVE25519 test passed!
ED25519  test passed!
CMAC     test passed!
PKCS7enveloped test passed!
PKCS7signed    test passed!
PKCS7encrypted test passed!
mutex    test passed!
memcb    test passed!
Crypt Test: Return code 0
```

### `b` wolfCrypt Benchmark

Performs benchmark of crypto algorithms.

```
Benchmark Test
RNG      25 kB took 0.002 seconds,   11.017 MB/s
AES enc  25 kB took 0.002 seconds,   15.090 MB/s
AES dec  25 kB took 0.002 seconds,   15.119 MB/s
AES-GCM  25 kB took 0.003 seconds,    9.433 MB/s
AES-CTR  25 kB took 0.001 seconds,   22.378 MB/s
AES-CCM  25 kB took 0.002 seconds,   15.306 MB/s
CHACHA   25 kB took 0.002 seconds,   16.063 MB/s
CHA-POLY 25 kB took 0.001 seconds,   20.447 MB/s
3DES     25 kB took 0.002 seconds,   10.717 MB/s

MD5      25 kB took  0.00 seconds,   31.576 MB/s
POLY1305 25 kB took 0.000 seconds,  201.575 MB/s
SHA      25 kB took  0.00 seconds,   43.761 MB/s
SHA-256  25 kB took 0.001 seconds,   19.299 MB/s
SHA-384  25 kB took 0.002 seconds,   14.577 MB/s
SHA-512  25 kB took 0.001 seconds,   21.718 MB/s
AES-CMAC 25 kB took  0.00 seconds,   34.925 MB/s

RSA 2048 public           2.445 milliseconds, avg over 1 iterations
RSA 2048 private         64.711 milliseconds, avg over 1 iterations

RSA 1024 key generation  318.755 milliseconds, avg over 5 iterations
RSA 2048 key generation  22648.396 milliseconds, avg over 5 iterations
DH  2048 key generation  23.119 milliseconds, avg over 1 iterations
DH  2048 key agreement   26.756 milliseconds, avg over 1 iterations

ECC  256 key generation   2.984 milliseconds, avg over 5 iterations
EC-DHE   key agreement    2.967 milliseconds, avg over 5 iterations
EC-DSA   sign   time      1.448 milliseconds, avg over 5 iterations
EC-DSA   verify time      3.304 milliseconds, avg over 5 iterations
ECC      encrypt          5.860 milliseconds, avg over 1 iterations
ECC      decrypt          6.360 milliseconds, avg over 1 iterations

CURVE25519 256 key generation  1.416 milliseconds, avg over 5 iterations
CURVE25519 key agreement       1.332 milliseconds, avg over 5 iterations

ED25519  key generation   0.320 milliseconds, avg over 5 iterations
ED25519  sign   time      0.595 milliseconds, avg over 5 iterations
ED25519  verify time      1.310 milliseconds, avg over 5 iterations
Benchmark Test: Return code 0
```

### `c` wolfSSL Client

To configure the host address and port modify the `TLS_HOST_REMOTE` and `TLS_PORT` macros at top of `wolfExamples.c`. This example uses TLS 1.2 to connect to a remote host.

### `s` wolfSSL Server

To configure the port to listen on modify `TLS_PORT` at top of `wolfExamples.c`.

### `l` wolfSSL Localhost Server/Client

Starts a TLS server thread listening on localhost. Starts the TLS client and performs connect, exchanges some data and disconnects.

```
Waiting for a connection...
Client connected successfully
Using Non-Blocking I/O: 0
Message for server:     Client:

Received:       I hear ya fa shizzle!

The client has closed the connection.
```

## References

For more information please contact info@wolfssl.com.
