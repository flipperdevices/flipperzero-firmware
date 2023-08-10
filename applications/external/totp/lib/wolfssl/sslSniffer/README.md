# wolfSSL Sniffer

The wolfSSL sniffer can be used to passively sniff SSL traffic including https traffic. Of course the server’s private key is required in order to decode the SSL handshake and allow future decryption of SSL messages. Input to the sniffer should be raw packets beginning with the IP header.

## Installation

The wolfSSL sniffer requires the wolfSSL library version 1.8.0 or later. Future releases can be obtained from http://www.wolfssl.com

To build and install wolfSSL including the wolfSSL sniffer:

```sh
./configure --enable-sniffer
make
sudo make install
```


## Build Options

The wolfSSL sniffer has several build options to include some extra behavior: SSL Statistics, Session Watching, Store Data Callback, Chain Input, and allowing STARTTLS protocols.

The SSL Statistics option provides the logging of some additional statistics regarding the sessions being decoded. The statistics tracking uses a mutex to protect access to the tracking storage. To enable this option, use the following configure command line and build as before:

`./configure --enable-sniffer CPPFLAGS=-DWOLFSSL_SNIFFER_STATS`

The Session Watching option allows the sniffer to watch any packet provided it without initial setup. It will start to decode all TLS sessions and when the server’s certificate is detected, the certificate is given to a callback function provided by the user which should provide the appropriate private key. To enable this option, use the following configure command line and build as before:

`./configure --enable-sniffer CPPFLAGS=-DWOLFSSL_SNIFFER_WATCH`

The Store Data Callback option allows the sniffer to take a callback that is called when storing the application data into a custom buffer rather than into the reallocated data pointer the callback is called in a loop until all data is consumed. To enable this option, use the following configure command line and build as before:

`./configure --enable-sniffer CPPFLAGS=-DWOLFSSL_SNIFFER_STORE_DATA_CB`

The Chain Input option allows the sniffer to receive its input as a struct iovec list rather than a pointer to a raw packet. To enable this option, use the following configure command line and build as before:

`./configure --enable-sniffer CPPFLAGS=-DWOLFSSL_SNIFFER_CHAIN_INPUT`

The STARTTLS option allows the sniffer to receive and ignore plaintext before receiving the first TLS handshake message. This is useful for protocols like SMTP and POP3 which start out in plaintext and switch to TLS during the connection. To enable this option, use the following configure command line and build as before:

`./configure --enable-sniffer CPPFLAGS=-DSTARTTLS_ALLOWED`

All options may be enabled with the following configure command line:

```sh
./configure --enable-sniffer \
    CPPFLAGS="-DWOLFSSL_SNIFFER_STATS -DWOLFSSL_SNIFFER_WATCH \
    -DWOLFSSL_SNIFFER_STORE_DATA_CB -DWOLFSSL_SNIFFER_CHAIN_INPUT \
    -DSTARTTLS_ALLOWED"
```

To add some other cipher support to the sniffer, you can add options like:

```sh
--enable-arc4
--enable-nullcipher
--enable-des3
```

By default, wolfSSL restricts RSA key sizes to 1024-bits minimum. To allow the decoding of smaller, less secure RSA keys like 512-bit keys, you will need to add the compiler flag `-DWOLFSSL_MIN_RSA_BITS=512` to CFLAGS or CPPFLAGS, or define it in your user-settings header.


## Synchronous Cryptography Offload Options

The sniffer can take advantage of some crypto offload hardware if available. If you have an Intel QuickAssist board or a Cavium OCTEON II or III. Currently, only the algorithms AES-CBC, AES-GCM, and DES3-CBC are offloaded to the hardware. These directions assume you already have the QAT or OCTEON-SDK libraries built.

To build for QAT, use the following configure options:

```sh
./configure --enable-sniffer --enable-cryptocb \
    --with-intelqa-sync=/path/to/qat
```

To build with OCTEON II support for a standalone host:

```sh
./configure --enable-sniffer --enable-cryptocb \
    --with-octeon-sync=/path/to/octeon-sdk
```

To build with OCTEON III support for a Linux host:

```sh
./configure --enable-sniffer --enable-cryptocb \
    --with-octeon-sync=/path/to/octeon-sdk \
    OCTEON_OBJ=obj-octeon3 OCTEON_HOST=linux
```


## Command Line Options

