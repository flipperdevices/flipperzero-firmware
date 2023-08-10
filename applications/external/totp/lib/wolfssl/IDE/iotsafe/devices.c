/* devices.c
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

/* Minimalist BSP for IoT-Safe example based on
 * ST P-L596G-CELL02 + Quectel BG96 modem
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "devices.h"
#define RTSCTS 0
#define AUTOBR 0
#define NVIC_UART1_IRQN          (37)
#define NVIC_UART2_IRQN          (38)

static char modem_rx_buf[256];
static uint32_t modem_rx_idx = 0;
static uint32_t modem_parser_idx = 0;

static void usart1_init(void)
{
    uint32_t reg;

    /* Enable PWR */
    RCC_APB1_ENR |= PWR_APB1_CLOCK_ER_VAL;
    /* Enable GPIOG */
    RCC_AHB2_ENR |= GPIOG_AHB2_CLOCK_ER_VAL;


    /* Enable VDDIO2 */
    while ((PWR_CR2 & PWR_CR2_IOSV) == 0) {
        PWR_CR2 |= PWR_CR2_IOSV;
        printf("Turning on VDDIO2\n");
        sleep_ms(1000);
    }
    /* Enable GPIOB */
    RCC_AHB2_ENR |= GPIOB_AHB2_CLOCK_ER_VAL;
    /* Set mode = AF */
    reg = GPIO_MODE(GPIOG_BASE) & ~(0x03 << (USART1_PIN_RX * 2));
    GPIO_MODE(GPIOG_BASE) = reg | (0x02 << (USART1_PIN_RX * 2));

    reg = GPIO_MODE(GPIOB_BASE) & ~(0x03 << (USART1_PIN_TX * 2));
    GPIO_MODE(GPIOB_BASE) = reg | (0x02 << (USART1_PIN_TX * 2));

    reg = GPIO_PUPD(GPIOG_BASE) & (0x03 << (USART1_PIN_RX * 2));
    
    reg = GPIO_PUPD(GPIOB_BASE) & (0x03 << (USART1_PIN_TX * 2));
    GPIO_PUPD(GPIOB_BASE) = reg | (0x01 << (USART1_PIN_TX * 2));
    
#if RTSCTS
    reg = GPIO_MODE(GPIOG_BASE) & ~(0x03 << (USART1_PIN_RTS * 2));
    GPIO_MODE(GPIOG_BASE) = reg | (0x02 << (USART1_PIN_RTS * 2));
    GPIO_PUPD(GPIOG_BASE) &= (0x03 << (USART1_PIN_RTS * 2));

    reg = GPIO_MODE(GPIOG_BASE) & ~(0x03 << (USART1_PIN_CTS * 2));
    GPIO_MODE(GPIOG_BASE) = reg | (0x02 << (USART1_PIN_CTS * 2));
    GPIO_PUPD(GPIOG_BASE) &= (0x03 << (USART1_PIN_CTS * 2));
#endif

    /* Set alternate functions */
    reg = GPIO_AFH(GPIOG_BASE) & ~(0xf << ((USART1_PIN_RX - 8) * 4));
    GPIO_AFH(GPIOG_BASE) = reg | (7 << ((USART1_PIN_RX - 8) * 4));
    reg = GPIO_AFL(GPIOB_BASE) & ~(0xf << ((USART1_PIN_TX) * 4));
    GPIO_AFL(GPIOB_BASE) = reg | (7 << ((USART1_PIN_TX) * 4));

#if RTSCTS
    /* RTS/CTS alt fn */
    reg = GPIO_AFH(GPIOG_BASE) & ~(0xf << ((USART1_PIN_RTS - 8) * 4));
    GPIO_AFH(GPIOG_BASE) = reg | (USART1_AF << ((USART1_PIN_RTS - 8) * 4));
    reg = GPIO_AFH(GPIOG_BASE) & ~(0xf << ((USART1_PIN_CTS - 8) * 4));
    GPIO_AFH(GPIOG_BASE) = reg | (USART1_AF << ((USART1_PIN_CTS - 8) * 4));
#endif
    /* Enable USART clock */
    RCC_APB2_ENR |= USART1_APB2_CLOCK_ER_VAL;
}

