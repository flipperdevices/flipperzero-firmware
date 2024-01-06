#include "../meal_pager_i.h"
#include "../helpers/meal_pager_custom_event.h"
#include "../helpers/retekess/meal_pager_retekess_t119.h"
#include "../helpers/retekess/meal_pager_retekess_td157.h"
#include "../helpers/retekess/meal_pager_retekess_td165.h"
#include "../helpers/retekess/meal_pager_retekess_td174.h"
#include "../views/meal_pager_transmit.h"
#include "../helpers/meal_pager_led.h"
#include "../helpers/subghz/subghz.h"
#include "../views/meal_pager_transmit.h"
#include <dolphin/dolphin.h>

void meal_pager_transmit_callback(Meal_PagerCustomEvent event, void* context) {
    furi_assert(context);
    Meal_Pager* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void meal_pager_scene_transmit_on_enter(void* context) {
    furi_assert(context);
    Meal_Pager* app = context;
    FURI_LOG_D(TAG, "Type is %lu", app->pager_type);

    app->stop_transmit = false;
    meal_pager_blink_start_compile(app);
    meal_pager_transmit_model_set_type(app->meal_pager_transmit, app->pager_type);
    meal_pager_transmit_model_set_station(app->meal_pager_transmit, app->current_station);
    meal_pager_transmit_model_set_pager(app->meal_pager_transmit, app->current_pager);
    meal_pager_transmit_set_callback(app->meal_pager_transmit, meal_pager_transmit_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, Meal_PagerViewIdTransmit);
    bool generated = false;
    switch(app->pager_type) {
    case Meal_PagerPagerTypeT119:
        generated = meal_pager_retekess_t119_generate_all(app);
        break;
    case Meal_PagerPagerTypeTD157:
        generated = meal_pager_retekess_td157_generate_all(app);
        break;
    case Meal_PagerPagerTypeTD165:
        generated = meal_pager_retekess_td165_generate_all(app);
        break;
    case Meal_PagerPagerTypeTD174:
        generated = meal_pager_retekess_td174_generate_all(app);
        break;
    default:
        generated = false;
        break;
    }
    if(!generated) {
        FURI_LOG_D(TAG, "Could not generate temp file");
        meal_pager_blink_stop(app);
        return;
    }
    FURI_LOG_D(TAG, "Generated tmp.sub");
    FURI_LOG_D(TAG, "Start Transmitting");
    if(!app->stop_transmit) {
        meal_pager_transmit_model_set_sending(app->meal_pager_transmit, 1);
        subghz_send(app);
    }
    dolphin_deed(DolphinDeedSubGhzSend);
}

bool meal_pager_scene_transmit_on_event(void* context, SceneManagerEvent event) {
    Meal_Pager* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case Meal_PagerCustomEventTransmitLeft:
        case Meal_PagerCustomEventTransmitRight:
            break;
        case Meal_PagerCustomEventTransmitUp:
        case Meal_PagerCustomEventTransmitDown:
            break;
        case Meal_PagerCustomEventTransmitBack:
            app->stop_transmit = true;
            app->state_notifications = SubGhzNotificationStateIDLE;
            meal_pager_blink_stop(app);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, Meal_PagerSceneMenu)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        case Meal_PagerCustomEventViewTransmitterSendStop:
            app->state_notifications = SubGhzNotificationStateIDLE;
            subghz_txrx_stop(app->subghz->txrx);
            meal_pager_blink_stop(app);
            meal_pager_transmit_model_set_sending(app->meal_pager_transmit, 0);
            scene_manager_next_scene(app->scene_manager, Meal_PagerSceneMenu);
            FURI_LOG_D(TAG, "Stop Event");
            break;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        if(app->state_notifications == SubGhzNotificationStateTx) {
            notification_message(app->notification, &sequence_blink_magenta_10);
        }
        return true;
    }

    return consumed;
}

void meal_pager_scene_transmit_on_exit(void* context) {
    Meal_Pager* app = context;
    UNUSED(app);
}