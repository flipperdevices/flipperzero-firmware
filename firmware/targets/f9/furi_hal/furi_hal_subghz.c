#include "furi_hal_subghz.h"
#include "furi_hal_version.h"
#include "furi_hal_rtc.h"

#include <furi_hal_gpio.h>
#include <furi_hal_spi.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>
#include <furi_hal_delay.h>

#include <stm32wbxx_ll_dma.h>

#include <furi.h>
#include <cc112x.h>
#include <stdio.h>

#define TAG "FuriHalSubGhz"

static volatile SubGhzState furi_hal_subghz_state = SubGhzStateInit;
static volatile SubGhzRegulation furi_hal_subghz_regulation = SubGhzRegulationTxRx;
static volatile FuriHalSubGhzPreset furi_hal_subghz_preset = FuriHalSubGhzPresetIDLE;

static const uint16_t furi_hal_subghz_preset_ook_270khz_async_regs[][2] = {
    // // https://e2e.ti.com/support/wireless-connectivity/sub-1-ghz-group/sub-1-ghz/f/sub-1-ghz-forum/382066/cc1101---don-t-know-the-correct-registers-configuration

    // /* GPIO GD0 */
    // {CC112X_IOCFG0, 0x0D}, // GD0 as async serial data output/input

    // /* FIFO and internals */
    // {cc112x_FIFOTHR, 0x47}, // The only important bit is ADC_RETENTION, FIFO Tx=33 Rx=32

    // /* Packet engine */
    // {cc112x_PKTCTRL0, 0x32}, // Async, continious, no whitening

    // /* Frequency Synthesizer Control */
    // {cc112x_FSCTRL1, 0x06}, // IF = (26*10^6) / (2^10) * 0x06 = 152343.75Hz

    // // Modem Configuration
    // {cc112x_MDMCFG0, 0x00}, // Channel spacing is 25kHz
    // {cc112x_MDMCFG1, 0x00}, // Channel spacing is 25kHz
    // {cc112x_MDMCFG2, 0x30}, // Format ASK/OOK, No preamble/sync
    // {cc112x_MDMCFG3, 0x32}, // Data rate is 3.79372 kBaud
    // {cc112x_MDMCFG4, 0x67}, // Rx BW filter is 270.833333kHz

    // /* Main Radio Control State Machine */
    // {cc112x_MCSM0, 0x18}, // Autocalibrate on idle-to-rx/tx, PO_TIMEOUT is 64 cycles(149-155us)

    // /* Frequency Offset Compensation Configuration */
    // {cc112x_FOCCFG,
    //  0x18}, // no frequency offset compensation, POST_K same as PRE_K, PRE_K is 4K, GATE is off

    // /* Automatic Gain Control */
    // {cc112x_AGCCTRL0,
    //  0x40}, // 01 - Low hysteresis, small asymmetric dead zone, medium gain; 00 - 8 samples agc; 00 - Normal AGC, 00 - 4dB boundary
    // {cc112x_AGCCTRL1,
    //  0x00}, // 0; 0 - LNA 2 gain is decreased to minimum before decreasing LNA gain; 00 - Relative carrier sense threshold disabled; 0000 - RSSI to MAIN_TARGET
    // {cc112x_AGCCTRL2, 0x03}, // 00 - DVGA all; 000 - MAX LNA+LNA2; 011 - MAIN_TARGET 24 dB

    // /* Wake on radio and timeouts control */
    // {cc112x_WORCTRL, 0xFB}, // WOR_RES is 2^15 periods (0.91 - 0.94 s) 16.5 - 17.2 hours

    // /* Frontend configuration */
    // {cc112x_FREND0, 0x11}, // Adjusts current TX LO buffer + high is PATABLE[1]
    // {cc112x_FREND1, 0xB6}, //

    // /* End  */
    // {0, 0},
};

