#include "cc112x.h"
#include <cmsis_os2.h>
#include <furi_hal_delay.h>
#include <assert.h>
#include <string.h>

CC112XStatus cc112x_strobe(FuriHalSpiBusHandle* handle, uint8_t strobe) {
    uint8_t tx[1] = {strobe};
    CC112XStatus rx[1] = {0};

    while(furi_hal_gpio_read(handle->miso))
        ;
    furi_hal_spi_bus_trx(handle, tx, (uint8_t*)rx, 1, CC112X_TIMEOUT);

    assert(rx[0].CHIP_RDYn == 0);
    return rx[0];
}

CC112XStatus cc112x_write_reg(FuriHalSpiBusHandle* handle, uint16_t reg, uint8_t data) {
    uint8_t tx[3] = {0};
    CC112XStatus rx[3] = {0};

    if((reg & CC112X_EXT_REG_MASK) == CC112X_EXT_REG_MASK) {
        tx[0] = (reg >> 8) & 0xFF;
        tx[1] = reg & 0xFF;
        tx[2] = data;
        while(furi_hal_gpio_read(handle->miso))
            ;
        furi_hal_spi_bus_trx(handle, tx, (uint8_t*)rx, 3, CC112X_TIMEOUT);
        assert((rx[1].CHIP_RDYn | rx[2].CHIP_RDYn) == 0);
        return rx[2];
    } else {
        tx[0] = reg;
        tx[1] = data;

        while(furi_hal_gpio_read(handle->miso))
            ;
        furi_hal_spi_bus_trx(handle, tx, (uint8_t*)rx, 2, CC112X_TIMEOUT);
        assert((rx[0].CHIP_RDYn | rx[1].CHIP_RDYn) == 0);
        return rx[1];
    }
}

CC112XStatus cc112x_read_reg(FuriHalSpiBusHandle* handle, uint16_t reg, uint8_t* data) {
    assert(sizeof(CC112XStatus) == 1);
    uint8_t tx[3] = {0};
    CC112XStatus rx[3] = {0};

    if((reg & CC112X_EXT_REG_MASK) == CC112X_EXT_REG_MASK) {
        tx[0] = ((reg >> 8) & 0xFF) | CC112X_READ;
        tx[1] = reg & 0xFF;
        tx[2] = 0;

        while(furi_hal_gpio_read(handle->miso))
            ;
        furi_hal_spi_bus_trx(handle, tx, (uint8_t*)rx, 3, CC112X_TIMEOUT);

        assert((rx[1].CHIP_RDYn) == 0);
        *data = *(uint8_t*)&rx[2];
        return rx[1];
    } else {
        tx[0] = reg;
        tx[1] = 0;
        while(furi_hal_gpio_read(handle->miso))
            ;
        furi_hal_spi_bus_trx(handle, tx, (uint8_t*)rx, 2, CC112X_TIMEOUT);

        assert((rx[0].CHIP_RDYn) == 0);
        *data = *(uint8_t*)&rx[1];
        return rx[0];
    }
}

uint8_t cc112x_get_partnumber(FuriHalSpiBusHandle* handle) {
    uint8_t partnumber = 0;
    cc112x_read_reg(handle, CC112X_PARTNUMBER | CC112X_BURST_EXT, &partnumber);
    return partnumber;
}

uint8_t cc112x_get_version(FuriHalSpiBusHandle* handle) {
    uint8_t version = 0;
    cc112x_read_reg(handle, CC112X_PARTVERSION | CC112X_BURST_EXT, &version);
    return version;
}

uint8_t cc112x_get_rssi(FuriHalSpiBusHandle* handle) {
    uint8_t rssi = 0;
    cc112x_read_reg(handle, CC112X_RSSI1 | CC112X_READ_EXT, &rssi);
    return rssi;
}

void cc112x_reset(FuriHalSpiBusHandle* handle) {
    cc112x_strobe(handle, CC112X_STROBE_SRES);
}

CC112XStatus cc112x_get_status(FuriHalSpiBusHandle* handle) {
    return cc112x_strobe(handle, CC112X_STROBE_SNOP);
}

void cc112x_shutdown(FuriHalSpiBusHandle* handle) {
    cc112x_strobe(handle, CC112X_STROBE_SPWD);
}

void cc112x_calibrate(FuriHalSpiBusHandle* handle) {
    cc112x_strobe(handle, CC112X_STROBE_SCAL);
}

void cc112x_switch_to_idle(FuriHalSpiBusHandle* handle) {
    cc112x_strobe(handle, CC112X_STROBE_SIDLE);
}

void cc112x_switch_to_rx(FuriHalSpiBusHandle* handle) {
    cc112x_strobe(handle, CC112X_STROBE_SRX);
}

void cc112x_switch_to_tx(FuriHalSpiBusHandle* handle) {
    cc112x_strobe(handle, CC112X_STROBE_STX);
}

void cc112x_flush_rx(FuriHalSpiBusHandle* handle) {
    cc112x_strobe(handle, CC112X_STROBE_SFRX);
}

void cc112x_flush_tx(FuriHalSpiBusHandle* handle) {
    cc112x_strobe(handle, CC112X_STROBE_SFTX);
}