The wolfSSL sniffer includes a test application `snifftest` in the `sslSniffer/sslSnifferTest/` directory. The command line application has several options that can be passed in at runtime to change the default behavior of the application. To execute a “live” sniff just run the application without any parameters and then pick an interface to sniff on followed by the port.

An example startup may look like this:

```sh
$ cd sslSniffer/sslSnifferTest
$ ./snifftest

1. en0 (No description available)
2. fw0 (No description available)
3. en1 (No description available)
4. fw1 (No description available)
5. p2p0 (No description available)
6. en3 (No description available)
7. lo0 (No description available)

Enter the interface number (1-7): 7
server = 127.0.0.1
server = ::1
server = fe80::1

Enter the port to scan: 11111
```

The above example sniffs on the localhost interface (lo0) with the default wolfSSL port of 11111 and uses the default wolfSSL server key `../../certs/server-key.pem` for RSA and `../../certs/ecc-key.pem` for ECC.

Trace output will be written to a file named `tracefile.txt`.

To decode a previously saved pcap file you will need to enter a few parameters.

The following table lists the accepted inputs in saved file mode.

Synopsis:

`snifftest  dumpFile pemKey [server] [port] [password] [threads]`

`snifftest` Options Summary:

```
Option      Description                                 Default Value
dumpFile    A previously saved pcap file                NA
pemKey      The server’s private key in PEM format      NA
server      The server’s IP address (v4 or v6)          127.0.0.1
port        The server port to sniff                    443
password    Private Key Password if required            NA
threads     The number of threads to run with           5
```

To decode a pcap file named test.pcap with a server key file called myKey.pem that was generated on the localhost with a server at port 443 just use:

`./snifftest test.pcap myKey.pem`

If the server was on 10.0.1.2 and on port 12345 you could instead use:

`./snifftest test.pcap myKey.pem 10.0.1.2 12345`

If the server was on localhost using IPv6 and on port 12345 you could instead use:

`./snifftest test.pcap myKey.pem ::1 12345`

If you wanted to use 15 threads to decode `test.pcap` and your key does not require a password, you could use a dummy password and run:

`./snifftest test.pcap myKey.pem 10.0.1.2 12345 pass 15`

## API Usage

The wolfSSL sniffer can be integrated into any application using the existing sniffer API.

Use the include `#include <wolfssl/sniffer.h>`.

### ssl_InitSniffer

```c
void ssl_InitSniffer(void);
```

Initializes the wolfSSL sniffer for use and should be called once per application.  

### ssl_FreeSniffer

```c
void ssl_FreeSniffer(void);
```

Frees all resources consumed by the wolfSSL sniffer and should be called when use of the wolfSSL sniffer is no longer required.

### ssl_Trace

```c
int ssl_Trace(const char* traceFile, char* error);
```

Enables Tracing when a file is passed in.  Disables Tracing if previously on and a NULL value is passed in for the file.

Returns Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem

### ssl_SetPrivateKey

```c
int ssl_SetPrivateKey(const char* serverAddress, int port,
                      const char* keyFile, int keyFormat,
                      const char* password, char* error);
```

Creates a sniffer session based on the `serverAddress` and `port` inputs using the ECC or RSA `keyFile` as the server’s key.

The `keyFormat` can be either `FILETYPE_PEM` or `FILETYPE_DER`. If the keyFile has password protection then the password parameter can hold the proper value.

Return Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem

### ssl_SetPrivateKeyBuffer

```c
int ssl_SetPrivateKeyBuffer(const char* address, int port,
                            const char* keyBuf, int keySz, int typeKey,
                            const char* password, char* error)
```

Creates a sniffer session based on the `serverAddress` and `port` inputs using the ECC or RSA `keyBuf` and `keySz` as the server’s key.

The `keyFormat` can be either `FILETYPE_PEM` or `FILETYPE_DER`. If the keyFile has password protection then the password parameter can hold the proper value.

Return Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem


### ssl_SetNamedPrivateKey

```c
int ssl_SetNamedPrivateKey(const char* name,
                           const char* serverAddress, int port,
                           const char* keyFile, int keyFormat,
                           const char* password, char* error);
```

Creates a sniffer session for a server named `name` based on the `serverAddress` and `port` inputs using the ECC or RSA `keyFile` as the server’s key.

The `keyFormat` can be either `FILETYPE_PEM` or `FILETYPE_DER`. If the keyFile has password protection then the password parameter can hold the proper value.

This function requires that the SNI extension is enabled in the build (`HAVE_SNI`).

Return Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem

### ssl_SetNamedPrivateKeyBuffer

