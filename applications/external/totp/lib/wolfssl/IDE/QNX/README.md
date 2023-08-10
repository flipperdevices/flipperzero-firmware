# QNX CAAM Driver And Examples

This directory contains;
- A Makefile for creating the QNX CAAM driver located at IDE/QNX/CAAM-DRIVER/Makefile
- A Makefile for creating wolfSSL library located at IDE/QNX/wolfssl/Makefile
- A Makefile for creating wolfCrypt unit tests located at IDE/QNX/testwolfcrypt/Makefile
- An example TLS server located at IDE/QNX/example-server/
- An example client located at IDE/QNX/example-client
- An example CMAC use located at IDE/QNX/example-cmac

To build either of these, first build wolfSSL with support for use with QNX CAAM. To do this use the configure option --enable-caam=qnx

```
bash
source ~/qnx700/qnxsdp-env.sh
./configure --host=arm-unknown-nto-qnx7.0.0eabi --enable-caam=qnx
make
```

Once the wolfSSL library has been built cd to IDE/QNX/CAAM-DRIVER and run "make". This will produce the wolfCrypt resource manager. It should be started on the device with root permissions. Once wolfCrypt is running on the device with root permissions then any user with access to open a connection to /dev/wolfCrypt can make use of the driver.  

### Momentics
To build in momentics IDE:

- Switch the workspace to be wolfssl/IDE/QNX/
- Import the three projects (File->Import->General->Existing Projects into Workspace, in "Select root directory" browse to the directory wolfssl/IDE/QNX/ and select the projects then click "Finish").
- Build

### Supported Operations By CAAM Driver
- ECC black key creation
- ECC black key sign / verify / ecdh
- Black blob creation and open
- Red/Black blob creation and open
- Cover keys (turn to black key)
- CMAC with and without black keys
- TRNG used by default to seed Hash DRBG
- AES operations (CTR)

### Performance

Typically on smaller block sizes (~ less than 7k) operations are slower with CAAM than software implementations. There is a tunable macro WOLFSSL_CAAM_QNX_MEMORY which sets the size of an internal buffer used currently with AES operations for performance. By default it is set to 250k but can be defined to smaller or larger values. If the block size being encrypted/decrypted does not fit within the buffer size then there will be a big performance hit.
