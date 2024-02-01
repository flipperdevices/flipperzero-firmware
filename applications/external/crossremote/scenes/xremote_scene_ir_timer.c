#include "../xremote.h"
#include "../models/cross/xremote_cross_remote.h"

void xremote_scene_ir_timer_callback(void* context) {
    XRemote* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, XRemoteCustomEventTextInput);
}

void xremote_scene_ir_timer_on_enter(void* context) {
    furi_assert(context);
    XRemote* app = context;
    IntInput* int_input = app->int_input;
    size_t enter_name_length = 5;
    char* str = "Transmit in ms (0 - 9999)";
    const char* constStr = str;
    CrossRemoteItem* item = xremote_cross_remote_get_item(app->cross_remote, app->edit_item);
    int_input_set_header_text(int_input, constStr);
    snprintf(app->text_store[1], 5, "%lu", item->time);

    int_input_set_result_callback(
        int_input,
        xremote_scene_ir_timer_callback,
        context,
        app->text_store[1],
        enter_name_length,
        false);

    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdIntInput);
}

bool xremote_scene_ir_timer_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(app->scene_manager);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        CrossRemoteItem* item = xremote_cross_remote_get_item(app->cross_remote, app->edit_item);
        xremote_cross_remote_item_set_time(item, atoi(app->text_store[1]));
        if (item->time > 9999) {
            item->time = 9999;
        }
        //app->first_station = atoi(app->text_store[0]);
        /*if(app->first_station > app->max_station) {
            app->first_station = app->max_station;
            snprintf(app->text_store[0], 5, "%lu", app->first_station);
        }*/
        scene_manager_previous_scene(app->scene_manager);
        return true;
    }
    return consumed;
}

void xremote_scene_ir_timer_on_exit(void* context) {
    XRemote* app = context;
    UNUSED(app);
}