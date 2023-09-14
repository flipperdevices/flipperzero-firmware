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
        "Version 1.2\n"
        "Only use on devices you own!\n"
        "Connect D0 (Green) to pin A4\n"
        "Connect D1 (White) to pin A7\n"
        "Connect GND (Black) to GND\n"
        "\nOption 1 (no mosfet):\n"
        "Add a 10K inline resistor on D0\n"
        "between keypad and door.\n"
        "Connect Flipper on door\n"
        "side. Add 10K inline resistor\n"
        "on D1 between keypad and\n"
        "door. Connect Flipper on\n"
        "door side. Do not inturrupt\n"
        "the D0/D1 connections while\n"
        "adding the resistors."
        "\n\nOption 2 (mosfet):\n"
        "Connect pin A6 to gate of\n"
        "mosfet for D1. Connect 5K\n"
        "resistor from gate to GND.\n"
        "Connect pin A7 to drain of\n"
        "mosfet for D1. Connect source\n"
        "of mosfet for D1 to GND.\n"
        "Connect pin B3 to gate of\n"
        "mosfet for D0. Connect 5K\n"
        "resistor from gate to GND.\n"
        "Connect pin A4 to drain of\n"
        "mosfet for D0. Connect\n"
        "source of mosfet for D0 to\n"
        "GND.\n");
    view_dispatcher_switch_to_view(app->view_dispatcher, WiegandWidgetView);
}
