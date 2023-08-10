#ifndef _USER_SETTINGS_H_
#define _USER_SETTINGS_H_

#define WOLFSSL_AZSPHERE
/* Client connects to the server with these details. */
#ifdef CUSTOM_SERVER_CONNECTION
    #ifndef SERVER_IP
        #define SERVER_IP "192.168.1.200" /* Local Test Server IP */
    #endif
	#ifndef DEFAULT_PORT
		#define DEFAULT_PORT 11111
	#endif
    #define CERT         ca_cert_der_2048
    #define SIZEOF_CERT  sizeof_ca_cert_der_2048
    static const char msg[] = "Are you listening wolfSSL Server?";
#else
    #ifndef SERVER_IP
        #define SERVER_IP "www.wolfssl.com"
    #endif
	#ifndef DEFAULT_PORT
		#define DEFAULT_PORT 443
	#endif
    #define CERT         wolfssl_website_root_ca
    #define SIZEOF_CERT  sizeof_wolfssl_website_root_ca
    static const char msg[] = "GET /index.html HTTP/1.1\r\n\r\n";
#endif

/* Math: Normal (!USE_FAST_MATH) */
#define SIZEOF_LONG_LONG 8
#define WC_RSA_BLINDING
#define ECC_TIMING_RESISTANT

/* Enable options */
#define HAVE_CHACHA
#define HAVE_POLY1305
#define HAVE_ECC
#define HAVE_SUPPORTED_CURVES
#define HAVE_TLS_EXTENSIONS
#define HAVE_ONE_TIME_AUTH
#define HAVE_TRUNCATED_HMAC
#define HAVE_EXTENDED_MASTER
#define HAVE_ALPN
#define HAVE_SNI
#define HAVE_OCSP
#define HAVE_AESGCM

/* Disable options */
#define NO_PWDBASED
#define NO_DSA
#define NO_DES3
#define NO_RC4
#define NO_MD4

/* Benchmark / Testing */
#define BENCH_EMBEDDED
#define USE_CERT_BUFFERS_2048
#define USE_CERT_BUFFERS_256

/* OS */
#define SINGLE_THREADED

/* Filesystem */
#define NO_FILESYSTEM

/* Debug */
#define printf Log_Debug
#define WOLFIO_DEBUG

#endif /* _USER_SETTINGS_H_ */
