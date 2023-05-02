#include "avr_isp_tpi_sw.h"

#include <furi.h>

//https://ww1.microchip.com/downloads/en/Appnotes/doc8373.pdf
//https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny4-5-9-10-Data-Sheet-DS40002060A.pdf

#define AVR_ISP_TPI_SW_DATA &gpio_ext_pa6
#define AVR_ISP_TPI_SW_SCK &gpio_ext_pb3
#define AVR_ISP_TPI_SW_RESET &gpio_ext_pb2

struct AvrIspTpiSw {
    AvrIspTpiSwSpeed speed_wait_time;
    const GpioPin* data;
    const GpioPin* sck;
    const GpioPin* res;
};

AvrIspTpiSw* avr_isp_tpi_sw_init(AvrIspTpiSwSpeed speed) {
    AvrIspTpiSw* instance = malloc(sizeof(AvrIspTpiSw));
    instance->speed_wait_time = speed;
    instance->data = AVR_ISP_TPI_SW_DATA;
    instance->sck = AVR_ISP_TPI_SW_SCK;
    instance->res = AVR_ISP_TPI_SW_RESET;

    furi_hal_gpio_init(instance->data, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_write(instance->sck, false);
    furi_hal_gpio_init(instance->sck, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(instance->res, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    return instance;
}

void avr_isp_tpi_sw_free(AvrIspTpiSw* instance) {
    furi_assert(instance);
    furi_hal_gpio_init(instance->res, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(instance->data, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(instance->sck, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    free(instance);
}

static inline void avr_isp_tpi_send_bit(AvrIspTpiSw* instance, bool bit) {
    furi_assert(instance);
    furi_hal_gpio_write(instance->data, bit);
    furi_hal_gpio_write(instance->sck, false);
    furi_delay_us(instance->speed_wait_time - 1);
    furi_hal_gpio_write(instance->sck, true);
    furi_delay_us(instance->speed_wait_time - 1);
}

static inline bool avr_isp_tpi_read_bit(AvrIspTpiSw* instance) {
    furi_assert(instance);

    furi_hal_gpio_write(instance->sck, false);
    furi_delay_us(instance->speed_wait_time - 1);
    bool ret = furi_hal_gpio_read(instance->data);
    furi_hal_gpio_write(instance->sck, true);
    furi_delay_us(instance->speed_wait_time - 1);
    return ret;
}

void avr_isp_tpi_sw_tx(AvrIspTpiSw* instance, uint8_t data) {
    furi_assert(instance);
    uint8_t parity = 0;

    //set data output
    furi_hal_gpio_init(instance->data, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    //send start bit
    avr_isp_tpi_send_bit(instance, false);

    //send data, LSB first
    for(uint8_t i = 0; i < 8; ++i) {
        avr_isp_tpi_send_bit(instance, data & 0x1);
        parity ^= data & 0x1;
        data >>= 1;
    }

    //send parity bit
    avr_isp_tpi_send_bit(instance, parity);

    //send 2 stop bit
    avr_isp_tpi_send_bit(instance, true);
    avr_isp_tpi_send_bit(instance, true);

    //set data input
    furi_hal_gpio_init(instance->data, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
}

bool avr_isp_tpi_sw_rx(AvrIspTpiSw* instance, uint8_t* data) {
    furi_assert(instance);

    //wait for start bit
    uint8_t timeout = 255;
    while((avr_isp_tpi_read_bit(instance) == true) && (timeout--)) {
    };
    if(timeout == 0) {
        return false;
    }

    //read data, LSB first
    data[0] = 0;
    uint8_t parity = 0;
    for(uint8_t i = 0; i < 8; ++i) {
        bool bit = avr_isp_tpi_read_bit(instance);
        data[0] >>= 1;
        data[0] |= (bit << 7);
        parity ^= bit;
    }
    // check parity
    parity = (parity == avr_isp_tpi_read_bit(instance));

    //read 2 stop bit
    avr_isp_tpi_read_bit(instance);
    avr_isp_tpi_read_bit(instance);

    return parity ? true : false;
}

// void avr_isp_tpi_sw_res_set(AvrIspTpiSw* instance, bool state) {
//     furi_assert(instance);
//     furi_hal_gpio_write(instance->res, state);
// }

// void avr_isp_tpi_sw_sck_set(AvrIspTpiSw* instance, bool state) {
//     furi_assert(instance);
//     furi_hal_gpio_write(instance->sck, state);
// }

void avr_isp_tpi_sw_start_pmode(AvrIspTpiSw* instance) {
    furi_assert(instance);

    furi_hal_gpio_write(instance->res, true);
    furi_delay_ms(20);

    //Wait tTOUT and then set the RESET pin low. This will reset the device and enable the TPI physical layer.
    // The RESET pin must then be kept low for the entire programming session
    furi_hal_gpio_write(instance->res, false);
    //wait reset timeout
    furi_delay_ms(20);
    //Keep the TPIDATA pin high for 16 TPICLK cycles

    //set data output
    furi_hal_gpio_init(instance->data, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    for(uint8_t i = 0; i < 32; i++) {
        avr_isp_tpi_send_bit(instance, true);
    }
    //set data input
    furi_hal_gpio_init(instance->data, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
}

void avr_isp_tpi_sw_end_pmode(AvrIspTpiSw* instance) {
    furi_assert(instance);
    //Set the RESET pin high. This will disable the TPI physical layer and reset the device.
    furi_hal_gpio_write(instance->res, true);
}