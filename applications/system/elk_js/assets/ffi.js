let record = ({
    open: function (name) { return fficall(arg.pointer, "furi_record_open", arg.string, name); },
    close: function (name) { return fficall(arg.none, "furi_record_close", arg.string, name); },
});

let notification = ({
    name: "notification",
    message: function (record, sequence) {
        return fficall(arg.none, "notification_message", arg.pointer, record, arg.pointer, sequence);
    },

    audiovisual_alert: function () { return ffires("sequence_audiovisual_alert"); },
    blink_blue_10: function () { return ffires("sequence_blink_blue_10"); },
    blink_blue_100: function () { return ffires("sequence_blink_blue_100"); },
    blink_cyan_10: function () { return ffires("sequence_blink_cyan_10"); },
    blink_cyan_100: function () { return ffires("sequence_blink_cyan_100"); },
    blink_green_10: function () { return ffires("sequence_blink_green_10"); },
    blink_green_100: function () { return ffires("sequence_blink_green_100"); },
    blink_magenta_10: function () { return ffires("sequence_blink_magenta_10"); },
    blink_magenta_100: function () { return ffires("sequence_blink_magenta_100"); },
    blink_red_10: function () { return ffires("sequence_blink_red_10"); },
    blink_red_100: function () { return ffires("sequence_blink_red_100"); },
    blink_start_blue: function () { return ffires("sequence_blink_start_blue"); },
    blink_start_cyan: function () { return ffires("sequence_blink_start_cyan"); },
    blink_start_green: function () { return ffires("sequence_blink_start_green"); },
    blink_start_magenta: function () { return ffires("sequence_blink_start_magenta"); },
    blink_start_red: function () { return ffires("sequence_blink_start_red"); },
    blink_start_yellow: function () { return ffires("sequence_blink_start_yellow"); },
    blink_stop: function () { return ffires("sequence_blink_stop"); },
    blink_white_100: function () { return ffires("sequence_blink_white_100"); },
    blink_yellow_10: function () { return ffires("sequence_blink_yellow_10"); },
    blink_yellow_100: function () { return ffires("sequence_blink_yellow_100"); },
    charged: function () { return ffires("sequence_charged"); },
    charging: function () { return ffires("sequence_charging"); },
    display_backlight_enforce_auto: function () { return ffires("sequence_display_backlight_enforce_auto"); },
    display_backlight_enforce_on: function () { return ffires("sequence_display_backlight_enforce_on"); },
    display_backlight_off: function () { return ffires("sequence_display_backlight_off"); },
    display_backlight_off_delay_1000: function () { return ffires("sequence_display_backlight_off_delay_1000"); },
    display_backlight_on: function () { return ffires("sequence_display_backlight_on"); },
    double_vibro: function () { return ffires("sequence_double_vibro"); },
    error: function () { return ffires("sequence_error"); },
    not_charging: function () { return ffires("sequence_not_charging"); },
    reset_blue: function () { return ffires("sequence_reset_blue"); },
    reset_display: function () { return ffires("sequence_reset_display"); },
    reset_green: function () { return ffires("sequence_reset_green"); },
    reset_red: function () { return ffires("sequence_reset_red"); },
    reset_rgb: function () { return ffires("sequence_reset_rgb"); },
    reset_sound: function () { return ffires("sequence_reset_sound"); },
    reset_vibro: function () { return ffires("sequence_reset_vibro"); },
    set_blue_255: function () { return ffires("sequence_set_blue_255"); },
    set_green_255: function () { return ffires("sequence_set_green_255"); },
    set_only_blue_255: function () { return ffires("sequence_set_only_blue_255"); },
    set_only_green_255: function () { return ffires("sequence_set_only_green_255"); },
    set_only_red_255: function () { return ffires("sequence_set_only_red_255"); },
    set_red_255: function () { return ffires("sequence_set_red_255"); },
    set_vibro_on: function () { return ffires("sequence_set_vibro_on"); },
    single_vibro: function () { return ffires("sequence_single_vibro"); },
    solid_yellow: function () { return ffires("sequence_solid_yellow"); },
    success: function () { return ffires("sequence_success"); },
});

let notification_app = record.open(notification.name);
notification.message(notification_app, notification.success());
record.close(notification.name);