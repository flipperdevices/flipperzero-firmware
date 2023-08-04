#include "../ublox_i.h"

void ublox_scene_about_on_enter(void* context) {
    Ublox* ublox = context;

    FuriString* s = furi_string_alloc();
    widget_add_text_box_element(
        ublox->widget,
        0,
        0,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!                                                      \e!\n",
        false);
    widget_add_text_box_element(
        ublox->widget,
        0,
        0,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!          u-blox GPS         \e!\n",
        false);

    furi_string_cat_printf(s, "\e#%s\n", "Information");
    furi_string_cat_printf(s, "Version: %s\n", UBLOX_VERSION_APP);
    furi_string_cat_printf(s, "Developed by: %s\n", UBLOX_DEVELOPED);
    furi_string_cat_printf(s, "GitHub: %s\n", UBLOX_GITHUB);

    furi_string_cat_printf(s, "\e#%s\n", "Description");
    furi_string_cat_printf(
        s,
        "This app is a multi-purpose tool for u-blox GPS modules connected over I2C."
        " It is compatible with 8 and 9 series GPS units, and probably other models,"
        " sold by Sparkfun and other vendors.\n");

    furi_string_cat_printf(s, "\e#%s\n", "Usage");
    furi_string_cat_printf(
        s,
        "Data Display shows GPS data. You can enable logging to a KML file to be"
        " viewed in a map program.\n"
        "Sync Time to GPS will sync the Flipper's RTC to the GPS. Note that this"
        " may be up to one second off, because there is no PPS signal connected.");

    widget_add_text_scroll_element(ublox->widget, 0, 16, 128, 50, furi_string_get_cstr(s));

    furi_string_free(s);

    view_dispatcher_switch_to_view(ublox->view_dispatcher, UbloxViewWidget);
}

bool ublox_scene_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void ublox_scene_about_on_exit(void* context) {
    furi_assert(context);
    Ublox* ublox = context;

    widget_reset(ublox->widget);
}
