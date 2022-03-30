#include "si446x.h"
#include <cmsis_os2.h>
#include <furi_hal_delay.h>
#include <assert.h>
#include <string.h>

bool si446x_wait_cts_spi(FuriHalSpiBusHandle* handle) {
    uint8_t buff_tx[2] = {SI446X_CMD_READ_CMD_BUFF, 0xFF};
    uint8_t buff_rx[2] = {0};
    //ToDo add TIMEOUT
    while(buff_rx[1] != SI446X_CTS_OK) {
        furi_hal_spi_acquire(handle);
        furi_hal_spi_bus_trx(handle, buff_tx, (uint8_t*)buff_rx, 2, SI446X_TIMEOUT);
        furi_hal_spi_release(handle);
    }
    return true;
}

bool si446x_write_data(FuriHalSpiBusHandle* handle, const uint8_t* data, uint8_t size) {
    si446x_wait_cts_spi(handle);
    furi_hal_spi_acquire(handle);
    bool ret = furi_hal_spi_bus_tx(handle, (uint8_t*)data, size, SI446X_TIMEOUT);
    furi_hal_spi_release(handle);
    return ret;
}

bool si446x_read_data(FuriHalSpiBusHandle* handle, uint8_t* data, uint8_t size) {
    uint8_t buff_tx[2] = {SI446X_CMD_READ_CMD_BUFF, 0xFF};
    uint8_t buff_rx[2] = {0};
    bool ret = false;
    while(buff_rx[1] != SI446X_CTS_OK) {
        furi_hal_spi_acquire(handle);
        furi_hal_spi_bus_trx(handle, buff_tx, (uint8_t*)buff_rx, 2, SI446X_TIMEOUT);
        if(buff_rx[1] == SI446X_CTS_OK) {
            ret = furi_hal_spi_bus_rx(handle, (uint8_t*)data, size, SI446X_TIMEOUT);
        }
        furi_hal_spi_release(handle);
    }
    return ret;
}

bool si446x_clear_interrupt_status(FuriHalSpiBusHandle* handle) {
    uint8_t buff_tx[] = {
        SI446X_CMD_GET_INT_STATUS, SI446X_CMD_NOP, SI446X_CMD_NOP, SI446X_CMD_NOP};
    return si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
}

uint8_t si446x_get_version(FuriHalSpiBusHandle* handle) {
    uint8_t buff_tx[] = {SI446X_CMD_PART_INFO};
    uint8_t buff_rx[8] = {0};
    si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
    si446x_read_data(handle, &buff_rx[0], sizeof(buff_rx));
    return buff_rx[0];
}

uint16_t si446x_get_partnumber(FuriHalSpiBusHandle* handle) {
    uint8_t buff_tx[] = {SI446X_CMD_PART_INFO};
    uint8_t buff_rx[9] = {0};
    si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
    si446x_read_data(handle, &buff_rx[0], sizeof(buff_rx));
    return buff_rx[1] << 8 | buff_rx[2];
}

bool si446x_set_state(FuriHalSpiBusHandle* handle, SI446X_State_t state) {
    uint8_t buff_tx[2] = {SI446X_CMD_CHANGE_STATE, state};
    return si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
}

SI446X_State_t si446x_get_state(FuriHalSpiBusHandle* handle) {
    //ToDo add fast reg, need setting config
    //uint8_t state;
    //state=si446x_get_fast_reg(handle,SI446X_CMD_FRR_B_READ);
    uint8_t buff_tx[] = {SI446X_CMD_REQUEST_DEVICE_STATE};
    uint8_t buff_rx[2] = {0};
    si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
    si446x_read_data(handle, &buff_rx[0], sizeof(buff_rx));
    return buff_rx[0];
}

SI446X_State_t si446x_get_current_channel(FuriHalSpiBusHandle* handle) {
    uint8_t buff_tx[] = {SI446X_CMD_REQUEST_DEVICE_STATE};
    uint8_t buff_rx[2] = {0};
    si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
    si446x_read_data(handle, &buff_rx[0], sizeof(buff_rx));
    return buff_rx[1];
}

