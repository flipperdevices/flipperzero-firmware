#include "../mag_i.h"

#define PIN_A 0
#define PIN_B 1 // currently unused
#define CLOCK_US 500 // typically set between 200-500us
#define TEST_STR "%%B123456781234567^LASTNAME/FIRST^YYMMSSSDDDDDDDDDDDDDDDDDDDDDDDDD?\0"
#define TEST_TRACK 0
// TODO: better way of setting temp test str,
//       text wrapping on screen? (Will be relevant for any loaded data too)

uint8_t magspoof_bit_dir = 0;
const char* test_str = TEST_STR;

void gpio_item_set_rfid_pin(uint8_t index, bool level) {
    if(index == 0) {
        furi_hal_gpio_write(&gpio_rfid_carrier_out, level);
        // A7 GPIO pin for debugging purposes
        // furi_hal_gpio_write(&gpio_ext_pa7, level);
    }
}

static void play_bit(uint8_t send_bit) {
    magspoof_bit_dir ^= 1;
    gpio_item_set_rfid_pin(PIN_A, magspoof_bit_dir);
    // PIN_B goes unused in current LF modulation.
    // Leaving legacy here in event we attempt downstream modulation,
    // rather than just modulating RFID_OUT upstream for signal forming
    gpio_item_set_rfid_pin(PIN_B, !magspoof_bit_dir);
    furi_delay_us(CLOCK_US);

    // NFC TEST
    //(magspoof_bit_dir) ? furi_hal_nfc_ll_txrx_on() : furi_hal_nfc_ll_txrx_off();

    if(send_bit) {
        magspoof_bit_dir ^= 1;
        gpio_item_set_rfid_pin(PIN_A, magspoof_bit_dir);
        gpio_item_set_rfid_pin(PIN_B, !magspoof_bit_dir);

        //(magspoof_bit_dir) ? furi_hal_nfc_ll_txrx_on() : furi_hal_nfc_ll_txrx_off();
    }
    furi_delay_us(CLOCK_US);
}

static void mag_spoof_test(FuriString* track_str, uint8_t track) {
    furi_hal_power_enable_otg();

    size_t from;
    size_t to;

    // TODO ';' in first track case
    if(track == 0) {
        from = furi_string_search_char(track_str, '%');
        to = furi_string_search_char(track_str, '?', from);
    } else if(track == 1) {
        from = furi_string_search_char(track_str, ';');
        to = furi_string_search_char(track_str, '?', from);
    } else {
        from = 0;
        to = furi_string_size(track_str);
    }
    if(from >= to) {
        return;
    }
    furi_string_mid(track_str, from, to - from + 1);

    const char* data = furi_string_get_cstr(track_str);

    printf("%s", data);

    furi_hal_ibutton_start_drive();
    furi_hal_ibutton_pin_low();

    // NFC TEST
    //furi_hal_nfc_exit_sleep();

    // Initializing at GpioSpeedLow seems sufficient for our needs; no improvements seen by increasing speed setting

    // this doesn't seem to make a difference, leaving it in
    furi_hal_gpio_init(&gpio_rfid_data_in, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_rfid_data_in, false);

    // false->ground RFID antenna; true->don't ground
    // skotopes (RFID dev) say normally you'd want RFID_PULL in high for signal forming, while modulating RFID_OUT
    // dunaevai135 had it low in their old code. Leaving low, as it doesn't seem to make a difference on my janky antenna
    furi_hal_gpio_init(&gpio_nfc_irq_rfid_pull, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_nfc_irq_rfid_pull, false);

    furi_hal_gpio_init(&gpio_rfid_carrier_out, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

    // A7 GPIO pin for debugging purposes
    // furi_hal_gpio_init(&gpio_ext_pa7, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

    // TODO: initialize pins on scene enter, perhaps, so as to avoid this delay each time the button is pressed?
    // Also, why is such a long delay needed?
    furi_delay_ms(300);

    // prevents interrupts &c. from impacting critical timings
    FURI_CRITICAL_ENTER();

    const uint8_t bitlen[] = {7, 5, 5};
    const int sublen[] = {32, 48, 48};
    int tmp, crc, lrc = 0;
    magspoof_bit_dir = 0;

    // First put out a bunch of leading zeros.
    for(uint8_t i = 0; i < 25; i++) {
        play_bit(0);
    }

    for(uint8_t i = 0; data[i] != '\0'; i++) {
        crc = 1;
        tmp = data[i] - sublen[track];

        for(uint8_t j = 0; j < bitlen[track] - 1; j++) {
            crc ^= tmp & 1;
            lrc ^= (tmp & 1) << j;
            play_bit(tmp & 1);
            tmp >>= 1;
        }
        play_bit(crc);
    }

    // finish calculating and send last "byte" (LRC)
    tmp = lrc;
    crc = 1;
    for(uint8_t j = 0; j < bitlen[track] - 1; j++) {
        crc ^= tmp & 1;
        play_bit(tmp & 1);
        tmp >>= 1;
    }
    play_bit(crc);

    // finish with 0's
    for(uint8_t i = 0; i < 5 * 5; i++) {
        play_bit(0);
    }

    gpio_item_set_rfid_pin(PIN_A, 0);
    gpio_item_set_rfid_pin(PIN_B, 0);
    // NFC TEST
    //furi_hal_nfc_ll_txrx_off();

    // end critical timing section
    FURI_CRITICAL_EXIT();

    // NFC TEST
    //furi_hal_nfc_start_sleep();

    furi_hal_rfid_pins_reset();
    furi_hal_power_disable_otg();
}

void mag_scene_emulate_test_on_enter(void* context) {
    Mag* mag = context;
    Widget* widget = mag->widget;

    //FuriString *tmp_string;
    //tmp_string = furi_string_alloc();

    widget_add_button_element(widget, GuiButtonTypeLeft, "Back", mag_widget_callback, mag);
    widget_add_button_element(widget, GuiButtonTypeRight, "Emulate", mag_widget_callback, mag);
    //widget_add_button_element(widget, GuiButtonTypeRight, "Re", mag_widget_callback, mag);
    //widget_add_button_element(widget, GuiButtonTypeCenter, "Two", mag_widget_callback, mag);

    //furi_string_printf(tmp_string, test_str);
    //widget_add_string_element(
    //    widget, 64, 0, AlignLeft, AlignTop, FontSecondary, furi_string_get_cstr(tmp_string));
    //furi_string_reset(tmp_string);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewWidget);
    //furi_string_free(tmp_string);
}

bool mag_scene_emulate_test_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeRight) {
            consumed = true;

            // Hardcoding a test string for the time being, while we debug/improve LF RFID TX
            FuriString* v = furi_string_alloc();
            furi_string_set_str(v, test_str);

            // blink led while spoofing
            notification_message(mag->notifications, &sequence_blink_start_cyan);
            mag_spoof_test(v, TEST_TRACK);
            // mag_spoof_single_track_rfid(v, TEST_TRACK);
            notification_message(mag->notifications, &sequence_blink_stop);

            furi_string_free(v);
        } else if(event.event == GuiButtonTypeLeft) {
            consumed = true;

            scene_manager_previous_scene(scene_manager);
        }
    }

    return consumed;
}

void mag_scene_emulate_test_on_exit(void* context) {
    Mag* mag = context;

    notification_message(mag->notifications, &sequence_blink_stop);
    widget_reset(mag->widget);
}
