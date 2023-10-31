wolfSSL using Analog Devices MAXQ1065 or MAX1080
================================================

## Overview

wolfSSL can be configured to use the MAXQ1065 or MAX1080 cryptographic
controllers. Product datasheets, user guides and other resources can be found at
Analog Devices website:

https://www.analog.com

## Build and Usage

Please use the appropriate SDK or Evkit to build wolfSSL.

Instructions for setting up the hardware and various other utilities and
prerequisite software and then building and using wolfSSL can be found in the
MAXQ SDK. The SDK is available upon request to Analog Devices.

The Evkit comes with all the other necessary utility software and scripts and
some prerequisite instructions such as how to copy the headers and library
files to the correct location. Once those instructions are followed, you are
ready to build wolfSSL.

To build for MAXQ1065 the following is sufficient:

```
./configure --enable-cryptocb --disable-extended-master --enable-psk \
            --enable-aesccm --enable-debug --disable-tls13 \
            --with-maxq10xx=MAXQ1065
make all
sudo make install
```
To build for MAXQ1080 the following is sufficient:

```
./configure --enable-cryptocb --disable-extended-master --enable-psk \
            --enable-aesccm --enable-debug --with-maxq10xx=MAXQ108x
make all
sudo make install
```

## Example Code

You can find a very simple example client application in our wolfssl-examples
repo on github:

https://github.com/wolfSSL/wolfssl-examples/tree/master/maxq10xx

NOTE: It assumes you have built wolfSSL using the SDK or Evkit.

