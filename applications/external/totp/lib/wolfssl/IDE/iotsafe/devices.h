/* devices.h
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
#ifndef STM32L496_DEVICES
#define STM32L496_DEVICES

/* CPU clock speed */
//#define CLOCK_SPEED 14200000
//#define CLOCK_SPEED 6000000
#define CLOCK_SPEED 40000000

/* Memory mapping */
#define USART1_BASE (0x40013800UL)
#define USART2_BASE (0x40004400UL)
#define GPIOA_BASE  (0x48000000UL)
#define GPIOB_BASE  (0x48000400UL)
#define GPIOC_BASE  (0x48000800UL)
#define GPIOD_BASE  (0x48000C00UL)
#define GPIOE_BASE  (0x48001000UL)
#define GPIOF_BASE  (0x48001400UL)
#define GPIOG_BASE  (0x48001800UL)
#define GPIOH_BASE  (0x48001C00UL)
#define GPIOI_BASE  (0x48002000UL)
#define RCC_BASE    (0x40021000UL)
#define PWR_BASE    (0x40007000UL)



/* USART */
#define USART_CR1(x) (*((volatile uint32_t *)(x + 0x00)))
#define USART_CR2(x) (*((volatile uint32_t *)(x + 0x04)))
#define USART_CR3(x) (*((volatile uint32_t *)(x + 0x08)))
#define USART_BRR(x) (*((volatile uint32_t *)(x + 0x0C)))
#define USART_ISR(x) (*((volatile uint32_t *)(x + 0x1C)))
#define USART_ICR(x) (*((volatile uint32_t *)(x + 0x20)))
#define USART_RDR(x) (*((volatile uint8_t *)(x + 0x24)))
#define USART_TDR(x) (*((volatile uint8_t *)(x + 0x28)))

/* GPIO */
#define GPIO_MODE(x) (*((volatile uint32_t *)(x + 0x00)))
#define GPIO_SPEED(x) (*((volatile uint32_t *)(x + 0x08)))
#define GPIO_PUPD(x) (*((volatile uint32_t *)(x + 0x0C)))
#define GPIO_AFL(x)  (*((volatile uint32_t *)(x + 0x20)))
#define GPIO_AFH(x)  (*((volatile uint32_t *)(x + 0x24)))
#define GPIO_BSSR(x) (*((volatile uint32_t *)(x + 0x18)))


/* RCC */
#define RCC_CR       (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR     (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_PLLCFGR  (*(volatile uint32_t *)(RCC_BASE + 0x0C))
#define RCC_CCIPR    (*(volatile uint32_t *)(RCC_BASE + 0x88))

#define RCC_AHB1_ENR (*(volatile uint32_t *)(RCC_BASE + 0x48))
#define RCC_AHB2_ENR (*(volatile uint32_t *)(RCC_BASE + 0x4C))
#define RCC_AHB3_ENR (*(volatile uint32_t *)(RCC_BASE + 0x50))
#define RCC_APB1_ENR (*(volatile uint32_t *)(RCC_BASE + 0x58))
#define RCC_APB2_ENR (*(volatile uint32_t *)(RCC_BASE + 0x60))

/* PWR */
#define PWR_CR1       (*(volatile uint32_t *)(PWR_BASE + 0x00))
#define PWR_CR2       (*(volatile uint32_t *)(PWR_BASE + 0x04))
#define PWR_SR2       (*(volatile uint32_t *)(PWR_BASE + 0x014))

#define PWR_CR1_DBP (1 << 8)
#define PWR_CR2_PVME2 (1 << 5)
#define PWR_CR2_IOSV (1 << 9)
#define PWR_SR2_PVMO2 (1 << 13)


/* FLASH registers + latency mask  */
#define FLASH_BASE 0x40022000
#define FLASH_ACR  (*(volatile uint32_t *)(FLASH_BASE + 0x00))
#define FLASH_ACR_LATENCY_MASK (0x03)

