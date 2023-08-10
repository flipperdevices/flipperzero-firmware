/* time-STM32F2xx.c
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

#include "time.h"

#define PERIPH_BASE           ((uint32_t)0x40000000)
/*-----------------------------------------------------------------------------
 *        initialize RTC
 *----------------------------------------------------------------------------*/
#include "stm32f2xx.h"

#define assert_param(a) /* null expansion */

#define RTC_RSF_MASK         ((uint32_t)0xFFFFFF5F)
#define SYNCHRO_TIMEOUT      ((uint32_t) 0x00008000)
#define Bcd2ToByte(v) \
   ((((uint8_t)(v & (uint8_t)0xF0) >> (uint8_t)0x4) * 10) + (v & (uint8_t)0x0F))
#define RTC_TR_RESERVED_MASK ((uint32_t)0x007F7F7F)
#define RTC_TR_MNT           ((uint32_t)0x00007000)
#define RTC_TR_MNU           ((uint32_t)0x00000F00)

#define PWR_OFFSET           (PWR_BASE - PERIPH_BASE)
#define CR_OFFSET            (PWR_OFFSET + 0x00)
#define DBP_BitNumber        0x08
#define CR_DBP_BB     (PERIPH_BB_BASE + (CR_OFFSET * 32) + (DBP_BitNumber * 4))
#define RTC_INIT_MASK        ((uint32_t)0xFFFFFFFF)
#define INITMODE_TIMEOUT     ((uint32_t) 0x00010000)

static void init_RTC()
{
    __IO uint32_t initcounter =  0x00 ;
    uint32_t initstatus = 0x00;    /* Enable the PWR clock : RCC_APB1Periph_PWR  */
    ((uint32_t *)RCC)[0x10] |= ((uint32_t)0x10000000) ;

    /* Allow access to RTC */
    *(__IO uint32_t *) CR_DBP_BB = ENABLE ;
      /* RCC_LSEConfig(RCC_LSE_ON) */
    *(__IO uint8_t *) (RCC_BASE + 0x70) =  ((uint8_t)0x00);
    /* Reset LSEBYP bit */
    *(__IO uint8_t *) (RCC_BASE + 0x70) =  ((uint8_t)0x00);
    *(__IO uint8_t *) (RCC_BASE + 0x70) =  ((uint8_t)0x01);
    /* Wait till LSE is ready */
        while((RCC->BDCR << 0x2) == 0x0) { }
      /* Select the RTC clock source: RCC_RTCCLKSource_LSE */
    ((RCC_TypeDef *)RCC)->BDCR |= (uint32_t)0x00000100;

    /* Enable the RTC Clock */
    *(__IO uint32_t *)  (PERIPH_BB_BASE + (((RCC_BASE - PERIPH_BASE)+ 0x70) * 32) + (0x0F* 4)) = (uint32_t)ENABLE;

    *(__IO uint32_t *) CR_DBP_BB = (uint32_t)ENABLE;
    RTC->ISR = (uint32_t) RTC_INIT_MASK;
    do {
      initstatus = RTC->ISR & RTC_ISR_INITF;
      initcounter++;
    } while((initcounter != INITMODE_TIMEOUT) && (initstatus == 0x00));

    /* Disable the write protection for RTC registers */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    RTC->CR &= ((uint32_t)~(RTC_CR_FMT));   /* Clear RTC CR FMT Bit */
    /* Set RTC_CR register */
    RTC->CR |=   ((uint32_t)0x00000000) ;   /* RTC_HourFormat_24 */

    /* Configure the RTC PRER */
    RTC->PRER = 0x7f ;
    RTC->PRER |= (uint32_t)(0xff << 16);

    /* Exit Initialization mode */
    RTC->ISR &= (uint32_t)~RTC_ISR_INIT;

    /* Enable the write protection for RTC registers */
    RTC->WPR = 0xFF;
}

/*-----------------------------------------------------------------------------
 *        initialize TIM
 *----------------------------------------------------------------------------*/
#define RCC_APB1Periph_TIM2              ((uint32_t)0x00000001)

