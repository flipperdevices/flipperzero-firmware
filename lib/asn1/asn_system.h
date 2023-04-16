/*
 * Copyright (c) 2003-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * Miscellaneous system-dependent types.
 */
#ifndef	ASN_SYSTEM_H
#define	ASN_SYSTEM_H

#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif

#ifndef _BSD_SOURCE
#define _BSD_SOURCE /* for snprintf() on some linux systems  */
#endif

#include <stdio.h>	/* For snprintf(3) */
#include <stdlib.h>	/* For *alloc(3) */
#include <string.h>	/* For memcpy(3) */
#include <sys/types.h>	/* For size_t */
#include <limits.h>	/* For LONG_MAX */
#include <stdarg.h>	/* For va_start */
#include <stddef.h>	/* for offsetof and ptrdiff_t */

#ifdef	_WIN32

#include <malloc.h>
#define	 snprintf	_snprintf
#define	 vsnprintf	_vsnprintf

/* To avoid linking with ws2_32.lib, here's the definition of ntohl() */
#define sys_ntohl(l)	((((l) << 24)  & 0xff000000)	\
			| (((l) << 8) & 0xff0000)	\
			| (((l) >> 8)  & 0xff00)	\
			| ((l >> 24) & 0xff))

#ifdef _MSC_VER			/* MSVS.Net */
#ifndef __cplusplus
#define inline __inline
#endif
#ifndef	ASSUMESTDTYPES	/* Standard types have been defined elsewhere */
#define	ssize_t		SSIZE_T
#if _MSC_VER < 1600
typedef	char		int8_t;
typedef	short		int16_t;
typedef	int		int32_t;
typedef	unsigned char	uint8_t;
typedef	unsigned short	uint16_t;
typedef	unsigned int	uint32_t;
#else /* _MSC_VER >= 1600 */
#include <stdint.h>
#endif /* _MSC_VER < 1600 */
#endif	/* ASSUMESTDTYPES */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <float.h>
#define isnan _isnan
#define finite _finite
#define copysign _copysign
#define	ilogb	_logb
#else	/* !_MSC_VER */
#include <stdint.h>
#endif	/* _MSC_VER */

#else	/* !_WIN32 */

#if defined(__vxworks)
#include <types/vxTypes.h>
#else	/* !defined(__vxworks) */

#include <inttypes.h>	/* C99 specifies this file */
#include <netinet/in.h> /* for ntohl() */
#define	sys_ntohl(foo)	ntohl(foo)
#endif	/* defined(__vxworks) */

#endif	/* _WIN32 */

#if	__GNUC__ >= 3 || defined(__clang__)
#define CC_ATTRIBUTE(attr)    __attribute__((attr))
#else
#define CC_ATTRIBUTE(attr)
#endif
#define CC_PRINTFLIKE(fmt, var)     CC_ATTRIBUTE(format(printf, fmt, var))
#define	CC_NOTUSED                  CC_ATTRIBUTE(unused)
#ifndef CC_ATTR_NO_SANITIZE
#define CC_ATTR_NO_SANITIZE(what)   CC_ATTRIBUTE(no_sanitize(what))
#endif

/* Figure out if thread safety is requested */
#if !defined(ASN_THREAD_SAFE) && (defined(THREAD_SAFE) || defined(_REENTRANT))
#define	ASN_THREAD_SAFE
#endif	/* Thread safety */

#ifndef	offsetof	/* If not defined by <stddef.h> */
#define	offsetof(s, m)	((ptrdiff_t)&(((s *)0)->m) - (ptrdiff_t)((s *)0))
#endif	/* offsetof */

#ifndef	MIN		/* Suitable for comparing primitive types (integers) */
#if defined(__GNUC__)
#define	MIN(a,b)	({ __typeof a _a = a; __typeof b _b = b;	\
	((_a)<(_b)?(_a):(_b)); })
#else	/* !__GNUC__ */
#define	MIN(a,b)	((a)<(b)?(a):(b))	/* Unsafe variant */
#endif /* __GNUC__ */
#endif	/* MIN */

#if __STDC_VERSION__ >= 199901L
#ifndef SIZE_MAX
#define SIZE_MAX   ((~((size_t)0)) >> 1)
#endif

#ifndef RSIZE_MAX   /* C11, Annex K */
#define RSIZE_MAX   (SIZE_MAX >> 1)
#endif
#ifndef RSSIZE_MAX   /* Halve signed size even further than unsigned */
#define RSSIZE_MAX   ((ssize_t)(RSIZE_MAX >> 1))
#endif
#else   /* Old compiler */
#undef  SIZE_MAX
#undef  RSIZE_MAX
#undef  RSSIZE_MAX
#define SIZE_MAX   ((~((size_t)0)) >> 1)
#define RSIZE_MAX   (SIZE_MAX >> 1)
#define RSSIZE_MAX   ((ssize_t)(RSIZE_MAX >> 1))
#endif

#if __STDC_VERSION__ >= 199901L
#define ASN_PRI_SIZE "zu"
#define ASN_PRI_SSIZE "zd"
#define ASN_PRIuMAX PRIuMAX
#define ASN_PRIdMAX PRIdMAX
#else
#define ASN_PRI_SIZE "lu"
#define ASN_PRI_SSIZE "ld"
#if LLONG_MAX > LONG_MAX
#define ASN_PRIuMAX "llu"
#define ASN_PRIdMAX "lld"
#else
#define ASN_PRIuMAX "lu"
#define ASN_PRIdMAX "ld"
#endif
#endif

#endif	/* ASN_SYSTEM_H */
