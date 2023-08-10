# wolfSSL CSharp Wrappers

This directory contains the CSharp wrapper for the wolfSSL TLS layer with examples.

* `wolfSSL_CSharp`: wolfSSL TLS layer wrappers (library)

Examples:
* `wolfSSL-DTLS-PSK-Server`
* `wolfSSL-DTLS-Server`
* `wolfSSL-Example-IOCallbacks`
* `wolfSSL-TLS-Client`
* `wolfSSL-TLS-PSK-Client`
* `wolfSSL-TLS-PSK-Server`
* `wolfSSL-TLS-Server`
* `wolfSSL-TLS-ServerThreaded`

## Windows

A Visual Studio solution `wolfSSL_CSharp.sln` is provided. This will allow you
to build the wrapper library and examples. It includes the wolfSSL Visual Studio
project directly.

## Linux (using Mono)

Prerequisites for linux:

```
apt install mono-tools-devel
```

Build wolfSSL and install:

```
./autogen.sh
./configure --enable-wolftpm
make
make check
sudo make install
```

Build and run the wrapper:

```
cd wrapper/CSharp

csc wolfSSL_CSharp/wolfSSL.cs wolfSSL_CSharp/X509.cs \
    wolfSSL-TLS-Server/wolfSSL-TLS-Server.cs

Run the example:

```
cp wolfSSL-TLS-Server.exe ../../certs
cd ../../certs

mono wolfSSL-TLS-Server.exe

Calling ctx Init from wolfSSL
Finished init of ctx .... now load in cert and key
Ciphers : TLS13-AES128-GCM-SHA256:TLS13-AES256-GCM-SHA384:TLS13-CHACHA20-POLY1305-SHA256:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA:ECDHE-RSA-AES128-SHA:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-CHACHA20-POLY1305:DHE-RSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305-OLD:ECDHE-ECDSA-CHACHA20-POLY1305-OLD:DHE-RSA-CHACHA20-POLY1305-OLD
Started TCP and waiting for a connection
```
