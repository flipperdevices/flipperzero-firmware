#include "mag_helpers.h"

#define TAG "MagHelpers"

// Haviv Board - pins gpio_ext_pa7 & gpio_ext_pa6 was swapped.
#define GPIO_PIN_A &gpio_ext_pa7
#define GPIO_PIN_B &gpio_ext_pa6
#define GPIO_PIN_ENABLE &gpio_ext_pa4
#define RFID_PIN_OUT &gpio_rfid_carrier_out

#define ZERO_PREFIX 25 // n zeros prefix
#define ZERO_BETWEEN 53 // n zeros between tracks
#define ZERO_SUFFIX 25 // n zeros suffix

// bits per char on a given track
const uint8_t bitlen[] = {7, 5, 5};
// char offset by track
const int sublen[] = {32, 48, 48};

uint8_t last_value = 2;

void play_halfbit(bool value, MagSetting* setting) {
    switch(setting->tx) {
    case MagTxStateRFID:
        furi_hal_gpio_write(RFID_PIN_OUT, value);
        /*furi_hal_gpio_write(RFID_PIN_OUT, !value);
        furi_hal_gpio_write(RFID_PIN_OUT, value);
        furi_hal_gpio_write(RFID_PIN_OUT, !value);
        furi_hal_gpio_write(RFID_PIN_OUT, value);*/
        break;
    case MagTxStateGPIO:
        furi_hal_gpio_write(GPIO_PIN_A, value);
        furi_hal_gpio_write(GPIO_PIN_B, !value);
        break;
    case MagTxStatePiezo:
        furi_hal_gpio_write(&gpio_speaker, value);
        /*furi_hal_gpio_write(&gpio_speaker, !value);
        furi_hal_gpio_write(&gpio_speaker, value);
        furi_hal_gpio_write(&gpio_speaker, !value);
        furi_hal_gpio_write(&gpio_speaker, value);*/

        break;
    case MagTxStateLF_P:
        furi_hal_gpio_write(RFID_PIN_OUT, value);
        furi_hal_gpio_write(&gpio_speaker, value);

        /* // Weaker but cleaner signal
        if(value) {
            furi_hal_gpio_write(RFID_PIN_OUT, value);
            furi_hal_gpio_write(&gpio_speaker, value);
            furi_delay_us(10);
            furi_hal_gpio_write(RFID_PIN_OUT, !value);
            furi_hal_gpio_write(&gpio_speaker, !value);
        } else {
            furi_delay_us(10);
        }*/

        /*furi_hal_gpio_write(RFID_PIN_OUT, value);
        furi_hal_gpio_write(&gpio_speaker, value);
        furi_hal_gpio_write(RFID_PIN_OUT, !value);
        furi_hal_gpio_write(&gpio_speaker, !value);
        furi_hal_gpio_write(RFID_PIN_OUT, value);
        furi_hal_gpio_write(&gpio_speaker, value);*/
        break;
    case MagTxStateNFC:
        // turn on for duration of half-bit? or "blip" the field on / off?
        // getting nothing from the mag reader either way
        //(value) ? furi_hal_nfc_ll_txrx_on() : furi_hal_nfc_ll_txrx_off();

        if(last_value == 2 || value != (bool)last_value) {
            furi_hal_nfc_ll_txrx_on();
            //furi_delay_us(64);
            furi_hal_nfc_ll_txrx_off();
        }
        break;
    case MagTxCC1101_434:
    case MagTxCC1101_868:
        if(last_value == 2 || value != (bool)last_value) {
            furi_hal_gpio_write(&gpio_cc1101_g0, true);
            furi_delay_us(64);
            furi_hal_gpio_write(&gpio_cc1101_g0, false);
        }
        break;
    default:
        break;
    }

    last_value = value;
}

