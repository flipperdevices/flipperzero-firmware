#include "../wiegand.h"

void wiegand_instructions_scene_on_enter(void* context) {
    App* app = context;
    widget_reset(app->widget);
    widget_add_string_element(app->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "Instructions");
    widget_add_text_scroll_element(
        app->widget,
        0,
        15,
        128,
        40,
        "Only use on devices you own!\n"
        "Connect D0 (Green) to pin A4\n"
        "Connect D1 (White) to pin A7\n"
        "Connect GND (Black) to GND\n"
        "Add a 10K inline resistor on D0\n"
        "between keypad and door.\n"
        "Connect Flipper on door\n"
        "side. Add 10K inline resistor\n"
        "on D1 between keypad and\n"
        "door. Connect Flipper on\n"
        "door side. Do not inturrupt\n"
        "the D0/D1 connections while\n"
        "adding the resistors.");
    view_dispatcher_switch_to_view(app->view_dispatcher, WiegandWidgetView);
}