/* RCC: Periph enable flags */
#define USART1_APB2_CLOCK_ER_VAL 	(1 << 14)
#define USART2_APB1_CLOCK_ER_VAL 	(1 << 17)
#define PWR_APB1_CLOCK_ER_VAL       (1 << 28)
#define GPIOA_AHB2_CLOCK_ER_VAL     (1 << 0)
#define GPIOB_AHB2_CLOCK_ER_VAL     (1 << 1)
#define GPIOC_AHB2_CLOCK_ER_VAL     (1 << 2)
#define GPIOD_AHB2_CLOCK_ER_VAL     (1 << 3)
#define GPIOE_AHB2_CLOCK_ER_VAL     (1 << 4)
#define GPIOF_AHB2_CLOCK_ER_VAL     (1 << 5)
#define GPIOG_AHB2_CLOCK_ER_VAL     (1 << 6)
#define GPIOH_AHB2_CLOCK_ER_VAL     (1 << 7)
#define GPIOI_AHB2_CLOCK_ER_VAL     (1 << 8)

/* Pinout: USART */
#define USART1_PIN_RX  (10)  /* PG10 */
#define USART1_PIN_TX  (6)   /* PB6  */
#define USART1_PIN_CTS (11)  /* PG11 */
#define USART1_PIN_RTS (12)  /* PG12 */
#define USART1_AF      (7)

#define USART2_PIN_RX  (6)   /* PD6 */
#define USART2_PIN_TX  (2)   /* PA2 */
#define USART2_AF      (7)



/* USART registers: flags */

#define USART_CR1_ENABLE         (1 << 0)
#define USART_CR1_TX_ENABLE      (1 << 3)
#define USART_CR1_RX_ENABLE      (1 << 2)
#define USART_CR1_RXNEIE         (1 << 5)
#define USART_CR1_PEIE           (1 << 8)
#define USART_CR1_PARITY_ODD     (1 << 9)
#define USART_CR1_PARITY_ENABLED (1 << 10)
#define USART_CR1_SYMBOL_LEN     (1 << 28)
#define USART_CR2_ABREN          (1 << 20)
#define USART_CR3_EIE            (1 << 0)
#define USART_CR3_RTSE           (1 << 8)
#define USART_CR3_CTSE           (1 << 9)
#define USART_ISR_TXE            (1 << 7)
#define USART_ISR_RXNE           (1 << 5)
#define USART_ICR_CTSCF          (1 << 9)
#define USART_ICR_CMCF           (1 << 17)

/* RCC_CR/CFGR/PLLCFGR values */
#define RCC_PRESCALER_DIV_NONE 0
#define RCC_PRESCALER_DIV_2    8
#define RCC_PRESCALER_DIV_4    9

#define RCC_CR_PLLRDY                (1 << 25)
#define RCC_CR_PLLON                 (1 << 24)
#define RCC_CR_MSIRGSEL              (1 << 3)
#define RCC_CR_MSIRDY                (1 << 1)
#define RCC_CR_MSION                 (1 << 0)
#define RCC_CR_HSIRDY                (1 << 10)
#define RCC_CR_HSION                 (1 << 8)
#define RCC_CR_MSIRANGE_SHIFT        4
#define RCC_CR_MSIRANGE_9            (0x09 << 4)
#define RCC_CR_MSIRANGE_6            (0x06 << 4)
#define RCC_CR_MSIRANGE_Msk          (0x0F << 4)

#define RCC_CFGR_HPRE_MASK            0x0F
#define RCC_CFGR_PPRE1_MASK           0x07
#define RCC_CFGR_PPRE2_MASK           0x07
#define RCC_CFGR_HPRE_SHIFT           4
#define RCC_CFGR_PPRE1_SHIFT          8
#define RCC_CFGR_PPRE2_SHIFT          11
#define RCC_CFGR_SW_MSI               0x0
#define RCC_CFGR_SW_PLL               0x3
#define RCC_CFGR_SW_MASK              0x3

/* Bits 0:1 SRC */
#define RCC_PLLCFGR_SRC_SHIFT       0
#define RCC_PLLCFGR_PLLSRC_MSI      0x1
#define RCC_PLLCFGR_PLLSRC_MASK     0x3

/* Bits 4:6 PLLM */
//#define PLLCFGR_PLLM                (0x4 << 4)
#define PLLCFGR_PLLM                (0x1 << 4)
#define RCC_PLLCFGR_PLLM_MASK       (0x7 << 4)

/* Bits 8:14 PLLN */
//#define PLLCFGR_PLLN                (71   << 8)
#define PLLCFGR_PLLN                (40   << 8)
#define RCC_PLLCFGR_PLLN_MASK       (0x7f << 8)

