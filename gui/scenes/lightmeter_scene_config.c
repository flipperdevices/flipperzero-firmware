#include "../../lightmeter.h"

static const char* iso_numbers[] = {
    // [ISO_6] = "6",
    // [ISO_12] = "12",
    // [ISO_25] = "25",
    // [ISO_50] = "50",
    [ISO_100] = "100",
    [ISO_200] = "200",
    [ISO_400] = "400",
    [ISO_800] = "800",
    [ISO_1600] = "1600",
    [ISO_3200] = "3200",
    [ISO_6400] = "6400",
    [ISO_12800] = "12800",
    [ISO_25600] = "25600",
    [ISO_51200] = "51200",
    [ISO_102400] = "102400",
};

static const char* nd_numbers[] = {
    [ND_0] = "0",
    [ND_3] = "3",
    [ND_6] = "6",
    [ND_9] = "9",
    [ND_12] = "12",
    [ND_15] = "15",
    [ND_18] = "18",
    [ND_21] = "21",
    [ND_24] = "24",
    [ND_27] = "27",
    [ND_30] = "30",
    [ND_33] = "33",
    [ND_36] = "36",
    [ND_39] = "39",
    [ND_42] = "42",
    [ND_45] = "45",
    [ND_48] = "48",
};

enum LightMeterSubmenuIndex {
    LightMeterSubmenuIndexISO,
    LightMeterSubmenuIndexND,
};

static void iso_numbers_cb(VariableItem* item) {
    LightMeterApp* lightmeter = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, iso_numbers[index]);

    LightMeterConfig* config = lightmeter->config;
    config->iso = index;
    lightmeter_app_set_config(lightmeter, config);
}

static void nd_numbers_cb(VariableItem* item) {
    LightMeterApp* lightmeter = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, nd_numbers[index]);

    LightMeterConfig* config = lightmeter->config;
    config->nd = index;
    lightmeter_app_set_config(lightmeter, config);
}

static void ok_cb(void* context, uint32_t index) {
    LightMeterApp* lightmeter = context;
    UNUSED(lightmeter);
    switch(index) {
    case 3:
        // view_dispatcher_send_custom_event(lightmeter->view_dispatcher, DapAppCustomEventHelp);
        break;
    case 4:
        // view_dispatcher_send_custom_event(lightmeter->view_dispatcher, DapAppCustomEventAbout);
        break;
    default:
        break;
    }
}

void lightmeter_scene_config_on_enter(void* context) {
    LightMeterApp* lightmeter = context;
    VariableItemList* var_item_list = lightmeter->var_item_list;
    VariableItem* item;
    LightMeterConfig* config = lightmeter->config;

    item = variable_item_list_add(
        var_item_list, "ISO", COUNT_OF(iso_numbers), iso_numbers_cb, lightmeter);
    variable_item_set_current_value_index(item, config->iso);
    variable_item_set_current_value_text(item, iso_numbers[config->iso]);

    item = variable_item_list_add(
        var_item_list, "ND", COUNT_OF(nd_numbers), nd_numbers_cb, lightmeter);
    variable_item_set_current_value_index(item, config->nd);
    variable_item_set_current_value_text(item, nd_numbers[config->nd]);

    item = variable_item_list_add(var_item_list, "Help and Pinout", 0, NULL, NULL);
    item = variable_item_list_add(var_item_list, "About", 0, NULL, NULL);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(lightmeter->scene_manager, LightMeterAppSceneConfig));

    variable_item_list_set_enter_callback(var_item_list, ok_cb, lightmeter);

    view_dispatcher_switch_to_view(lightmeter->view_dispatcher, LightMeterAppViewVarItemList);
}

bool lightmeter_scene_config_on_event(void* context, SceneManagerEvent event) {
    LightMeterApp* lightmeter = context;
    bool consumed = false;

    UNUSED(lightmeter);

    if(event.type == SceneManagerEventTypeTick) {
        consumed = true;
    }
    return consumed;
}

void lightmeter_scene_config_on_exit(void* context) {
    LightMeterApp* lightmeter = context;
    variable_item_list_reset(lightmeter->var_item_list);
    main_view_set_iso(lightmeter->main_view, lightmeter->config->iso);
}