```c
int ssl_SetNamedPrivateKeyBuffer(const char* name,
                                 const char* address, int port,
                                 const char* keyBuf, int keySz, int typeKey,
                                 const char* password, char* error)
```

Creates a sniffer session for a server named `name` based on the `serverAddress` and `port` inputs using the ECC or RSA `keyBuf` and `keySz` as the server’s key.

The `keyFormat` can be either `FILETYPE_PEM` or `FILETYPE_DER`. If the keyFile has password protection then the password parameter can hold the proper value.

This function requires that the SNI extension is enabled in the build (`HAVE_SNI`).

Return Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem

### ssl_SetNamedEphemeralKey

```c
int ssl_SetNamedEphemeralKey(const char* name,
                             const char* address, int port,
                             const char* keyFile, int typeKey,
                             const char* password, char* error)
```

Creates a sniffer session for a server named `name` based on the `serverAddress` and `port` inputs using ECC or DH static ephemeral key.

The `keyFormat` can be either `FILETYPE_PEM` or `FILETYPE_DER`. If the keyFile has password protection then the password parameter can hold the proper value.

This function requires that static ephemeral key support (`WOLFSSL_STATIC_EPHEMERAL`) and the SNI extension (`HAVE_SNI`) are enabled in the build.

Return Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem

### ssl_SetEphemeralKey

```c
int ssl_SetEphemeralKey(const char* address, int port, 
                        const char* keyFile, int typeKey, 
                        const char* password, char* error)
```
Creates a sniffer session based on the `serverAddress` and `port` inputs using ECC or DH static ephemeral key.

The `keyFormat` can be either `FILETYPE_PEM` or `FILETYPE_DER`. If the keyFile has password protection then the password parameter can hold the proper value.

This function requires that static ephemeral key support (`WOLFSSL_STATIC_EPHEMERAL`) and the SNI extension (`HAVE_SNI`) are enabled in the build.

Return Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem

### ssl_DecodePacket

```c
int ssl_DecodePacket(const unsigned char** packet, int length,
    unsigned char* data, char* error);
```

### ssl_DecodePacketWithSessionInfo

```c
int ssl_DecodePacketWithSessionInfo(const unsigned char* packet, int length,
    unsigned char** data, SSLInfo* sslInfo, char* error);
```

Decodes a raw packet that begins with the IP header and is length bytes long. Any SSL application data will be stored in data which should be at least 16,384 bytes, the maximum SSL record size. Information about the SSL session will be copied into `sslInfo` if it is non-null.

The `SSLInfo` structure can be found in `sniffer.h`. It has information about the protocol version, cipher suite, server name indication, and key size in bits.

```c
typedef struct SSLInfo
{
    unsigned char  isValid;                    /* indicates if the info in this struct is valid: 0 = no, 1 = yes */
    unsigned char  protocolVersionMajor;       /* SSL Version: major */
    unsigned char  protocolVersionMinor;       /* SSL Version: minor */
    unsigned char  serverCipherSuite0;         /* first byte, normally 0 */
    unsigned char  serverCipherSuite;          /* second byte, actual suite */
    unsigned char  serverCipherSuiteName[256]; /* cipher name, e.g., "TLS_RSA_..." */
    unsigned char  serverNameIndication[128];
    unsigned int   keySize;
} SSLInfo;
```

Return Values:

* >0 on success and indicates the number of bytes written to data
* 0 indicates no SSL data is ready yet
* -1 if a problem occurred, the string error will hold a message describing the problem


### ssl_SetConnectionCb

```c
int ssl_SetConnectionCb(SSLConnCb cb);
```

Sets a callback function that will be called when the full session information is known and will provide a pointer to the session’s information. The callback function has the signature:

```c
typedef void (*SSLConnCb)(const void* session, SSLInfo* info, void* ctx);
```

Where session is the current session. info will be a pointer to the session’s info. The ctx is application specific context data passed to the callback.

Return Values:

* >0 on success and indicates the number of bytes written to data
* 0 indicates no SSL data is ready yet
* -1 if a problem occurred, the string error will hold a message describing the problem

### ssl_SetConnectionCtx

```c
int ssl_SetConnectionCtx(void* ctx);
```

Stores ctx, a pointer to application specific context data that will be passed to the connection callback function. The wolfSSL sniffer will not know anything about the context data.

Return Values:

* 0 on success
* -1 if a problem occurred


### ssl_SetKeyCallback

This feature is enabled by default and will be called when a key is required for a session using static ephemeral keys with TLS v1.3.

