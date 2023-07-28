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
        2,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!          u-blox GPS         \e!\n",
        false);

    furi_string_printf(s, "%s\n", "u-blox GPS");

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
