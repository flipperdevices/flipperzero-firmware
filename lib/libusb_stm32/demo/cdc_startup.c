/* This file is the part of the Lightweight USB device Stack for STM32 microcontrollers
 *
 * Copyright ©2016 Dmitry Filimonchuk <dmitrystu[at]gmail[dot]com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "stm32.h"

static void cdc_init_rcc (void) {
#if defined(STM32L0)
    _BST(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    _BMD(PWR->CR, PWR_CR_VOS, PWR_CR_VOS_0);
    _WBC(PWR->CSR, PWR_CSR_VOSF);
    /* set FLASH latency to 1 */
    _BST(FLASH->ACR, FLASH_ACR_LATENCY);
    /* set clock at 32MHz PLL 6/3 HSI */
    _BMD(RCC->CFGR, RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC, RCC_CFGR_PLLDIV3 | RCC_CFGR_PLLMUL6);
    _BST(RCC->CR, RCC_CR_HSION);
    _WBS(RCC->CR, RCC_CR_HSIRDY);
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    /* switch clock to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);

#elif defined(STM32L1)
    _BST(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    _BMD(PWR->CR, PWR_CR_VOS, PWR_CR_VOS_0);
    _WBC(PWR->CSR, PWR_CSR_VOSF);
    /* set FLASH latency to 1 */
    _BST(FLASH->ACR, FLASH_ACR_ACC64);
    _BST(FLASH->ACR, FLASH_ACR_LATENCY);
    /* set clock at 32 MHz PLL 6/3 HSI */
    _BMD(RCC->CFGR, RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC, RCC_CFGR_PLLDIV3 | RCC_CFGR_PLLMUL6);
    _BST(RCC->CR, RCC_CR_HSION);
    _WBS(RCC->CR, RCC_CR_HSIRDY);
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    /* switch clock to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);

#elif defined(STM32L476xx)
    _BST(RCC->APB1ENR1, RCC_APB1ENR1_PWREN);
    /* Set power Range 1 */
    _BMD(PWR->CR1, PWR_CR1_VOS, PWR_CR1_VOS_0);
    _WBC(PWR->SR2, PWR_SR2_VOSF);
    /* Adjust Flash latency */
    _BMD(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2WS);
    /* set clock 48Mhz MSI */
    _BMD(RCC->CR, RCC_CR_MSIRANGE, RCC_CR_MSIRANGE_11 | RCC_CR_MSIRGSEL);
    /* set MSI as 48MHz USB */
    _BMD(RCC->CCIPR, RCC_CCIPR_CLK48SEL, RCC_CCIPR_CLK48SEL_0 | RCC_CCIPR_CLK48SEL_1);
    /* enable GPIOA clock */
    _BST(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
    /* set GP11 and GP12 as USB data pins AF10 */
    _BST(GPIOA->AFR[1], (0x0A << 12) | (0x0A << 16));
    _BMD(GPIOA->MODER, (0x03 << 22) | (0x03 << 24), (0x02 << 22) | (0x02 << 24));

#elif defined(STM32F103x6)
    /* set flash latency 1WS */
    _BMD(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_1);
    /* use PLL 48MHz clock from 8Mhz HSI */
    _BMD(RCC->CFGR,
         RCC_CFGR_PLLMULL | RCC_CFGR_PLLSRC | RCC_CFGR_USBPRE,
         RCC_CFGR_PLLMULL12 | RCC_CFGR_USBPRE);
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    /* switch to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);

#elif defined(STM32F303xE)
    /* set flash latency 1WS */
    _BMD(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_1);
    /* use PLL 48MHz clock from 8Mhz HSI */
    _BMD(RCC->CFGR,
         RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC | RCC_CFGR_USBPRE,
         RCC_CFGR_PLLMUL12 | RCC_CFGR_USBPRE);
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    /* switch to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);

#elif defined(STM32F303xC)
    /* set flash latency 1WS */
    _BMD(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_1);
    /* use PLL 48MHz clock from 8Mhz HSI */
    _BMD(RCC->CFGR,
         RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC | RCC_CFGR_USBPRE,
         RCC_CFGR_PLLMUL12 | RCC_CFGR_USBPRE);
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    /* switch to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);
    
    _BST(RCC->AHBENR, RCC_AHBENR_GPIOAEN);
    _BST(GPIOA->AFR[1], (0x0E << 12) | (0x0E << 16));
    _BMD(GPIOA->MODER, (0x03 << 22) | (0x03 << 24), (0x02 << 22) | (0x02 << 24));

#elif defined(STM32F373xC)
    /* set flash latency 1WS */
    _BMD(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_1);
    _BMD(RCC->CFGR,
         RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC | RCC_CFGR_USBPRE | RCC_CFGR_PPRE1,
         RCC_CFGR_PLLMUL12  | RCC_CFGR_USBPRE | RCC_CFGR_PPRE1_DIV2);
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    /* switch to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);

    /* Enabling USB PA11 PA12 AF 0x0E.
     * This is still undocumented in the 7th!! revision of the datasheet.
     * Thank you, mo***rs from ST for extra 5 hrs of debugging.
     */
    _BST(RCC->AHBENR, RCC_AHBENR_GPIOAEN);
    _BST(GPIOA->AFR[1], (0x0E << 12) | (0x0E << 16));
    _BMD(GPIOA->MODER, (0x03 << 22) | (0x03 << 24), (0x02 << 22) | (0x02 << 24)); // MCO


#elif defined(STM32F429xx) || defined(STM32F405xx) \
    || defined(STM32F401xC) || defined(STM32F401xE)  || defined(STM32F411xE)
    /* set flash latency 2WS */
    _BMD(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2WS);
    /* setting up PLL 16MHz HSI, VCO=144MHz, PLLP = 72MHz PLLQ = 48MHz  */
    _BMD(RCC->PLLCFGR,
        RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLP,
        _VAL2FLD(RCC_PLLCFGR_PLLM, 8) | _VAL2FLD(RCC_PLLCFGR_PLLN, 72) | _VAL2FLD(RCC_PLLCFGR_PLLQ, 3));
    /* enabling PLL */
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    /* switching to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);
    #if defined(USBD_PRIMARY_OTGHS)
    /* enabling GPIOB and setting PB13, PB14 and PB15 to AF11 (USB_OTG2FS) */
    _BST(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);
    #if defined(USBD_VBUS_DETECT)
    _BST(GPIOB->AFR[1], (0x0C << 24) | (0x0C << 28) | (0x0C << 20));
    _BMD(GPIOB->MODER, (0x03 << 28) | (0x03 << 30) | (0x03 << 26), (0x02 << 28) | (0x02 << 30) | (0x02 << 26));
    #else //defined(USBD_VBUS_DETECT)
    _BST(GPIOB->AFR[1], (0x0C << 24) | (0x0C << 28));
    _BMD(GPIOB->MODER, (0x03 << 28) | (0x03 << 30), (0x02 << 28) | (0x02 << 30));
    #endif //defined(USBD_VBUS_DETECT)
    #else  //defined(USBD_PRIMARY_OTGHS)
    /* enabling GPIOA and setting PA11 and PA12 to AF10 (USB_FS) */
    _BST(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    _BST(GPIOA->AFR[1], (0x0A << 12) | (0x0A << 16));
    _BMD(GPIOA->MODER, (0x03 << 22) | (0x03 << 24), (0x02 << 22) | (0x02 << 24));
    #endif //defined(USBD_PRIMARY_OTGHS)

#elif defined(STM32F446xx) || defined(STM32F745xx)
    /* set flash latency 2WS */
    _BMD(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2WS);
    /* setting up PLL 16MHz HSI, VCO=144MHz, PLLP = 72MHz PLLQ = 48MHz  */
    _BMD(RCC->PLLCFGR,
        RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLQ,
        _VAL2FLD(RCC_PLLCFGR_PLLM, 8) | _VAL2FLD(RCC_PLLCFGR_PLLN, 72) | _VAL2FLD(RCC_PLLCFGR_PLLQ, 3));
    /* enabling PLL */
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    /* switching to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);
    /* enabling GPIOA and setting PA11 and PA12 to AF10 (USB_FS) */
    #if defined(USBD_PRIMARY_OTGHS)
    _BST(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);
    _BST(GPIOB->AFR[1], (0x0C << 24) | (0x0C << 28));
    _BMD(GPIOB->MODER, (0x03 << 28) | (0x03 << 30), (0x02 << 28) | (0x02 << 30));
    #else
    _BST(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    _BST(GPIOA->AFR[1], (0x0A << 12) | (0x0A << 16));
    _BMD(GPIOA->MODER, (0x03 << 22) | (0x03 << 24), (0x02 << 22) | (0x02 << 24));
    #endif

#elif defined(STM32F105xC) || defined(STM32F107xC)
    _BST(RCC->CR, RCC_CR_HSION);
    _WBS(RCC->CR, RCC_CR_HSIRDY);
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSI);
    _BMD(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_HSI);
    /* set flash latency 1WS */
    _BMD(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_1);

    _BST(RCC->CR, RCC_CR_HSEON);
    _WBS(RCC->CR, RCC_CR_HSERDY);

    /* switch to HSE */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSE);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_HSE);

    #if defined(HSE_25MHZ)
    RCC->CFGR = RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLSRC | \
                RCC_CFGR_ADCPRE_DIV8 | RCC_CFGR_PPRE2_DIV1 | \
                RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_HPRE_DIV1 | \
                RCC_CFGR_OTGFSPRE*0;
    /* PREDIV1SRC= PLL2, PLL2MUL = 8, PREDIV1 = 5, PREDIV2=5 */
    RCC->CFGR2 = RCC_CFGR2_PREDIV1SRC | RCC_CFGR2_PLL2MUL8 | \
                 RCC_CFGR2_PREDIV1_DIV5 | RCC_CFGR2_PREDIV2_DIV5;

    _BST(RCC->CR, RCC_CR_PLL2ON);
    _WBS(RCC->CR, RCC_CR_PLL2RDY);

    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    #else
    RCC->CFGR = RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLSRC | RCC_CFGR_ADCPRE_DIV8 | \
                RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_HPRE_DIV1;
    _BMD(RCC->CFGR2, RCC_CFGR2_PREDIV1, RCC_CFGR2_PREDIV1_DIV1);
    _BCL(RCC->CFGR2, RCC_CFGR2_PREDIV1SRC);
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    #endif

    RCC->CFGR |= RCC_CFGR_MCO_PLLCLK_DIV2;

    /* set flash latency 2WS */
    _BMD(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2);
    /* switch to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);
#elif defined(STM32L433xx)
    /* using HSI16 as AHB/CPU clock, HSI48 as USB PHY clock */
    _BST(RCC->CR, RCC_CR_HSION);
    _WBS(RCC->CR, RCC_CR_HSIRDY);
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSI);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_HSI);
    _BST(RCC->CRRCR, RCC_CRRCR_HSI48ON);
    _WBS(RCC->CRRCR, RCC_CRRCR_HSI48RDY);
    _BMD(RCC->CCIPR, RCC_CCIPR_CLK48SEL, 0);
    /* setup PA11 PA12 to AF10 (USB FS) */
    _BST(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
    _BMD(GPIOA->MODER, (0x03 << 22) | (0x03 << 24), (0x02 << 22) | (0x02 << 24));
    _BST(GPIOA->AFR[1], (0x0A << 12) | (0x0A << 16));
    /* Disabling USB Vddusb power isolation. Vusb connected to Vdd */
    _BST(RCC->APB1ENR1, RCC_APB1ENR1_PWREN);
    _BST(PWR->CR2, PWR_CR2_USV);
#elif defined(STM32F070xB)
    /* set flash latency 1WS */
    _BST(FLASH->ACR, FLASH_ACR_LATENCY);
    /* use PLL 48MHz clock from 8Mhz HSI */
    _BMD(RCC->CFGR,
        RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC | RCC_CFGR_USBPRE,
        RCC_CFGR_PLLMUL12 | RCC_CFGR_USBPRE);
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    /* switch to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);
    _BST(RCC->CFGR3, RCC_CFGR3_USBSW_PLLCLK);
#elif defined(STM32F042x6)
    /* set flash latency 1WS */
    _BST(FLASH->ACR, FLASH_ACR_LATENCY);
    /* use HSI48 as clock incl. USB PHY clock, no PLL */
	_BST(RCC->CR2, RCC_CR2_HSI48ON);
	_WBS(RCC->CR2, RCC_CR2_HSI48RDY);
#elif defined(STM32G4)
    /* using HSI16 as AHB/CPU clock, HSI48 as USB PHY clock */
    _BST(RCC->CRRCR, RCC_CRRCR_HSI48ON);
    _WBS(RCC->CRRCR, RCC_CRRCR_HSI48RDY);
#elif defined(STM32WB55xx)

#else
    #error Not supported
#endif
}

void __libc_init_array(void) {

}

void SystemInit(void) {
    cdc_init_rcc();
}