The public key being used will be provided allowing lookup of the corresponding private key.

The `privKey` buffer is a dynamic buffer assigned via a call to setup a static ephemeral key via `ssl_SetNamedEphemeralKey` or `ssl_SetEphemeralKey`.

```c
typedef int (*SSLKeyCb)(void* vSniffer, int namedGroup,
    const unsigned char* srvPub, unsigned int srvPubSz,
    const unsigned char* cliPub, unsigned int cliPubSz,
    unsigned char* privKey, unsigned int* privKeySz,
    void* ctx, char* error);

int ssl_SetKeyCallback(SSLKeyCb cb, void* ctx, char* error);
```

The parameter `vSniffer` is a typeless pointer to the current sniffer session (`SnifferSession`). The `namedGroup` is the TLS defined named groups like `WOLFSSL_ECC_SECP256R1` or `WOLFSSL_FFDHE_2048`. The server and client public key information are provided to lookup the private key to be used for this session. The loaded private key to be used will be passed in `key`. If a different key should be used it can optionally be returned in `privKey` and `privKeySz`.

Return Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem


## API Usage: SSL Statistics options

For an example on the use of the sniffer stats option, search the source `snifftest.c` for `WOLFSSL_SNIFFER_STATS`.

See the header file `sniffer.h` for the structure `SSLStats` for the list of statistics that can be read.

```c
typedef struct SSLStats
{
    unsigned long int sslStandardConns;      /* server_hello count not including resumed sessions */
    unsigned long int sslClientAuthConns;    /* client's who have presented certificates (mutual authentication) */
    unsigned long int sslResumedConns;       /* resumed connections */
    unsigned long int sslEphemeralMisses;    /* TLS v1.2 and older PFS / ephemeral connections missed (not able to decrypt) */
    unsigned long int sslResumeMisses;       /* Resumption sessions not found */
    unsigned long int sslCiphersUnsupported; /* No cipher suite match found when compared to supported */
    unsigned long int sslKeysUnmatched;      /* Key callback failures (not found). Applies to WOLFSSL_SNIFFER_WATCH only */
    unsigned long int sslKeyFails;           /* Failures loading or using keys */
    unsigned long int sslDecodeFails;        /* Dropped packets (not application_data or match protocol version) */
    unsigned long int sslAlerts;             /* Number of decoded alert messages */
    unsigned long int sslDecryptedBytes;     /* Number of decrypted bytes */
    unsigned long int sslEncryptedBytes;     /* Number of encrypted bytes */
    unsigned long int sslEncryptedPackets;   /* Number of encrypted packets */
    unsigned long int sslDecryptedPackets;   /* Number of decrypted packets */
    unsigned long int sslKeyMatches;         /* Key callback successes (failures tracked in sslKeysUnmatched). Applies to WOLFSSL_SNIFFER_WATCH only. */
    unsigned long int sslEncryptedConns;     /* Number of created sniffer sessions */
    unsigned long int sslResumptionInserts;  /* Number of sessions reused with resumption */
} SSLStats;
```

### ssl_ResetStatistics

```c
int ssl_ResetStatistics(void);
```

Zeroes out the SSL sniffer statistics tracking storage.

Return Values:

* 0 on success
* -1 if a problem occurred

### ssl_ReadStatistics

```c
int ssl_ReadStatistics(SSLStats* stats);
```

Copies the SSL sniffer statistics into the provided `SSLStats` record, stats.

Return Values:

* 0 on success
* -1 if a problem occurred

### ssl_ReadResetStatistics

```c
int ssl_ReadResetStatistics(SSLStats* stats);
```

Copies the SSL sniffer statistics into the provided `SSLStats` records, stats, and then zeroes out the SSL sniffer statistics tracking storage in one action.

Return Values:

* 0 on success
* -1 if a problem occurred


## API Usage: Session Watching option

For an example on the use of the session watching option, search the source `snifftest.c` for `WOLFSSL_SNIFFER_WATCH`.

### ssl_SetWatchKeyCallback

```c
int ssl_SetWatchKeyCallback(SSLWatchCb cb, char* error);
int ssl_SetWatchKeyCallback_ex(SSLWatchCb cb, int devId, char* error);
```

Assigns a callback function to the wolfSSL sniffer used to locate and load a private key for a session at the time the sniffer knows the true identity of the server, when receiving its certificate message. The callback function is given to the parameter cb and any error string will be written into error. The function ssl_SetWatchKeyCallback_ex() takes an additional parameter called devId, the device ID of the hardware device handling the cryptography. The callback function has the signature:

