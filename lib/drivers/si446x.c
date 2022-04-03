#include "si446x.h"
#include <cmsis_os2.h>
#include <furi_hal_delay.h>
#include <assert.h>
#include <string.h>

bool si446x_wait_cts_spi(FuriHalSpiBusHandle* handle) {
    uint8_t buff_tx[2] = {SI446X_CMD_READ_CMD_BUFF, 0xFF};
    uint8_t buff_rx[2] = {0};
    uint8_t timeout = 40;
    while((buff_rx[1] != SI446X_CTS_OK) && timeout) {
        furi_hal_spi_acquire(handle);
        furi_hal_spi_bus_trx(handle, buff_tx, (uint8_t*)buff_rx, 2, SI446X_TIMEOUT);
        furi_hal_spi_release(handle);
        timeout--;
        delay(1);
        if(!timeout) {
            return false;
        }
    }
    return true;
}

bool si446x_write_data(FuriHalSpiBusHandle* handle, const uint8_t* data, uint8_t size) {
    bool ret = false;
    furi_hal_spi_acquire(handle);
    ret = furi_hal_spi_bus_tx(handle, (uint8_t*)data, size, SI446X_TIMEOUT);
    furi_hal_spi_release(handle);
    if(!ret) {
        ret = si446x_wait_cts_spi(handle);
    }
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

bool si446x_set_rssi_threshold(FuriHalSpiBusHandle* handle, int rssi_dbi) {
    if(rssi_dbi < -134 || rssi_dbi > -6) return false;
    uint8_t rssi[1] = {0};
    rssi[0] = (uint8_t)(2 * (rssi_dbi + 134)) & 0xFF;
    return si446x_set_properties(handle, SI446X_PROP_MODEM_RSSI_THRESH, &rssi[0], sizeof(rssi));
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
    //no need to poll CTS
    furi_hal_spi_acquire(handle);
    bool ret = furi_hal_spi_bus_tx(handle, &buff_tx[0], sizeof(buff_tx), SI446X_TIMEOUT);
    furi_hal_spi_release(handle);
    return ret;
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
        0, //rx len hi package
        len_package_rx, //rx len lo package
        SI446X_STATE_NOCHANGE, // RX Timeout
        state_on_tx_finish, //SI446X_STATE_RX, // RX Valid
        state_on_tx_finish // IDLE_STATE // RX Invalid (using SI446X_STATE_SLEEP for the INVALID_SYNC fix)
    };
    //no need to poll CTS
    furi_hal_spi_acquire(handle);
    bool ret = furi_hal_spi_bus_tx(handle, &buff_tx[0], sizeof(buff_tx), SI446X_TIMEOUT);
    furi_hal_spi_release(handle);
    return ret;
}

bool si446x_switch_to_idle(FuriHalSpiBusHandle* handle) {
    return si446x_set_state(handle, SI446X_STATE_READY);
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
    return si446x_write_data(handle, &buff_tx[0], pin + 2);
}

bool si446x_read_gpio(FuriHalSpiBusHandle* handle, SI446X_GPIO_t pin) {
    uint8_t buff_tx[] = {SI446X_CMD_GPIO_PIN_CFG};
    uint8_t buff_rx[7] = {0};
    si446x_write_data(handle, &buff_tx[0], sizeof(buff_tx));
    si446x_read_data(handle, &buff_rx[0], pin + 2);
    return (buff_rx[pin] & 0x80);
}