static void usart2_init(void)
{
    uint32_t reg;
    RCC_AHB2_ENR |= GPIOD_AHB2_CLOCK_ER_VAL | GPIOA_AHB2_CLOCK_ER_VAL;

    /* Set mode = AF */
    reg = GPIO_MODE(GPIOD_BASE) & ~(0x03 << (USART2_PIN_RX * 2));
    GPIO_MODE(GPIOD_BASE) = reg | (0x02 << (USART2_PIN_RX * 2));

    reg = GPIO_MODE(GPIOA_BASE) & ~(0x03 << (USART2_PIN_TX * 2));
    GPIO_MODE(GPIOA_BASE) = reg | (0x02 << (USART2_PIN_TX * 2));

    /* Set alternate functions */
    reg = GPIO_AFL(GPIOD_BASE) & ~(0xf << ((USART2_PIN_RX) * 4));
    GPIO_AFL(GPIOD_BASE) = reg | (7 << ((USART2_PIN_RX) * 4));
    reg = GPIO_AFL(GPIOA_BASE) & ~(0xf << ((USART2_PIN_TX) * 4));
    GPIO_AFL(GPIOA_BASE) = reg | (7 << ((USART2_PIN_TX) * 4));

    /* Enable USART clock */
    RCC_APB1_ENR |= USART2_APB1_CLOCK_ER_VAL;
}

int usart_tx(uint32_t dev, const uint8_t c)
{
    volatile uint32_t reg;
    do {
        reg = USART_ISR(dev);
    } while ((reg & USART_ISR_TXE) == 0);
    USART_TDR(dev) = c;
    return 1;
}

int usart_rx(uint32_t dev, uint8_t *c)
{
    int ret = 0;
    if (dev == USART1_BASE) {
        if (modem_rx_idx > modem_parser_idx) {
            *c = (uint8_t)(modem_rx_buf[modem_parser_idx++]);
            if (modem_rx_idx == modem_parser_idx) {
                modem_rx_idx = 0;
                modem_parser_idx = 0;
            }
            ret = 1;
        }
    } else {
        volatile uint32_t reg = USART_ISR(dev);
        if ((reg & USART_ISR_RXNE) != 0) {
            reg = USART_RDR(dev);
            *c = (uint8_t)(reg & 0xff);
            ret = 1;
        }
    }
    return ret;
}

int usart_init(uint32_t dev, uint32_t bitrate, uint8_t data, char parity, uint8_t stop)
{
    int rtscts = 0;

    if (dev == USART1_BASE) {
        usart1_init();
#if RTSCTS
        rtscts = 1;
#endif
    }
    else if (dev == USART2_BASE)
        usart2_init();
    else
        return -1;
    /* Turn off the device */
    USART_CR1(dev) &= ~(USART_CR1_ENABLE);

    /* Configure for TX + RX */
    USART_CR1(dev) |= (USART_CR1_TX_ENABLE | USART_CR1_RX_ENABLE);

    /* Configure clock */
    USART_BRR(dev) = CLOCK_SPEED / (bitrate);

    /* Configure data bits */
    if (data == 8)
        USART_CR1(dev) &= ~USART_CR1_SYMBOL_LEN;
    else
        USART_CR1(dev) |= USART_CR1_SYMBOL_LEN;

    /* Configure parity */
    switch (parity) {
        case 'O':
            USART_CR1(dev) |= USART_CR1_PARITY_ODD;
            /* fall through to enable parity */
            /* FALL THROUGH */
        case 'E':
            USART_CR1(dev) |= USART_CR1_PARITY_ENABLED;
            break;
        default:
            USART_CR1(dev) &= ~(USART_CR1_PARITY_ENABLED | USART_CR1_PARITY_ODD);
    }
    /* Set stop bits (not supported) */
    (void)stop;

    /* Set rtscts */
    if (rtscts)
        USART_CR3(dev) |= USART_CR3_CTSE | USART_CR3_RTSE;

#if AUTOBR
    /* Enable ABR */
    USART_CR2(dev) |= USART_CR2_ABREN;
#endif


    if (dev == USART1_BASE) {
        USART_CR1(dev) |= USART_CR1_RXNEIE | USART_CR1_PEIE;
        USART_CR3(dev) |= USART_CR3_EIE;
        nvic_irq_enable(NVIC_UART1_IRQN);
        nvic_irq_setprio(NVIC_UART1_IRQN, 0);
    }

    /* Turn on uart */
    USART_CR1(dev) |= USART_CR1_ENABLE;
    return 0;
}

/* STDOUT on USART2 */
int _write(void *r, uint8_t *text, int len)
{
    char *p = (char *)text;
    (void)r;
    while(*p && (p < (char *)(text + len))) {
        usart_tx(USART2_BASE, *p);
        p++;
    }
    return len;
}

/* newlib backend calls */

