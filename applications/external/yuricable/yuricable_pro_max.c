#include <yuricable_pro_max.h>
#include <yuricable_pro_max_icons.h>

#define BACKLIGHT_ON 1
#define TAG "YURICABLE_PRO_MAX"
#define SDQ_PIN gpio_ext_pa7 // GPIO 2

const char* yuricable_get_submenu_title_string(YuriCableProMaxSubmenuTitles title) {
    if(title < YuriCableProMaxSubmenuTitlesCount) {
        return YuriCableProMaxSubmenuTitlesStrings[title];
    } else {
        return "Unknown";
    }
}

static void yuricable_battery_info_update_model(void* ctx) {
    furi_assert(ctx);
    App* app = ctx;
    power_get_info(app->power, &app->info);
}

static void yuricable_draw_battery_consumption(Widget* widget, double cons) {
    FuriString* string = furi_string_alloc_set("------------------------");
    if(cons >= 0) {
        furi_string_printf(string, "-- Battery Utilization");
    } else if(cons < -1) {
        furi_string_printf(string, "%1.1fk Battery Utilization", -cons);
    } else {
        furi_string_printf(string, "%3.f%% Battery Utilization", -(cons * 1000));
    }
    widget_add_string_element(
        widget, 20, 45, AlignLeft, AlignCenter, FontSecondary, furi_string_get_cstr(string));
    furi_string_free(string);
}

static int32_t yuricable_led_worker(void* ctx) {
    furi_assert(ctx);
    App* app = (App*)ctx;
    while(1) {
        uint32_t events = furi_thread_flags_get();
        if(events & WorkerEvtStop) {
            break;
        }
        if(app->data->sdq->listening) {
            if(app->data->sdq->resetInProgress) {
                furi_hal_light_sequence("rgb G.g.G");
            } else if(!app->data->sdq->connected) {
                furi_hal_light_sequence("rgb B.b.B");
            } else if(app->data->sdq->error != SDQDeviceErrorNone) {
                furi_hal_light_sequence("rgb R.r.R");
            }
        } else if(app->data->ledMainMenu) {
            furi_hal_light_set(LightRed, 255);
            furi_hal_light_set(LightGreen, 0);
            furi_hal_light_set(LightBlue, 255);
        } else if(app->data->sdq->commandExecuted) {
            furi_hal_light_set(LightRed, 0);
            furi_hal_light_set(LightGreen, 255);
            furi_hal_light_set(LightBlue, 0);
        } else {
            furi_hal_light_set(LightRed, 255);
            furi_hal_light_set(LightGreen, 0);
            furi_hal_light_set(LightBlue, 255);
        }
    }
    return 0;
}

FuriString* yuricable_command_callback(char* command, void* ctx) {
    furi_assert(ctx);
    App* yuricable_context = ctx;
    UNUSED(yuricable_context);
    if(strcmp(command, "start") == 0) {
        if(yuricable_context->data->sdq->listening) {
            return furi_string_alloc_printf("already listening");
        }
        sdq_device_start(yuricable_context->data->sdq);
        icon_animation_start(yuricable_context->data->listeningAnimation);
        return furi_string_alloc_printf("started");
    }
    if(strcmp(command, "stop") == 0) {
        if(!yuricable_context->data->sdq->listening) {
            return furi_string_alloc_printf("already stopped");
        }
        sdq_device_stop(yuricable_context->data->sdq);
        icon_animation_stop(yuricable_context->data->listeningAnimation);
        return furi_string_alloc_printf("stopped");
    }
    if(strncmp(command, "mode", 4) == 0) {
        if(command[4] == ' ') {
            char* mode = command + 5;
            if(strcmp(mode, "dfu") == 0) {
                yuricable_context->data->sdq->runCommand = SDQDeviceCommand_DFU;
                yuricable_context->data->sdq->commandExecuted = false;
                return furi_string_alloc_printf("set mode dfu");
            }
            if(strcmp(mode, "reset") == 0) {
                yuricable_context->data->sdq->runCommand = SDQDeviceCommand_RESET;
                yuricable_context->data->sdq->commandExecuted = false;
                return furi_string_alloc_printf("set mode reset");
            }
            if(strcmp(mode, "dcsd") == 0) {
                yuricable_context->data->sdq->runCommand = SDQDeviceCommand_DCSD;
                yuricable_context->data->sdq->commandExecuted = false;
                return furi_string_alloc_printf("set mode dcsd");
            }
        }
        return furi_string_alloc_printf("use: /mode <dfu | reset | dcsd>");
    }
    if(strncmp(command, "help", 4) == 0) {
        return furi_string_alloc_printf(
            "commands:\r\n/start\r\n/stop\r\n/mode <dfu | reset | dcsd>");
    }
    return furi_string_alloc_printf("%s is no valid command", command);
}