static const uint16_t furi_hal_subghz_preset_ook_650khz_async_regs[][2] = {
    //{CC112X_IOCFG3              , 0xB0, // GPIO3 IO Pin Configuration
    {CC112X_IOCFG2, CC112XIocfgHW}, // GPIO2 IO Pin Configuration
    {CC112X_IOCFG1, 0xB0}, // GPIO1 IO Pin Configuration
    {CC112X_IOCFG0, 0x09}, // GPIO0 IO Pin Configuration
    {CC112X_SYNC1, 0x00}, // Sync Word Configuration [15:8]
    {CC112X_SYNC0, 0b01111010}, // 7A Sync Word Configuration [7:0]
    {CC112X_SYNC_CFG0, 0b00001000}, //  8 Sync Word Length Configuration Reg. 0
    {CC112X_DEVIATION_M, 0x26}, // Frequency Deviation Configuration
    {CC112X_MODCFG_DEV_E, 0b00011101}, // 0x1D Modulation Format and Frequency Deviation Config.
    {CC112X_DCFILT_CFG, 0x13}, // Digital DC Removal Configuration
    {CC112X_PREAMBLE_CFG1, 0b00000000}, // 0 Preamble Length Configuration Reg. 1
    {CC112X_PREAMBLE_CFG0, 0b00010011}, // 13 Preamble Detection Configuration Reg. 0
    {CC112X_IQIC, 0x00}, // Digital Image Channel Compensation Configuration
    {CC112X_CHAN_BW,
     0b01000001}, //0x01}, //   Channel Filter Configuration - Configurazione storica
    {CC112X_MDMCFG1, 0b00000110}, // 6 General Modem Parameter Configuration Reg. 1
    {CC112X_MDMCFG0, 0b01000001}, // 41 General Modem Parameter Configuration Reg. 0
    {CC112X_SYMBOL_RATE2, 0xA9}, //0x5F },   //Symbol Rate Configuration Exponent and Mantissa [1..
    {CC112X_SYMBOL_RATE1, 0x99}, //0x21 },   //Symbol Rate Configuration Mantissa [15:8]
    {CC112X_SYMBOL_RATE0, 0x9A}, //0x2D },   //Symbol Rate Configuration Mantissa [7:0]
    {CC112X_AGC_REF, 0x30}, // AGC Reference Level Configuration
    {CC112X_AGC_CS_THR, 0xEC}, // Carrier Sense Threshold Configuration
    {CC112X_AGC_CFG3,
     0b11100000}, //0b10100000},  // C0 Automatic Gain Control Configuration Reg. 3.
    {CC112X_AGC_CFG2,
     0b11000000}, //0b00100000},  // 20 Automatic Gain Control Configuration Reg. 2
    {CC112X_AGC_CFG1,
     0b00010111}, // 0b00001010},  // A Automatic Gain Control Configuration Reg. 1
    {CC112X_AGC_CFG0,
     0b00111111}, //0b10011111},  // 9F Automatic Gain Control Configuration Reg. 0 (1..0 - количеств выборок для 1\0)
    {CC112X_FS_CFG, 0x14}, // Frequency Synthesizer Configuration
    {CC112X_PKT_CFG2, 0b00000111}, // 7 Packet Configuration Reg. 2
    {CC112X_PKT_CFG1, 0b00000000}, // Packet Configuration Reg. 1
    {CC112X_PKT_CFG0, 0b00000000}, // Packet Configuration Reg. 0
    {CC112X_PA_CFG2, 0x3C}, // Power Amplifier Configuration Reg. 2
    {CC112X_PA_CFG0, 0x7E}, // Power Amplifier Configuration Reg. 0
    {CC112X_PKT_LEN, 0x09}, // Packet Length Configuration. 7:0 PACKET_LENGTH
    {CC112X_FREQOFF_CFG, 0b00100011}, //0x22},  // Frequency Offset Correction Configuration
    {CC112X_TOC_CFG, 0x0A}, // Timing Offset Correction Configuration
    {CC112X_FREQ2, 0x6C}, //Frequency Configuration [23:16]
    {CC112X_FREQ1, 0x7A}, //Frequency Configuration [15:8]
    {CC112X_FREQ0, 0xE1}, //Frequency Configuration [7:0]
    {CC112X_FS_DIG1, 0x00}, // Frequency Synthesizer Digital Reg. 1
    {CC112X_FS_DIG0, 0x5F}, // Frequency Synthesizer Digital Reg. 0
    {CC112X_FS_CAL1, 0x40}, // Frequency Synthesizer Calibration Reg. 1
    {CC112X_FS_CAL0, 0x0E}, // Frequency Synthesizer Calibration Reg. 0
    {CC112X_FS_DIVTWO, 0x03}, // Frequency Synthesizer Divide by 2
    {CC112X_FS_DSM0, 0x33}, // FS Digital Synthesizer Module Configuration Reg. 0
    {CC112X_FS_DVC0, 0x17}, // Frequency Synthesizer Divider Chain Configuration ..
    {CC112X_FS_PFD, 0x50}, // Frequency Synthesizer Phase Frequency Detector Con..
    {CC112X_FS_PRE, 0x6E}, // Frequency Synthesizer Prescaler Configuration
    {CC112X_FS_REG_DIV_CML, 0x14}, // Frequency Synthesizer Divider Regulator Configurat..
    {CC112X_FS_SPARE, 0xAC}, // Frequency Synthesizer Spare
    {CC112X_FS_VCO0, 0xB4}, // FS Voltage Controlled Oscillator Configuration Reg..
    {CC112X_XOSC5, 0x0E}, // Crystal Oscillator Configuration Reg. 5
    {CC112X_XOSC1, 0x03}, // Crystal Oscillator Configuration Reg. 1
    {CC112X_SERIAL_STATUS, 0x08}, // Serial Status

    // //{CC112X_IOCFG3,             0xB0 },   //GPIO3 IO Pin Configuration
    // {CC112X_IOCFG2,             0xB0 },   //GPIO2 IO Pin Configuration
    // {CC112X_IOCFG1,             0xB0 },   //GPIO1 IO Pin Configuration
    // {CC112X_IOCFG0,             CC112XIocfgSerialDataOutput },   //GPIO0 IO Pin Configuration
    // {CC112X_SYNC_CFG1,          0x0B },   //Sync Word Detection Configuration Reg. 1
    // {CC112X_SYNC_CFG0,          0x03 },   //Sync Word Length Configuration Reg. 0
    // {CC112X_MODCFG_DEV_E,       0x1B },   //Modulation Format and Frequency Deviation Configur..
    // {CC112X_DCFILT_CFG,         0x1C },   //Digital DC Removal Configuration
    // {CC112X_PREAMBLE_CFG1,      0x00 },   //Preamble Length Configuration Reg. 1
    // {CC112X_IQIC,               0x46 },   //Digital Image Channel Compensation Configuration
    // {CC112X_CHAN_BW,            0x01 },   //Channel Filter Configuration
    // {CC112X_MDMCFG1,            0x06 },   //General Modem Parameter Configuration Reg. 1
    // {CC112X_MDMCFG0,            0x65 },   //General Modem Parameter Configuration Reg. 0
    // {CC112X_SYMBOL_RATE2,       0x5F },   //Symbol Rate Configuration Exponent and Mantissa [1..
    // {CC112X_SYMBOL_RATE1,       0x21 },   //Symbol Rate Configuration Mantissa [15:8]
    // {CC112X_SYMBOL_RATE0,       0x2D },   //Symbol Rate Configuration Mantissa [7:0]
    // {CC112X_AGC_REF,            0x20 },   //AGC Reference Level Configuration
    // {CC112X_AGC_CS_THR,         0x19 },   //Carrier Sense Threshold Configuration
    // {CC112X_AGC_CFG1,           0xA9 },   //Automatic Gain Control Configuration Reg. 1
    // {CC112X_AGC_CFG0,           0xCF },   //Automatic Gain Control Configuration Reg. 0
    // {CC112X_FIFO_CFG,           0x00 },   //FIFO Configuration
    // {CC112X_FS_CFG,             0x14 },   //Frequency Synthesizer Configuration
    // {CC112X_PKT_CFG2,           0x07 },   //Packet Configuration Reg. 2
    // {CC112X_PKT_CFG0,           0x20 },   //Packet Configuration Reg. 0
    // {CC112X_PA_CFG2,            0x3C },   //Power Amplifier Configuration Reg. 2
    // {CC112X_PA_CFG0,            0x7E },   //Power Amplifier Configuration Reg. 0
    // {CC112X_PKT_LEN,            0xFF },   //Packet Length Configuration
    // {CC112X_IF_MIX_CFG,         0x00 },   //IF Mix Configuration
    // {CC112X_FREQOFF_CFG,        0x22 },   //Frequency Offset Correction Configuration
    // {CC112X_FREQ2,              0x6C },   //Frequency Configuration [23:16]
    // {CC112X_FREQ1,              0x7A },   //Frequency Configuration [15:8]
    // {CC112X_FREQ0,              0xE1 },   //Frequency Configuration [7:0]
    // {CC112X_FS_DIG1,            0x00 },   //Frequency Synthesizer Digital Reg. 1
    // {CC112X_FS_DIG0,            0x5F },   //Frequency Synthesizer Digital Reg. 0
    // {CC112X_FS_CAL1,            0x40 },   //Frequency Synthesizer Calibration Reg. 1
    // {CC112X_FS_CAL0,            0x0E },   //Frequency Synthesizer Calibration Reg. 0
    // {CC112X_FS_DIVTWO,          0x03 },   //Frequency Synthesizer Divide by 2
    // {CC112X_FS_DSM0,            0x33 },   //FS Digital Synthesizer Module Configuration Reg. 0
    // {CC112X_FS_DVC0,            0x17 },   //Frequency Synthesizer Divider Chain Configuration ..
    // {CC112X_FS_PFD,             0x50 },   //Frequency Synthesizer Phase Frequency Detector Con..
    // {CC112X_FS_PRE,             0x6E },   //Frequency Synthesizer Prescaler Configuration
    // {CC112X_FS_REG_DIV_CML,     0x14 },   //Frequency Synthesizer Divider Regulator Configurat..
    // {CC112X_FS_SPARE,           0xAC },   //Frequency Synthesizer Spare
    // {CC112X_FS_VCO0,            0xB4 },   //FS Voltage Controlled Oscillator Configuration Reg..
    // {CC112X_XOSC5,              0x0E },   //Crystal Oscillator Configuration Reg. 5
    // {CC112X_XOSC1,              0x03 },   //Crystal Oscillator Configuration Reg. 1
    // {CC112X_SERIAL_STATUS       , 0x08},  // Serial Status
    /* End  */
    {0, 0},
};
static const uint16_t furi_hal_subghz_preset_2fsk_dev2_38khz_async_regs[][2] = {

};
static const uint16_t furi_hal_subghz_preset_2fsk_dev47_6khz_async_regs[][2] = {

};
static const uint16_t furi_hal_subghz_preset_msk_99_97kb_async_regs[][2] = {

};
static const uint16_t furi_hal_subghz_preset_gfsk_9_99kb_async_regs[][2] = {

};

