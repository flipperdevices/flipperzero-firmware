#include "weather_station_app_i.h"

#include <furi.h>
#include <furi_hal.h>

static bool weather_station_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    WeatherStationApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool weather_station_app_back_event_callback(void* context) {
    furi_assert(context);
    WeatherStationApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void weather_station_app_tick_event_callback(void* context) {
    furi_assert(context);
    WeatherStationApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

WeatherStationApp* weather_station_app_alloc() {
    WeatherStationApp* app = malloc(sizeof(WeatherStationApp));

    app->gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&weather_station_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, weather_station_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, weather_station_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, weather_station_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // app->var_item_list = variable_item_list_alloc();
    // view_dispatcher_add_view(
    //     app->view_dispatcher,
    //     TemperatureViewVarItemList,
    //     variable_item_list_get_view(app->var_item_list));

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, WeatherStationViewSubmenu, submenu_get_view(app->submenu));

    app->weather_station_show = weather_station_show_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        WeatherStationViewShow,
        weather_station_show_get_view(app->weather_station_show));

    //init Worker & Protocol
    app->txrx = malloc(sizeof(WeatherStationTxRx));
    app->txrx->preset = malloc(sizeof(WeatherStationPresetDefinition));
    string_init(app->txrx->preset->name);
    weather_station_init(app, "AM650", 433920000, FuriHalSubGhzPresetOok650Async);

    app->txrx->worker = subghz_worker_alloc();
    //app->txrx->fff_data = flipper_format_string_alloc();

    app->txrx->environment = subghz_environment_alloc();
    //todo set protocol_registry
    app->txrx->receiver = subghz_receiver_alloc_init(app->txrx->environment);
    subghz_receiver_set_filter(app->txrx->receiver, SubGhzProtocolFlag_Decodable);

    subghz_worker_set_overrun_callback(
        app->txrx->worker, (SubGhzWorkerOverrunCallback)subghz_receiver_reset);
    subghz_worker_set_pair_callback(
        app->txrx->worker, (SubGhzWorkerPairCallback)subghz_receiver_decode);
    subghz_worker_set_context(app->txrx->worker, app->txrx->receiver);

    scene_manager_next_scene(app->scene_manager, WeatherStationSceneStart);

    return app;
}

void weather_station_app_free(WeatherStationApp* app) {
    furi_assert(app);

    //CC1101 off
    weather_station_sleep(app);

    // Views
    // view_dispatcher_remove_view(app->view_dispatcher, WeatherStationViewVarItemList);
    view_dispatcher_remove_view(app->view_dispatcher, WeatherStationViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, WeatherStationViewShow);

    submenu_free(app->submenu);
    weather_station_show_free(app->weather_station_show);
    // variable_item_list_free(app->var_item_list);
    //weather_station_pwm_free(app->pwm_view);

    //Worker & Protocol
    subghz_receiver_free(app->txrx->receiver);
    subghz_environment_free(app->txrx->environment);
    subghz_worker_free(app->txrx->worker);
    //flipper_format_free(app->txrx->fff_data);
    //subghz_history_free(app->txrx->history);
    string_clear(app->txrx->preset->name);
    free(app->txrx->preset);
    free(app->txrx);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Close records
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t weather_station_app(void* p) {
    UNUSED(p);
    WeatherStationApp* weather_station_app = weather_station_app_alloc();

    view_dispatcher_run(weather_station_app->view_dispatcher);

    weather_station_app_free(weather_station_app);

    return 0;
}
