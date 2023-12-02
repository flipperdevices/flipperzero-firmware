/**************************************************************************//**
 * @file     system_M480.h
 * @version  V1.00
 * @brief    CMSIS Cortex-M4 Core Peripheral Access Layer Header File for M480
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef __SYSTEM_M480_H__
#define __SYSTEM_M480_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/

#ifndef __HSI
#define __HSI       (12000000UL)    /*!< PLL default output is 50MHz */
#endif

#ifndef __HXT
#define __HXT       (12000000UL)    /*!< External Crystal Clock Frequency     */
#endif

#ifndef __LXT
#define __LXT       (32768UL)       /*!< External Crystal Clock Frequency 32.768KHz */
#endif

#define __HIRC      (12000000UL)    /*!< Internal 12M RC Oscillator Frequency */
#define __LIRC      (10000UL)       /*!< Internal 10K RC Oscillator Frequency */
#define __SYS_OSC_CLK     (    ___HSI)    /* Main oscillator frequency        */


#define __SYSTEM_CLOCK    (1UL*__HXT)

extern uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */
extern uint32_t CyclesPerUs;         /*!< Cycles per micro second              */
extern uint32_t PllClock;            /*!< PLL Output Clock Frequency           */


/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the micro controller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);

/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock
 *         retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_M480_H__ */

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