void furi_hal_subghz_init() {
    furi_assert(furi_hal_subghz_state == SubGhzStateInit);
    furi_hal_subghz_state = SubGhzStateIdle;
    furi_hal_subghz_preset = FuriHalSubGhzPresetIDLE;

    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);

#ifdef FURI_HAL_SUBGHZ_TX_GPIO
    furi_hal_gpio_init(&FURI_HAL_SUBGHZ_TX_GPIO, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
#endif

    // Reset PIN
    furi_hal_gpio_init(&gpio_rf_sw_0, GpioModeOutputPushPull, GpioPullDown, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_rf_sw_0, false); //cc1120 reset pin down
    furi_hal_delay_us(100);
    furi_hal_gpio_write(&gpio_rf_sw_0, true); //cc1120 reset pin up

    // Reset
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    cc112x_reset(&furi_hal_spi_bus_handle_subghz);

    cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG0, CC112XIocfgHighz);

    // Prepare GD0 for power on self test
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);

    // GD0 low
    cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG0, CC112XIocfgHW);
    while(furi_hal_gpio_read(&gpio_cc1101_g0) != false)
        ;

    // GD0 high
    cc112x_write_reg(
        &furi_hal_spi_bus_handle_subghz, CC112X_IOCFG0, CC112XIocfgHW | CC112X_IOCFG_INV);
    while(furi_hal_gpio_read(&gpio_cc1101_g0) != true)
        ;

    // Reset GD0 to floating state
    cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG0, CC112XIocfgHighz);
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    // RF switches
    cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG2, CC112XIocfgHW);
    cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG3, CC112XIocfgHW);

    // // Go to sleep
    cc112x_shutdown(&furi_hal_spi_bus_handle_subghz);

    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);

    furi_hal_subghz_dump_state();
    FURI_LOG_I(TAG, "Init OK");
}

