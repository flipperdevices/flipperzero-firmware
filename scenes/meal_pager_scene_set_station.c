#include "../meal_pager_i.h"
#include "../helpers/meal_pager_custom_event.h"
#include "../helpers/meal_pager_storage.h"
#include "../helpers/retekess/meal_pager_retekess_t119.h"
#include "../helpers/retekess/meal_pager_retekess_td157.h"
#include "../helpers/retekess/meal_pager_retekess_td165.h"
#include "../helpers/retekess/meal_pager_retekess_td174.h"
#include "../views/meal_pager_transmit.h"
#include "../helpers/meal_pager_led.h"
#include "../helpers/subghz/subghz.h"
#include "../views/meal_pager_transmit.h"
#include <dolphin/dolphin.h>

void meal_pager_set_station_callback(void* context) {
    furi_assert(context);
    Meal_Pager* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, Meal_PagerCustomerEventIntInput);
}

/*static void meal_pager_int_input_callback(void* context) {
    furi_assert(context);
    Meal_Pager* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, Meal_PagerCustomerEventIntInput);
}*/

void meal_pager_scene_set_station_on_enter(void* context) {
    furi_assert(context);
    Meal_Pager* app = context;
    IntInput* int_input = app->int_input;
    size_t enter_name_length = 4;
    meal_pager_set_max_values(app);
    char *str = "Set first Station (0 - 9999)";
    const char *constStr = str;
    snprintf(str, 36, "Set first Station (0 - %lu)", app->max_station);
    
    int_input_set_header_text(int_input, constStr);
    
    int_input_set_result_callback(
        int_input,
        meal_pager_set_station_callback,
        context,
        app->text_buffer,
        enter_name_length,
        false);
        
    UNUSED(app);
    UNUSED(enter_name_length);
    view_dispatcher_switch_to_view(app->view_dispatcher, Meal_PagerViewIdIntInput);
}


bool meal_pager_scene_set_station_on_event(void* context, SceneManagerEvent event) {
    Meal_Pager* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(app->scene_manager);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case Meal_PagerCustomEventTransmitLeft:
        case Meal_PagerCustomEventTransmitRight:
            break;
        case Meal_PagerCustomEventTransmitUp:
        case Meal_PagerCustomEventTransmitDown:
            break;
        case Meal_PagerCustomEventTransmitBack:
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, Meal_PagerSceneMenu)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
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

void meal_pager_scene_set_station_on_exit(void* context) {
    Meal_Pager* app = context;
    UNUSED(app);
}