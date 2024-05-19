#include "../meal_pager_i.h"
#include "../helpers/meal_pager_custom_event.h"
#include "../helpers/meal_pager_led.h"
#include <dolphin/dolphin.h>

void meal_pager_set_first_station_callback(void* context) {
    furi_assert(context);
    Meal_Pager* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, Meal_PagerCustomerEventIntInput);
}

void meal_pager_scene_set_first_station_on_enter(void* context) {
    furi_assert(context);
    Meal_Pager* app = context;
    IntInput* int_input = app->int_input;
    size_t enter_name_length = 5;
    meal_pager_set_max_values(app);
    char* str = "Set First Station (0 - 9999)";
    const char* constStr = str;
    snprintf(str, 36, "Set First Station (0 - %lu)", app->max_station);

    int_input_set_header_text(int_input, constStr);

    int_input_set_result_callback(
        int_input,
        meal_pager_set_first_station_callback,
        context,
        app->text_store[0],
        enter_name_length,
        false);

    view_dispatcher_switch_to_view(app->view_dispatcher, Meal_PagerViewIdIntInput);
}

bool meal_pager_scene_set_first_station_on_event(void* context, SceneManagerEvent event) {
    Meal_Pager* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(app->scene_manager);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        app->first_station = atoi(app->text_store[0]);
        if(app->first_station > app->max_station) {
            app->first_station = app->max_station;
            snprintf(app->text_store[0], sizeof(app->text_store[0]), "%lu", app->first_station);
        }
        app->first_station_char = app->text_store[0];
        scene_manager_previous_scene(app->scene_manager);
        return true;
    } else if(event.type == SceneManagerEventTypeTick) {
        if(app->state_notifications == SubGhzNotificationStateTx) {
            app->state_notifications = SubGhzNotificationStateIDLE;
            subghz_txrx_stop(app->subghz->txrx);
            meal_pager_blink_stop(app);
            meal_pager_transmit_model_set_sending(app->meal_pager_transmit, 0);
        }
        return true;
    }

    return consumed;
}

void meal_pager_scene_set_first_station_on_exit(void* context) {
    Meal_Pager* app = context;
    UNUSED(app);
}