bool si446x_set_properties(
    FuriHalSpiBusHandle* handle,
    SI446X_Prop_t prop,
    uint8_t* data,
    uint8_t size) {
    if(size > 12) {
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

static bool si446x_get_outdiv_and_band(uint32_t freq_hz, uint8_t* outdiv, uint8_t* band) {
    // See Si446x Data Sheet section 5.3.1
    // Also the Si446x PLL Synthesizer / VCO_CNT Calculator Rev 0.4
    //ToDo It is necessary to check for different chips different frequencies and different coefficients
    if(freq_hz <= 1050000000 && freq_hz >= 850000000) {
        outdiv[0] = 4, band[0] = 0;
    } else if(freq_hz <= 525000000 && freq_hz >= 420000000) {
        outdiv[0] = 8, band[0] = 2;
    } else if(freq_hz < 420000000 && freq_hz >= 350000000) {
        outdiv[0] = 10, band[0] = 1;
    } else if(freq_hz < 350000000 && freq_hz >= 284000000) {
        outdiv[0] = 12, band[0] = 3;
    } else if(freq_hz <= 175000000 && freq_hz >= 142000000) {
        outdiv[0] = 24, band[0] = 5;
    } else {
        return false;
    }
    return true;
}

uint32_t si446x_set_frequency_and_step_channel(
    FuriHalSpiBusHandle* handle,
    uint32_t freq_hz,
    uint32_t step_channel_hz) {
    uint8_t outdiv;
    uint8_t band;

    if(!si446x_get_outdiv_and_band(freq_hz, &outdiv, &band)) {
        return 0;
    }

    uint8_t sy_sel = 8;
    uint8_t modem_clkgen[] = {sy_sel | band};
    if(!si446x_set_properties(
           handle, SI446X_PROP_MODEM_CLKGEN_BAND, &modem_clkgen[0], sizeof(modem_clkgen))) {
        return 0;
    }
    //sy_sel set => NPRESC=2
    uint32_t f_pfd = 2 * SI446X_QUARTZ / outdiv;
    uint32_t n = ((uint32_t)(freq_hz / f_pfd)) - 1;
    float ratio = freq_hz / (float)f_pfd;
    float rest = ratio - (float)n;
    uint32_t m = (uint32_t)(rest * 0x80000UL);
    uint32_t channel_increment = 0x80000UL * outdiv * step_channel_hz / (2 * SI446X_QUARTZ);
    uint8_t freq_control[] = {
        (uint8_t)n,
        (m >> 16) & 0xFF,
        (m >> 8) & 0xff,
        m & 0xff,
        (channel_increment >> 8) & 0xFF,
        channel_increment & 0xFF};
    if(!si446x_set_properties(
           handle, SI446X_PROP_FREQ_CONTROL_INTE, &freq_control[0], sizeof(freq_control))) {
        return 0;
    }
    //ToDo check!
    return (uint32_t)((float)m / 0x80000UL + n) * f_pfd;
}

bool si446x_set_deviation(FuriHalSpiBusHandle* handle, uint32_t freq_hz, uint32_t deviation_hz) {
    //sy_sel set => NPRESC=2
    //look datasheet API MODEM_FREQ_DEV
    uint8_t outdiv;
    uint8_t band;
    if(!si446x_get_outdiv_and_band(freq_hz, &outdiv, &band)) {
        return false;
    }
    uint32_t modem_freq_dev =
        (uint32_t)((0x80000UL * outdiv * deviation_hz) / (2 * SI446X_QUARTZ));
    uint8_t modem_freq_dev_buff[] = {
        (modem_freq_dev >> 16) & 0xFF, (modem_freq_dev >> 8) & 0xFF, modem_freq_dev & 0xFF};
    return si446x_set_properties(
        handle, SI446X_PROP_MODEM_FREQ_DEV_2, &modem_freq_dev_buff[0], sizeof(modem_freq_dev_buff));
}

bool si446x_set_bps(FuriHalSpiBusHandle* handle, uint32_t freq_hz, uint32_t bps) {
    //Fxtal for DR > 200kbps, and Fxtal/10 for DR ≥ 200kbps
    //Fxtal or Fxtal/10 (e.g., 0x1C9C380 = 30M decimal, or 0x02DC6C0 =3M decimal, assuming the crystal frequency is 30 MHz).
    uint8_t div = 0;
    if(bps > 200000) {
        div = 1;
    } else {
        div = 10;
    }
    bps *= div;

    //ToDo TXOSR = 0, 10x if 2GFSK or 4GFS is required see API datasheet MODEM_TX_NCO_MODE
    uint8_t txosr = 0;
    uint32_t ncomod = SI446X_QUARTZ / div;
    uint8_t modem_bps_buff[] = {
        (bps >> 16) & 0xFF,
        (bps >> 8) & 0xFF,
        bps & 0xFF,
        ((ncomod >> 24) & 0xFF) | txosr,
        (ncomod >> 16) & 0xFF,
        (ncomod >> 8) & 0xFF,
        ncomod & 0xFF};
    return si446x_set_properties(
        handle, SI446X_PROP_MODEM_DATA_RATE_2, &modem_bps_buff[0], sizeof(modem_bps_buff));
}