void yuricable_menu_callback(void* ctx, uint32_t index) {
    furi_assert(ctx);
    App* app = ctx;
    switch(index) {
    case YuriCableProMaxMainMenuSceneDCSD:
        scene_manager_handle_custom_event(
            app->scene_manager, YuriCableProMaxMainMenuSceneDCSDModeEvent);
        break;
    case YuriCableProMaxMainMenuSceneReset:
        scene_manager_handle_custom_event(
            app->scene_manager, YuriCableProMaxMainMenuSceneResetModeEvent);
        break;
    case YuriCableProMaxMainMenuSceneDFU:
        scene_manager_handle_custom_event(
            app->scene_manager, YuriCableProMaxMainMenuSceneDFUModeEvent);
        break;
    case YuriCableProMaxMainMenuSceneCharging:
        scene_manager_handle_custom_event(
            app->scene_manager, YuriCableProMaxMainMenuSceneChargingModeEvent);
        break;
    }
}

void yuricable_main_menu_scene_on_enter(void* ctx) {
    furi_assert(ctx);
    App* app = ctx;
    submenu_reset(app->submenu);
    submenu_set_header(
        app->submenu, yuricable_get_submenu_title_string(YuriCableProMaxMainMenuTitle));
    submenu_add_item(
        app->submenu,
        yuricable_get_submenu_title_string(YuriCableProMaxDCSDSubmenuTitle),
        YuriCableProMaxMainMenuSceneDCSD,
        yuricable_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        yuricable_get_submenu_title_string(YuriCableProMaxResetSubmenuTitle),
        YuriCableProMaxMainMenuSceneReset,
        yuricable_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        yuricable_get_submenu_title_string(YuriCableProMaxDFUSubmenuTitle),
        YuriCableProMaxMainMenuSceneDFU,
        yuricable_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        yuricable_get_submenu_title_string(YuriCableProMaxChargingSubmenuTitle),
        YuriCableProMaxMainMenuSceneCharging,
        yuricable_menu_callback,
        app);
    view_dispatcher_switch_to_view(app->view_dispatcher, YuriCableProMaxSubmenuView);
}

bool yuricable_main_menu_scene_on_event(void* ctx, SceneManagerEvent event) {
    furi_assert(ctx);
    App* app = ctx;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case YuriCableProMaxMainMenuSceneDCSDModeEvent:
            app->data->sdq->runCommand = SDQDeviceCommand_DCSD;
            app->data->selectedSubmenu = YuriCableProMaxDCSDSubmenuTitle;
            scene_manager_next_scene(app->scene_manager, YuriCableProMaxDCSDScene);
            consumed = true;
            break;
        case YuriCableProMaxMainMenuSceneResetModeEvent:
            app->data->sdq->runCommand = SDQDeviceCommand_RESET;
            app->data->selectedSubmenu = YuriCableProMaxResetSubmenuTitle;
            scene_manager_next_scene(app->scene_manager, YuriCableProMaxResetScene);
            consumed = true;
            break;
        case YuriCableProMaxMainMenuSceneDFUModeEvent:
            app->data->sdq->runCommand = SDQDeviceCommand_DFU;
            app->data->selectedSubmenu = YuriCableProMaxDFUSubmenuTitle;
            scene_manager_next_scene(app->scene_manager, YuriCableProMaxDFUScene);
            consumed = true;
            break;
        case YuriCableProMaxMainMenuSceneChargingModeEvent:
            app->data->sdq->runCommand = SDQDeviceCommand_CHARGING;
            app->data->selectedSubmenu = YuriCableProMaxChargingSubmenuTitle;
            scene_manager_next_scene(app->scene_manager, YuriCableProMaxCharginScene);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    app->data->ledMainMenu = false;
    return consumed;
}

