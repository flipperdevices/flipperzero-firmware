/* time.c
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


/*-----------------------------------------------------------------------------
 *        initialize RTC
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include "lpc43xx_rtc.h"
#include "lpc43xx_cgu.h"

static void init_RTC()
{
    /* Enable GPIO register interface clock                                     */
    LPC_CCU1->CLK_M4_GPIO_CFG     |= 1;
    while (!(LPC_CCU1->CLK_M4_GPIO_STAT   & 1)) ;

    /* RTC Block section ------------------------------------------------------ */
    /* Init RTC module */
    RTC_Init(LPC_RTC);

    /* Set ALARM time for second */
    RTC_SetAlarmTime (LPC_RTC, RTC_TIMETYPE_SECOND, 30);

    /* Set the AMR for 30s match alarm interrupt */
    RTC_AlarmIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);

    /* Set the CIIR for minute counter interrupt*/
    RTC_CntIncrIntConfig (LPC_RTC, RTC_TIMETYPE_MINUTE, ENABLE);

    /* Enable rtc (starts increase the tick counter and second counter register) */
    RTC_Cmd(LPC_RTC, ENABLE);

}

/*-----------------------------------------------------------------------------
 *        initialize TIM
 *----------------------------------------------------------------------------*/

#include "lpc43xx_timer.h"

static void init_TIM()
{
    TIM_TIMERCFG_Type TIM_ConfigStruct;
    /* Initialize timer 0, prescale count time of 1uS */
    TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_TICKVAL;
    TIM_ConfigStruct.PrescaleValue  = 204;  /* 204MHz */
    /* Set configuration for Tim_config and Tim_MatchConfig */
    TIM_Init(LPC_TIMER2, TIM_TIMER_MODE,&TIM_ConfigStruct);
    TIM_ResetCounter(LPC_TIMER2);
    /* To start timer 2 */
    TIM_Cmd(LPC_TIMER2,ENABLE);
}

double current_time()
{
    return (double)LPC_TIMER2->TC/1000000.0;
}


void init_time(void) {
    init_RTC() ;
    init_TIM() ;
}

#include <time.h>

struct tm *Cyassl_MDK_gmtime(const time_t *c)
{
    static struct tm date ;

  	RTC_TIME_Type RTCFullTime;
    RTC_GetFullTime (LPC_RTC, &RTCFullTime);

    date.tm_year = RTCFullTime.YEAR + 100 ;
    date.tm_mon = RTCFullTime.MONTH - 1 ;
    date.tm_mday = RTCFullTime.DOM ;
    date.tm_hour = RTCFullTime.HOUR ;
    date.tm_min = RTCFullTime.MIN ;
    date.tm_sec = RTCFullTime.SEC ;

    #if defined(DEBUG_CYASSL)
    {
        extern void CYASSL_MSG(char *msg) ;
        char msg[100] ;
        sprintf(msg, "Debug::Cyassl_KEIL_gmtime(DATE=/%4d/%02d/%02d TIME=%02d:%02d:%02d)\n",
        RTCFullTime.YEAR+2000,  RTCFullTime.MONTH, RTCFullTime.DOM,
        RTCFullTime.HOUR,  RTCFullTime.MIN,  RTCFullTime.SEC) ;
        CYASSL_MSG(msg) ;
    }
    #endif

    return(&date) ;
}

typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

#include <stdio.h>

void time_main(void *args)
{
    char * datetime ;
    int year ;
  	RTC_TIME_Type RTCFullTime;

    if( args == NULL || ((func_args *)args)->argc == 1) {
        RTC_GetFullTime (LPC_RTC, &RTCFullTime);
        printf("Date: %d/%d/%d, Time: %02d:%02d:%02d\n",
        RTCFullTime.MONTH, RTCFullTime.DOM, RTCFullTime.YEAR+2000,
        RTCFullTime.HOUR,  RTCFullTime.MIN,  RTCFullTime.SEC) ;
    } else if(((func_args *)args)->argc == 3 &&
              ((func_args *)args)->argv[1][0] == '-' &&
              ((func_args *)args)->argv[1][1] == 'd' ) {

								datetime = ((func_args *)args)->argv[2];
        sscanf(datetime, "%d/%d/%d",
             (int *)&RTCFullTime.MONTH, (int *)&RTCFullTime.DOM, &year) ;
        RTCFullTime.YEAR = year - 2000 ;
        RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, RTCFullTime.MONTH);
        RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, RTCFullTime.YEAR);
        RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, RTCFullTime.DOM);
    } else if(((func_args *)args)->argc == 3 &&
              ((func_args *)args)->argv[1][0] == '-' &&
              ((func_args *)args)->argv[1][1] == 't' ) {
        RTC_GetFullTime (LPC_RTC, &RTCFullTime);
								datetime = ((func_args *)args)->argv[2];
        sscanf(datetime, "%d:%d:%d",
            (int *)&RTCFullTime.HOUR,
            (int *)&RTCFullTime.MIN,
            (int *)&RTCFullTime.SEC
        ) ;
        RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, RTCFullTime.SEC);
        RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, RTCFullTime.MIN);
        RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR, RTCFullTime.HOUR);
    } else printf("Invalid argument\n") ;
}
