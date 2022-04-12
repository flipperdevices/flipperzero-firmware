#include <furi.h>
#include <furi_hal.h>
#include <flipper.h>
#include <alt_boot.h>

#define TAG "Main"

__attribute__((always_inline)) inline void main_ram() {
    // Flipper critical FURI HAL
    furi_hal_init_early();
    furi_hal_clock_init();
    furi_hal_console_init();
    furi_hal_rtc_init();

    // Initialize FURI layer
    furi_init();

    // Flipper FURI HAL
    furi_hal_init();

    // Init flipper
    flipper_init();

    furi_run();

    while(1) {
    }
}

__attribute__((always_inline)) inline void main_normal() {
    // Flipper critical FURI HAL
    furi_hal_init_early();

    if(furi_hal_boot_get_mode() == FuriHalBootModeDFU) {
        furi_hal_boot_set_mode(FuriHalBootModeNormal);
        flipper_boot_dfu_exec();
        furi_hal_power_reset();
    } else if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecuteUpdate)) {
        flipper_boot_update_exec();
        // if things go nice, we shouldn't reach this point.
        // But if we do, abandon
        furi_hal_rtc_reset_flag(FuriHalRtcFlagExecuteUpdate);
        furi_hal_rtc_set_flag(FuriHalRtcFlagExecutePostUpdate);
        furi_hal_power_reset();
    } else {
        furi_hal_clock_init();
        furi_hal_console_init();
        furi_hal_rtc_init();
        // Initialize FURI layer
        furi_init();

        // Flipper FURI HAL
        furi_hal_init();

        // Init flipper
        flipper_init();

        furi_run();
    }

    while(1) {
    }
}

int main(void) {
#ifdef FURI_RAM_EXEC
    main_ram();
#else
    main_normal();
#endif
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
