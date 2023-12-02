#include "../esp_flasher_app_i.h"

void esp_flasher_scene_about_widget_callback(GuiButtonType result, InputType type, void* context) {
    EspFlasherApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

#define ESP_FLASHER_APP_DESCRIPTION                     \
    "\e#Information\nVersion: " ESP_FLASHER_APP_VERSION \
    "\nDeveloped by: 0xchocolate\n(@cococode on discord) using espressif's esp-serial-flasher library\nGithub: https://github.com/0xchocolate/flipperzero-esp-flasher\n\n\e#Description\nApp to flash ESP chips from\nthe flipper (over UART) using\nbin files on the sd card.\nReset the chip into bootloader\nmode before flashing.\n\n Supported targets:\n- ESP32\n- ESP8266\n- ESP32-S2\n- ESP32-S3\n- ESP32-C3\n- ESP32-C2\n- ESP32-H2"

void esp_flasher_scene_about_on_enter(void* context) {
    EspFlasherApp* app = context;

    widget_add_text_box_element(
        app->widget,
        0,
        0,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!                                                      \e!\n",
        false);
    widget_add_text_box_element(
        app->widget,
        0,
        2,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!        ESP Flasher       \e!\n",
        false);
    widget_add_text_scroll_element(app->widget, 0, 16, 128, 50, ESP_FLASHER_APP_DESCRIPTION);

    view_dispatcher_switch_to_view(app->view_dispatcher, EspFlasherAppViewWidget);
}

bool esp_flasher_scene_about_on_event(void* context, SceneManagerEvent event) {
    EspFlasherApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);

    return consumed;
}

void esp_flasher_scene_about_on_exit(void* context) {
    EspFlasherApp* app = context;

    // Clear views
    widget_reset(app->widget);
}