void furi_hal_subghz_sleep() {
    furi_assert(furi_hal_subghz_state == SubGhzStateIdle);
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);

    cc112x_switch_to_idle(&furi_hal_spi_bus_handle_subghz);

    cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG0, CC112XIocfgHighz);
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    cc112x_shutdown(&furi_hal_spi_bus_handle_subghz);

    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);

    furi_hal_subghz_preset = FuriHalSubGhzPresetIDLE;
}

void furi_hal_subghz_dump_state() {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    printf(
        "[furi_hal_subghz] cc112x chip (CC1121-0x40, CC1120-0x48, CC1125-0x58, CC1175-0x5A):  0x%X, version 0x%X\r\n",
        cc112x_get_partnumber(&furi_hal_spi_bus_handle_subghz),
        cc112x_get_version(&furi_hal_spi_bus_handle_subghz));
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_load_preset(FuriHalSubGhzPreset preset) {
    if(preset == FuriHalSubGhzPresetOok650Async) {
        furi_hal_subghz_load_registers(furi_hal_subghz_preset_ook_650khz_async_regs);
        //furi_hal_subghz_load_patable(furi_hal_subghz_preset_ook_async_patable);
    } else if(preset == FuriHalSubGhzPresetOok270Async) {
        furi_hal_subghz_load_registers(furi_hal_subghz_preset_ook_270khz_async_regs);
        //furi_hal_subghz_load_patable(furi_hal_subghz_preset_ook_async_patable);
    } else if(preset == FuriHalSubGhzPreset2FSKDev238Async) {
        furi_hal_subghz_load_registers(furi_hal_subghz_preset_2fsk_dev2_38khz_async_regs);
        //furi_hal_subghz_load_patable(furi_hal_subghz_preset_2fsk_async_patable);
    } else if(preset == FuriHalSubGhzPreset2FSKDev476Async) {
        furi_hal_subghz_load_registers(furi_hal_subghz_preset_2fsk_dev47_6khz_async_regs);
        //furi_hal_subghz_load_patable(furi_hal_subghz_preset_2fsk_async_patable);
    } else if(preset == FuriHalSubGhzPresetMSK99_97KbAsync) {
        furi_hal_subghz_load_registers(furi_hal_subghz_preset_msk_99_97kb_async_regs);
        // furi_hal_subghz_load_patable(furi_hal_subghz_preset_msk_async_patable);
    } else if(preset == FuriHalSubGhzPresetGFSK9_99KbAsync) {
        furi_hal_subghz_load_registers(furi_hal_subghz_preset_gfsk_9_99kb_async_regs);
        //furi_hal_subghz_load_patable(furi_hal_subghz_preset_gfsk_async_patable);
    } else {
        furi_crash("SugGhz: Missing config.");
    }
    furi_hal_subghz_preset = preset;
}

void furi_hal_subghz_load_registers(const uint16_t data[][2]) {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    cc112x_reset(&furi_hal_spi_bus_handle_subghz);
    uint32_t i = 0;
    while(data[i][0]) {
        cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, data[i][0], (uint8_t)data[i][1]);
        i++;
    }
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_load_patable(const uint8_t data[8]) {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    cc112x_set_pa_table(&furi_hal_spi_bus_handle_subghz, data);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_write_packet(const uint8_t* data, uint8_t size) {
    // furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // cc112x_flush_tx(&furi_hal_spi_bus_handle_subghz);
    // cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_FIFO, size);
    // cc112x_write_fifo(&furi_hal_spi_bus_handle_subghz, data, size);
    // furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_flush_rx() {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    cc112x_flush_rx(&furi_hal_spi_bus_handle_subghz);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_flush_tx() {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    cc112x_flush_tx(&furi_hal_spi_bus_handle_subghz);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

bool furi_hal_subghz_rx_pipe_not_empty() {
    // CC112XRxBytes status[1];
    // furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // cc112x_read_reg(
    //     &furi_hal_spi_bus_handle_subghz, (CC112X_STATUS_RXBYTES) | CC112X_BURST, (uint8_t*)status);
    // furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
    // // TODO: you can add a buffer overflow flag if needed
    // if(status->NUM_RXBYTES > 0) {
    //     return true;
    // } else {
    //     return false;
    // }
    return 0;
}

bool furi_hal_subghz_is_rx_data_crc_valid() {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    uint8_t data[1];
    cc112x_read_reg(&furi_hal_spi_bus_handle_subghz, CC112X_LQI_VAL | CC112X_BURST, data);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
    if(((data[0] >> 7) & 0x01)) {
        return true;
    } else {
        return false;
    }
}

void furi_hal_subghz_read_packet(uint8_t* data, uint8_t* size) {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    cc112x_read_fifo(&furi_hal_spi_bus_handle_subghz, data, size);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_shutdown() {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // Reset and shutdown
    cc112x_shutdown(&furi_hal_spi_bus_handle_subghz);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_reset() {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    cc112x_switch_to_idle(&furi_hal_spi_bus_handle_subghz);
    cc112x_reset(&furi_hal_spi_bus_handle_subghz);
    cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG0, CC112XIocfgHighz);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_idle() {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    cc112x_switch_to_idle(&furi_hal_spi_bus_handle_subghz);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_rx() {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    cc112x_switch_to_rx(&furi_hal_spi_bus_handle_subghz);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

bool furi_hal_subghz_tx() {
    if(furi_hal_subghz_regulation != SubGhzRegulationTxRx) return false;
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    cc112x_switch_to_tx(&furi_hal_spi_bus_handle_subghz);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
    return true;
}

float furi_hal_subghz_get_rssi() {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    int8_t rssi_dec = cc112x_get_rssi(&furi_hal_spi_bus_handle_subghz);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);

    float rssi = rssi_dec;

#define RSSI_OFFSET 99 /* constant defined in datasheet  ??????????????*/

    if(rssi >= 128) {
        rssi = rssi - 256;
    }
    rssi = rssi - RSSI_OFFSET;

    return rssi;
}

uint8_t furi_hal_subghz_get_lqi() {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    uint8_t data[1];
    cc112x_read_reg(&furi_hal_spi_bus_handle_subghz, CC112X_LQI_VAL | CC112X_BURST, data);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
    return data[0] & 0x7F;
}

bool furi_hal_subghz_is_frequency_valid(uint32_t value) {
    if(!(value >= 136700000 && value <= 160000000) &&
       !(value >= 164000000 && value <= 192000000) &&
       !(value >= 205000000 && value <= 240000000) &&
       !(value >= 273300000 && value <= 320000000) &&
       !(value >= 410000000 && value <= 480000000) &&
       !(value >= 820000000 && value <= 960000000)) {
        return false;
    }

    return true;
}

uint32_t furi_hal_subghz_set_frequency_and_path(uint32_t value) {
    value = furi_hal_subghz_set_frequency(value);
    if(value >= 136700000 && value <= 160000000) {
        furi_hal_subghz_set_path(FuriHalSubGhzPath315);
    } else if(value >= 164000000 && value <= 192000000) {
        furi_hal_subghz_set_path(FuriHalSubGhzPath315);
    } else if(value >= 205000000 && value <= 240000000) {
        furi_hal_subghz_set_path(FuriHalSubGhzPath315);
    } else if(value >= 273300000 && value <= 320000000) {
        furi_hal_subghz_set_path(FuriHalSubGhzPath315);
    } else if(value >= 410000000 && value <= 480000000) {
        furi_hal_subghz_set_path(FuriHalSubGhzPath433);
    } else if(value >= 820000000 && value <= 960000000) {
        furi_hal_subghz_set_path(FuriHalSubGhzPath868);
    } else {
        furi_crash("SugGhz: Incorrect frequency during set.");
    }
    return value;
}

bool furi_hal_subghz_is_tx_allowed(uint32_t value) {
    //checking regional settings
    bool is_allowed = false;
    switch(furi_hal_version_get_hw_region()) {
    case FuriHalVersionRegionEuRu:
        //433,05..434,79; 868,15..868,55
        if(!(value >= 433050000 && value <= 434790000) &&
           !(value >= 868150000 && value <= 868550000)) {
        } else {
            is_allowed = true;
        }
        break;
    case FuriHalVersionRegionUsCaAu:
        //304,10..321,95; 433,05..434,79; 915,00..928,00
        //ToDo partial range supported
        if(!(value >= 304100000 && value <= 321950000) &&
           !(value >= 433050000 && value <= 434790000) &&
           !(value >= 915000000 && value <= 928000000)) {
        } else {
            if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
                if((value >= 304100000 && value <= 321950000) &&
                   ((furi_hal_subghz_preset == FuriHalSubGhzPresetOok270Async) ||
                    (furi_hal_subghz_preset == FuriHalSubGhzPresetOok650Async))) {
                    //furi_hal_subghz_load_patable(furi_hal_subghz_preset_ook_async_patable_au);
                }
            }
            is_allowed = true;
        }
        break;
    case FuriHalVersionRegionJp:
        //312,00..315,25; 920,50..923,50
        if(!(value >= 312000000 && value <= 315250000) &&
           !(value >= 920500000 && value <= 923500000)) {
        } else {
            is_allowed = true;
        }
        break;

    default:
        is_allowed = true;
        break;
    }
    return is_allowed;
}

uint32_t furi_hal_subghz_set_frequency(uint32_t value) {
    if(furi_hal_subghz_is_tx_allowed(value)) {
        furi_hal_subghz_regulation = SubGhzRegulationTxRx;
    } else {
        furi_hal_subghz_regulation = SubGhzRegulationOnlyRx;
    }

    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    uint32_t real_frequency = cc112x_set_frequency(&furi_hal_spi_bus_handle_subghz, value);
    cc112x_calibrate(&furi_hal_spi_bus_handle_subghz);

    while(true) {
        CC112XStatus status = cc112x_get_status(&furi_hal_spi_bus_handle_subghz);
        if(status.STATE == CC112XStateIDLE) break;
    }

    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
    return real_frequency;
}

void furi_hal_subghz_set_path(FuriHalSubGhzPath path) {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    if(path == FuriHalSubGhzPath433) {
        cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG3, CC112XIocfgHW);
        cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG2, CC112XIocfgHW);
    } else if(path == FuriHalSubGhzPath315) {
        cc112x_write_reg(
            &furi_hal_spi_bus_handle_subghz, CC112X_IOCFG3, CC112XIocfgHW | CC112X_IOCFG_INV);
        cc112x_write_reg(
            &furi_hal_spi_bus_handle_subghz, CC112X_IOCFG2, CC112XIocfgHW | CC112X_IOCFG_INV);
    } else if(path == FuriHalSubGhzPath868) {
        cc112x_write_reg(
            &furi_hal_spi_bus_handle_subghz, CC112X_IOCFG3, CC112XIocfgHW | CC112X_IOCFG_INV);
        cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG2, CC112XIocfgHW);
    } else if(path == FuriHalSubGhzPathIsolate) {
        cc112x_write_reg(&furi_hal_spi_bus_handle_subghz, CC112X_IOCFG3, CC112XIocfgHW);
        cc112x_write_reg(
            &furi_hal_spi_bus_handle_subghz, CC112X_IOCFG2, CC112XIocfgHW | CC112X_IOCFG_INV);
    } else {
        furi_crash("SubGhz: Incorrect path during set.");
    }
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

volatile uint32_t furi_hal_subghz_capture_delta_duration = 0;
volatile FuriHalSubGhzCaptureCallback furi_hal_subghz_capture_callback = NULL;
volatile void* furi_hal_subghz_capture_callback_context = NULL;

static void furi_hal_subghz_capture_ISR() {
    // Channel 1
    if(LL_TIM_IsActiveFlag_CC1(TIM2)) {
        LL_TIM_ClearFlag_CC1(TIM2);
        furi_hal_subghz_capture_delta_duration = LL_TIM_IC_GetCaptureCH1(TIM2);
        if(furi_hal_subghz_capture_callback) {
            furi_hal_subghz_capture_callback(
                true,
                furi_hal_subghz_capture_delta_duration,
                (void*)furi_hal_subghz_capture_callback_context);
        }
    }
    // Channel 2
    if(LL_TIM_IsActiveFlag_CC2(TIM2)) {
        LL_TIM_ClearFlag_CC2(TIM2);
        if(furi_hal_subghz_capture_callback) {
            furi_hal_subghz_capture_callback(
                false,
                LL_TIM_IC_GetCaptureCH2(TIM2) - furi_hal_subghz_capture_delta_duration,
                (void*)furi_hal_subghz_capture_callback_context);
        }
    }
}

void furi_hal_subghz_start_async_rx(FuriHalSubGhzCaptureCallback callback, void* context) {
    furi_assert(furi_hal_subghz_state == SubGhzStateIdle);
    furi_hal_subghz_state = SubGhzStateAsyncRx;

    furi_hal_subghz_capture_callback = callback;
    furi_hal_subghz_capture_callback_context = context;

    furi_hal_gpio_init_ex(
        &gpio_cc1101_g0, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn1TIM2);

    // Timer: base
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 64 - 1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 0x7FFFFFFE;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV4;
    LL_TIM_Init(TIM2, &TIM_InitStruct);

    // Timer: advanced
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_DisableARRPreload(TIM2);
    LL_TIM_SetTriggerInput(TIM2, LL_TIM_TS_TI2FP2);
    LL_TIM_SetSlaveMode(TIM2, LL_TIM_SLAVEMODE_RESET);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
    LL_TIM_EnableMasterSlaveMode(TIM2);
    LL_TIM_DisableDMAReq_TRIG(TIM2);
    LL_TIM_DisableIT_TRIG(TIM2);

    // Timer: channel 1 indirect
    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_INDIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_FALLING);
    LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);

    // Timer: channel 2 direct
    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV32_N8);

    // ISR setup
    furi_hal_interrupt_set_isr(FuriHalInterruptIdTIM2, furi_hal_subghz_capture_ISR, NULL);

    // Interrupts and channels
    LL_TIM_EnableIT_CC1(TIM2);
    LL_TIM_EnableIT_CC2(TIM2);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);

    // Start timer
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);

    // Switch to RX
    furi_hal_subghz_rx();
}

