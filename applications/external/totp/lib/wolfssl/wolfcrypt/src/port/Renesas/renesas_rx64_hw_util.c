/* renesas_rx64_hw_util.c
 *
 * Contributed by Johnson Controls Tyco IP Holdings LLP.
 *
 * Use of this Software is subject to the GPLv2 License
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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

#if defined(WOLFSSL_RENESAS_RX64_HASH)

#include <wolfssl/wolfcrypt/port/Renesas/renesas-rx64-hw-crypt.h>

#include <stdio.h>
/* mutex */
static wolfSSL_Mutex rx64_hw_mutex;
static int rx64_hw_CryptHwMutexInit_ = 0;

/*
* lock hw engine.
* this should be called before using engine.
*/
int rx64_hw_lock(void)
{
    int ret = 0;

    WOLFSSL_MSG("enter rx64_hw_lock");

    if (rx64_hw_CryptHwMutexInit_ == 0){
        ret = wc_InitMutex(&rx64_hw_mutex);
        if (ret == 0) {
            rx64_hw_CryptHwMutexInit_ = 1;
        } else {
            WOLFSSL_MSG(" mutex initialization failed.");
            return -1;
        }
    }
    if (wc_LockMutex(&rx64_hw_mutex) != 0) {
        /* this should not happen */
        return -1;
    }

    WOLFSSL_MSG("leave rx64_hw_lock");
    return ret;
}

/*
* release hw engine
*/
void rx64_hw_unlock(void)
{
    WOLFSSL_MSG("enter rx64_hw_unlock");
    /* unlock hw engine for next use */
    wc_UnLockMutex(&rx64_hw_mutex);
    WOLFSSL_MSG("leave rx64_hw_unlock");
}

/* open RX64 HW drivers for use */
void rx64_hw_Open(void)
{
    int ret = -1;
    if (rx64_hw_lock() == 0) {
        /* Enable the SHA coprocessor function. */
        R_Sha_Init();
        /* unlock hw */
        rx64_hw_unlock();
        ret = 0;
    } else {
        WOLFSSL_MSG("Failed to lock rx64 hw \n");
    }
    return ret;
}

/* close RX64 HW driver */
void rx64_hw_Close(void)
{
    if (rx64_hw_lock() == 0) {
        /* Disable the SHA coprocessor function. */
        R_Sha_Close();
        /* unlock hw */
        rx64_hw_unlock();
    } else {
        WOLFSSL_MSG("Failed to unlock rx64 hw \n");
    }
}


#endif /* WOLFSSL_RENESAS_RX64_HASH */