extern unsigned int _start_heap;
void * _sbrk(unsigned int incr)
{
    static unsigned char *heap = NULL;
    void *old_heap = heap;
    if (((incr >> 2) << 2) != incr)
        incr = ((incr >> 2) + 1) << 2;
    if (old_heap == NULL)
        old_heap = heap = (unsigned char *)&_start_heap;
    heap += incr;
    return old_heap;
}
void * _sbrk_r(unsigned int incr)
{
    static unsigned char *heap = NULL;
    void *old_heap = heap;
    if (((incr >> 2) << 2) != incr)
        incr = ((incr >> 2) + 1) << 2;
    if (old_heap == NULL)
        old_heap = heap = (unsigned char *)&_start_heap;
    heap += incr;
    return old_heap;
}

int _close(int fd)
{
    return -1;
}

int _fstat(int fd)
{
    return -1;
}

int _lseek(int fd, int whence, int off)
{
    return -1;
}

int _read(uint8_t *buf, int len)
{
    return -1;
}

int _isatty(int fd)
{
    return 1;
}

/* Clock + waitstates settings */

static void flash_set_waitstates(unsigned int waitstates)
{
    uint32_t reg = FLASH_ACR;
    if ((reg & FLASH_ACR_LATENCY_MASK) != waitstates)
        FLASH_ACR |= ((reg & ~FLASH_ACR_LATENCY_MASK) | waitstates);
}

void clock_pll_on(void)
{
    uint32_t reg32;
    uint32_t cpu_freq;
    uint32_t hpre, ppre1, ppre2;
    uint32_t flash_waitstates;

    /* Select clock parameters (CPU Speed = 80MHz) */
    cpu_freq = 80000000;
    flash_waitstates = 4;
    flash_set_waitstates(flash_waitstates);

    /* Configure + enable internal high-speed oscillator. */
    RCC_CR = (RCC_CR & (~RCC_CR_MSIRANGE_Msk)) | RCC_CR_MSIRANGE_6;
    RCC_CR |= RCC_CR_MSIRGSEL;
    RCC_CR |= RCC_CR_MSION;

    DMB();
    while ((RCC_CR & RCC_CR_MSIRDY) == 0)
        ;
    /* Select MSI as SYSCLK source. */
    reg32 = RCC_CFGR;
    reg32 &= ~(RCC_CFGR_SW_MASK);
    RCC_CFGR = (reg32 | RCC_CFGR_SW_MSI);
    DMB();
    /*
     * Set prescalers
     */
    hpre = RCC_PRESCALER_DIV_NONE;
    ppre1 = RCC_PRESCALER_DIV_NONE;
    ppre2 = RCC_PRESCALER_DIV_NONE;
    reg32 = RCC_CFGR;
    reg32 &= ~(RCC_CFGR_HPRE_MASK << RCC_CFGR_HPRE_SHIFT);
    RCC_CFGR = (hpre & RCC_CFGR_HPRE_MASK) << RCC_CFGR_HPRE_SHIFT;
    DMB();
    reg32 = RCC_CFGR;
    reg32 &= ~(RCC_CFGR_PPRE1_MASK << RCC_CFGR_PPRE1_SHIFT);
    RCC_CFGR = (reg32 | (ppre1 << RCC_CFGR_PPRE1_SHIFT));
    DMB();
    reg32 &= ~(RCC_CFGR_PPRE2_MASK << RCC_CFGR_PPRE2_SHIFT);
    RCC_CFGR = (reg32 | (ppre2 << RCC_CFGR_PPRE2_SHIFT));
    DMB();
    /* Set PLLCFGR parameter */
    RCC_PLLCFGR = PLLCFGR_PLLM | PLLCFGR_PLLN |
        PLLCFGR_PLLP | PLLCFGR_PLLQ |
        PLLCFGR_PLLR | RCC_PLLCFGR_PLLP_EN |
        RCC_PLLCFGR_PLLQ_EN | RCC_PLLCFGR_PLLR_EN |
        RCC_PLLCFGR_PLLSRC_MSI;

    /* Enable PLL oscillator and wait for it to stabilize. */
    RCC_CR |= RCC_CR_PLLON;
    DMB();
    while ((RCC_CR & RCC_CR_PLLRDY) == 0)
        ;

    /* Select PLL as SYSCLK source. */
    reg32 = RCC_CFGR;
    reg32 &= ~(RCC_CFGR_SW_MASK);
    RCC_CFGR = (reg32 | RCC_CFGR_SW_PLL);
    DMB();
    /* Wait for PLL clock to be selected (via SWS, bits 3:2) */
    while (((RCC_CFGR >> 2) & RCC_CFGR_SW_MASK) != RCC_CFGR_SW_PLL)
        ;

    RCC_CCIPR |= (1 << 26);

    PWR_CR1 |= PWR_CR1_DBP;
}


