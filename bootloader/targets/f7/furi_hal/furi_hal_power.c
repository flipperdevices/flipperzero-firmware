#include <furi_hal.h>

void furi_hal_power_enable_external_3_3v() {
    LL_GPIO_SetOutputPin(PERIPH_POWER_GPIO_Port, PERIPH_POWER_Pin);
}

void furi_hal_power_disable_external_3_3v() {
    LL_GPIO_ResetOutputPin(PERIPH_POWER_GPIO_Port, PERIPH_POWER_Pin);
}
