#include "gb_cartridge_app.h"

bool gb_cartridge_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    GBCartridge* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void gb_cartridge_app_tick_event_callback(void* context) {
    furi_assert(context);
    GBCartridge* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool gb_cartridge_app_navigation_event_callback(void* context) {
    furi_assert(context);
    GBCartridge* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

GBCartridge* gb_cartridge_app_app_alloc() {
    GBCartridge* app = malloc(sizeof(GBCartridge));
    app->gui = furi_record_open(RECORD_GUI);
    app->storage = furi_record_open(RECORD_STORAGE);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    //  Create MALVEKE dir
    if(storage_common_stat(app->storage, MALVEKE_APP_FOLDER, NULL) == FSE_NOT_EXIST) {
        storage_simply_mkdir(app->storage, MALVEKE_APP_FOLDER);
    }
    //  Create MALVEKE RAM dir
    if(storage_common_stat(app->storage, MALVEKE_APP_FOLDER_RAMS, NULL) == FSE_NOT_EXIST) {
        storage_simply_mkdir(app->storage, MALVEKE_APP_FOLDER_RAMS);
    }
    //  Create MALVEKE ROM dir
    if(storage_common_stat(app->storage, MALVEKE_APP_FOLDER_ROMS, NULL) == FSE_NOT_EXIST) {
        storage_simply_mkdir(app->storage, MALVEKE_APP_FOLDER_ROMS);
    }
    //  Create MALVEKE Photos dir
    if(storage_common_stat(app->storage, MALVEKE_APP_FOLDER_PHOTOS, NULL) == FSE_NOT_EXIST) {
        storage_simply_mkdir(app->storage, MALVEKE_APP_FOLDER_PHOTOS);
    }

    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&gb_cartridge_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, gb_cartridge_app_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, gb_cartridge_app_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, gb_cartridge_app_custom_event_callback);
    app->submenu = variable_item_list_alloc();

    // Set defaults, in case no config loaded
    app->gameboy_rom_option_selected_index = 0;
    app->gameboy_rom_option_selected_text = "gb";
    app->haptic = 1;
    app->speaker = 1;
    app->led = 1;
    app->save_settings = 1;

    app->cart_title = " - ";
    app->cart_dump_rom_filename = "malveke_rom";
    app->cart_dump_rom_extension = "gb";

    app->cart_dump_ram_filename = "malveke_ram";
    app->cart_dump_ram_extension = "sav";

    // Used for File Browser
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->file_path = furi_string_alloc();

    app->uart = usart_init(app);
    app->lp_uart = lp_uart_init(app);

    view_dispatcher_add_view(
        app->view_dispatcher, GBCartridgeViewIdMenu, variable_item_list_get_view(app->submenu));
    app->gb_cartridge_startscreen = gb_cartridge_startscreen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBCartridgeViewIdStartscreen,
        gb_cartridge_startscreen_get_view(app->gb_cartridge_startscreen));
    app->gb_cartridge_scene_1 = gb_cartridge_scene_1_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBCartridgeViewIdScene1,
        gb_cartridge_scene_1_get_view(app->gb_cartridge_scene_1));
    app->gb_cartridge_scene_2 = gb_cartridge_scene_2_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBCartridgeViewIdScene2,
        gb_cartridge_scene_2_get_view(app->gb_cartridge_scene_2));
    app->gb_cartridge_scene_3 = gb_cartridge_scene_3_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBCartridgeViewIdScene3,
        gb_cartridge_scene_3_get_view(app->gb_cartridge_scene_3));
    app->gb_cartridge_scene_4 = gb_cartridge_scene_4_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBCartridgeViewIdScene4,
        gb_cartridge_scene_4_get_view(app->gb_cartridge_scene_4));
    app->gb_cartridge_scene_5 = gb_cartridge_scene_5_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBCartridgeViewIdScene5,
        gb_cartridge_scene_5_get_view(app->gb_cartridge_scene_5));

    // app->button_menu = button_menu_alloc();
    // view_dispatcher_add_view(app->view_dispatcher, GBCartridgeViewIdScene3, button_menu_get_view(app->button_menu));

    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBCartridgeViewIdSettings,
        variable_item_list_get_view(app->variable_item_list));

    //End Scene Additions

    //  Enable 5v
    furi_hal_power_enable_otg();
    furi_delay_ms(1);
    furi_hal_power_insomnia_enter();
    return app;
}

void gb_cartridge_app_app_free(GBCartridge* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, GBCartridgeViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, GBCartridgeViewIdScene1);
    view_dispatcher_remove_view(app->view_dispatcher, GBCartridgeViewIdScene2);
    view_dispatcher_remove_view(app->view_dispatcher, GBCartridgeViewIdScene3);
    view_dispatcher_remove_view(app->view_dispatcher, GBCartridgeViewIdScene4);
    view_dispatcher_remove_view(app->view_dispatcher, GBCartridgeViewIdScene5);
    view_dispatcher_remove_view(app->view_dispatcher, GBCartridgeViewIdSettings);
    view_dispatcher_remove_view(app->view_dispatcher, GBCartridgeViewIdStartscreen);

    variable_item_list_free(app->submenu);

    // View Dispatcher
    view_dispatcher_free(app->view_dispatcher);
    // Scene manager
    scene_manager_free(app->scene_manager);

    //
    uart_free(app->uart);
    uart_free(app->lp_uart);

    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);

    app->gui = NULL;
    app->notification = NULL;
    app->storage = NULL;

    //Remove whatever is left
    free(app);
}

int32_t gb_cartridge_app(void* p) {
    UNUSED(p);
    // Disable expansion protocol to avoid interference with UART Handle
    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(expansion);
    // uint8_t attempts = 0;
    // while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
    //     furi_hal_power_enable_otg();
    //     furi_delay_ms(10);
    // }
    // furi_delay_ms(200);
    // furi_hal_power_disable_external_3_3v();
    // furi_delay_ms(100);
    // furi_hal_power_enable_external_3_3v();
    // furi_delay_ms(200);

    GBCartridge* app = gb_cartridge_app_app_alloc();

    // if user hasn't confirmed whether to save pcaps and logs to sdcard, then prompt when scene starts
    // app->need_to_prompt_settings_init =
    //     (!storage_file_exists(app->storage, SAVE_PCAP_SETTING_FILEPATH) ||
    //      !storage_file_exists(app->storage, SAVE_LOGS_SETTING_FILEPATH));

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(
        app->scene_manager, GBCartridgeSceneStartscreen); //Start with start screen
    //scene_manager_next_scene(app->scene_manager, GBCartridgeSceneMenu); //if you want to directly start with Menu
    // scene_manager_next_scene(app->scene_manager, GBCartridgeSceneScene_4);

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();
    gb_cartridge_app_app_free(app);

    if(furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
    }

    // Return previous state of expansion
    expansion_enable(expansion);
    furi_record_close(RECORD_EXPANSION);

    return 0;
}
