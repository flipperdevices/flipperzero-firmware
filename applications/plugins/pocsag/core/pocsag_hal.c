#include <furi.h>
#include <furi_hal.h>
#include <cc1101.h>
#include <furi_hal_subghz_configs.h>


static const uint8_t furi_hal_subghz_preset_2fsk_dev9_5khz_async_regs[][2] = {
        /* GPIO GD0 */
        {CC1101_IOCFG0, 0x0D}, // GD0 as async serial data output/input

        /* Frequency Synthesizer Control */
        {CC1101_FSCTRL1, 0x06}, // IF = (26*10^6) / (2^10) * 0x06 = 152343.75Hz

        /* Packet engine */
        {CC1101_PKTCTRL0, 0x32}, // Async, continious, no whitening
        {CC1101_PKTCTRL1, 0x04},

        // // Modem Configuration
        {CC1101_MDMCFG0, 0x00},
        {CC1101_MDMCFG1, 0x02},
        {CC1101_MDMCFG2, 0x04}, // Format 2-FSK/FM, No preamble/sync, Disable (current optimized)
        {CC1101_MDMCFG3, 0x83}, // Data rate is 4.79794 kBaud
        {CC1101_MDMCFG4, 0x67}, //Rx BW filter is 270.833333 kHz
        {CC1101_DEVIATN, 0x24}, //Deviation 9.5 kHz

        /* Main Radio Control State Machine */
        {CC1101_MCSM0, 0x18}, // Autocalibrate on idle-to-rx/tx, PO_TIMEOUT is 64 cycles(149-155us)

        /* Frequency Offset Compensation Configuration */
        {CC1101_FOCCFG,
                0x16}, // no frequency offset compensation, POST_K same as PRE_K, PRE_K is 4K, GATE is off

        /* Automatic Gain Control */
        {CC1101_AGCCTRL0,
                0x91}, //10 - Medium hysteresis, medium asymmetric dead zone, medium gain ; 01 - 16 samples agc; 00 - Normal AGC, 01 - 8dB boundary
        {CC1101_AGCCTRL1,
                0x00}, // 0; 0 - LNA 2 gain is decreased to minimum before decreasing LNA gain; 00 - Relative carrier sense threshold disabled; 0000 - RSSI to MAIN_TARGET
        {CC1101_AGCCTRL2, 0x07}, // 00 - DVGA all; 000 - MAX LNA+LNA2; 111 - MAIN_TARGET 42 dB

        /* Wake on radio and timeouts control */
        {CC1101_WORCTRL, 0xFB}, // WOR_RES is 2^15 periods (0.91 - 0.94 s) 16.5 - 17.2 hours

        /* Frontend configuration */
        {CC1101_FREND0, 0x10}, // Adjusts current TX LO buffer
        {CC1101_FREND1, 0x56},

        /* End  */
        {0, 0},
};
//
//
//static const uint8_t furi_hal_subghz_preset_ook_650khz_async_regs[][2] = {
//        // https://e2e.ti.com/support/wireless-connectivity/sub-1-ghz-group/sub-1-ghz/f/sub-1-ghz-forum/382066/cc1101---don-t-know-the-correct-registers-configuration
//
//        /* GPIO GD0 */
//        {CC1101_IOCFG0, 0x0D}, // GD0 as async serial data output/input
//
//        /* FIFO and internals */
//        {CC1101_FIFOTHR, 0x07}, // The only important bit is ADC_RETENTION
//
//        /* Packet engine */
//        {CC1101_PKTCTRL0, 0x32}, // Async, continious, no whitening
//
//        /* Frequency Synthesizer Control */
//        {CC1101_FSCTRL1, 0x06}, // IF = (26*10^6) / (2^10) * 0x06 = 152343.75Hz
//
//        // Modem Configuration
//        {CC1101_MDMCFG0, 0x00}, // Channel spacing is 25kHz
//        {CC1101_MDMCFG1, 0x00}, // Channel spacing is 25kHz
//        {CC1101_MDMCFG2, 0x30}, // Format ASK/OOK, No preamble/sync
//        {CC1101_MDMCFG3, 0x32}, // Data rate is 3.79372 kBaud
//        {CC1101_MDMCFG4, 0x17}, // Rx BW filter is 650.000kHz
//
//        /* Main Radio Control State Machine */
//        {CC1101_MCSM0, 0x18}, // Autocalibrate on idle-to-rx/tx, PO_TIMEOUT is 64 cycles(149-155us)
//
//        /* Frequency Offset Compensation Configuration */
//        {CC1101_FOCCFG,
//                0x18}, // no frequency offset compensation, POST_K same as PRE_K, PRE_K is 4K, GATE is off
//
//        /* Automatic Gain Control */
//        // {CC1101_AGCTRL0,0x40}, // 01 - Low hysteresis, small asymmetric dead zone, medium gain; 00 - 8 samples agc; 00 - Normal AGC, 00 - 4dB boundary
//        // {CC1101_AGCTRL1,0x00}, // 0; 0 - LNA 2 gain is decreased to minimum before decreasing LNA gain; 00 - Relative carrier sense threshold disabled; 0000 - RSSI to MAIN_TARGET
//        // {CC1101_AGCCTRL2, 0x03}, // 00 - DVGA all; 000 - MAX LNA+LNA2; 011 - MAIN_TARGET 24 dB
//        //MAGN_TARGET for RX filter BW =< 100 kHz is 0x3. For higher RX filter BW's MAGN_TARGET is 0x7.
//        {CC1101_AGCCTRL0,
//                0x91}, // 10 - Medium hysteresis, medium asymmetric dead zone, medium gain ; 01 - 16 samples agc; 00 - Normal AGC, 01 - 8dB boundary
//        {CC1101_AGCCTRL1,
//                0x0}, // 0; 0 - LNA 2 gain is decreased to minimum before decreasing LNA gain; 00 - Relative carrier sense threshold disabled; 0000 - RSSI to MAIN_TARGET
//        {CC1101_AGCCTRL2, 0x07}, // 00 - DVGA all; 000 - MAX LNA+LNA2; 111 - MAIN_TARGET 42 dB
//
//        /* Wake on radio and timeouts control */
//        {CC1101_WORCTRL, 0xFB}, // WOR_RES is 2^15 periods (0.91 - 0.94 s) 16.5 - 17.2 hours
//
//        /* Frontend configuration */
//        {CC1101_FREND0, 0x11}, // Adjusts current TX LO buffer + high is PATABLE[1]
//        {CC1101_FREND1, 0xB6}, //
//
//        /* End  */
//        {0, 0},
//};


void pocsag_hal_reset() {
    furi_hal_subghz_reset();
    furi_hal_subghz_idle();
    furi_hal_subghz_load_registers((uint8_t*)furi_hal_subghz_preset_2fsk_dev9_5khz_async_regs);
    furi_hal_subghz_load_patable(furi_hal_subghz_preset_2fsk_async_patable);
//    furi_hal_subghz_load_registers((uint8_t*)furi_hal_subghz_preset_ook_650khz_async_regs);
//    furi_hal_subghz_load_patable(furi_hal_subghz_preset_ook_async_patable);
}