static void init_TIM()
{
      uint16_t tmpcr1 = 0;

    ((uint32_t *)RCC)[0x10] |= RCC_APB1Periph_TIM2 ;

    tmpcr1 = TIM2->CR1 ;
    tmpcr1 &=   (uint16_t) (~(((uint16_t)0x0010) | ((uint16_t)0x0060) ));
                                     /* CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS) */
    tmpcr1 |= (uint16_t)0x0000  ;    /* CR1 |= TIM_CounterMode_Up */
    TIM2->CR1=  tmpcr1 ;

    TIM2->ARR = 0xffffffff ;         /* ARR= TIM_Period */
    TIM2->PSC = 60 ;                 /* PSC = TIM_Prescaler */
    TIM2->EGR = ((uint16_t)0x0001) ; /* EGR = TIM_PSCReloadMode_Immediate */

    *(uint16_t *)(PERIPH_BASE+0x0) |=((uint16_t)0x0001) ;
                                     /* TIM_Cmd(TIM2, ENABLE) ; */
}

void init_time(void) {
      init_RTC() ;
    init_TIM() ;
}

static void GetTime(uint8_t *h, uint8_t *m, uint8_t *s)
{
    uint32_t tmpreg = 0;
    tmpreg = (uint32_t)(RTC->TR & RTC_TR_RESERVED_MASK);
    *h = (uint8_t)Bcd2ToByte((uint8_t)((tmpreg & (RTC_TR_HT | RTC_TR_HU)) >> 16));
    *m = (uint8_t)Bcd2ToByte((uint8_t)((tmpreg & (RTC_TR_MNT | RTC_TR_MNU)) >>8));
    *s = (uint8_t)Bcd2ToByte((tmpreg & (RTC_TR_ST | RTC_TR_SU)));
}

static uint32_t ByteToBcd2(uint8_t Value)
{
  uint8_t bcdhigh = 0;
  while (Value >= 10)  {
    bcdhigh++;
    Value -= 10;
  }
  return  ((uint8_t)(bcdhigh << 4) | Value);
}

static void SetTime(uint8_t h, uint8_t m, uint8_t s)
{
     __IO uint32_t synchrocounter = 0;
     uint32_t synchrostatus = 0x00;
     __IO uint32_t initcounter = 0;
     uint32_t initstatus = 0x00;
     uint32_t tmpreg ;

     tmpreg = ((ByteToBcd2(h) << 16) | (ByteToBcd2(m) << 8) |  ByteToBcd2(s)) ;
     /* Disable the write protection for RTC registers */
     RTC->WPR = 0xCA;
     RTC->WPR = 0x53;
     RTC->ISR &= (uint32_t)~RTC_ISR_INIT;

     RTC->ISR = (uint32_t)RTC_INIT_MASK;

     /* Wait till RTC is in INIT state and if Time out is reached exit */
     do {
         initstatus = RTC->ISR & RTC_ISR_INITF;
         initcounter++;
     } while((initcounter != INITMODE_TIMEOUT) && (initstatus == 0x00));

     RTC->TR = (uint32_t)(tmpreg & RTC_TR_RESERVED_MASK);

     RTC->ISR &= (uint32_t)RTC_RSF_MASK;
     /* Wait the registers to be synchronised */
     do {
         synchrostatus = RTC->ISR & RTC_ISR_RSF;
         synchrocounter++;
     } while((synchrocounter != SYNCHRO_TIMEOUT) && (synchrostatus == 0x00));

     RTC->WPR = 0xFF;
}

static void GetDate(uint8_t *y, uint8_t *m, uint8_t *d)
{
    uint32_t tmpreg = 0;
    tmpreg = (uint32_t)(RTC->DR & RTC_TR_RESERVED_MASK);
    *y = (uint8_t)Bcd2ToByte((uint8_t)((tmpreg & (RTC_DR_YT|RTC_DR_YU)) >>16));
    *m = (uint8_t)Bcd2ToByte((uint8_t)((tmpreg & (RTC_DR_MT|RTC_DR_MU)) >> 8));
    *d = (uint8_t)Bcd2ToByte((uint8_t)(tmpreg & (RTC_DR_DT |RTC_DR_DU)));
}