void play_track(uint8_t* bits_manchester, uint16_t n_bits, MagSetting* setting, bool reverse) {
    for(uint16_t i = 0; i < n_bits; i++) {
        uint16_t j = (reverse) ? (n_bits - i - 1) : i;
        uint8_t byte = j / 8;
        uint8_t bitmask = 1 << (7 - (j % 8));
        /* Bits are stored in their arrays like on a card (LSB first). This is not how usually bits are stored in a
         * byte, with the MSB first. the var bitmask creates the pattern to iterate through each bit, LSB first, like so
         * 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x80... masking bits one by one from the current byte
         *
         * I've chosen this LSB approach since bits and bytes are hard enough to visualize with the 5/8 and 7/8 encoding
         * MSR uses. It's a biiit more complicated to process, but visualizing it with printf or a debugger is
         * infinitely easier
         *
         * Encoding the following pairs of 5 bits as 5/8: A1234 B1234 C1234 D1234
         * using this LSB format looks like: A1234B12 34C1234D 12340000
         * using the MSB format, looks like: 21B4321A D4321C43 00004321
         * this means reading each byte backwards when printing/debugging, and the jumping 16 bits ahead, reading 8 more
         * bits backward, jumping 16 more bits ahead.
         *
         * I find this much more convenient for debugging, with the tiny incovenience of reading the bits in reverse
         * order. Thus, the reason for the bitmask above
         */

        bool bit = !!(bits_manchester[byte] & bitmask);

        // TODO: reimplement timing delays. Replace fixed furi_hal_cortex_delay_us to wait instead to a specific value
        // for DWT->CYCCNT. Note timer is aliased to 64us as per
        // #define FURI_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND (SystemCoreClock / 1000000) | furi_hal_cortex.c

        play_halfbit(bit, setting);
        furi_delay_us(setting->us_clock);
        // if (i % 2 == 1) furi_delay_us(setting->us_interpacket);
    }
}

