#include "api-hal-subghz.h"
#include <stm32wbxx_ll_gpio.h>
#include <api-hal-gpio.h>
#include <api-hal-spi.h>
#include <cc1101.h>
#include "main.h"

void api_hal_subghz_init() {
    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);
    // Reset and shutdown
    cc1101_reset(device);
    cc1101_shutdown(device);
    api_hal_spi_device_return(device);
}

void api_hal_subghz_set_frequency(uint32_t value) {

}

void api_hal_subghz_set_path(ApiHalSubGhzPath path) {
    if (path == ApiHalSubGhzPath1) {
        LL_GPIO_ResetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_SetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    } else if (path == ApiHalSubGhzPath2) {
        LL_GPIO_SetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_ResetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    } else if (path == ApiHalSubGhzPath3) {
        LL_GPIO_SetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_SetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    } else if (path == ApiHalSubGhzPathIsolation) {
        LL_GPIO_ResetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_ResetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    }
}
