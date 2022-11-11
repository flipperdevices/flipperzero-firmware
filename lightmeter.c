#include "lightmeter.h"
#include "lightmeter_helper.h"

#define WORKER_TAG "Main app"

// static const int iso_numbers[] = {
//     [ISO_6] = 6,
//     [ISO_12] = 12,
//     [ISO_25] = 25,
//     [ISO_50] = 50,
//     [ISO_100] = 100,
//     [ISO_200] = 200,
//     [ISO_400] = 400,
//     [ISO_800] = 800,
//     [ISO_1600] = 1600,
//     [ISO_3200] = 3200,
//     [ISO_6400] = 6400,
//     [ISO_12800] = 12800,
//     [ISO_25600] = 25600,
//     [ISO_51200] = 51200,
//     [ISO_102400] = 102400,
// };

// static const int nd_numbers[] = {
//     [ND_0] = 0,
//     [ND_2] = 2,
//     [ND_4] = 4,
//     [ND_8] = 8,
//     [ND_16] = 16,
//     [ND_32] = 32,
//     [ND_64] = 64,
//     [ND_128] = 128,
//     [ND_256] = 256,
//     [ND_512] = 512,
//     [ND_1024] = 1024,
//     [ND_2048] = 2048,
//     [ND_4096] = 4096,
// };

// static const float aperture_numbers[] = {
//     [AP_1] = 1.0,
//     [AP_1_4] = 1.4,
//     [AP_2] = 2.0,
//     [AP_2_8] = 2.8,
//     [AP_4] = 4.0,
//     [AP_5_6] = 5.6,
//     [AP_8] = 8,
//     [AP_11] = 11,
//     [AP_16] = 16,
//     [AP_22] = 22,
//     [AP_32] = 32,
//     [AP_45] = 45,
//     [AP_64] = 64,
//     [AP_90] = 90,
//     [AP_128] = 128,
// };

// static const float speed_numbers[] = {
//     [SPEED_8000] = 1.0 / 8000, [SPEED_4000] = 1.0 / 4000, [SPEED_2000] = 1.0 / 2000,
//     [SPEED_1000] = 1.0 / 1000, [SPEED_500] = 1.0 / 500,   [SPEED_250] = 1.0 / 250,
//     [SPEED_125] = 1.0 / 125,   [SPEED_60] = 1.0 / 60,     [SPEED_30] = 1.0 / 30,
//     [SPEED_15] = 1.0 / 15,     [SPEED_8] = 1.0 / 8,       [SPEED_4] = 1.0 / 4,
//     [SPEED_2] = 1.0 / 2,       [SPEED_1S] = 1.0,          [SPEED_2S] = 2.0,
//     [SPEED_4S] = 4.0,          [SPEED_8S] = 8.0,          [SPEED_15S] = 15.0,
//     [SPEED_30S] = 30.0,
// };

static bool lightmeter_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    LightMeterApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool lightmeter_back_event_callback(void* context) {
    furi_assert(context);
    LightMeterApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void lightmeter_tick_event_callback(void* context) {
    furi_assert(context);
    LightMeterApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

LightMeterApp* lightmeter_app_alloc(uint32_t first_scene) {
    LightMeterApp* app = malloc(sizeof(LightMeterApp));

    // Sensor
    bh1750_set_power_state(1);
    bh1750_init();
    bh1750_set_mode(ONETIME_HIGH_RES_MODE);
    bh1750_set_mt_reg(100);

    // Set default values to config
    app->config = malloc(sizeof(LightMeterConfig));
    app->config->iso = DEFAULT_ISO;
    app->config->nd = DEFAULT_ND;
    app->config->aperture = DEFAULT_APERTURE;
    app->config->dome = DEFAULT_DOME;

    // Records
    app->gui = furi_record_open(RECORD_GUI);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    notification_message(
        app->notifications, &sequence_display_backlight_enforce_on); // force on backlight

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&lightmeter_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, lightmeter_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, lightmeter_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, lightmeter_tick_event_callback, furi_ms_to_ticks(200));
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->main_view = main_view_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, LightMeterAppViewMainView, main_view_get_view(app->main_view));

    // Set default values to main view from config
    main_view_set_iso(app->main_view, app->config->iso);
    main_view_set_nd(app->main_view, app->config->nd);
    main_view_set_aperture(app->main_view, app->config->aperture);
    main_view_set_speed(app->main_view, DEFAULT_SPEED);
    main_view_set_dome(app->main_view, app->config->dome);

    // Variable item list
    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        LightMeterAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    // Widget
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, LightMeterAppViewAbout, widget_get_view(app->widget));
    view_dispatcher_add_view(
        app->view_dispatcher, LightMeterAppViewHelp, widget_get_view(app->widget));

    // Set first scene
    scene_manager_next_scene(app->scene_manager, first_scene); //! this to switch
    return app;
}

void lightmeter_app_free(LightMeterApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, LightMeterAppViewMainView);
    main_view_free(app->main_view);

    // Variable item list
    view_dispatcher_remove_view(app->view_dispatcher, LightMeterAppViewVarItemList);
    variable_item_list_free(app->var_item_list);

    //  Widget
    view_dispatcher_remove_view(app->view_dispatcher, LightMeterAppViewAbout);
    view_dispatcher_remove_view(app->view_dispatcher, LightMeterAppViewHelp);
    widget_free(app->widget);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Records
    furi_record_close(RECORD_GUI);
    notification_message(
        app->notifications,
        &sequence_display_backlight_enforce_auto); // set backlight back to auto
    furi_record_close(RECORD_NOTIFICATION);

    bh1750_set_power_state(0);

    free(app);
}

int32_t lightmeter_app(void* p) {
    UNUSED(p);
    uint32_t first_scene = LightMeterAppSceneMain;
    LightMeterApp* app = lightmeter_app_alloc(first_scene);
    view_dispatcher_run(app->view_dispatcher);
    lightmeter_app_free(app);
    return 0;
}

void lightmeter_app_set_config(LightMeterApp* context, LightMeterConfig* config) {
    LightMeterApp* app = context;
    app->config = config;
}

void lightmeter_app_i2c_callback(LightMeterApp* context) {
    LightMeterApp* app = context;

    float EV = 0;
    float lux = 0;
    bool response = 0;

    if(bh1750_trigger_manual_conversion() == BH1750_OK) response = 1;

    if(response) {
        bh1750_read_light(&lux);

        if(main_view_get_dome(app->main_view)) lux *= DOME_COEFFICIENT;

        EV = lux2ev(lux);
    }

    main_view_set_lux(app->main_view, lux);
    main_view_set_EV(app->main_view, EV);
    main_view_set_response(app->main_view, response);
}
