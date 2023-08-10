# wolfSSL and QUIC

wolfSSL *supports* QUIC implementations. It does not offer an implementation of the QUIC protocol.

What does that mean?

TLS works on top of the Internet's TCP protocol stack. The TCP is shipped as part of the Operating System
and accessible via system calls and APIs. TCP itself is not secured by TLS, only the data sent via it is
protected. TCP does not have to know anything about TLS.

QUIC, on the other hand, is always protected by TLS. A QUIC implementation does always need an 
implementation of the TLS protocol, specifically TLSv1.3. It does this in new ways and TLS
implementations need to accommodate these. Those specifics have been added to wolfSSL.

## wolfSSL Configuration

QUIC support is enabled in the common autoconf style via:

```
> ./configure --enable-quic
```

this drags in some very basic features. To have everything necessary for a QUIC protocol implementation like ngtcp2, one would do:

```
 ./configure --enable-quic --enable-session-ticket --enable-earlydata --enable-psk
```

**Note**: for now, quic is not enabled via `--enable-all` and always needs to be specified.

## ngtcp2

One of the recommended QUIC implementations is [ngtpc2](https://github.com/ngtcp2/ngtcp2). It now
supports use of wolfSSL as its "crypto" library. Configuring ngtcp2 with wolfSSL is done via:

```
> ./configure --with-wolfssl
```

Built this way, one gets a complete, standard-conform QUIC library.

### HTTP/3 != QUIC

HTTP/3 is designed for the QUIC protocol. But QUIC does not know anything about HTTP/3. [nghttp3](https://github.com/ngtcp2/nghttp3) is an implementation of HTTP/3 that works with ngtcp2.

A *complete* HTTP/3 choice would be `nghttp3 + ngtcp2 + wolfSSL` plus the UDP of the operating system. The examples provided in ngtcp2 build exactly that and give you clients and servers that illustrate the use of the APIs.

### curl - all in one

The above configuration has also been added to [curl](https://github.com/curl/curl) and its library `libcurl`. Which will, once it leaves the experimental stage, make QUIC available in your favourite command line tool and for all applications built on top of `libcurl`.

### why?

Why all these different blocks? 

The separation of HTTP/3 and QUIC is natural when you think about the relationship between TCP and HTTP/1.1. Like TCP, QUIC can and will carry other protocols. HTTP/3 is only the first one. Most likely 'DNS over QUIC' (DoQ) is the next popular, replacing DoH.

The separation of QUIC's "crypto" parts from its other protocol enabling functions is a matter of security. In its experimental beginnings, QUIC had its own security design. With the emerging TLSv1.3 and all it improvements, plus decades of experience, it seemed rather unwise to have something separate in QUIC.

Therefore, the complete TLSv1.3 handshake became part of the QUIC protocol, with some restrictions and simplifications (UDP based QUIC does not accommodate broken TCP middle boxes). With the need for a complete TLSv1.3 stack, QUIC implementers happily make use of existing TLS libraries. 

## wolfSSL API

The exposed API carries all methods that the [quictls/openssl](https://github.com/quictls/openssl) introduces. This seems to become the standard, since other *SLL libraries have picked those up or are about to. The methods are all in the `wolfSSL_` prefix. There are some additional methods, which are covered below.

### Core Interworking

At the base is the struct `WOLFSSL_QUIC_METHOD` which carries four callbacks:

* `set_encryption_secrets()`: to forward generated secrets.
* `add_handshake_data()`: to forward Handshake messages.
* `flush_flight()`: to tell the QUIC protocol handler to flush any buffered data.
* `send_alert()`: to forward SSL alerts.

A QUIC protocol handler installs these via `wolfSSL_CTX_set_quic_method()` or `wolfSSL_set_quic_method()`. When CRYPTO messages arrive from the peer, those are added via `wolfSSL_provide_quic_data()` to the `WOLFSSL*` instance:

```
  DATA ---recv+decrypt---+
                         v 
            wolfSSL_provide_quic_data(ssl, ...)
            wolfSSL_do_handshake(ssl);
                  +-> add_handshake_data_callback(REPLY)
                          |
  REPLY <--encrypt+send---+
```

The wolfSSL instance performs the common TLSv1.3 handshake processing with the significant change that it does not encrypt or decrypt messages itself. It computes all the secrets and MACs as usual, however.

Encryption and Decryption is done by the QUIC protocol handler. Which is why it gets access to the secrets
at the different encryption levels: `initial`(no encryption), `handshake`, `application` and `earlydata`.

For sending data, the level to use for encryption is a call parameter in `add_handshake_data()`. For received data, the level to use for decryption can be interrogated via `wolfSSL_quic_read_level()`.

When the handshake is done, any additional CRYPTO messages are received in the same way, only `wolfSSL_process_quic_post_handshake()` is invoked to process them.

### Crypto Support

At the basic level, there are:

* `wolfSSL_quic_get_aead()`: to get the AEAD cipher negotiated
* `wolfSSL_quic_get_md()`:  to get the MD negotiated
* `wolfSSL_quic_get_hp()`: to get the EVP_CIPHER for header protection
* `wolfSSL_quic_get_aead_tag_len()`: the get the tag length of the negotiated AEAD cipher

In addition to that, the wolfSSL QUIC API offers the following functions:

* `wolfSSL_quic_crypt_new()`: to setup a `WOLFSSL_EVP_CIPHER_CTX` for en- or decryption with AEAD cipher, key and iv.
* `wolfSSL_quic_aead_encrypt()`: to encrypt with such a context and params
* `wolfSSL_quic_aead_decrypt()`: to decrypt with such a context and params

and for key generation `wolfSSL_quic_hkdf_extract()`, `wolfSSL_quic_hkdf_expand()` and `wolfSSL_quic_hkdf()`.

## Tests

Tests have been added in `tests/quic.c` to run as part of `unit.tests`. Those go from basic checks on providing data and receiving secrets to complete handshakes between SSL client and server instances. These handshakes are done plain, with session resumption and with early data.

These tests exchange the handshake messages between the SSL instances unencrypted, verifying their sequence and contents. They also verify that client and sever did indeed generate identical secrets for the different encryption levels.
