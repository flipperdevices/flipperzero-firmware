#include "../spi_mem_app_i.h"

static void spi_mem_scene_chip_detected_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    SPIMemApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

static void spi_mem_scene_chip_detected_print_chip_info(Widget* widget, SPIMemChip* chip_info) {
    FuriString* tmp_string = furi_string_alloc();
    widget_add_string_element(
        widget, 40, 12, AlignLeft, AlignTop, FontSecondary, chip_info->vendor_name);
    widget_add_string_element(
        widget, 40, 20, AlignLeft, AlignTop, FontSecondary, chip_info->model_name);
    furi_string_printf(tmp_string, "Size: %u KB", chip_info->size / 1024);
    widget_add_string_element(
        widget, 40, 28, AlignLeft, AlignTop, FontSecondary, furi_string_get_cstr(tmp_string));
    furi_string_free(tmp_string);
}

void spi_mem_scene_chip_detected_on_enter(void* context) {
    SPIMemApp* app = context;
    widget_add_button_element(
        app->widget, GuiButtonTypeLeft, "Retry", spi_mem_scene_chip_detected_widget_callback, app);
    widget_add_button_element(
        app->widget, GuiButtonTypeRight, "Read", spi_mem_scene_chip_detected_widget_callback, app);
    widget_add_icon_element(app->widget, 0, 12, &I_Dip8_32x36);
    widget_add_string_element(
        app->widget, 64, 9, AlignCenter, AlignBottom, FontPrimary, "Detected SPI chip");
    spi_mem_scene_chip_detected_print_chip_info(app->widget, app->chip_info);
    view_dispatcher_switch_to_view(app->view_dispatcher, SPIMemViewWidget);
}

bool spi_mem_scene_chip_detected_on_event(void* context, SceneManagerEvent event) {
    SPIMemApp* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeBack) {
        success = true;
        scene_manager_search_and_switch_to_previous_scene(app->scene_manager, SPIMemSceneStart);
    } else if(event.type == SceneManagerEventTypeCustom) {
        success = true;
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_previous_scene(app->scene_manager);
        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(app->scene_manager, SPIMemSceneReadFilename);
        }
    }
    return success;
}
void spi_mem_scene_chip_detected_on_exit(void* context) {
    SPIMemApp* app = context;
    widget_reset(app->widget);
}