/* Bits 27:31 PLLPDIV */
#define PLLCFGR_PLLP                (2 << 27)
#define RCC_PLLCFGR_PLLP_MASK       (0x1F << 27)

/* Bits 21:22 PLLQ */
#define PLLCFGR_PLLQ                (0 << 21)
#define RCC_PLLCFGR_PLLQ_MASK       (0x3 << 21)

/* Bits 25:26 PLLR */
//#define PLLCFGR_PLLR                (2 << 25)
#define PLLCFGR_PLLR                (0 << 25)
#define RCC_PLLCFGR_PLLR_MASK       (0x3 << 25)

/* Enablers */
#define RCC_PLLCFGR_PLLP_EN         (0 << 16)
#define RCC_PLLCFGR_PLLQ_EN         (0 << 20)
#define RCC_PLLCFGR_PLLR_EN         (1 << 24)

/* Systick */
#define SYSTICK_BASE (0xE000E010)
#define SYSTICK_CSR     (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_RVR     (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_CVR     (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))
#define SYSTICK_CALIB   (*(volatile uint32_t *)(SYSTICK_BASE + 0x0C))


/* STMod+ connector pinout 
 *
 * Connector            STM32L4
 * pins                 pins
 * 
 * 1      11            PG11    PH2
 * 2      12            PB6     PB2
 * 3      13            PG10    PA4
 * 4      14            PG12    PA0
 * 5      15            GND     5V
 * 6      16            5V      GND
 * 7      17            PB8     PC7
 * 8      18            PI3     PC2
 * 9      19            PD3     PB12
 * 10     20            PB7     PC2
 *
 */

#define STMOD_EN_PORT   GPIOD_BASE
#define STMOD_EN_PIN    3
#define STMOD_SIM_SELECT0_PORT GPIOC_BASE
#define STMOD_SIM_SELECT0_PIN  2
#define STMOD_SIM_SELECT1_PORT GPIOI_BASE
#define STMOD_SIM_SELECT1_PIN  3
#define STMOD_MODEM_RST_PORT   GPIOB_BASE
#define STMOD_MODEM_RST_PIN    2
#define STMOD_MODEM_DTR_PORT   GPIOA_BASE
#define STMOD_MODEM_DTR_PIN    0

void stmod_modem_enable(void);
void stmod_modem_disable(void);

/* inline functions for GPIO */
static inline void gpio_set(uint32_t port, uint32_t pin)
{
    GPIO_BSSR(port) |= (1 << pin);   
}

static inline void gpio_clear(uint32_t port, uint32_t pin)
{
    GPIO_BSSR(port) |= (1 << (16 + pin));
}


/* Exported functions (from devices.c) */
void clock_pll_on(void);
int usart_init(uint32_t dev, uint32_t bitrate, uint8_t data, char parity, uint8_t stop);
int usart_tx(uint32_t dev, const uint8_t c);
int usart_rx(uint32_t dev, uint8_t *c);
void systick_enable(void);
void sleep_ms(unsigned ms);


/* Assembly helpers */
#define DMB() __asm__ volatile ("dmb")

/* Nvic */
#define NVIC_ISER_BASE (0xE000E100)
#define NVIC_ICER_BASE (0xE000E180)
#define NVIC_ICPR_BASE (0xE000E280)
#define NVIC_IPRI_BASE (0xE000E400)

static inline void nvic_irq_enable(uint8_t n)
{
    int i = n / 32;
    volatile uint32_t *nvic_iser = ((volatile uint32_t *)(NVIC_ISER_BASE + 4 * i));
    *nvic_iser |= (1 << (n % 32));
}

static inline void nvic_irq_disable(uint8_t n)
{
    int i = n / 32;
    volatile uint32_t *nvic_icer = ((volatile uint32_t *)(NVIC_ICER_BASE + 4 * i));
    *nvic_icer |= (1 << (n % 32));
}

static inline void nvic_irq_setprio(uint8_t n, uint8_t prio)
{
    volatile uint8_t *nvic_ipri = ((volatile uint8_t *)(NVIC_IPRI_BASE + n));
    *nvic_ipri = prio;
}


static inline void nvic_irq_clear(uint8_t n)
{
    int i = n / 32;
    volatile uint8_t *nvic_icpr = ((volatile uint8_t *)(NVIC_ICPR_BASE + 4 * i));
    *nvic_icpr = (1 << (n % 32));
}


#endif /* guard */