void tx_init_rfid() {
    // initialize RFID system for TX

    // OTG needed for RFID? Or just legacy from GPIO?
    // furi_hal_power_enable_otg();
    furi_hal_ibutton_pin_configure();

    // furi_hal_ibutton_start_drive();
    furi_hal_ibutton_pin_write(false);

    // Initializing at GpioSpeedLow seems sufficient for our needs; no improvements seen by increasing speed setting

    // this doesn't seem to make a difference, leaving it in
    furi_hal_gpio_init(&gpio_rfid_data_in, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_rfid_data_in, false);

    // false->ground RFID antenna; true->don't ground
    // skotopes (RFID dev) say normally you'd want RFID_PULL in high for signal forming, while modulating RFID_OUT
    // dunaevai135 had it low in their old code. Leaving low, as it doesn't seem to make a difference on my janky antenna
    furi_hal_gpio_init(&gpio_nfc_irq_rfid_pull, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_nfc_irq_rfid_pull, false);

    furi_hal_gpio_init(RFID_PIN_OUT, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

    furi_delay_ms(300);
}

void tx_deinit_rfid() {
    // reset RFID system
    furi_hal_gpio_write(RFID_PIN_OUT, 0);

    furi_hal_rfid_pins_reset();
    furi_hal_power_disable_otg();
}

void tx_init_rf(int hz) {
    // presets and frequency will need some experimenting
    furi_hal_subghz_reset();
    // furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);
    // furi_hal_subghz_load_preset(FuriHalSubGhzPresetGFSK9_99KbAsync);
    // furi_hal_subghz_load_preset(FuriHalSubGhzPresetMSK99_97KbAsync);
    // furi_hal_subghz_load_preset(FuriHalSubGhzPreset2FSKDev238Async);
    // furi_hal_subghz_load_preset(FuriHalSubGhzPreset2FSKDev476Async);
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_subghz_set_frequency_and_path(hz);
    furi_hal_subghz_tx();
    furi_hal_gpio_write(&gpio_cc1101_g0, false);
}

void tx_init_piezo() {
    // TODO: some special mutex acquire procedure? c.f. furi_hal_speaker.c
    furi_hal_gpio_init(&gpio_speaker, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
}

void tx_deinit_piezo() {
    // TODO: some special mutex release procedure?
    furi_hal_gpio_init(&gpio_speaker, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

bool tx_init(MagSetting* setting) {
    // Initialize configured TX method
    switch(setting->tx) {
    case MagTxStateRFID:
        tx_init_rfid();
        break;
    case MagTxStateGPIO:
        furi_hal_power_enable_otg();
        // gpio_item_configure_all_pins(GpioModeOutputPushPull);
        furi_hal_gpio_init(GPIO_PIN_A, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(GPIO_PIN_B, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(GPIO_PIN_ENABLE, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

        furi_hal_gpio_write(GPIO_PIN_ENABLE, 1);

        // had some issues with ~300; bumped higher temporarily
        furi_delay_ms(500);
        break;
    case MagTxStatePiezo:
        tx_init_piezo();
        break;
    case MagTxStateLF_P:
        tx_init_piezo();
        tx_init_rfid();
        break;
    case MagTxStateNFC:
        furi_hal_nfc_exit_sleep();
        break;
    case MagTxCC1101_434:
        tx_init_rf(434000000);
        break;
    case MagTxCC1101_868:
        tx_init_rf(868000000);
        break;
    default:
        return false;
    }

    return true;
}

bool tx_deinit(MagSetting* setting) {
    // Reset configured TX method
    switch(setting->tx) {
    case MagTxStateRFID:
        tx_deinit_rfid();
        break;
    case MagTxStateGPIO:
        furi_hal_gpio_write(GPIO_PIN_A, 0);
        furi_hal_gpio_write(GPIO_PIN_B, 0);
        furi_hal_gpio_write(GPIO_PIN_ENABLE, 0);

        // set back to analog output mode?
        //gpio_item_configure_all_pins(GpioModeAnalog);
        furi_hal_power_disable_otg();
        break;
    case MagTxStatePiezo:
        tx_deinit_piezo();
        break;
    case MagTxStateLF_P:
        tx_deinit_piezo();
        tx_deinit_rfid();
        break;
    case MagTxStateNFC:
        furi_hal_nfc_ll_txrx_off();
        furi_hal_nfc_start_sleep();
        break;
    case MagTxCC1101_434:
    case MagTxCC1101_868:
        furi_hal_gpio_write(&gpio_cc1101_g0, false);
        furi_hal_subghz_reset();
        furi_hal_subghz_idle();
        break;
    default:
        return false;
    }

    return true;
}

void mag_spoof(Mag* mag) {
    MagSetting* setting = mag->setting;

    // TODO: cleanup this section. Possibly move precompute + tx_init to emulate_on_enter?
    FuriString* ft1 = mag->mag_dev->dev_data.track[0].str;
    FuriString* ft2 = mag->mag_dev->dev_data.track[1].str;
    FuriString* ft3 = mag->mag_dev->dev_data.track[2].str;

    char *data1, *data2, *data3;
    data1 = malloc(furi_string_size(ft1) + 1);
    data2 = malloc(furi_string_size(ft2) + 1);
    data3 = malloc(furi_string_size(ft3) + 1);
    strncpy(data1, furi_string_get_cstr(ft1), furi_string_size(ft1));
    strncpy(data2, furi_string_get_cstr(ft2), furi_string_size(ft2));
    strncpy(data3, furi_string_get_cstr(ft3), furi_string_size(ft3));

    if(furi_log_get_level() >= FuriLogLevelDebug) {
        debug_mag_string(data1, bitlen[0], sublen[0]);
        debug_mag_string(data2, bitlen[1], sublen[1]);
        debug_mag_string(data3, bitlen[2], sublen[2]);
    }

    uint8_t bits_t1_raw[64] = {0x00}; // 68 chars max track 1 + 1 char crc * 7 approx =~ 483 bits
    uint8_t bits_t1_manchester[128] = {0x00}; // twice the above
    uint16_t bits_t1_count = mag_encode(
        data1, (uint8_t*)bits_t1_manchester, (uint8_t*)bits_t1_raw, bitlen[0], sublen[0]);
    uint8_t bits_t2_raw[64] = {0x00}; // 68 chars max track 1 + 1 char crc * 7 approx =~ 483 bits
    uint8_t bits_t2_manchester[128] = {0x00}; // twice the above
    uint16_t bits_t2_count = mag_encode(
        data2, (uint8_t*)bits_t2_manchester, (uint8_t*)bits_t2_raw, bitlen[1], sublen[1]);
    uint8_t bits_t3_raw[64] = {0x00};
    uint8_t bits_t3_manchester[128] = {0x00};
    uint16_t bits_t3_count = mag_encode(
        data3, (uint8_t*)bits_t3_manchester, (uint8_t*)bits_t3_raw, bitlen[2], sublen[2]);

    if(furi_log_get_level() >= FuriLogLevelDebug) {
        printf(
            "Manchester bitcount: T1: %d, T2: %d, T3: %d\r\n",
            bits_t1_count,
            bits_t2_count,
            bits_t3_count);
        printf("T1 raw: ");
        for(int i = 0; i < bits_t1_count / 16; i++) printf("%02x ", bits_t1_raw[i]);
        printf("\r\nT1 manchester: ");
        for(int i = 0; i < bits_t1_count / 8; i++) printf("%02x ", bits_t1_manchester[i]);
        printf("\r\nT2 raw: ");
        for(int i = 0; i < bits_t2_count / 16; i++) printf("%02x ", bits_t2_raw[i]);
        printf("\r\nT2 manchester: ");
        for(int i = 0; i < bits_t2_count / 8; i++) printf("%02x ", bits_t2_manchester[i]);
        printf("\r\nT3 raw: ");
        for(int i = 0; i < bits_t3_count / 16; i++) printf("%02x ", bits_t3_raw[i]);
        printf("\r\nT3 manchester: ");
        for(int i = 0; i < bits_t3_count / 8; i++) printf("%02x ", bits_t3_manchester[i]);
        printf("\r\nBitwise emulation done\r\n\r\n");
    }

    last_value = 2;
    bool bit = false;

    if(!tx_init(setting)) return;

    FURI_CRITICAL_ENTER();
    for(uint16_t i = 0; i < (ZERO_PREFIX * 2); i++) {
        // is this right?
        if(!!(i % 2)) bit ^= 1;
        play_halfbit(bit, setting);
        furi_delay_us(setting->us_clock);
    }

    if((setting->track == MagTrackStateOneAndTwo) || (setting->track == MagTrackStateOne))
        play_track((uint8_t*)bits_t1_manchester, bits_t1_count, setting, false);

    if((setting->track == MagTrackStateOneAndTwo))
        for(uint16_t i = 0; i < (ZERO_BETWEEN * 2); i++) {
            if(!!(i % 2)) bit ^= 1;
            play_halfbit(bit, setting);
            furi_delay_us(setting->us_clock);
        }

    if((setting->track == MagTrackStateOneAndTwo) || (setting->track == MagTrackStateTwo))
        play_track(
            (uint8_t*)bits_t2_manchester,
            bits_t2_count,
            setting,
            (setting->reverse == MagReverseStateOn));

    if((setting->track == MagTrackStateThree))
        play_track((uint8_t*)bits_t3_manchester, bits_t3_count, setting, false);

    for(uint16_t i = 0; i < (ZERO_SUFFIX * 2); i++) {
        if(!!(i % 2)) bit ^= 1;
        play_halfbit(bit, setting);
        furi_delay_us(setting->us_clock);
    }
    FURI_CRITICAL_EXIT();

    free(data1);
    free(data2);
    free(data3);
    tx_deinit(setting);
}

uint16_t add_bit(bool value, uint8_t* out, uint16_t count) {
    uint8_t bit = count % 8;
    uint8_t byte = count / 8;
    if(value) {
        out[byte] |= 0x01;
    }
    if(bit < 7) out[byte] <<= 1;
    return count + 1;
}

uint16_t add_bit_manchester(bool value, uint8_t* out, uint16_t count) {
    static bool toggle = 0;
    toggle ^= 0x01;
    count = add_bit(toggle, out, count);
    if(value) toggle ^= 0x01;
    count = add_bit(toggle, out, count);
    return count;
}

uint16_t mag_encode(
    char* data,
    uint8_t* out_manchester,
    uint8_t* out_raw,
    uint8_t track_bits,
    uint8_t track_ascii_offset) {
    /*
     * track_bits - the number of raw (data) bits on the track. on ISO cards, that's 7 for track 1, or 5 for 2/3 - this is samy's bitlen
     *            - this count includes the parity bit
     * track_ascii_offset - how much the ascii values are offset. track 1 makes space (ascii 32) become data 0x00,
     *                    - tracks 2/3 make ascii "0" become data 0x00 - this is samy's sublen
     *
     */

    uint16_t raw_bits_count = 0;
    uint16_t output_count = 0;
    int tmp, crc, lrc = 0;

    /* // why are we adding zeros to the encoded string if we're also doing it while playing?
    for(int i = 0; i < ZERO_PREFIX; i++) {
        output_count = add_bit_manchester(0, out_manchester, output_count);
        raw_bits_count = add_bit(0, out_raw, raw_bits_count);
    }*/

    for(int i = 0; *(data + i) != 0; i++) {
        crc = 1;
        tmp = *(data + i) - track_ascii_offset;

        for(int j = 0; j < track_bits - 1; j++) {
            crc ^= tmp & 1;
            lrc ^= (tmp & 1) << j;
            raw_bits_count = add_bit(tmp & 0x01, out_raw, raw_bits_count);
            output_count = add_bit_manchester(tmp & 0x01, out_manchester, output_count);
            tmp >>= 1;
        }
        raw_bits_count = add_bit(crc, out_raw, raw_bits_count);
        output_count = add_bit_manchester(crc, out_manchester, output_count);
    }

    // LRC byte
    tmp = lrc;
    crc = 1;
    for(int j = 0; j < track_bits - 1; j++) {
        crc ^= tmp & 0x01;
        raw_bits_count = add_bit(tmp & 0x01, out_raw, raw_bits_count);
        output_count = add_bit_manchester(tmp & 0x01, out_manchester, output_count);
        tmp >>= 1;
    }
    raw_bits_count = add_bit(crc, out_raw, raw_bits_count);
    output_count = add_bit_manchester(crc, out_manchester, output_count);

    return output_count;
}

void debug_mag_string(char* data, uint8_t track_bits, uint8_t track_ascii_offset) {
    uint8_t bits_raw[64] = {0}; // 68 chars max track 1 + 1 char crc * 7 approx =~ 483 bits
    uint8_t bits_manchester[128] = {0}; // twice the above
    int numbits = 0;

    printf("Encoding [%s] with %d bits\r\n", data, track_bits);
    numbits = mag_encode(
        data, (uint8_t*)bits_manchester, (uint8_t*)bits_raw, track_bits, track_ascii_offset);
    printf("Got %d bits\r\n", numbits);
    printf("Raw byte stream:     ");
    for(int i = 0; i < numbits / 8 / 2; i++) {
        printf("%02x", bits_raw[i]);
        if(i % 4 == 3) printf(" ");
    }

    printf("\r\n");

    printf("Bits                 ");
    int space_counter = 0;
    for(int i = 0; i < numbits / 2; i++) {
        /*if(i < ZERO_PREFIX) {
            printf("X");
            continue;
        } else if(i == ZERO_PREFIX) {
            printf(" ");
            space_counter = 0;
        }*/
        printf("%01x", (bits_raw[i / 8] & (1 << (7 - (i % 8)))) != 0);
        if((space_counter) % track_bits == track_bits - 1) printf(" ");
        space_counter++;
    }

    printf("\r\n");

    printf("Manchester encoded, byte stream: ");
    for(int i = 0; i < numbits / 8; i++) {
        printf("%02x", bits_manchester[i]);
        if(i % 4 == 3) printf(" ");
    }
    printf("\r\n\r\n");
}