void furi_hal_subghz_stop_async_rx() {
    furi_assert(furi_hal_subghz_state == SubGhzStateAsyncRx);
    furi_hal_subghz_state = SubGhzStateIdle;

    // Shutdown radio
    furi_hal_subghz_idle();

    FURI_CRITICAL_ENTER();
    LL_TIM_DeInit(TIM2);
    FURI_CRITICAL_EXIT();
    furi_hal_interrupt_set_isr(FuriHalInterruptIdTIM2, NULL, NULL);

    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

#define API_HAL_SUBGHZ_ASYNC_TX_BUFFER_FULL (256)
#define API_HAL_SUBGHZ_ASYNC_TX_BUFFER_HALF (API_HAL_SUBGHZ_ASYNC_TX_BUFFER_FULL / 2)
#define API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME 333

typedef struct {
    uint32_t* buffer;
    bool flip_flop;
    FuriHalSubGhzAsyncTxCallback callback;
    void* callback_context;
    uint64_t duty_high;
    uint64_t duty_low;
} FuriHalSubGhzAsyncTx;

static FuriHalSubGhzAsyncTx furi_hal_subghz_async_tx = {0};

static void furi_hal_subghz_async_tx_refill(uint32_t* buffer, size_t samples) {
    while(samples > 0) {
        bool is_odd = samples % 2;
        LevelDuration ld =
            furi_hal_subghz_async_tx.callback(furi_hal_subghz_async_tx.callback_context);

        if(level_duration_is_wait(ld)) {
            return;
        } else if(level_duration_is_reset(ld)) {
            // One more even sample required to end at low level
            if(is_odd) {
                *buffer = API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME;
                buffer++;
                samples--;
                furi_hal_subghz_async_tx.duty_low += API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME;
            }
            break;
        } else {
            // Inject guard time if level is incorrect
            bool level = level_duration_get_level(ld);
            if(is_odd == level) {
                *buffer = API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME;
                buffer++;
                samples--;
                if(!level) {
                    furi_hal_subghz_async_tx.duty_high += API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME;
                } else {
                    furi_hal_subghz_async_tx.duty_low += API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME;
                }
                // This code must be invoked only once: when encoder starts with low level.
                // Otherwise whole thing will crash.
                furi_check(samples > 0);
            }

            uint32_t duration = level_duration_get_duration(ld);
            furi_assert(duration > 0);
            *buffer = duration;
            buffer++;
            samples--;

            if(level) {
                furi_hal_subghz_async_tx.duty_high += duration;
            } else {
                furi_hal_subghz_async_tx.duty_low += duration;
            }
        }
    }

    memset(buffer, 0, samples * sizeof(uint32_t));
}

static void furi_hal_subghz_async_tx_dma_isr() {
    furi_assert(furi_hal_subghz_state == SubGhzStateAsyncTx);
    if(LL_DMA_IsActiveFlag_HT1(DMA1)) {
        LL_DMA_ClearFlag_HT1(DMA1);
        furi_hal_subghz_async_tx_refill(
            furi_hal_subghz_async_tx.buffer, API_HAL_SUBGHZ_ASYNC_TX_BUFFER_HALF);
    }
    if(LL_DMA_IsActiveFlag_TC1(DMA1)) {
        LL_DMA_ClearFlag_TC1(DMA1);
        furi_hal_subghz_async_tx_refill(
            furi_hal_subghz_async_tx.buffer + API_HAL_SUBGHZ_ASYNC_TX_BUFFER_HALF,
            API_HAL_SUBGHZ_ASYNC_TX_BUFFER_HALF);
    }
}

static void furi_hal_subghz_async_tx_timer_isr() {
    if(LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
        LL_TIM_ClearFlag_UPDATE(TIM2);
        if(LL_TIM_GetAutoReload(TIM2) == 0) {
            if(furi_hal_subghz_state == SubGhzStateAsyncTx) {
                furi_hal_subghz_state = SubGhzStateAsyncTxLast;
                //forcibly pulls the pin to the ground so that there is no carrier
                furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullDown, GpioSpeedLow);
            } else {
                furi_hal_subghz_state = SubGhzStateAsyncTxEnd;
                LL_TIM_DisableCounter(TIM2);
            }
        }
    }
}