/* Modem via STMod+ connector */
static int stmod_en_init = 0;

static void stmod_pin_init(void)
{
    uint32_t reg;
    RCC_AHB2_ENR |=
        GPIOA_AHB2_CLOCK_ER_VAL |
        GPIOB_AHB2_CLOCK_ER_VAL |
        GPIOC_AHB2_CLOCK_ER_VAL |
        GPIOD_AHB2_CLOCK_ER_VAL |
        GPIOI_AHB2_CLOCK_ER_VAL;

    /* 'enable' pin */
    reg = GPIO_MODE(STMOD_EN_PORT) & ~(0x03 << (STMOD_EN_PIN * 2));
    GPIO_MODE(STMOD_EN_PORT) = reg | (0x01 << (STMOD_EN_PIN * 2));

    /* RST pin */
    reg = GPIO_MODE(STMOD_MODEM_RST_PORT) & ~(0x03 << (STMOD_MODEM_RST_PIN * 2));
    GPIO_MODE(STMOD_MODEM_RST_PORT) = reg | (0x01 << (STMOD_MODEM_RST_PIN * 2));
    
    /* DTR pin */
    reg = GPIO_MODE(STMOD_MODEM_DTR_PORT) & ~(0x03 << (STMOD_MODEM_DTR_PIN * 2));
    GPIO_MODE(STMOD_MODEM_DTR_PORT) = reg | (0x01 << (STMOD_MODEM_DTR_PIN * 2));

    /* Sim select pins */
    reg = GPIO_MODE(STMOD_SIM_SELECT0_PORT) & ~(0x03 << (STMOD_SIM_SELECT0_PIN * 2));
    GPIO_MODE(STMOD_SIM_SELECT0_PORT) = reg | (0x01 << (STMOD_SIM_SELECT0_PIN * 2));
    reg = GPIO_MODE(STMOD_SIM_SELECT1_PORT) & ~(0x03 << (STMOD_SIM_SELECT1_PIN * 2));
    GPIO_MODE(STMOD_SIM_SELECT1_PORT) = reg | (0x01 << (STMOD_SIM_SELECT1_PIN * 2));

}

void stmod_modem_enable(void)
{
    if (!stmod_en_init) {
        stmod_pin_init();
        stmod_en_init = 1;
    }

    /* initial pin state */
    gpio_set(STMOD_EN_PORT, STMOD_EN_PIN);
    gpio_set(STMOD_MODEM_RST_PORT, STMOD_MODEM_RST_PIN);
    gpio_set(STMOD_MODEM_DTR_PORT, STMOD_MODEM_DTR_PIN);
    sleep_ms(200);
    gpio_clear(STMOD_MODEM_RST_PORT, STMOD_MODEM_RST_PIN);
    gpio_clear(STMOD_EN_PORT, STMOD_EN_PIN);
    sleep_ms(2500);

    /* ON/OFF sequence to clear state */
    gpio_set(STMOD_EN_PORT, STMOD_EN_PIN);
    sleep_ms(700);
    gpio_clear(STMOD_EN_PORT, STMOD_EN_PIN);
    sleep_ms(1000);
    gpio_set(STMOD_EN_PORT, STMOD_EN_PIN);
    sleep_ms(50);
    gpio_clear(STMOD_EN_PORT, STMOD_EN_PIN);
    sleep_ms(30);
    gpio_set(STMOD_EN_PORT, STMOD_EN_PIN); /* Modem is on. */
    printf("Modem booting...\n");
    sleep_ms(5000);
    printf("Modem is on.\r\n");

}

void stmod_modem_disable(void)
{
    if (!stmod_en_init) {
        stmod_pin_init();
        stmod_en_init = 1;
    }
    gpio_clear(STMOD_EN_PORT, STMOD_EN_PIN);
}

extern volatile unsigned jiffies;
void systick_enable(void)
{
    SYSTICK_RVR = ((CLOCK_SPEED / 1000) - 1);
    SYSTICK_CVR = 0;
    SYSTICK_CSR |= 0x07;
}

void sleep_ms(unsigned ms)
{
    unsigned end = jiffies + ms;
    while(jiffies < end)
        __asm__ volatile("wfi");
}

void isr_usart1(void)
{
    uint32_t reg;
    reg = USART_ISR(USART1_BASE);
    if (reg & USART_ISR_RXNE) {
        modem_rx_buf[modem_rx_idx++] = (char)USART_RDR(USART1_BASE);
    } else {
        USART_ICR(USART1_BASE) |= 2 | USART_ICR_CMCF; /* FECF + CMCF*/
    }
}