```c
typedef int (*SSLWatchCb)(void* vSniffer,
    const unsigned char* certHash, unsigned int certHashSz,
    const unsigned char* certChain, unsigned int certChainSz,
    void* ctx, char* error);
```

The parameter `vSniffer` is a typeless pointer to the current sniffer session and is meant to be passed directly to the function `ssl_SetWatchKey_file` or `ssl_SetWatchKey_buffer`. The `certHash` is a SHA-256 hash of the certificate sent by the server, and its size is `certHashSz`. A pointer to certificate message’s payload is provided in the parameter `certChain`, and the certificate chain’s size in `certChainSz`. This will be a list of pairs of 24-bit certificate sizes and raw DER certificates in network order from the wire. The application space callback context data is provided in parameter ctx and is set by the function `ssl_SetWatchKeyCtx`. Any error string is copied into parameter error. Your callback function can use these values to locate the appropriate private key and load it into the sniffer session with the function `ssl_SetWatchKey_file` or `ssl_SetWatchKey_buffer`.

Return Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem

### ssl_SetWatchKeyCtx

```c
int ssl_SetWatchKeyCtx(void* ctx, char* error);
```

Stores ctx, a pointer to application specific context data that will be passed to the watch key callback function. The wolfSSL sniffer will not know anything about the context data.

Return Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem

### ssl_SetWatchKey_file

```c
int ssl_SetWatchKey_file(void* vSniffer, const char* keyFile, int keyType,
    const char* password, char* error);
```

This function must be called from the watch key callback. Gives the sniffer session, vSniffer, the private key to be used to decrypt the pre-master secret. The key’s file name is given in the parameter keyFile, and that file will be loaded. The keyType is either `FILETYPE_PEM` or `FILETYPE_DER`. If the private key is encrypted, provide the text string password. Any error string is returned in error.

Return Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem

### ssl_SetWatchKey_buffer

```c
int ssl_SetWatchKey_buffer(void* vSniffer, const unsigned char* key, 
    unsigned int keySz, int keyType, char* error);
```

This function must be called from the watch key callback. Gives the sniffer session, vSniffer, the private key to be used to decrypt the pre-master secret. The key is passed in by the pointer key, and is of size keySz. The keyType is either `FILETYPE_PEM` or `FILETYPE_DER`. Any error string is returned in error.

Return Values:

* 0 on success
* -1 if a problem occurred, the string error will hold a message describing the problem


## API Usage: Store Data Callback option

For an example on the use of the store data callback option, search the source `snifftest.c` for `WOLFSSL_SNIFFER_STORE_DATA_CB`.

### ssl_SetStoreDataCallback

```c
int ssl_SetStoreDataCallback(SSLStoreDataCb cb);
```

Assigns a callback function to the wolfSSL sniffer used to store data when processing an application data record. The callback function is given to the parameter cb.

The callback function has the signature:

```c
typedef int (*SSLStoreDataCb)(const unsigned char* decryptBuf,
    unsigned int decryptBufSz, unsigned int decryptBufOffset,
    void* ctx);
```

The parameter `decryptBuf` is a pointer to the beginning of the decrypted application data buffer. The value `decryptBufSz` is the number of bytes stored in the `decryptBuf`. `decryptBufOffset` is the offset into the `decryptBuf` where a copy should start. The `ctx` is an application specific parameter passed in the call to `ssl_DecodePacketWithSessionInfoStoreData()`. The callback should return the number of bytes copied out of `decryptBuf`. `decryptBufOffset` is a running sum of the bytes returned by the callback, and the loop stops when all bytes are consumed.

Return Values:

* 0 on success
* -1 if a problem occurred

### ssl_DecodePacketWithSessionInfoStoreData

```c
int ssl_DecodePacketWithSessionInfoStoreData(const unsigned char* packet,
    in1t length, void* ctx, SSLInfo* sslInfo, char* error);
```

Decodes a raw packet that begins with the IP header and is length bytes long. Any SSL application data will be handed to the store data callback function, along with the parameter `ctx`. Information about the SSL session will be copied into `sslInfo` if it is non-null.

The SSLInfo structure can be found in sniffer.h. It has information about the protocol version, cipher suite, server name indication, and key size in bits.

Return Values:

* >0 on success and indicates the number of bytes written by the store data callback
* 0 indicates no SSL data is ready yet
* -1 if a problem occurred, the string error will hold a message describing the problem


