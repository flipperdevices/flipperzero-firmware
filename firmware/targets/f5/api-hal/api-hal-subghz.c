#include "api-hal-subghz.h"
#include <stm32wbxx_ll_gpio.h>
#include <api-hal-gpio.h>
#include <api-hal-spi.h>
#include <cc1101.h>
#include "main.h"

void api_hal_subghz_init() {
    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);
    cc1101_init(device);
    api_hal_spi_device_return(device);
}

void api_hal_subghz_band_set(RfBand band) {
    if (band == RfBand1) {
        LL_GPIO_ResetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_SetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    } else if (band == RfBand2) {
        LL_GPIO_SetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_ResetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    } else if (band == RfBand3) {
        LL_GPIO_SetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_SetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    } else if (band == RfBandIsolation) {
        LL_GPIO_ResetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_ResetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    }
}