static int32_t yuricable_update_battery_consumption_worker(void* ctx) {
    furi_assert(ctx);
    App* app = ctx;
    while(1) {
        uint32_t events = furi_thread_flags_get();
        if(events & WorkerEvtStop) {
            break;
        }
        float consumption = app->info.current_gauge;
        widget_reset(app->widget);
        widget_add_string_element(
            app->widget,
            25,
            15,
            AlignLeft,
            AlignCenter,
            FontPrimary,
            yuricable_get_submenu_title_string(app->data->selectedSubmenu));
        widget_add_string_element(
            app->widget, 15, 30, AlignLeft, AlignCenter, FontSecondary, "Connect an iPhone now!");
        yuricable_draw_battery_consumption(app->widget, (double)consumption);
        widget_add_string_element(app->widget, 15, 45, AlignLeft, AlignCenter, FontSecondary, "+");
        yuricable_battery_info_update_model(app);
        furi_delay_ms(500);
    }
    return 0;
}

void yuricable_sdq_scene_on_enter(void* ctx) {
    furi_assert(ctx);
    App* app = ctx;
    widget_reset(app->widget);
    sdq_device_start(app->data->sdq);
    widget_add_string_element(
        app->widget,
        25,
        15,
        AlignLeft,
        AlignCenter,
        FontPrimary,
        yuricable_get_submenu_title_string(app->data->selectedSubmenu));
    widget_add_string_element(
        app->widget, 15, 30, AlignLeft, AlignCenter, FontSecondary, "Connect an iPhone now!");
    if(app->data->sdq->runCommand == SDQDeviceCommand_CHARGING &&
       !furi_hal_power_check_otg_fault()) {
        furi_hal_power_enable_otg();
        furi_hal_power_insomnia_enter();
        float consumption = app->info.current_gauge;
        yuricable_draw_battery_consumption(app->widget, (double)consumption);
        widget_add_string_element(app->widget, 15, 45, AlignLeft, AlignCenter, FontSecondary, "+");
        furi_thread_start(app->battery_info_update_thread);
    }
    view_dispatcher_switch_to_view(app->view_dispatcher, YuriCableProMaxWidgetView);
}

bool yuricable_sdq_scene_on_event(void* ctx, SceneManagerEvent event) {
    UNUSED(ctx);
    UNUSED(event);
    return false; // event not handled.
}

void yuricable_sdq_scene_on_exit(void* ctx) {
    furi_assert(ctx);
    App* app = ctx;
    if(app->data->sdq->listening) {
        sdq_device_stop(app->data->sdq);
    }
    if(app->data->sdq->runCommand == SDQDeviceCommand_CHARGING &&
       furi_hal_power_is_otg_enabled()) {
        furi_hal_power_insomnia_exit();
        furi_hal_power_disable_otg();
        furi_thread_flags_set(furi_thread_get_id(app->battery_info_update_thread), WorkerEvtStop);
    }
    app->data->selectedSubmenu = YuriCableProMaxMainMenuTitle;
    app->data->sdq->commandExecuted = false;
    app->data->ledMainMenu = true;
    UNUSED(ctx);
}

void yuricable_main_menu_scene_on_exit(void* ctx) {
    furi_assert(ctx);
    App* app = ctx;
    submenu_reset(app->submenu);
}

void (*const yuricable_scene_on_enter_handlers[])(void*) = {
    yuricable_main_menu_scene_on_enter,
    yuricable_sdq_scene_on_enter,
    yuricable_sdq_scene_on_enter,
    yuricable_sdq_scene_on_enter,
    yuricable_sdq_scene_on_enter};

bool (*const yuricable_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    yuricable_main_menu_scene_on_event,
    yuricable_sdq_scene_on_event,
    yuricable_sdq_scene_on_event,
    yuricable_sdq_scene_on_event,
    yuricable_sdq_scene_on_event};

void (*const yuricable_scene_on_exit_handlers[])(void*) = {
    yuricable_main_menu_scene_on_exit,
    yuricable_sdq_scene_on_exit,
    yuricable_sdq_scene_on_exit,
    yuricable_sdq_scene_on_exit,
    yuricable_sdq_scene_on_exit};