uint8_t si446x_get_fast_reg(FuriHalSpiBusHandle* handle, uint8_t reg) {
    uint8_t buff_rx[1] = {0};
    si446x_write_data(handle, &reg, 1);
    si446x_read_data(handle, &buff_rx[0], sizeof(buff_rx));
    return buff_rx[0];
}

uint8_t si446x_get_get_rssi(FuriHalSpiBusHandle* handle) {
    //ToDo add fast reg, need setting config
    // uint8_t rssi;
    // rssi = si446x_get_fast_reg(handle, SI446X_CMD_FRR_A_READ);
    // return rssi;
    uint8_t buff_tx[] = {SI446X_CMD_GET_MODEM_STATUS};
    uint8_t buff_rx[4] = {0};
    si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
    si446x_read_data(handle, &buff_rx[0], sizeof(buff_rx));
    return buff_rx[2];
}

uint8_t si446x_get_get_lqi(FuriHalSpiBusHandle* handle) {
    uint8_t buff_tx[] = {SI446X_CMD_GET_MODEM_STATUS};
    uint8_t buff_rx[4] = {0};
    si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
    si446x_read_data(handle, &buff_rx[0], sizeof(buff_rx));
    return buff_rx[3];
}

bool si446x_switch_to_start_tx(
    FuriHalSpiBusHandle* handle,
    uint8_t channel,
    SI446X_State_t state_on_tx_finish,
    uint8_t len_package_tx) {
    uint8_t buff_tx[] = {
        SI446X_CMD_START_TX, channel, (uint8_t)(state_on_tx_finish << 4), 0, len_package_tx, 0, 0};
    return si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
}

bool si446x_switch_to_start_rx(
    FuriHalSpiBusHandle* handle,
    uint8_t channel,
    SI446X_State_t state_on_tx_finish,
    uint8_t len_package_rx) {
    uint8_t buff_tx[] = {
        SI446X_CMD_START_RX,
        channel,
        0, //condition
        0, //rx lrn hi package
        len_package_rx, //rx lrn lo package
        SI446X_STATE_NOCHANGE, // RX Timeout
        SI446X_STATE_NOCHANGE, //SI446X_STATE_RX, // RX Valid
        state_on_tx_finish // IDLE_STATE // RX Invalid (using SI446X_STATE_SLEEP for the INVALID_SYNC fix)
    };
    return si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
}

bool si446x_switch_to_idle(FuriHalSpiBusHandle* handle) {
    // uint8_t buff_tx[] = {SI446X_CMD_CHANGE_STATE, SI446X_STATE_READY};
    // furi_hal_spi_acquire(handle);
    // bool ret = furi_hal_spi_bus_tx(handle, (uint8_t*)buff_tx, sizeof(buff_tx), SI446X_TIMEOUT);
    // furi_hal_spi_release(handle);

    return si446x_set_state(handle, SI446X_STATE_READY);
    //return ret;
}

bool si446x_write_gpio(FuriHalSpiBusHandle* handle, SI446X_GPIO_t pin, uint8_t gpio_mode) {
    uint8_t buff_tx[] = {
        SI446X_CMD_GPIO_PIN_CFG,
        SI446X_GPIO_MODE_DONOTHING, //SI446X_GPIO_Mode_t
        SI446X_GPIO_MODE_DONOTHING, //SI446X_GPIO_Mode_t
        SI446X_GPIO_MODE_DONOTHING, //SI446X_GPIO_Mode_t
        SI446X_GPIO_MODE_DONOTHING, //SI446X_GPIO_Mode_t
        SI446X_NIRQ_MODE_DONOTHING, //SI446X_NIRQ_Mode_t
        SI446X_SDO_MODE_DONOTHING, //SI446X_SDO_Mode_t
        SI446X_GPIO_DRV_HIGH};
    buff_tx[pin + 1] = gpio_mode;
    return si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
}

