/**
MIT License

Copyright (c) 2023 Vojtech Suk (https://github.com/sukvojte)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

/**
This file is based on modified original from https://github.com/aappleby/PicoRVD
*/

#include "swio.h"
#include "swio_inner.h"

#include "../utils.h"
#include "../config.h"

#define TAG "WSF_SWIO"

WchSwioFlasher_SWIO* WchSwioFlasher_SWIO_create() {
    WchSwioFlasher_SWIO* handle = malloc(sizeof(WchSwioFlasher_SWIO));

    // TODO: check Flipper freq and MCU type

    furi_hal_gpio_init(&SWI_GPIO, GpioModeOutputPushPull, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_init(&SWI_HW_RST_GPIO, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_write(&SWI_HW_RST_GPIO, true);
    furi_hal_gpio_write(&SWI_GPIO, true);

    return handle;
}

WchSwioFlasher_Error WchSwioFlasher_SWIO_hw_reset(WchSwioFlasher_SWIO* handle) {
    UNUSED(handle);
    furi_hal_gpio_write(&SWI_GPIO, true);
    furi_hal_gpio_write(&SWI_HW_RST_GPIO, false);
    furi_delay_ms(50);
    furi_hal_gpio_write(&SWI_HW_RST_GPIO, true);

    return WchSwioFlasher_Ok;
}

void WchSwioFlasher_SWIO_destroy(WchSwioFlasher_SWIO* handle) {
    furi_hal_gpio_init(&SWI_GPIO, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    free(handle);
}

WchSwioFlasher_Error WchSwioFlasher_SWIO_init(WchSwioFlasher_SWIO* handle) {
    UNUSED(handle);

    // Do it twice for sure
    for(uint8_t i = 0; i < 2; i++) {
        CHECK_ERR_M(
            WchSwioFlasher_SWIO_write(handle, WCH_DM_SHDWCFGR, 0x5AA50400),
            "unable to write magic to SHDWCFGR");

        CHECK_ERR_M(
            WchSwioFlasher_SWIO_write(handle, WCH_DM_CFGR, 0x5AA50400),
            "unable to write magic to CFGR");
    }

    uint32_t cpbr = 0;
    CHECK_ERR_M(WchSwioFlasher_SWIO_read(handle, WCH_DM_CPBR, &cpbr), "unable to read CPBR");

    uint16_t swd_version = cpbr >> 16;
    switch(swd_version) {
    case 1:
        break;
    default:
        return LOG_ERR_M(
            WchSwioFlasher_Error_TargetNotKnown,
            "SWD version %04X isn't supported",
            _UI(swd_version));
    }

    FURI_LOG_I(TAG, "Found %04X version SWD interface", _UI(swd_version));

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error
    WchSwioFlasher_SWIO_write(WchSwioFlasher_SWIO* handle, uint8_t address, uint32_t data) {
    UNUSED(handle);
    return WchSwioFlasher_SWIO_rxtx(address, WchSwioFlasher_SWIO_Write, &data);
}

WchSwioFlasher_Error
    WchSwioFlasher_SWIO_read(WchSwioFlasher_SWIO* handle, uint8_t address, uint32_t* data) {
    UNUSED(handle);
    return WchSwioFlasher_SWIO_rxtx(address, WchSwioFlasher_SWIO_Read, data);
}
