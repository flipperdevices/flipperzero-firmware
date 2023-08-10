#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

#define WOLFCRYPT_ONLY
#define HAVE_ECC
#define FP_ECC
//#define FP_ENTRIES 1
#define NO_DSA
#define NO_DH
#define NO_RSA
//#define DEBUG_WOLFSSL

#define USE_FAST_MATH
#define TFM_TIMING_RESISTANT
#ifdef HAVE_ECC
	#define ECC_TIMING_RESISTANT
#endif
#ifndef NO_RSA
	#define WC_RSA_BLINDING
#endif

#if 1
	#define WOLFSSL_HAVE_SP_RSA
	#define WOLFSSL_HAVE_SP_ECC
	#define WOLFSSL_SP_MATH

	#if 1
		/* ARM NEON instructions */
		#define WOLFSSL_SP_ARM64_ASM
	#endif
	#if 1
		/* Use DSP */
		#define WOLFSSL_DSP
	#endif
#endif

#endif