bool si446x_read_gpio(FuriHalSpiBusHandle* handle, SI446X_GPIO_t pin) {
    uint8_t buff_tx[] = {SI446X_CMD_GPIO_PIN_CFG};
    uint8_t buff_rx[7] = {0};
    si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
    si446x_read_data(handle, &buff_rx[0], sizeof(buff_rx));
    return (buff_rx[pin] & 0x80);
}

bool si446x_set_properties(
    FuriHalSpiBusHandle* handle,
    SI446X_Prop_t prop,
    uint8_t* data,
    uint8_t size) {
    if(size>12) {
        return false;
    }
    uint8_t buff_tx[16] = {SI446X_CMD_SET_PROPERTY, (uint8_t)(prop >> 8), size, (uint8_t)prop};
    memcpy(buff_tx + 4, data, size);
    return si446x_write_data(handle, &buff_tx[0], size + 4);
}

bool si446x_get_properties(
    FuriHalSpiBusHandle* handle,
    SI446X_Prop_t prop,
    uint8_t* data,
    uint8_t size) {
    uint8_t buff_tx[] = {SI446X_CMD_SET_PROPERTY, (uint8_t)(prop >> 8), size, (uint8_t)prop};
    si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
    return si446x_read_data(handle, &data[0], size);
}

bool si446x_set_pa(FuriHalSpiBusHandle* handle, uint8_t pa_level) {
    uint8_t buff_tx[] = {pa_level & 0x7F};
    return si446x_set_properties(handle, SI446X_PROP_PA_PWR_LVL, &buff_tx[0], sizeof(buff_tx));
}

uint32_t si446x_set_frequency_and_step_channel(
    FuriHalSpiBusHandle* handle,
    uint32_t freq_hz,
    uint32_t step_channel_hz) {
    // See Si446x Data Sheet section 5.3.1
    // Also the Si446x PLL Synthesizer / VCO_CNT Calculator Rev 0.4
    uint8_t outdiv;
    uint8_t band;

    if(freq_hz <= 1050000000 && freq_hz >= 850000000) {
        outdiv = 4, band = 0;
    } else if(freq_hz <= 525000000 && freq_hz >= 420000000) {
        outdiv = 8, band = 2;
    } else if(freq_hz < 420000000 && freq_hz >= 350000000) {
        outdiv = 10, band = 1;
    } else if(freq_hz < 350000000 && freq_hz >= 284000000) {
        outdiv = 12, band = 3;
    } else if(freq_hz <= 175000000 && freq_hz >= 142000000) {
        outdiv = 24, band = 5;
    } else {
        return 0;
    }
    uint8_t sy_sel = 8;
    uint8_t modem_clkgen[] = {sy_sel | band};
    if(!si446x_set_properties(
           handle, SI446X_PROP_MODEM_CLKGEN_BAND, &modem_clkgen[0], sizeof(modem_clkgen))) {
        return 0;
    }

    uint32_t f_pfd = 2 * SI446X_QUARTZ / outdiv;
    uint32_t n = ((uint32_t)(freq_hz / f_pfd)) - 1;
    float ratio = freq_hz / (float)f_pfd;
    float rest = ratio - (float)n;
    uint32_t m = (uint32_t)(rest * 524288UL);
    uint8_t m2 = m / 0x10000;
    uint8_t m1 = (m - m2 * 0x10000) / 0x100;
    uint8_t m0 = (m - m2 * 0x10000 - m1 * 0x100);

    uint32_t channel_increment = 524288UL * outdiv * step_channel_hz / (2 * SI446X_QUARTZ);
    uint8_t c1 = channel_increment / 0x100;
    uint8_t c0 = channel_increment - (0x100 * c1);

    uint8_t freq_control[] = {(uint8_t)n, m2, m1, m0, c1, c0};
    if(!si446x_set_properties(
           handle, SI446X_PROP_FREQ_CONTROL_INTE, &freq_control[0], sizeof(freq_control))) {
        return 0;
    }
    return (uint32_t)((float)((uint32_t)m2 << 16 | m1 << 8 | m0) / 524288UL + n) * f_pfd;
}