bool furi_hal_subghz_start_async_tx(FuriHalSubGhzAsyncTxCallback callback, void* context) {
    furi_assert(furi_hal_subghz_state == SubGhzStateIdle);
    furi_assert(callback);

    //If transmission is prohibited by regional settings
    if(furi_hal_subghz_regulation != SubGhzRegulationTxRx) return false;

    furi_hal_subghz_async_tx.callback = callback;
    furi_hal_subghz_async_tx.callback_context = context;

    furi_hal_subghz_state = SubGhzStateAsyncTx;

    furi_hal_subghz_async_tx.duty_low = 0;
    furi_hal_subghz_async_tx.duty_high = 0;

    furi_hal_subghz_async_tx.buffer =
        malloc(API_HAL_SUBGHZ_ASYNC_TX_BUFFER_FULL * sizeof(uint32_t));
    furi_hal_subghz_async_tx_refill(
        furi_hal_subghz_async_tx.buffer, API_HAL_SUBGHZ_ASYNC_TX_BUFFER_FULL);

    // Connect cc112x_GD0 to TIM2 as output
    furi_hal_gpio_init_ex(
        &gpio_cc1101_g0, GpioModeAltFunctionPushPull, GpioPullDown, GpioSpeedLow, GpioAltFn1TIM2);

    // Configure DMA
    LL_DMA_InitTypeDef dma_config = {0};
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (TIM2->ARR);
    dma_config.MemoryOrM2MDstAddress = (uint32_t)furi_hal_subghz_async_tx.buffer;
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = API_HAL_SUBGHZ_ASYNC_TX_BUFFER_FULL;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    dma_config.Priority = LL_DMA_MODE_NORMAL;
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &dma_config);
    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch1, furi_hal_subghz_async_tx_dma_isr, NULL);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

    // Configure TIM2
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 64 - 1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 1000;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM2, &TIM_InitStruct);
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_EnableARRPreload(TIM2);

    // Configure TIM2 CH2
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_TOGGLE;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH2);
    LL_TIM_DisableMasterSlaveMode(TIM2);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdTIM2, furi_hal_subghz_async_tx_timer_isr, NULL);

    LL_TIM_EnableIT_UPDATE(TIM2);
    LL_TIM_EnableDMAReq_UPDATE(TIM2);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);

    // Start counter
    LL_TIM_GenerateEvent_UPDATE(TIM2);