static const SceneManagerHandlers yuricable_scene_manager_handlers = {
    .on_enter_handlers = yuricable_scene_on_enter_handlers,
    .on_event_handlers = yuricable_scene_on_event_handlers,
    .on_exit_handlers = yuricable_scene_on_exit_handlers,
    .scene_num = YuriCableProMaxSceneCount,
};

static bool yuricable_custom_callback(void* ctx, uint32_t custom_event) {
    furi_assert(ctx);
    App* app = ctx;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool yuricable_back_event_callback(void* ctx) {
    furi_assert(ctx);
    App* app = ctx;
    return scene_manager_handle_back_event(app->scene_manager);
}

App* app_alloc() {
    App* app = malloc(sizeof(App));
    // Initialize LED Worker Thread
    app->led_thread = furi_thread_alloc_ex("LEDWorker", 1024, yuricable_led_worker, app);
    app->battery_info_update_thread = furi_thread_alloc_ex(
        "UpdateBatteryConsumptionWorker", 1024, yuricable_update_battery_consumption_worker, app);
    // Initialize YuriCableContext
    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    app->data = malloc(sizeof(YuriCableData));
    app->data->listeningAnimation = icon_animation_alloc(&A_Round_loader_8x8);
    app->data->ledMainMenu = true;
    app->data->ledSequenceCommandExecutedPlayed = false;
    app->queue = furi_message_queue_alloc(8, sizeof(Event));
    // Initialize USB UART bridge
    UsbUartConfig bridgeConfig = {
        .vcp_ch = 1, .uart_ch = 0, .baudrate_mode = 0, .baudrate = 115200};
    UsbUartBridge* uartBridge = usb_uart_enable(&bridgeConfig);
    usb_uart_set_command_callback(uartBridge, yuricable_command_callback, app);
    // Initialize SDQ
    app->data->sdq = sdq_device_alloc(&SDQ_PIN, uartBridge);
    app->data->sdq->runCommand = SDQDeviceCommand_NONE;
    app->data->selectedSubmenu = YuriCableProMaxMainMenuTitle;
    // Initialize SceneManager and Gui
    app->scene_manager = scene_manager_alloc(&yuricable_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, yuricable_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, yuricable_back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, YuriCableProMaxSubmenuView, submenu_get_view(app->submenu));
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, YuriCableProMaxWidgetView, widget_get_view(app->widget));
    app->gui = furi_record_open(RECORD_GUI);
    app->power = furi_record_open(RECORD_POWER);
    return app;
}

void app_free(App* app) {
    furi_assert(app);
    furi_hal_light_set(LightRed, 0);
    furi_hal_light_set(LightBlue, 0);
    furi_hal_light_set(LightGreen, 0);
    // Free LED and Batter Info Update Thread
    furi_thread_flags_set(furi_thread_get_id(app->led_thread), WorkerEvtStop);
    furi_thread_join(app->led_thread);
    furi_thread_join(app->battery_info_update_thread);
    furi_thread_free(app->led_thread);
    furi_thread_free(app->battery_info_update_thread);
    // Free Power Util
    furi_record_close(RECORD_POWER);
    // Free Gui
    furi_record_close(RECORD_GUI);
    view_dispatcher_remove_view(app->view_dispatcher, YuriCableProMaxSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, YuriCableProMaxWidgetView);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    // Free SDQ
    sdq_device_free(app->data->sdq);
    icon_animation_free(app->data->listeningAnimation);
    free(app->data);
    // Free App
    furi_mutex_free(app->mutex);
    furi_message_queue_free(app->queue);
    free(app);
}

int32_t yuricable_pro_max_app(void* p) {
    UNUSED(p);
    FURI_LOG_I(TAG, "Starting YuriCable Pro Max");
    // Alloc App Struct
    App* app = app_alloc();
    // Start LED Worker
    furi_thread_start(app->led_thread);
    // Start Gui
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, YuriCableProMaxMainMenuScene);
    view_dispatcher_run(app->view_dispatcher);
    // Free and Quit
    app_free(app);
    return 0;
}
