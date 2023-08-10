/* MSP430 example main.c
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


#include <wolfssl/wolfcrypt/settings.h>

#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/curve25519.h>
#include <wolfssl/wolfcrypt/chacha20_poly1305.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <msp430f5659.h>

/* Without __root on some of the functions, IAR's "Discard Unused Publics"
   will optimize out some of the functions
 */
#if defined(__IAR_SYSTEMS_ICC__)
#define IAR_KEEP __root
#else
#define IAR_KEEP
#endif

#define ECC_256_BIT_FIELD 32 /* 256-bit curve field */

#define WOLF_GEN_MEM    (2*1024)
#define CHACHA_TEST_LEN 1024

static byte gWolfMem[WOLF_GEN_MEM];
static byte generatedCiphertext[CHACHA_TEST_LEN];
static byte generatedPlaintext[CHACHA_TEST_LEN];

#define MCLK_FREQ_MHZ 8 /* MCLK = 8MHz */

static const byte  key[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
};

static const byte plaintext[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras lacus odio, pretium vel sagittis ac, facilisis quis diam. Vivamus condimentum velit sed dolor consequat interdum. Etiam eleifend ornare felis, eleifend egestas odio vulputate eu. Sed nec orci nunc. Etiam quis mi augue. Donec ullamcorper suscipit lorem, vel luctus augue cursus fermentum. Etiam a porta arcu, in convallis sem. Integer efficitur elementum diam, vel scelerisque felis posuere placerat. Donec vestibulum sit amet leo sit amet tincidunt. Etiam et vehicula turpis. Phasellus quis finibus sapien. Sed et tristique turpis. Nullam vitae sagittis tortor, et aliquet lorem. Cras a leo scelerisque, convallis lacus ut, fermentum urna. Mauris quis urna diam. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Nam aliquam vehicula orci id pulvinar. Proin mollis, libero sollicitudin tempor ultrices, massa augue tincidunt turpis, sit amet aliquam neque nibh nec dui. Fusce finibus massa quis rutrum suscipit cras amet";

static const byte  iv[] = {
    0x07, 0x00, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43,
    0x44, 0x45, 0x46, 0x47
};

static const byte  aad[] = { /* additional data */
    0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7
};

volatile unsigned int seconds;

IAR_KEEP unsigned int msp430_time(long *x)
{
    return seconds;
}

static void print_secret(char* who, byte* s, int sLen)
{
    int i;
    printf("%ss' Secret: ", who);
    for (i = 0; i < sLen; i++) {
        printf("%02x", s[i]);
    }
    printf("\r\n");
}

/* This is a very crude RNG, do not use in production */
IAR_KEEP unsigned int msp430_rnd(void)
{
    unsigned int result = TA0R ^ TA2R;
    printf("Rand generated: %d\r\n", result);
    return result;
}

static void uart_init()
{
    P8SEL |= BIT3 + BIT2;
    UCA1CTLW0 = UCSWRST;                      /* Put eUSCI in reset */
    UCA1CTLW0 |= UCSSEL__SMCLK;               /* CLK = SMCLK */
    /* Baud Rate calculation
       This was calculated to produce 115200 for a 16MHz clock, so it produces
       57600 at 8MHz
       16000000/(16*115200) = 8.6805
       Fractional portion = 0.6805
       Use Table 24-5 in Family User Guide
    */
    UCA1BR0 = 8;
    UCA1BR1 = 0x00;
    UCA1MCTL |= UCOS16 | UCBRF_11 | UCBRS_0;
    UCA1CTLW0 &= ~UCSWRST;                    /* Initialize eUSCI */
    UCA1IE |= UCRXIE;                         /* Enable USCI_A0 RX interrupt */
}

#if defined(__IAR_SYSTEMS_ICC__)
IAR_KEEP size_t __write(int fd, const unsigned char *_ptr, size_t len)
#else
int write(int fd, const char *_ptr, int len)
#endif
{
  size_t i;

  for(i=0 ; i<len ; i++) {
    while(!(UCA1IFG&UCTXIFG));
    UCA1TXBUF = (unsigned char) _ptr[i];
  }

  return len;
}

static void SetVcoreUp (unsigned int level)
{
    /* Change VCORE voltage level */
    PMMCTL0_H = PMMPW_H;                    /* Open PMM registers for write */
    SVSMHCTL = SVSHE                        /* Set SVS/SVM high side new level */
           + SVSHRVL0 * level
           + SVMHE
           + SVSMHRRL0 * level;
    SVSMLCTL = SVSLE                        /* Set SVM low side to new level */
           + SVMLE
           + SVSMLRRL0 * level;
    while ((PMMIFG & SVSMLDLYIFG) == 0);    /* Wait till SVM is settled */
    PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);      /* Clear already set flags */
    PMMCTL0_L = PMMCOREV0 * level;          /* Set VCore to new level */
    if ((PMMIFG & SVMLIFG))                 /* Wait till new level reached */
    while ((PMMIFG & SVMLVLRIFG) == 0);
    SVSMLCTL = SVSLE                        /* Set SVS/SVM low side to new level */
          + SVSLRVL0 * level
          + SVMLE
          + SVSMLRRL0 * level;
    PMMCTL0_H = 0x00;                       /* Lock PMM registers for write access */
}

