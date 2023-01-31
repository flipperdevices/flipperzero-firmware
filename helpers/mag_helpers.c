#include "mag_helpers.h"

#define TAG "MagHelpers"

#define GPIO_PIN_A &gpio_ext_pa6
#define GPIO_PIN_B &gpio_ext_pa7
#define GPIO_PIN_ENABLE &gpio_ext_pa4
#define RFID_PIN_OUT &gpio_rfid_carrier_out

#define ZERO_PREFIX 25 // n zeros prefix
#define ZERO_BETWEEN 53 // n zeros between tracks
#define ZERO_SUFFIX 25 // n zeros suffix

// bits per char on a given track
const uint8_t bitlen[] = {7, 5, 5};
// char offset by track
const int sublen[] = {32, 48, 48};
uint8_t bit_dir = 0;
uint8_t last_value = 2;

void bitbang_raw(bool value, MagSetting* setting) {
    switch(setting->tx) {
    case MagTxStateRFID:
        furi_hal_gpio_write(RFID_PIN_OUT, value);
        break;
    case MagTxStateGPIOA6A7:
        furi_hal_gpio_write(GPIO_PIN_A, value);
        furi_hal_gpio_write(GPIO_PIN_B, !value);
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

void play_bit_rf(bool bit, MagSetting* setting) {
    bit_dir ^= 1;
    furi_hal_gpio_write(&gpio_cc1101_g0, true);
    furi_delay_us(64);
    furi_hal_gpio_write(&gpio_cc1101_g0, false);
    furi_delay_us(setting->us_clock);

    if(bit) {
        furi_hal_gpio_write(&gpio_cc1101_g0, true);
        furi_delay_us(64);
        furi_hal_gpio_write(&gpio_cc1101_g0, false);
    }
    furi_delay_us(setting->us_clock);
    furi_delay_us(setting->us_interpacket);
}

void play_bit_rfid(uint8_t send_bit, MagSetting* setting) {
    // internal TX over RFID coil
    bit_dir ^= 1;
    furi_hal_gpio_write(RFID_PIN_OUT, bit_dir);
    furi_delay_us(setting->us_clock);

    if(send_bit) {
        bit_dir ^= 1;
        furi_hal_gpio_write(RFID_PIN_OUT, bit_dir);
    }
    furi_delay_us(setting->us_clock);

    furi_delay_us(setting->us_interpacket);
}

void play_bit_gpio(uint8_t send_bit, MagSetting* setting) {
    // external TX over motor driver wired to PIN_A and PIN_B
    bit_dir ^= 1;
    furi_hal_gpio_write(GPIO_PIN_A, bit_dir);
    furi_hal_gpio_write(GPIO_PIN_B, !bit_dir);
    furi_delay_us(setting->us_clock);

    if(send_bit) {
        bit_dir ^= 1;
        furi_hal_gpio_write(GPIO_PIN_A, bit_dir);
        furi_hal_gpio_write(GPIO_PIN_B, !bit_dir);
    }
    furi_delay_us(setting->us_clock);

    furi_delay_us(setting->us_interpacket);
}

bool play_bit(uint8_t send_bit, MagSetting* setting) {
    // Initialize configured TX method
    switch(setting->tx) {
    case MagTxStateRFID:
        play_bit_rfid(send_bit, setting);
        break;
    case MagTxStateGPIOA6A7:
        play_bit_gpio(send_bit, setting);
        break;
    case MagTxCC1101_434:
    case MagTxCC1101_868:
        play_bit_rf(send_bit & 0x01, setting);
        break;
    default:
        return false;
    }

    return true;
}

void tx_init_rfid() {
    // initialize RFID system for TX
    furi_hal_power_enable_otg();

    furi_hal_ibutton_start_drive();
    furi_hal_ibutton_pin_low();

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

void tx_init_gpio() {
    furi_hal_power_enable_otg();
    // gpio_item_configure_all_pins(GpioModeOutputPushPull);
    furi_hal_gpio_init(GPIO_PIN_A, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(GPIO_PIN_B, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(GPIO_PIN_ENABLE, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_write(GPIO_PIN_ENABLE, 1);

    // had some issues with ~300; bumped higher temporarily
    furi_delay_ms(500);
}

void tx_deinit_gpio() {
    furi_hal_gpio_write(GPIO_PIN_A, 0);
    furi_hal_gpio_write(GPIO_PIN_B, 0);
    furi_hal_gpio_write(GPIO_PIN_ENABLE, 0);

    // set back to analog output mode?
    //gpio_item_configure_all_pins(GpioModeAnalog);
    furi_hal_power_disable_otg();
}

void tx_init_rf(int hz) {
    // presets and frequency will need some experimenting
    furi_hal_subghz_reset();
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);
    // furi_hal_subghz_load_preset(FuriHalSubGhzPresetGFSK9_99KbAsync);
    // furi_hal_subghz_load_preset(FuriHalSubGhzPresetMSK99_97KbAsync);
    // furi_hal_subghz_load_preset(FuriHalSubGhzPreset2FSKDev238Async);
    // furi_hal_subghz_load_preset(FuriHalSubGhzPreset2FSKDev476Async);
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_subghz_set_frequency_and_path(hz);
    furi_hal_subghz_tx();
    furi_hal_gpio_write(&gpio_cc1101_g0, false);
}

void tx_deinit_rf() {
    furi_hal_gpio_write(&gpio_cc1101_g0, false);
    furi_hal_subghz_reset();
    furi_hal_subghz_idle();
}

bool tx_init(MagSetting* setting) {
    // Initialize configured TX method
    switch(setting->tx) {
    case MagTxStateRFID:
        tx_init_rfid();
        break;
    case MagTxStateGPIOA6A7:
        tx_init_gpio();
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
    case MagTxStateGPIOA6A7:
        tx_deinit_gpio();
        break;
    case MagTxCC1101_434:
    case MagTxCC1101_868:
        tx_deinit_rf();
        break;
    default:
        return false;
    }

    return true;
}

// due for deprecation
void track_to_bits(uint8_t* bit_array, const char* track_data, uint8_t track_index) {
    // convert individual track to bits

    int tmp, crc, lrc = 0;
    int i = 0;

    // convert track data to bits
    for(uint8_t j = 0; track_data[j] != '\0'; j++) {
        crc = 1;
        tmp = track_data[j] - sublen[track_index];

        for(uint8_t k = 0; k < bitlen[track_index] - 1; k++) {
            crc ^= tmp & 1;
            lrc ^= (tmp & 1) << k;
            bit_array[i] = tmp & 1;
            i++;
            tmp >>= 1;
        }
        bit_array[i] = crc;
        i++;
    }

    FURI_LOG_D(TAG, "LRC");
    // finish calculating final "byte" (LRC)
    tmp = lrc;
    crc = 1;
    for(uint8_t j = 0; j < bitlen[track_index] - 1; j++) {
        crc ^= tmp & 1;
        bit_array[i] = tmp & 1;
        i++;
        tmp >>= 1;
    }
    bit_array[i] = crc;
    i++;

    // My makeshift end sentinel. All other values 0/1
    bit_array[i] = 2;
    i++;

    // Log the output (messy but works)
    //char output[500] = {0x0};
    /*FuriString* tmp_str;
    tmp_str = furi_string_alloc();
    for(uint8_t j = 0; bit_array[j] != 2; j++) {
        furi_string_cat_printf(tmp_str, "%d", (bit_array[j] & 1));
        //strcat(output, furi_string_get_cstr(tmp_str));
    }
    FURI_LOG_D(TAG, "Track %d: %s", (track_index + 1), track_data);
    FURI_LOG_D(TAG, "Track %d: %s", (track_index + 1), furi_string_get_cstr(tmp_str));*/
    //furi_string_free(tmp_str);
}

void mag_spoof_bitwise(Mag* mag) {
    MagSetting* setting = mag->setting;

    FuriString* ft1 = mag->mag_dev->dev_data.track[0].str;
    FuriString* ft2 = mag->mag_dev->dev_data.track[1].str;

    char* data1;
    char* data2;
    data1 = malloc(furi_string_size(ft1) + 1);
    data2 = malloc(furi_string_size(ft2) + 1);
    strncpy(data1, furi_string_get_cstr(ft1), furi_string_size(ft1));
    strncpy(data2, furi_string_get_cstr(ft2), furi_string_size(ft2));

    if(furi_log_get_level() >= FuriLogLevelDebug) {
        debug_msr_string(data1, BITS_TRACK1, OFFSET_TRACK1);
        debug_msr_string(data2, BITS_TRACK2, OFFSET_TRACK2);
    }

    uint8_t bits_t1_raw[64] = {0x00}; // 68 chars max track 1 + 1 char crc * 7 approx =~ 483 bits
    uint8_t bits_t1_manchester[128] = {0x00}; // twice the above
    uint16_t bits_t1_count = msr_encode(
        data1, (uint8_t*)bits_t1_manchester, (uint8_t*)bits_t1_raw, BITS_TRACK1, OFFSET_TRACK1);
    uint8_t bits_t2_raw[64] = {0x00}; // 68 chars max track 1 + 1 char crc * 7 approx =~ 483 bits
    uint8_t bits_t2_manchester[128] = {0x00}; // twice the above
    uint16_t bits_t2_count = msr_encode(
        data2, (uint8_t*)bits_t2_manchester, (uint8_t*)bits_t2_raw, BITS_TRACK2, OFFSET_TRACK2);

    if(furi_log_get_level() >= FuriLogLevelDebug) {
        printf("Manchester bitcount: T1: %d, T2: %d\r\n", bits_t1_count, bits_t2_count);

        printf("T1 raw: ");
        for(int i = 0; i < bits_t1_count / 16; i++) printf("%02x ", bits_t1_raw[i]);
        printf("\r\n");

        printf("T1 manchester: ");
        for(int i = 0; i < bits_t1_count / 8; i++) printf("%02x ", bits_t1_manchester[i]);
        printf("\r\n");

        printf("T2 raw: ");
        for(int i = 0; i < bits_t2_count / 16; i++) printf("%02x ", bits_t2_raw[i]);
        printf("\r\n");

        printf("T2 manchester: ");
        for(int i = 0; i < bits_t2_count / 8; i++) printf("%02x ", bits_t2_manchester[i]);
        printf("\r\n");

        printf("Bitwise emulation done\r\n\r\n");
    }

    if(!tx_init(setting)) return;
    last_value = 2;
    FURI_CRITICAL_ENTER();
    bool bit = false;

    if((setting->track == MagTrackStateAll))
        for(uint16_t i = 0; i < ZERO_PREFIX; i++) {
            bit ^= 0xFF;
            bitbang_raw(bit, setting);
            furi_delay_us(setting->us_clock * 2);
        }

    if((setting->track == MagTrackStateAll) || (setting->track == MagTrackStateOne))
        for(uint16_t i = 0; i < bits_t1_count; i++) {
            uint8_t byte = i / 8;
            uint8_t bitmask = 1 << (7 - (i % 8));
            /* this comment is mostly for zw's convenience:
             *
             * bits are stored in their arrays like on a card (LSB first). This is not how usually bits are stored in a
             * byte, with the MSB first. the var bitmask creates the pattern to iterate through each bit, LSB first, like so
             * 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x80... masking bits one by one from the current byte
             *
             * i've chosen this LSB approach since bits and bytes are hard enough to visualize with the 5/8 and 7/8 encoding
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
             * order. THus, the reason for the bitmask above
             */

            bit = !!(bits_t1_manchester[byte] & bitmask);

            // TODO: reimplement timing delays. Replace fixed furi_hal_cortex_delay_us to wait instead to a specific value
            // for DWT->CYCCNT. Note timer is aliased to 64us as per
            // #define FURI_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND (SystemCoreClock / 1000000) | furi_hal_cortex.c

            bitbang_raw(bit, setting);
            furi_delay_us(setting->us_clock);
            // if (i % 2 == 1) furi_delay_us(setting->us_interpacket);
        }

    if((setting->track == MagTrackStateAll))
        for(uint16_t i = 0; i < ZERO_BETWEEN; i++) {
            bit ^= 0xFF;
            bitbang_raw(bit, setting);
            furi_delay_us(setting->us_clock * 2);
        }

    if((setting->track == MagTrackStateAll) || (setting->track == MagTrackStateTwo))
        for(uint16_t i = 0; i < bits_t2_count; i++) {
            uint16_t j = bits_t2_count - i - 1;
            uint8_t byte = j / 8;
            uint8_t bitmask = 1 << (7 - (j % 8));
            bool bit = !!(bits_t2_manchester[byte] & bitmask);
            bitbang_raw(bit, setting);
            furi_delay_us(setting->us_clock);
            // if (i % 2 == 1) furi_delay_us(setting->us_interpacket);
        }

    if((setting->track == MagTrackStateAll))
        for(uint16_t i = 0; i < ZERO_SUFFIX; i++) {
            bit ^= 0xFF;
            bitbang_raw(bit, setting);
            furi_delay_us(setting->us_clock * 2);
        }

    FURI_CRITICAL_EXIT();
    free(data1);
    free(data2);
    tx_deinit(setting);
}

// due for deprecation
void mag_spoof(Mag* mag) {
    MagSetting* setting = mag->setting;

    // precompute tracks (WIP; ignores reverse and 3rd track)
    // likely will be reworked to antirez's bitmap method anyway...
    const char* data1 = furi_string_get_cstr(mag->mag_dev->dev_data.track[0].str);
    const char* data2 = furi_string_get_cstr(mag->mag_dev->dev_data.track[1].str);
    uint8_t bit_array1[2 * (strlen(data1) * bitlen[0]) + 1];
    uint8_t bit_array2[2 * (strlen(data2) * bitlen[1]) + 1];
    track_to_bits(bit_array1, data1, 0);
    track_to_bits(bit_array2, data2, 1);

    bool spoofed = false;
    do {
        // Initialize configured TX method
        if(!tx_init(setting)) break;

        // Critical timing section (need to eliminate ifs? does this impact timing?)
        FURI_CRITICAL_ENTER();
        // Prefix of zeros
        for(uint16_t i = 0; i < ZERO_PREFIX; i++) {
            if(!play_bit(0, setting)) break;
        }

        // Track 1
        if((setting->track == MagTrackStateAll) || (setting->track == MagTrackStateOne)) {
            for(uint16_t i = 0; bit_array1[i] != 2; i++) {
                if(!play_bit((bit_array1[i] & 1), setting)) break;
            }
        }

        // Zeros between tracks
        if(setting->track == MagTrackStateAll) {
            for(uint16_t i = 0; i < ZERO_BETWEEN; i++) {
                if(!play_bit(0, setting)) break;
            }
        }

        // Track 2 (TODO: Reverse track)
        if((setting->track == MagTrackStateAll) || (setting->track == MagTrackStateTwo)) {
            for(uint16_t i = 0; bit_array2[i] != 2; i++) {
                if(!play_bit((bit_array2[i] & 1), setting)) break;
            }
        }

        // Suffix of zeros
        for(uint16_t i = 0; i < ZERO_SUFFIX; i++) {
            if(!play_bit(0, setting)) break;
        }
        FURI_CRITICAL_EXIT();

        // Reset configured TX method
        if(!tx_deinit(setting)) break;
        spoofed = true;
    } while(0);

    UNUSED(spoofed);
    /*if(!spoofed) {
        // error handling?
        // cleanup?
    }*/
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

uint16_t msr_encode(
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

    for(int i = 0; i < PREFIX_NUM_ZEROES; i++) {
        output_count = add_bit_manchester(0, out_manchester, output_count);
        raw_bits_count = add_bit(0, out_raw, raw_bits_count);
    }

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

void debug_msr_string(char* data, uint8_t track_bits, uint8_t track_ascii_offset) {
    uint8_t bits_raw[64] = {0}; // 68 chars max track 1 + 1 char crc * 7 approx =~ 483 bits
    uint8_t bits_manchester[128] = {0}; // twice the above
    int numbits = 0;

    printf("Encoding [%s] with %d bits\r\n", data, track_bits);
    numbits = msr_encode(
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
        if(i < PREFIX_NUM_ZEROES) {
            printf("X");
            continue;
        } else if(i == PREFIX_NUM_ZEROES) {
            printf(" ");
            space_counter = 0;
        }
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