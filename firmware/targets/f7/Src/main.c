#include "main.h"

#include <furi.h>
#include <furi_hal.h>
#include <flipper.h>
#include <update.h>

#define TAG "Main"

int main(void) {
    // Flipper critical FURI HAL
    furi_hal_init_critical();

#ifdef FURI_RAM_EXEC
    if(false) {
#else
    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecuteUpdate)) {
        //if (true) {
#endif
        flipper_update_exec();
        // if things go nice, we shouldn't reach this point.
        // But if we do, abandon
        furi_hal_rtc_reset_flag(FuriHalRtcFlagExecuteUpdate);
        furi_hal_rtc_set_flag(FuriHalRtcFlagExecutePostUpdate);
        furi_hal_power_reset();
    } else {
        // Initialize FURI layer
        furi_init();

        // Initialize ST HAL
        HAL_Init();

        // Flipper FURI HAL
        furi_hal_init();

        // CMSIS initialization
        osKernelInitialize();
        FURI_LOG_I(TAG, "KERNEL OK");

        // Init flipper
        flipper_init();

        // Start kernel
        osKernelStart();
    }
    while(1) {
    }
}

void Error_Handler(void) {
    furi_crash("ErrorHandler");
}

#ifdef USE_FULL_ASSERT
/**
    * @brief  Reports the name of the source file and the source line number
    *         where the assert_param error has occurred.
    * @param  file: pointer to the source file name
    * @param  line: assert_param error line source number
    * @retval None
    */
void assert_failed(uint8_t* file, uint32_t line) {
    furi_crash("HAL assert failed");
}
#endif /* USE_FULL_ASSERT */