/* Stop WDT
   We need to do this before main() because when there is ~4K of data to
   initialize the watchdog will fire before initialization completes, sending
   it into an endless loop of nothing.
   See: https://www.iar.com/knowledge/support/technical-notes/general/my-msp430-does-not-start-up/
*/
#if defined(__IAR_SYSTEMS_ICC__)
int __low_level_init()
{
    WDTCTL = WDTPW | WDTHOLD;
    return 1;
}
#else
static void __attribute__((naked, used, section(".crt_0042")))
disable_watchdog (void)
{
    WDTCTL = WDTPW | WDTHOLD;
}
#endif

int main(void)
{
    byte generatedAuthTag[16];
    WOLFSSL_HEAP_HINT* HEAP_HINT = NULL;
    int ret;
    int start;

    /* NOTE: Change core voltage one level at a time.. */
    SetVcoreUp (0x01);
    SetVcoreUp (0x02);
    SetVcoreUp (0x03);

    /* USC module configuration, Fdcoclockdiv = Fmclk = 8MHz */
    UCSCTL8 &= ~SMCLKREQEN;         /* disable SMCLK clock requests */
    UCSCTL3 = (0*FLLREFDIV0)        /* FLL ref divider 1 */
            + SELREF2;              /* set REFOCLK as FLL reference clock source */
    UCSCTL4 = SELA__REFOCLK         /* ACLK = REFO */
            + SELM__DCOCLKDIV       /* MCLK = DCOCLKDIV */
            + SELS__DCOCLKDIV;      /* SMCLK = DCOCLKDIV */
    __bis_SR_register(SCG0);        /* disable FLL operation */
    UCSCTL0	= 0x0000;               /* lowest possible DCO, MOD */
    UCSCTL1 = DISMOD_L              /* modulation disabled */
            + DCORSEL_6;            /* DCO range for 8MHz operation */

    UCSCTL2 = FLLD_0                /* D=FLLD=1, so that Fdco=8MHz */
            + 243;                  /* DCO multiplier for 8MHz */
                                    /* (N + 1) * FLLRef = Fdcodiv */
                                    /* (243 + 1) * 32768 = 8MHz (multiplier N = 243) */
    __bic_SR_register(SCG0);        /* re-enable FLL operation */

    /* worst-case settling time for the DCO when the DCO range bits have been
       32 x 32 x 8 MHz / 32,768 Hz = 250000 = MCLK cycles for DCO to settle
    */
    __delay_cycles(250000);

    TA0CCR0 = 32768-1;
    TA0CCTL0 |= CCIE;
    TA0CTL = TASSEL_1 + ID_0 + MC_1;
    TA2CTL = TASSEL_2 | MC_2;
    P1DIR = 1;
    P1OUT = 0;
    uart_init();
#if defined(__IAR_SYSTEMS_ICC__)
    __enable_interrupt();
#else
    _enable_interrupts();
#endif
    printf("START!\r\n");
#ifdef HAVE_ECC
    WC_RNG rng;
    ecc_key AliceKey, BobKey;

    byte AliceSecret[ECC_256_BIT_FIELD] = {0};
    byte BobSecret[ECC_256_BIT_FIELD] = {0};
    word32 secretLen = 0;

    if (wc_LoadStaticMemory(&HEAP_HINT, gWolfMem, sizeof(gWolfMem),
        WOLFMEM_GENERAL, 10) != 0) {
    	printf("unable to load static memory");
    }

    ret = wc_InitRng_ex(&rng, HEAP_HINT, INVALID_DEVID);
    if (ret != 0) {
        printf("RNG init fail: %d\r\n", ret);
        return ret;
    }
    printf("RNG init\r\n");

    ret = wc_ecc_init(&AliceKey);
    if (ret != 0) {
        printf("Alice init fail\r\n");
        goto only_rng;
    }
    printf("Alice init\r\n");

    ret = wc_ecc_init(&BobKey);
    if (ret != 0) {
        printf("Bob init fail\r\n");
        goto alice_and_rng;
    }
    printf("Bob init\r\n");

    start = seconds;
    ret = wc_ecc_make_key(&rng, ECC_256_BIT_FIELD, &AliceKey);
    if (ret != 0) {
        printf("Alice keygen fail\r\n");
        goto all_three;
    }
    printf("Alice keygen %d seconds\r\n", seconds - start);
    start = seconds;
    ret = wc_ecc_make_key(&rng, ECC_256_BIT_FIELD, &BobKey);
    if (ret != 0) {
        printf("Bob keygen fail\r\n");
        goto all_three;
    }
    printf("Bob keygen %d seconds\r\n", seconds - start);
    start = seconds;
    secretLen = ECC_256_BIT_FIELD; /* explicit set */
    ret = wc_ecc_shared_secret(&AliceKey, &BobKey, AliceSecret, &secretLen);
    if (ret != 0) {
        printf("Shared secret fail\r\n");
        goto all_three;
    }
    printf("Bob secret %d seconds\r\n", seconds - start);
    start = seconds;
    secretLen = ECC_256_BIT_FIELD; /* explicit reset for best practice */
    ret = wc_ecc_shared_secret(&BobKey, &AliceKey, BobSecret, &secretLen);
    if (ret == 0) {
        if (XMEMCMP(AliceSecret, BobSecret, secretLen))
            printf("Failed to generate a common secret\n");
    } else {
        goto all_three;
    }
    printf("Alice secret %d seconds\r\n", seconds - start);

    printf("Successfully generated a common secret\r\n");
    print_secret("Alice", AliceSecret, (int) secretLen);
    print_secret("Bob", BobSecret, (int) secretLen);

all_three:
    wc_ecc_free(&BobKey);
alice_and_rng:
    wc_ecc_free(&AliceKey);
only_rng:
    wc_FreeRng(&rng);

    printf(
        "ChaCha20/Poly1305 Encryption Start, 1000 iterations, %d bytes\r\n",
        (int)strlen((const char*)plaintext));
    start = seconds;
    for (int i=0; i <= 1000; i++) {
        ret = wc_ChaCha20Poly1305_Encrypt(key, iv, aad, sizeof(aad), plaintext,
                strlen((const char*)plaintext), generatedCiphertext,
                generatedAuthTag);
        if (ret) {
            printf("ChaCha error: %d\r\n", ret);
            break;
        }
    }
    printf("\r\nEnd %d seconds\r\n", seconds - start);

    start = seconds;
    printf("ChaCha20/Poly1305 Decryption Start, 1000 iterations\r\n");
    start = seconds;
    for (int i=0; i <= 1000; i++) {
        ret = wc_ChaCha20Poly1305_Decrypt(key, iv, aad, sizeof(aad),
                generatedCiphertext, strlen((const char*)plaintext),
                generatedAuthTag, generatedPlaintext);
        if (ret) {
            printf("ChaCha error: %d\r\n", ret);
            break;
        }
    }
    printf("\r\nEnd %d seconds\r\n", seconds - start);
#else
    printf("Configure wolfSSL with --enable-ecc and try again\n");
    ret = -1;
#endif
    printf("end\r\n");
    while(1) {
        __delay_cycles(8000000);
    }
    return ret;
}

/* Timer ISR */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TIMER0_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    seconds++;
    P1OUT = seconds ^ 2;
    fprintf(stderr, ".");
}
