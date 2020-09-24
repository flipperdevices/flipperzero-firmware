#include "boot.h"
#include "cmsis_os.h"
#include "stm32l4xx.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"

// Boot request enum
#define BOOT_REQUEST_NONE   0x00000000
#define BOOT_REQUEST_DFU    0xDF00B000

void on_os_boot()
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

void restart_in_dfu()
{
  // Request DFU on boot
  LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, BOOT_REQUEST_DFU);
  // Reset
  NVIC_SystemReset();
}