static void SetDate(uint8_t y, uint8_t m, uint8_t d)
{
     __IO uint32_t synchrocounter = 0;
     uint32_t synchrostatus = 0x00;
     __IO uint32_t initcounter = 0;
     uint32_t initstatus = 0x00;
     uint32_t tmpreg = 0 ;

    tmpreg = ((ByteToBcd2(y) << 16) | (ByteToBcd2(m) << 8) |  ByteToBcd2(d)) ;
    /* Disable the write protection for RTC registers */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    RTC->ISR &= (uint32_t)~RTC_ISR_INIT;

    RTC->ISR = (uint32_t)RTC_INIT_MASK;

    /* Wait till RTC is in INIT state and if Time out is reached exit */
    do {
        initstatus = RTC->ISR & RTC_ISR_INITF;
        initcounter++;
    } while((initcounter != INITMODE_TIMEOUT) && (initstatus == 0x00));

    RTC->DR = (uint32_t)(tmpreg & RTC_TR_RESERVED_MASK);

    RTC->ISR &= (uint32_t)RTC_RSF_MASK;
    /* Wait the registers to be synchronised */
    do {
        synchrostatus = RTC->ISR & RTC_ISR_RSF;
        synchrocounter++;
    } while((synchrocounter != SYNCHRO_TIMEOUT) && (synchrostatus == 0x00));

    RTC->WPR = 0xFF;
}


#include <stdio.h>
void CYASSL_MSG(const char *msg) ;

struct tm *Cyassl_MDK_gmtime(const time_t *c)
{
    uint8_t h, m, s ;
    uint8_t y, mo, d ;
    static struct tm date ;

    GetTime(&h, &m, &s) ;
    GetDate(&y, &mo, &d) ;

    date.tm_year = y + 100 ;
    date.tm_mon = mo - 1 ;
    date.tm_mday = d ;
    date.tm_hour = h ;
    date.tm_min = m ;
    date.tm_sec = s ;

    #if defined(DEBUG_CYASSL)
    {
        char msg[100] ;
        sprintf(msg,
        "Debug::Cyassl_KEIL_gmtime(DATE=/%2d/%02d/%04d TIME=%02d:%02d:%02d)\n",
                d,  mo,  y+2000, h,  m,  s) ;
        CYASSL_MSG(msg) ;
    }
    #endif

    return(&date) ;
}

double current_time()
{
      return ((double)TIM2->CNT/1000000.0) ;
}

typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

void time_main(void *args)
{
    char * datetime ;
    uint8_t h, m, s ;
    uint8_t y, mo, d ;

    if( args == NULL || ((func_args *)args)->argc == 1) {
        GetTime(&h, &m, &s) ;
        GetDate(&y, &mo, &d) ;
        printf("Date: %d/%d/%d, Time: %02d:%02d:%02d\n",
             mo, d, y+2000, h, m, s) ;
    } else if(((func_args *)args)->argc == 3 &&
              ((func_args *)args)->argv[1][0] == '-' &&
              ((func_args *)args)->argv[1][1] == 'd' ) {
        datetime = ((func_args *)args)->argv[2];
        sscanf(datetime, "%d/%d/%d", (int *)&mo, (int *)&d, (int *) &y) ;
        SetDate(y-2000, mo, d) ;
    } else if(((func_args *)args)->argc == 3 &&
              ((func_args *)args)->argv[1][0] == '-' &&
              ((func_args *)args)->argv[1][1] == 't' ) {
        datetime = ((func_args *)args)->argv[2];
        sscanf(datetime, "%d:%d:%d",
            (int *)&h, (int *)&m, (int *)&s) ;
        SetTime(h, m, s) ;
    } else printf("Invalid argument\n") ;
}


/*******************************************************************
      time()
********************************************************************/
time_t time(time_t * t) { return 0 ; }
