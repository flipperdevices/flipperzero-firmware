// Modified by vad7, 24.02.2023
//
#include "nrf24.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <assert.h>
#include <string.h>

void nrf24_init() {
    // this is needed if multiple SPI devices are connected to the same bus but with different CS pins
    if(CFW_SETTINGS()->spi_nrf24_handle == SpiDefault) {
        furi_hal_gpio_init_simple(&gpio_ext_pc3, GpioModeOutputPushPull);
        furi_hal_gpio_write(&gpio_ext_pc3, true);
    } else if(CFW_SETTINGS()->spi_nrf24_handle == SpiExtra) {
        furi_hal_gpio_init_simple(&gpio_ext_pa4, GpioModeOutputPushPull);
        furi_hal_gpio_write(&gpio_ext_pa4, true);
    }

    furi_hal_spi_bus_handle_init(nrf24_HANDLE);
    furi_hal_spi_acquire(nrf24_HANDLE);
    furi_hal_gpio_init(nrf24_CE_PIN, GpioModeOutputPushPull, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_write(nrf24_CE_PIN, false);
}

void nrf24_deinit() {
    furi_hal_spi_release(nrf24_HANDLE);
    furi_hal_spi_bus_handle_deinit(nrf24_HANDLE);
    furi_hal_gpio_write(nrf24_CE_PIN, false);
    furi_hal_gpio_init(nrf24_CE_PIN, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    // resetting the CS pins to floating
    if(CFW_SETTINGS()->spi_nrf24_handle == SpiDefault) {
        furi_hal_gpio_init_simple(&gpio_ext_pc3, GpioModeAnalog);
    } else if(CFW_SETTINGS()->spi_nrf24_handle == SpiExtra) {
        furi_hal_gpio_init_simple(&gpio_ext_pa4, GpioModeAnalog);
    }
}

void nrf24_spi_trx(FuriHalSpiBusHandle* handle, uint8_t* tx, uint8_t* rx, uint8_t size) {
    furi_hal_gpio_write(handle->cs, false);
    furi_hal_spi_bus_trx(handle, tx, rx, size, nrf24_TIMEOUT);
    furi_hal_gpio_write(handle->cs, true);
}

uint8_t nrf24_write_reg(FuriHalSpiBusHandle* handle, uint8_t reg, uint8_t data) {
    uint8_t buf[] = {W_REGISTER | (REGISTER_MASK & reg), data};
    nrf24_spi_trx(handle, buf, buf, 2);
    //FURI_LOG_D("NRF_WR", " #%02X=%02X", reg, data);
    return buf[0];
}

uint8_t
    nrf24_write_buf_reg(FuriHalSpiBusHandle* handle, uint8_t reg, uint8_t* data, uint8_t size) {
    uint8_t buf[size + 1];
    buf[0] = W_REGISTER | (REGISTER_MASK & reg);
    memcpy(&buf[1], data, size);
    nrf24_spi_trx(handle, buf, buf, size + 1);
    //FURI_LOG_D("NRF_WR", " #%02X(%02X)=0x%02X%02X%02X%02X%02X", reg, size, data[0], data[1], data[2], data[3], data[4] );
    return buf[0];
}

uint8_t nrf24_read_reg(FuriHalSpiBusHandle* handle, uint8_t reg, uint8_t* data, uint8_t size) {
    uint8_t buf[size + 1];
    memset(buf, 0, size + 1);
    buf[0] = R_REGISTER | (REGISTER_MASK & reg);
    nrf24_spi_trx(handle, buf, buf, size + 1);
    memcpy(data, &buf[1], size);
    return buf[0];
}

uint8_t nrf24_read_register(FuriHalSpiBusHandle* handle, uint8_t reg) {
    uint8_t buf[] = {R_REGISTER | (REGISTER_MASK & reg), 0};
    nrf24_spi_trx(handle, buf, buf, 2);
    return buf[1];
}

uint8_t nrf24_flush_rx(FuriHalSpiBusHandle* handle) {
    uint8_t tx[] = {FLUSH_RX};
    uint8_t rx[] = {0};
    nrf24_spi_trx(handle, tx, rx, 1);
    return rx[0];
}

uint8_t nrf24_flush_tx(FuriHalSpiBusHandle* handle) {
    uint8_t tx[] = {FLUSH_TX};
    uint8_t rx[] = {0};
    nrf24_spi_trx(handle, tx, rx, 1);
    return rx[0];
}

uint8_t nrf24_get_maclen(FuriHalSpiBusHandle* handle) {
    uint8_t maclen;
    nrf24_read_reg(handle, REG_SETUP_AW, &maclen, 1);
    maclen &= 3;
    return maclen + 2;
}

uint8_t nrf24_set_maclen(FuriHalSpiBusHandle* handle, uint8_t maclen) {
    assert(maclen > 1 && maclen < 6);
    uint8_t status = 0;
    status = nrf24_write_reg(handle, REG_SETUP_AW, maclen - 2);
    return status;
}

uint8_t nrf24_status(FuriHalSpiBusHandle* handle) {
    uint8_t tx = RF24_NOP;
    nrf24_spi_trx(handle, &tx, &tx, 1);
    return tx;
}

uint32_t nrf24_get_rate(FuriHalSpiBusHandle* handle) {
    uint8_t setup = 0;
    uint32_t rate = 0;
    nrf24_read_reg(handle, REG_RF_SETUP, &setup, 1);
    setup &= 0x28;
    if(setup == 0x20)
        rate = 250000; // 250kbps
    else if(setup == 0x08)
        rate = 2000000; // 2Mbps
    else if(setup == 0x00)
        rate = 1000000; // 1Mbps

    return rate;
}

uint8_t nrf24_set_rate(FuriHalSpiBusHandle* handle, uint32_t rate) {
    uint8_t r6 = 0;
    uint8_t status = 0;
    if(!rate) rate = 2000000;

    nrf24_read_reg(handle, REG_RF_SETUP, &r6, 1); // RF_SETUP register
    r6 = r6 & (~0x28); // Clear rate fields.
    if(rate == 2000000)
        r6 = r6 | 0x08;
    else if(rate == 1000000)
        r6 = r6;
    else if(rate == 250000)
        r6 = r6 | 0x20;

    status = nrf24_write_reg(handle, REG_RF_SETUP, r6); // Write new rate.
    return status;
}

uint8_t nrf24_get_chan(FuriHalSpiBusHandle* handle) {
    uint8_t channel = 0;
    nrf24_read_reg(handle, REG_RF_CH, &channel, 1);
    return channel;
}

uint8_t nrf24_set_chan(FuriHalSpiBusHandle* handle, uint8_t chan) {
    uint8_t status;
    status = nrf24_write_reg(handle, REG_RF_CH, chan);
    return status;
}

uint8_t nrf24_get_src_mac(FuriHalSpiBusHandle* handle, uint8_t* mac) {
    uint8_t size = 0;
    uint8_t status = 0;
    size = nrf24_get_maclen(handle);
    status = nrf24_read_reg(handle, REG_RX_ADDR_P0, mac, size);
    return status;
}

uint8_t nrf24_set_src_mac(FuriHalSpiBusHandle* handle, uint8_t* mac, uint8_t size) {
    uint8_t status = 0;
    uint8_t clearmac[] = {0, 0, 0, 0, 0};
    nrf24_set_maclen(handle, size);
    nrf24_write_buf_reg(handle, REG_RX_ADDR_P0, clearmac, 5);
    status = nrf24_write_buf_reg(handle, REG_RX_ADDR_P0, mac, size);
    return status;
}

uint8_t nrf24_get_dst_mac(FuriHalSpiBusHandle* handle, uint8_t* mac) {
    uint8_t size = 0;
    uint8_t status = 0;
    size = nrf24_get_maclen(handle);
    status = nrf24_read_reg(handle, REG_TX_ADDR, mac, size);
    return status;
}

uint8_t nrf24_set_dst_mac(FuriHalSpiBusHandle* handle, uint8_t* mac, uint8_t size) {
    uint8_t status = 0;
    uint8_t clearmac[] = {0, 0, 0, 0, 0};
    nrf24_set_maclen(handle, size);
    nrf24_write_buf_reg(handle, REG_TX_ADDR, clearmac, 5);
    status = nrf24_write_buf_reg(handle, REG_TX_ADDR, mac, size);
    return status;
}

uint8_t nrf24_get_packetlen(FuriHalSpiBusHandle* handle, uint8_t pipe) {
    uint8_t len = 0;
    if(pipe > 5) pipe = 0;
    nrf24_read_reg(handle, RX_PW_P0 + pipe, &len, 1);
    return len;
}

uint8_t nrf24_set_packetlen(FuriHalSpiBusHandle* handle, uint8_t len) {
    uint8_t status = 0;
    status = nrf24_write_reg(handle, RX_PW_P0, len);
    return status;
}

// packet_size: 0 - dyn payload (read from PL_WID), 1 - read from pipe size, >1 - override
// Return STATUS reg + additional: RX_DR - new data available, 0x80 - NRF24 hardware error
uint8_t nrf24_rxpacket(
    FuriHalSpiBusHandle* handle,
    uint8_t* packet,
    uint8_t* ret_packetsize,
    uint8_t packet_size) {
    uint8_t status = 0;
    uint8_t buf[33]; // 32 max payload size + 1 for command

    status = nrf24_status(handle);
    if(!(status & RX_DR)) {
        uint8_t st = nrf24_read_register(handle, REG_FIFO_STATUS);
        if(st == 0xFF || st == 0) return 0x80; // hardware error
        if((st & 1) == 0) {
            FURI_LOG_D("NRF", "FIFO PKT");
            status |= RX_DR; // packet in FIFO buffer
        }
    }
    if(status & RX_DR) {
        if(status & 0x80) return 0x80; // hardware error
        if(packet_size == 1)
            packet_size = nrf24_get_packetlen(handle, (status >> 1) & 7);
        else if(packet_size == 0) {
            buf[0] = R_RX_PL_WID;
            buf[1] = 0xFF;
            nrf24_spi_trx(handle, buf, buf, 2);
            packet_size = buf[1];
        }
        if(packet_size > 32 || packet_size == 0) packet_size = 32;
        memset(buf, 0, packet_size + 1);
        buf[0] = R_RX_PAYLOAD;
        nrf24_spi_trx(handle, buf, buf, packet_size + 1);
        memcpy(packet, &buf[1], packet_size);
        nrf24_write_reg(handle, REG_STATUS, RX_DR); // clear RX_DR
    }
    if(status & (MAX_RT)) { // MAX_RT
        nrf24_write_reg(handle, REG_STATUS, (MAX_RT)); // clear MAX_RT.
    }

    *ret_packetsize = packet_size;
    return status;
}

// Return 0 when error
uint8_t nrf24_txpacket(FuriHalSpiBusHandle* handle, uint8_t* payload, uint8_t size, bool ack) {
    uint8_t status = 0;
    uint8_t buf[size + 1];
    buf[0] = ack ? W_TX_PAYLOAD : W_TX_PAYLOAD_NOACK;
    memcpy(&buf[1], payload, size);
    nrf24_set_tx_mode(handle);
    nrf24_spi_trx(handle, buf, buf, size + 1);
    uint32_t start_time = furi_get_tick();
    do {
        furi_delay_us(100);
        status = nrf24_status(handle);
    } while(!(status & (TX_DS | MAX_RT)) && furi_get_tick() - start_time < 100UL);
    if(status & MAX_RT) {
        if(furi_log_get_level() == FuriLogLevelDebug)
            FURI_LOG_D(
                "NRF", "MAX RT: %X (%X)", nrf24_read_register(handle, REG_OBSERVE_TX), status);
        nrf24_flush_tx(handle);
    }
    furi_hal_gpio_write(nrf24_CE_PIN, false);
    //nrf24_set_idle(handle);
    if(status & (TX_DS | MAX_RT)) nrf24_write_reg(handle, REG_STATUS, TX_DS | MAX_RT);
    return status & TX_DS;
}

uint8_t nrf24_power_up(FuriHalSpiBusHandle* handle) {
    uint8_t status = 0;
    uint8_t cfg = 0;
    nrf24_read_reg(handle, REG_CONFIG, &cfg, 1);
    cfg = cfg | 2;
    status = nrf24_write_reg(handle, REG_CONFIG, cfg);
    //furi_delay_ms(1000);
    return status;
}

uint8_t nrf24_set_idle(FuriHalSpiBusHandle* handle) {
    uint8_t status = 0;
    uint8_t cfg = 0;
    nrf24_read_reg(handle, REG_CONFIG, &cfg, 1);
    cfg &= 0xfc; // clear bottom two bits to power down the radio
    status = nrf24_write_reg(handle, REG_CONFIG, cfg);
    furi_hal_gpio_write(nrf24_CE_PIN, false);
    return status;
}

uint8_t nrf24_set_rx_mode(FuriHalSpiBusHandle* handle) {
    uint8_t cfg = 0;
    cfg = nrf24_read_register(handle, REG_CONFIG);
    cfg |= 0x03; // PWR_UP, and PRIM_RX
    cfg = nrf24_write_reg(handle, REG_CONFIG, cfg);
    furi_hal_gpio_write(nrf24_CE_PIN, true);
    return cfg;
}

uint8_t nrf24_set_tx_mode(FuriHalSpiBusHandle* handle) {
    uint8_t reg;
    furi_hal_gpio_write(nrf24_CE_PIN, false);
    //nrf24_write_reg(handle, REG_STATUS, TX_DS | MAX_RT);
    reg = nrf24_read_register(handle, REG_CONFIG);
    reg &= ~0x01; // disable PRIM_RX
    reg |= 0x02; // PWR_UP
    reg = nrf24_write_reg(handle, REG_CONFIG, reg);
    furi_hal_gpio_write(nrf24_CE_PIN, true);
    return reg;
}

void hexlify(uint8_t* in, uint8_t size, char* out) {
    memset(out, 0, size * 2);
    for(int i = 0; i < size; i++)
        snprintf(out + strlen(out), sizeof(out + strlen(out)), "%02X", in[i]);
}

uint64_t bytes_to_int64(uint8_t* bytes, uint8_t size, bool bigendian) {
    uint64_t ret = 0;
    for(int i = 0; i < size; i++)
        if(bigendian)
            ret |= bytes[i] << ((size - 1 - i) * 8);
        else
            ret |= bytes[i] << (i * 8);

    return ret;
}

void int64_to_bytes(uint64_t val, uint8_t* out, bool bigendian) {
    for(int i = 0; i < 8; i++) {
        if(bigendian)
            out[i] = (val >> ((7 - i) * 8)) & 0xff;
        else
            out[i] = (val >> (i * 8)) & 0xff;
    }
}

uint32_t bytes_to_int32(uint8_t* bytes, bool bigendian) {
    uint32_t ret = 0;
    for(int i = 0; i < 4; i++)
        if(bigendian)
            ret |= bytes[i] << ((3 - i) * 8);
        else
            ret |= bytes[i] << (i * 8);

    return ret;
}

void int32_to_bytes(uint32_t val, uint8_t* out, bool bigendian) {
    for(int i = 0; i < 4; i++) {
        if(bigendian)
            out[i] = (val >> ((3 - i) * 8)) & 0xff;
        else
            out[i] = (val >> (i * 8)) & 0xff;
    }
}

uint64_t bytes_to_int16(uint8_t* bytes, bool bigendian) {
    uint16_t ret = 0;
    for(int i = 0; i < 2; i++)
        if(bigendian)
            ret |= bytes[i] << ((1 - i) * 8);
        else
            ret |= bytes[i] << (i * 8);

    return ret;
}

void int16_to_bytes(uint16_t val, uint8_t* out, bool bigendian) {
    for(int i = 0; i < 2; i++) {
        if(bigendian)
            out[i] = (val >> ((1 - i) * 8)) & 0xff;
        else
            out[i] = (val >> (i * 8)) & 0xff;
    }
}


uint8_t nrf24_set_mac(uint8_t mac_addr, uint8_t* mac, uint8_t mlen) {
    uint8_t addr[5];
    for(int i = 0; i < mlen; i++) addr[i] = mac[mlen - i - 1];
    return nrf24_write_buf_reg(nrf24_HANDLE, mac_addr, addr, mlen);
}