#ifdef FURI_HAL_SUBGHZ_TX_GPIO
    furi_hal_gpio_write(&FURI_HAL_SUBGHZ_TX_GPIO, true);
#endif
    furi_hal_subghz_tx();

    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);
    return true;
}

bool furi_hal_subghz_is_async_tx_complete() {
    return furi_hal_subghz_state == SubGhzStateAsyncTxEnd;
}

void furi_hal_subghz_stop_async_tx() {
    furi_assert(
        furi_hal_subghz_state == SubGhzStateAsyncTx ||
        furi_hal_subghz_state == SubGhzStateAsyncTxLast ||
        furi_hal_subghz_state == SubGhzStateAsyncTxEnd);

    // Shutdown radio
    furi_hal_subghz_idle();
#ifdef FURI_HAL_SUBGHZ_TX_GPIO
    furi_hal_gpio_write(&FURI_HAL_SUBGHZ_TX_GPIO, false);
#endif

    // Deinitialize Timer
    FURI_CRITICAL_ENTER();
    LL_TIM_DeInit(TIM2);
    furi_hal_interrupt_set_isr(FuriHalInterruptIdTIM2, NULL, NULL);

    // Deinitialize DMA
    LL_DMA_DeInit(DMA1, LL_DMA_CHANNEL_1);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch1, NULL, NULL);

    // Deinitialize GPIO
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    FURI_CRITICAL_EXIT();

    free(furi_hal_subghz_async_tx.buffer);

    float duty_cycle =
        100.0f * (float)furi_hal_subghz_async_tx.duty_high /
        ((float)furi_hal_subghz_async_tx.duty_low + (float)furi_hal_subghz_async_tx.duty_high);
    FURI_LOG_D(
        TAG,
        "Async TX Radio stats: on %0.0fus, off %0.0fus, DutyCycle: %0.0f%%",
        (double)furi_hal_subghz_async_tx.duty_high,
        (double)furi_hal_subghz_async_tx.duty_low,
        (double)duty_cycle);

    furi_hal_subghz_state = SubGhzStateIdle;
}
