/* cpuid.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#include <wolfssl/wolfcrypt/cpuid.h>

#if defined(HAVE_CPUID) || defined(HAVE_CPUID_INTEL)
    static word32 cpuid_check = 0;
    static word32 cpuid_flags = 0;
#endif

#ifdef HAVE_CPUID_INTEL
    /* Each platform needs to query info type 1 from cpuid to see if aesni is
     * supported. Also, let's setup a macro for proper linkage w/o ABI conflicts
     */

    #ifndef _MSC_VER
        #define cpuid(reg, leaf, sub)\
            __asm__ __volatile__ ("cpuid":\
                "=a" ((reg)[0]), "=b" ((reg)[1]), "=c" ((reg)[2]), "=d" ((reg)[3]) :\
                "a" (leaf), "c"(sub));
    #else
        #include <intrin.h>

        #define cpuid(a,b,c) __cpuidex((int*)a,b,c)
    #endif /* _MSC_VER */

    #define EAX 0
    #define EBX 1
    #define ECX 2
    #define EDX 3

    static word32 cpuid_flag(word32 leaf, word32 sub, word32 num, word32 bit)
    {
        int got_intel_cpu = 0;
        int got_amd_cpu = 0;
        unsigned int reg[5];
        reg[4] = '\0';
        cpuid(reg, 0, 0);

        /* check for Intel cpu */
        if (XMEMCMP((char *)&(reg[EBX]), "Genu", 4) == 0 &&
            XMEMCMP((char *)&(reg[EDX]), "ineI", 4) == 0 &&
            XMEMCMP((char *)&(reg[ECX]), "ntel", 4) == 0) {
            got_intel_cpu = 1;
        }

        /* check for AMD cpu */
        if (XMEMCMP((char *)&(reg[EBX]), "Auth", 4) == 0 &&
            XMEMCMP((char *)&(reg[EDX]), "enti", 4) == 0 &&
            XMEMCMP((char *)&(reg[ECX]), "cAMD", 4) == 0) {
            got_amd_cpu = 1;
        }

        if (got_intel_cpu || got_amd_cpu) {
            cpuid(reg, leaf, sub);
            return ((reg[num] >> bit) & 0x1);
        }
        return 0;
    }


    void cpuid_set_flags(void)
    {
        if (!cpuid_check) {
            if (cpuid_flag(1, 0, ECX, 28)) { cpuid_flags |= CPUID_AVX1  ; }
            if (cpuid_flag(7, 0, EBX,  5)) { cpuid_flags |= CPUID_AVX2  ; }
            if (cpuid_flag(7, 0, EBX,  8)) { cpuid_flags |= CPUID_BMI2  ; }
            if (cpuid_flag(1, 0, ECX, 30)) { cpuid_flags |= CPUID_RDRAND; }
            if (cpuid_flag(7, 0, EBX, 18)) { cpuid_flags |= CPUID_RDSEED; }
            if (cpuid_flag(1, 0, ECX, 25)) { cpuid_flags |= CPUID_AESNI ; }
            if (cpuid_flag(7, 0, EBX, 19)) { cpuid_flags |= CPUID_ADX   ; }
            if (cpuid_flag(1, 0, ECX, 22)) { cpuid_flags |= CPUID_MOVBE ; }
            if (cpuid_flag(7, 0, EBX,  3)) { cpuid_flags |= CPUID_BMI1  ; }

            cpuid_check = 1;
        }
    }
#elif defined(HAVE_CPUID)
    void cpuid_set_flags(void)
    {
        if (!cpuid_check) {
            cpuid_flags = 0;
            cpuid_check = 1;
        }
    }
#endif

#ifdef HAVE_CPUID

    word32 cpuid_get_flags(void)
    {
        if (!cpuid_check)
            cpuid_set_flags();
        return cpuid_flags;
    }

    void cpuid_select_flags(word32 flags)
    {
        cpuid_flags = flags;
    }

    void cpuid_set_flag(word32 flag)
    {
        cpuid_flags |= flag;
    }

    void cpuid_clear_flag(word32 flag)
    {
        cpuid_flags &= ~flag;
    }

#endif /* HAVE_CPUID */
