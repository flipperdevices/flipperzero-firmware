#include "../wifi_marauder_app_i.h"

enum WifiMarauderItem {
    WifiMarauderItemScan,
    WifiMarauderItemAttack,
    WifiMarauderItemSniff,
    WifiMarauderItemChannel,
    WifiMarauderItemUpdate,
    WifiMarauderItemReboot,
};

// TODO: check if all these channels are actually supported
#define NUM_CHANNELS (14)
const char* const channel_select_text[NUM_CHANNELS] = {
    "1", "2", "3", "4", "5", "6", "7",
    "8", "9", "10", "11", "12", "13", "14",
};

static void wifi_marauder_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    WifiMarauderApp* app = context;

    if(index == WifiMarauderItemScan) {
        view_dispatcher_send_custom_event(app->view_dispatcher, WifiMarauderStartEventScan);
    } else if(index == WifiMarauderItemAttack) {
        view_dispatcher_send_custom_event(app->view_dispatcher, WifiMarauderStartEventAttack);
    } else if(index == WifiMarauderItemSniff) {
        view_dispatcher_send_custom_event(app->view_dispatcher, WifiMarauderStartEventSniff);
    } else if(index == WifiMarauderItemUpdate) {
        view_dispatcher_send_custom_event(app->view_dispatcher, WifiMarauderStartEventUpdate);
    } else if(index == WifiMarauderItemReboot) {
        view_dispatcher_send_custom_event(app->view_dispatcher, WifiMarauderStartEventReboot);
    }
}

static void wifi_marauder_scene_start_var_list_change_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, channel_select_text[index]);
    app->selected_wifi_channel = index + 1;
    view_dispatcher_send_custom_event(app->view_dispatcher, WifiMarauderStartEventChannel);
}

void wifi_marauder_scene_start_on_enter(void* context) {
    WifiMarauderApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    VariableItem* item;
    variable_item_list_set_enter_callback(
        var_item_list, wifi_marauder_scene_start_var_list_enter_callback, app);

    variable_item_list_add(var_item_list, "Scan", 0, NULL, NULL);
    variable_item_list_add(var_item_list, "Attack", 0, NULL, NULL);
    variable_item_list_add(var_item_list, "Sniff", 0, NULL, NULL);

    item = variable_item_list_add(
        var_item_list,
        "Channel",
        NUM_CHANNELS,
        wifi_marauder_scene_start_var_list_change_callback,
        app);
    // TODO: get current channel to display here
    // don't forget that the index is channel - 1
    variable_item_set_current_value_index(item, 0);
    variable_item_set_current_value_text(item, channel_select_text[0]);

    variable_item_list_add(var_item_list, "Update", 0, NULL, NULL);
    variable_item_list_add(var_item_list, "Reboot", 0, NULL, NULL);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, WifiMarauderSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, WifiMarauderAppViewVarItemList);
}

bool wifi_marauder_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    //WifiMarauderApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        // TODO
        // if (event.event == WifiMarauderStartEventScan) {
        //     scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneStart, WifiMarauderItemScan);
        //     scene_manager_next_scene(app->scene_manager, WifiMarauderSceneScan);
        // } else if (event.event == WifiMarauderStartEventAttack) {
        //     scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneStart, WifiMarauderItemAttack);
        //     scene_manager_next_scene(app->scene_manager, WifiMarauderSceneAttack);
        // } else if (event.event == WifiMarauderStartEventSniff) {
        //     scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneStart, WifiMarauderItemSniff);
        //     scene_manager_next_scene(app->scene_manager, WifiMarauderSceneSniff);
        // } else if (event.event == WifiMarauderStartEventUpdate) {
        //     scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneStart, WifiMarauderItemUpdate);
        //     scene_manager_next_scene(app->scene_manager, WifiMarauderSceneUpdate);
        // } else if (event.event == WifiMarauderStartEventReboot) {
        //     scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneStart, WifiMarauderItemReboot);
        //     scene_manager_next_scene(app->scene_manager, WifiMarauderSceneReboot);
        // } else if (event.event == WifiMarauderStartEventChannel) {
        //     // TODO: Send app->selected_wifi_channel to UART
        // }
        consumed = true;
    }

    return consumed;
}

void wifi_marauder_scene_start_on_exit(void* context) {
    WifiMarauderApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
