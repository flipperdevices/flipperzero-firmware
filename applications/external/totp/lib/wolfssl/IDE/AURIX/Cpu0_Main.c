/* Cpu0_Main.c
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

/* Infineon includes */
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "IfxAsclin_Asc.h"
#include "IfxCpu_Irq.h"
#include "IfxPort.h"
#include "SysSe/Bsp/Bsp.h"

/* For mapping stdio printf */
#include <stdio.h>
#include <string.h>

/* used to wait for CPU sync event */
IFX_ALIGN(4) IfxCpu_syncEvent g_cpuSyncEvent = 0;

#define SERIAL_BAUDRATE         115200  /* Baud rate in bit/s */
#define SERIAL_PIN_RX           IfxAsclin0_RXA_P14_1_IN /* RX pin of the board */
#define SERIAL_PIN_TX           IfxAsclin0_TX_P14_0_OUT /* TX pin of the board */
#define INTPRIO_ASCLIN0_TX      19  /* Priority of the ISR */
#define ASC_TX_BUFFER_SIZE      128 /* Definition of the buffer size */

/* Declaration of the ASC handle */
static IfxAsclin_Asc g_asc;

/* Declaration of the FIFOs parameters:
 * The transfer buffers allocate memory for the data itself and for FIFO runtime
 * variables. 8 more bytes have to be added to ensure a proper circular buffer
 * handling independent from the address to which the buffers have been located.
 */
static uint8 g_ascTxBuffer[ASC_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];

/******************************************************************************/
/*----Function Implementations------------------------------------------------*/
/******************************************************************************/

/* Re-target the C library printf function to the asc lin. */
int fputc(int ch, FILE *f)
{
    Ifx_SizeT count;
    /* convert to CRLF */
    if (ch == (int)'\n') {
        int chcr = (int)'\r';
        count = 1;
        IfxAsclin_Asc_write(&g_asc, &chcr, &count, TIME_INFINITE);    
    }
    count = 1;
    IfxAsclin_Asc_write(&g_asc, &ch, &count, TIME_INFINITE);
    return ch;
}

/* Add the Interrupt Service Routine */
IFX_INTERRUPT(asclin0_Tx_ISR, 0, INTPRIO_ASCLIN0_TX);
void asclin0_Tx_ISR(void)
{
    IfxAsclin_Asc_isrTransmit(&g_asc);
}

static void init_UART(void)
{
    IfxAsclin_Asc_Config ascConfig;

    IfxCpu_Irq_installInterruptHandler(asclin0_Tx_ISR, INTPRIO_ASCLIN0_TX);

    /* Port pins configuration */
    const IfxAsclin_Asc_Pins pins = {
        NULL_PTR,         IfxPort_InputMode_pullUp,     /* CTS pin not used   */
        &SERIAL_PIN_RX,   IfxPort_InputMode_pullUp,     /* RX pin             */
        NULL_PTR,         IfxPort_OutputMode_pushPull,  /* RTS pin not used   */
        &SERIAL_PIN_TX,   IfxPort_OutputMode_pushPull,  /* TX pin             */
        IfxPort_PadDriver_cmosAutomotiveSpeed1
    };

    /* Initialize an instance of IfxAsclin_Asc_Config with default values */
    IfxAsclin_Asc_initModuleConfig(&ascConfig, SERIAL_PIN_TX.module);

    /* Set the desired baud rate */
    ascConfig.baudrate.baudrate = SERIAL_BAUDRATE;

    /* ISR priorities and interrupt target */
    ascConfig.interrupt.txPriority = INTPRIO_ASCLIN0_TX;
    ascConfig.interrupt.typeOfService = IfxCpu_Irq_getTos(IfxCpu_getCoreIndex());

    /* FIFO configuration */
    ascConfig.txBuffer = &g_ascTxBuffer;
    ascConfig.txBufferSize = ASC_TX_BUFFER_SIZE;

    ascConfig.pins = &pins;

    /* Initialize module with above parameters  */
    IfxAsclin_Asc_initModule(&g_asc, &ascConfig);

    /* Turn off buffers, so I/O occurs immediately */
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

int send_UART(const char* str)
{
    Ifx_SizeT count = (Ifx_SizeT)strlen(str);
    IfxAsclin_Asc_write(&g_asc, str, &count, TIME_INFINITE);
    return (int)count;
}

void core0_main(void)
{
    IfxCpu_enableInterrupts();

    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    /* Initialize the UART to board VCOM */
    init_UART();

    /* bare metal loop */
    while(1)
    {
        extern void run_wolf_tests(void);
        run_wolf_tests();

        /* wait 5 seconds */
        waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 5 * 1000));
    } /* while */
}
