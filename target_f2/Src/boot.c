#include "boot.h"
#include "cmsis_os.h"
#include "stm32l4xx.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_gpio.h"

// Boot request enum
#define BOOT_REQUEST_NONE   0x00000000
#define BOOT_REQUEST_DFU    0xDF00B000
// Boot to DFU pin
#define BOOT_DFU_PORT       GPIOB
#define BOOT_DFU_PIN        GPIO_PIN_8
// USB pins
#define BOOT_USB_PORT       GPIOA
#define BOOT_USB_DM_PIN     GPIO_PIN_11
#define BOOT_USB_DP_PIN     GPIO_PIN_12
#define BOOT_USB_PIN        ( BOOT_USB_DM_PIN | BOOT_USB_DP_PIN )

// TODO: replace with HAL-LL
void boot_delay(uint32_t Delay)
{
  __IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
  uint32_t tmpDelay = Delay;

  /* Add this code to indicate that local variable is not used */
  ((void)tmp);

  /* Add a period to guaranty minimum wait */
  if(tmpDelay < 0xFFFFFFFFU)
  {
    tmpDelay++;
  }

  while (tmpDelay != 0U)
  {
    if((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
    {
      tmpDelay--;
    }
  }
}

void boot_before_hw_hook()
{
  // Init clock and power
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();
  // Enable RTC
  LL_RCC_EnableRTC();
  // Check backup registry
  if (LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) == BOOT_REQUEST_DFU) {
    // Reset SRAM value to default
    LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, BOOT_REQUEST_NONE);
    // Remap, set MSP and pass control
    __HAL_REMAPMEMORY_SYSTEMFLASH();
    __ASM volatile ("movs r3, #0\nldr r3, [r3, #0]\nMSR msp, r3\n" : : : "r3", "sp");
    ((void (*)(void)) *((uint32_t*) 0x00000004))();
  }
}

void boot_after_hw_hook()
{
  // Ensure USB pins configuration
  LL_GPIO_SetPinMode(BOOT_USB_PORT, BOOT_USB_DP_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinMode(BOOT_USB_PORT, BOOT_USB_DM_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinSpeed(BOOT_USB_PORT, BOOT_USB_DP_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
  LL_GPIO_SetPinSpeed(BOOT_USB_PORT, BOOT_USB_DM_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
  LL_GPIO_SetPinOutputType(BOOT_USB_PORT, BOOT_USB_DP_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinOutputType(BOOT_USB_PORT, BOOT_USB_DM_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  // USB cable reset
  LL_GPIO_ResetOutputPin(BOOT_USB_PORT, BOOT_USB_PIN);
  boot_delay(10);
  LL_GPIO_SetOutputPin(BOOT_USB_PORT, BOOT_USB_PIN);
  // Check if DFU key pressed
  if ((BOOT_DFU_PORT->IDR & BOOT_DFU_PIN) != 0x00u) {
    boot_restart_in_dfu();
  }
}

void boot_restart_in_dfu()
{
  // Request DFU on boot
  LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, BOOT_REQUEST_DFU);
  // Reset
  NVIC_SystemReset();
}