static bool cc112x_get_lodiv_and_band(uint32_t freq_hz, uint8_t* lodiv, uint8_t* band) {
    // See CC1120 Data Sheet 9.12 RF Programming

    if(freq_hz <= 960000000 && freq_hz >= 820000000) {
        lodiv[0] = 4, band[0] = 0b0010;
    } else if(freq_hz <= 480000000 && freq_hz >= 410000000) {
        lodiv[0] = 8, band[0] = 0b0100;
    } else if(freq_hz < 320000000 && freq_hz >= 273300000) {
        lodiv[0] = 12, band[0] = 0b0110;
    } else if(freq_hz < 240000000 && freq_hz >= 205000000) {
        lodiv[0] = 16, band[0] = 0b1000;
    } else if(freq_hz <= 192000000 && freq_hz >= 164000000) {
        lodiv[0] = 20, band[0] = 0b1010;
    } else if(freq_hz <= 160000000 && freq_hz >= 136700000) {
        lodiv[0] = 24, band[0] = 0b1011;
    } else {
        return false;
    }
    return true;
}

uint32_t cc112x_set_frequency(FuriHalSpiBusHandle* handle, uint32_t value) {
    uint8_t lodiv;
    uint8_t band;
    //printf("%d\r\n", value);
    if(!cc112x_get_lodiv_and_band(value, &lodiv, &band)) {
        return 0;
    }
    // See CC1120 Data Sheet 9.12 RF Programming
    uint8_t fs_lock_en =
        (1 << 4); // 0 - Out of lock detector disabled, 1 - Out of lock detector enabled
    uint8_t freq_sync_conf = fs_lock_en | band;
    cc112x_write_reg(handle, CC112X_FS_CFG, freq_sync_conf);

    // uint8_t freq_off_1 = 0;
    // uint8_t freq_off_0 = 0;
    // //cc112x_read_reg(handle, CC112X_FREQOFF1 | CC112X_BURST_EXT, &freq_off_1);
    // //cc112x_read_reg(handle, CC112X_FREQOFF0 | CC112X_BURST_EXT, &freq_off_0);

    uint64_t fvco = value * lodiv;
    uint64_t freq = (fvco << 16) / CC112X_QUARTZ;

    assert((freq & CC112X_FMASK) == freq);

    cc112x_write_reg(handle, CC112X_FREQ2, (freq >> 16) & 0xFF);
    cc112x_write_reg(handle, CC112X_FREQ1, (freq >> 8) & 0xFF);
    cc112x_write_reg(handle, CC112X_FREQ0, (freq >> 0) & 0xFF);

    return (uint32_t)((freq * CC112X_QUARTZ) >> 16) / lodiv;
}

uint32_t cc112x_set_intermediate_frequency(FuriHalSpiBusHandle* handle, uint32_t value) {
    // uint64_t real_value = value * CC112X_IFDIV / CC112X_QUARTZ;
    // assert((real_value & 0xFF) == real_value);

    // cc112x_write_reg(handle, CC112X_FSCTRL0, (real_value >> 0) & 0xFF);

    // uint64_t real_frequency = real_value * CC112X_QUARTZ / CC112X_IFDIV;

    // return (uint32_t)real_frequency;
    return 0;
}

void cc112x_set_pa_table(FuriHalSpiBusHandle* handle, const uint8_t value[8]) {
    // uint8_t tx[9] = {CC112X_PATABLE | CC112X_BURST};
    // CC112XStatus rx[9] = {0};

    // memcpy(&tx[1], &value[0], 8);

    // while(furi_hal_gpio_read(handle->miso))
    //     ;
    // furi_hal_spi_bus_trx(handle, tx, (uint8_t*)rx, sizeof(rx), CC112X_TIMEOUT);

    // assert((rx[0].CHIP_RDYn | rx[8].CHIP_RDYn) == 0);
}

uint8_t cc112x_write_fifo(FuriHalSpiBusHandle* handle, const uint8_t* data, uint8_t size) {
    // uint8_t buff_tx[64];
    // uint8_t buff_rx[64];
    // buff_tx[0] = CC112X_FIFO | CC112X_BURST;
    // memcpy(&buff_tx[1], data, size);

    // // Start transaction
    // // Wait IC to become ready
    // while(furi_hal_gpio_read(handle->miso))
    //     ;
    // // Tell IC what we want
    // furi_hal_spi_bus_trx(handle, buff_tx, (uint8_t*)buff_rx, size + 1, CC112X_TIMEOUT);

    // return size;
    return 0;
}

uint8_t cc112x_read_fifo(FuriHalSpiBusHandle* handle, uint8_t* data, uint8_t* size) {
    // uint8_t buff_tx[64];
    // buff_tx[0] = CC112X_FIFO | CC112X_READ | CC112X_BURST;
    // uint8_t buff_rx[2];

    // // Start transaction
    // // Wait IC to become ready
    // while(furi_hal_gpio_read(handle->miso))
    //     ;

    // // First byte - packet length
    // furi_hal_spi_bus_trx(handle, buff_tx, buff_rx, 2, CC112X_TIMEOUT);

    // // Check that the packet is placed in the receive buffer
    // if(buff_rx[1] > 64) {
    //     *size = 64;
    // } else {
    //     *size = buff_rx[1];
    // }
    // furi_hal_spi_bus_trx(handle, &buff_tx[1], data, *size, CC112X_TIMEOUT);

    // return *size;
    return 0;
}
