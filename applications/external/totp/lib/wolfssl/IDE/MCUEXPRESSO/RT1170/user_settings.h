#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

#define FREERTOS

#define WOLFSSL_NO_REALLOC
#define SINGLE_THREADED
#define NO_FILESYSTEM
#define WOLFSSL_LWIP
#define HAVE_ECC

#define WOLFSSL_SHA224
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512

#define WOLFSSL_AES_COUNTER
#define OPENSSL_EXTRA
#define WOLFSSL_NO_SOCK

#define WOLFSSL_KEY_GEN
#define WOLFSSL_CERT_GEN
#define WOLFSSL_CERT_REQ
#define WOLFSSL_CERT_EXT

#define HAVE_PKCS7
#ifdef HAVE_PKCS7
    #define WOLFSSL_AES_DIRECT
    #define HAVE_AES_KEYWRAP
    #define HAVE_X963_KDF
    #define HAVE_SMIME
#endif

#define WOLFCRYPT_ONLY
#define USE_FAST_MATH

/* harden options */
#ifdef USE_FAST_MATH
    #define TFM_TIMING_RESISTANT
#endif
#ifndef NO_RSA
    #define WC_RSA_BLINDING
#endif
#ifdef HAVE_ECC
    #define ECC_TIMING_RESISTANT
#endif

#define WOLFSSL_USE_ALIGN
#define WOLFSSL_IMXRT1170_CAAM
#define SIZEOF_LONG_LONG 8
#define SIZEOF_LONG 4
#define WOLF_CRYPTO_CB

/* using the RTC */
//#define NO_ASN_TIME
#ifndef NO_ASN_TIME
	#define FREESCALE_SNVS_RTC
#endif

#define NO_CRYPT_TEST
#ifndef NO_CRYPT_TEST
    #define NO_MAIN_DRIVER

    #include <stdio.h>
    #include <stdarg.h>
    static void myPrintf(const char* fmt, ...)
    {
	    int ret;
	    char line[150];
	    va_list ap;

	    va_start(ap, fmt);
	    ret = vsnprintf(line, sizeof(line), fmt, ap);
	    line[sizeof(line)-1] = '\0';

	    DbgConsole_Printf("%s", line);

	    /* add CR on newlines */
	    if (ret > 0 && line[ret-1] == '\n') {
	        DbgConsole_Printf("\r");
        }
    }
    #define XPRINTF myPrintf
    #define USE_CERT_BUFFERS_2048
    #define USE_CERT_BUFFERS_1024
    #define USE_CERT_BUFFERS_256
    #define NO_WRITE_TEMP_FILES
    #define BENCH_EMBEDDED
#endif

#endif /* USER_SETTINGS_H */
