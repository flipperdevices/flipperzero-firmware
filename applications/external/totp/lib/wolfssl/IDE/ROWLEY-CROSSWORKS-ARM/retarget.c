/* retarget.c
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


#include "hw.h"
#include "user_settings.h"
#include <stdio.h>

void __assert(const char *__expression, const char *__filename, int __line)
{
    printf("Assert: %s, File %s (%d)\n", __expression, __filename, __line);
}

unsigned long ksdk_time(unsigned long* timer)
{
    (void)timer;
    return hw_get_time_sec();
}

unsigned int LowResTimer(void)
{
    return hw_get_time_sec();
}

double current_time(int reset)
{
    double time;
    (void)reset;
    time = hw_get_time_sec();
    time += (double)hw_get_time_msec() / 1000;
    return time;
}

unsigned int custom_rand_generate(void)
{
    return hw_rand();
}

int custom_rand_generate_block(unsigned char* output, unsigned int sz)
{
    uint32_t i = 0;

    while (i < sz)
    {
        /* If not aligned or there is odd/remainder */
        if( (i + sizeof(CUSTOM_RAND_TYPE)) > sz ||
            ((uint32_t)&output[i] % sizeof(CUSTOM_RAND_TYPE)) != 0
        ) {
            /* Single byte at a time */
            output[i++] = (unsigned char)custom_rand_generate();
        }
        else {
            /* Use native 8, 16, 32 or 64 copy instruction */
            *((CUSTOM_RAND_TYPE*)&output[i]) = custom_rand_generate();
            i += sizeof(CUSTOM_RAND_TYPE);
        }
    }

    return 0;
}

// Debug print handler
int __putchar(int c, __printf_tag_ptr ctx)
{
    hw_uart_printchar(c);
}

/* C library support function to write buffer (always to UART) */
int __write(int __fildes, const unsigned char *__buf, unsigned __len)
{
    (void)__fildes;
    for (unsigned i = 0; i < __len; i++) {
        hw_uart_printchar((int)__buf[i]);
    }
}

extern unsigned char __stack_process_start__[];
unsigned char * __aeabi_read_tp(void)
{
    // thread-local storage addressing refers to the thread pointer
    // This is returning start address of stack process
    return (__stack_process_start__);
}

/* Stubs */
void __heap_lock(void)
{
}

void __heap_unlock(void)
{
}

void __printf_lock(void)
{
}

void __printf_unlock(void)
{
}

void __scanf_lock(void)
{
}

void __scanf_unlock(void)
{
}

void __debug_io_lock(void)
{
}

void __debug_io_unlock(void)
{
}