## API Usage: Chain Input option

For an example on the use of the chain input option, search the source `snifftest.c` for `WOLFSSL_SNIFFER_CHAIN_INPUT`.

### ssl_DecodePacketWithChain

```c
int ssl_DecodePacketWithChain(void* vChain, unsigned int chainSz,
    unsigned char** data, char* error);
```

Decodes a raw chain of packet buffers stored in an iovec passed in as the value vChain. The first buffer in the chain begins with the IP header. The chain is chainSz packets long. Any SSL application data will be stored in data, which may be allocated by the sniffer. Information about the SSL session will be copied into sslInfo if it is non-null.

The SSLInfo structure can be found in sniffer.h. It has information about the protocol version, cipher suite, server name indication, and key size in bits.

Return Values:

* >0 on success and indicates the number of bytes written by the store data callback
* 0 indicates no SSL data is ready yet
* -1 if a problem occurred, the string error will hold a message describing the problem

### ssl_DecodePacketWithChainSessionInfoStoreData

```c
int ssl_DecodePacketWithChainSessionInfoStoreData(void* vChain,
    unsigned int chainSz, void* ctx, SSLInfo* sslInfo, char* error);
```

This combines the options of decoding a chain input and storing data using a callback. Decodes a raw chain of packet buffers stored in an iovec passed in as the value vChain. The first buffer in the chain begins with the IP header. The chain is chainSz packets long. Any SSL application data will be handed to the store data callback function, along with the parameter ctx. Information about the SSL session will be copied into sslInfo if it is non-null.

The SSLInfo structure can be found in sniffer.h. It has information about the protocol version, cipher suite, server name indication, and key size in bits.

Return Values:

* >0 on success and indicates the number of bytes written by the store data callback
* 0 indicates no SSL data is ready yet
* -1 if a problem occurred, the string error will hold a message describing the problem


## Notes

### Performance

Once your SSL sniffing is working as expected you should be able to get some performance gains by compiling wolfSSL with fastmath enabled. You can do this by adding `--enable-fastmath` to your ./configure options.

### Start up

Remember to always start the sniffing application before the server.  This is important because if the SSL handshake is missed then future packets from that session will not be decoded.  In addition, any future sessions that use the “missed” session to do session resumption, renegotiation, or use session tickets based on that “missed” session will have the same problems.

### Cipher Suite Limitations

As a passive sniffer the wolfSSL sniffer will not be able to decode any SSL session that uses DHE (Ephemeral Diffie-Hellman) because it will not have access to the temporary key that the server generates.  You may need to disable DHE cipher suites on the server and/or client to prevent these cipher suites from being used.

### Thread Safety

Access to the sniffer session table is thread safe.  What is not thread safe, is using the same sniffer session from multiple threads.  For example, say sniffer session A is created by thread X. If 3 new packets come in for session A and threads X, Y, and Z all try to handle those packets concurrently that's a problem.  Ideally, the main thread would associate an ssl sniffer session (client ip/client port <-> server ip/server port) with a particular thread and use that same thread for the lifetime of the session.  Short of that, the sniffer session would need a lock which isn't ideal in a multithreaded scenario because once thread X locks the first packet from session A threads Y and Z would be blocked until thread X is done.  That defeats the whole purpose doing multithreaded sniffing.

### Server Name Indication

Some webservers use virtual domain name mapping where multiple servers using separate SSL keys and certificates are sharing the same IP address and port. The Server Name Indication client hello extension allows the SSL client to specify the name of the server it is connecting to. When running the configure command in section 2.1, add the option --enable-sni.

### STARTTLS

Many protocols use ssl as a layer between them and the network layer, and have a dedicated port for the secure connection. Other protocols start out on their classic well known port number in the clear and then offer the “STARTTLS” command which tells the server the client wants to use ssl. The server responds with an affirmation, and the client sends its TLS client hello message and starts negotiation. The sniffer can ignore non-TLS messages on a session until the client starts to negotiate TLS and then proceed as normal.


## Missing Features

### PSK

While wolfSSL supports Pre Shared Keys, the current version of the sniffer does not.

### Client Certificate URLs

Neither wolfSSL nor the sniffer current supports the TLS extension Client Certificate URLs.

### Secure Renegotiation

While wolfSSL supports secure renegotiation, the current version of the sniffer does not. The sniffer does support session resumption.


## Support

For issues or questions please email support@wolfssl.com.
