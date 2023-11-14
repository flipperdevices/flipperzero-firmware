#include "esp_flasher_app_i.h"

#include <furi.h>
#include <furi_hal.h>

static bool esp_flasher_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    EspFlasherApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool esp_flasher_app_back_event_callback(void* context) {
    furi_assert(context);
    EspFlasherApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void esp_flasher_app_tick_event_callback(void* context) {
    furi_assert(context);
    EspFlasherApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

EspFlasherApp* esp_flasher_app_alloc() {
    EspFlasherApp* app = malloc(sizeof(EspFlasherApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->storage = furi_record_open(RECORD_STORAGE);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&esp_flasher_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, esp_flasher_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, esp_flasher_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, esp_flasher_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        EspFlasherAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    app->text_box = text_box_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, EspFlasherAppViewConsoleOutput, text_box_get_view(app->text_box));
    app->text_box_store = furi_string_alloc();
    furi_string_reserve(app->text_box_store, ESP_FLASHER_TEXT_BOX_STORE_SIZE);

    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, EspFlasherAppViewWidget, widget_get_view(app->widget));

    // Submenu
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, EspFlasherAppViewSubmenu, submenu_get_view(app->submenu));

    app->flash_worker_busy = false;

    app->reset = false;
    app->boot = false;

    scene_manager_next_scene(app->scene_manager, EspFlasherSceneStart);

    return app;
}

void esp_flasher_make_app_folder(EspFlasherApp* app) {
    furi_assert(app);

    if(!storage_simply_mkdir(app->storage, ESP_APP_FOLDER)) {
        dialog_message_show_storage_error(app->dialogs, "Cannot create\napp folder");
    }
}

void esp_flasher_app_free(EspFlasherApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, EspFlasherAppViewVarItemList);
    view_dispatcher_remove_view(app->view_dispatcher, EspFlasherAppViewConsoleOutput);
    view_dispatcher_remove_view(app->view_dispatcher, EspFlasherAppViewWidget);
    view_dispatcher_remove_view(app->view_dispatcher, EspFlasherAppViewSubmenu);

    widget_free(app->widget);
    text_box_free(app->text_box);
    furi_string_free(app->text_box_store);
    submenu_free(app->submenu);
    variable_item_list_free(app->var_item_list);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    esp_flasher_uart_free(app->uart);

    // Close records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_NOTIFICATION);

    free(app);
}

int32_t esp_flasher_app(void* p) {
    UNUSED(p);

    uint8_t attempts = 0;
    bool otg_was_enabled = furi_hal_power_is_otg_enabled();
    while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
        furi_hal_power_enable_otg();
        furi_delay_ms(10);
    }
    furi_delay_ms(200);

    EspFlasherApp* esp_flasher_app = esp_flasher_app_alloc();

    esp_flasher_make_app_folder(esp_flasher_app);

    esp_flasher_app->uart = esp_flasher_usart_init(esp_flasher_app);

    view_dispatcher_run(esp_flasher_app->view_dispatcher);

    esp_flasher_app_free(esp_flasher_app);

    if(furi_hal_power_is_otg_enabled() && !otg_was_enabled) {
        furi_hal_power_disable_otg();
    }

    return 0;
}
