/* main.c
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

/* IoT-safe example
 * main for STM32L4
 */

#include <stdio.h>
#include "devices.h"
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/port/iotsafe/iotsafe.h>

#define STDIO USART2_BASE
#define MODEM  USART1_BASE

#define RX_TIMEOUT 100


extern volatile unsigned long jiffies;


int usart1_read(char *buf, int len)
{
    int i = 0;
    char c;
    int ret;
    volatile unsigned long start = jiffies;
    memset(buf, 0, len);
    do {
        ret = usart_rx(MODEM, &c);
        if (ret > 0) {
            buf[i++] = c;
            if (c == '\n') {
                return i;
            }
        } else {
            __asm__ volatile("wfi");
        }

    } while(((jiffies - start) < RX_TIMEOUT) && (i < len));
    if (i == 0) {
        return 0;
    }
    return i;
}

int usart1_write(const char *buf, int len)
{
    int i = 0;
    do {
        usart_tx(MODEM, buf[i++]);
    } while(i < len);
    return len;
}

extern int memory_tls_test(void);

void main(void)
{
    uint32_t last_mark = 0;
    int i;
    uint8_t randombytes[16];
    int ret;
    char c;
    WC_RNG rng;
    clock_pll_on();
    systick_enable();
    if (usart_init(STDIO, 115200, 8, 'N', 1) < 0)
        while(1)
            ;
    printf("wolfSSL IoT-SAFE demo\n");

    printf("Press a key to continue...\n");
    do {
        ret = usart_rx(STDIO, &c);
    } while (ret <= 0);
    printf("%c\n", c);

    printf("Initializing modem...\n");
    stmod_modem_enable();
    sleep_ms(1000);
    printf("System up and running\r\n");
    sleep_ms(1000);

    printf("Initializing wolfSSL...\n");
    wolfSSL_Init();
    wolfSSL_Debugging_ON();

    printf("Initializing modem port\n");
    if(usart_init(MODEM, 115200, 8, 'N', 1) < 0)
        while(1)
            ;
    printf("Initializing IoTSafe I/O...\n");
    wolfIoTSafe_SetCSIM_read_cb(usart1_read);
    wolfIoTSafe_SetCSIM_write_cb(usart1_write);

    /* IoT-Safe system up and running. */
    printf("Initializing RNG...\n");
    wc_InitRng(&rng);

    printf("Getting RND...\n");
    wc_RNG_GenerateBlock(&rng, randombytes, 16);
    wc_FreeRng(&rng);
    printf("Random bytes: ");
    for (i = 0; i < 16; i++) {
        printf("%02X", randombytes[i]);
    }
    printf("\n");

    memory_tls_test();

    while(1) {
        if (usart_rx(STDIO, &c)) {
            usart_tx(MODEM, c);
            usart_tx(STDIO, c);
        }
        if (usart_rx(MODEM, &c)) {
            usart_tx(STDIO, c);
        }
        __asm__ volatile("wfi");
    }
}
