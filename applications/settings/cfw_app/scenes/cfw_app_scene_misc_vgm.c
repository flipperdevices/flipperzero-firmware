#include "../cfw_app.h"

enum VarItemListIndex {
    VarItemListIndexColors,
    VarItemListIndexForeground,
    VarItemListIndexBackground,
};

void cfw_app_scene_misc_vgm_var_item_list_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

const char* const colors_names[VgmColorModeCount] = {
    "Default",
    "Custom",
    "Rainbow",
    "RGB Backlight",
};
static void cfw_app_scene_misc_vgm_colors_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, colors_names[index]);
    cfw_settings.vgm_color_mode = index;
    app->save_settings = true;
    variable_item_set_locked(
        variable_item_list_get(app->var_item_list, VarItemListIndexForeground),
        index != VgmColorModeCustom,
        NULL);
    variable_item_set_locked(
        variable_item_list_get(app->var_item_list, VarItemListIndexBackground),
        index != VgmColorModeCustom,
        NULL);
    expansion_disable(app->expansion);
    expansion_enable(app->expansion);
}

static const struct {
    char* name;
    Rgb565Color color;
} vgm_colors[] = {
    {"Orange", {0xFC00}}, {"Black", {0x0000}}, {"Red", {0xF800}},    {"Maroon", {0x8000}},
    {"Yellow", {0xFFE0}}, {"Olive", {0x8400}}, {"Lime", {0x07E0}},   {"Green", {0x0400}},
    {"Aqua", {0x07EF}},   {"Cyan", {0x069A}},  {"Azure", {0x03FF}},  {"Teal", {0x0410}},
    {"Blue", {0x001F}},   {"Navy", {0x0010}},  {"Purple", {0x8010}}, {"Fuchsia", {0xF81F}},
    {"Pink", {0xA8F5}},   {"Brown", {0xA145}}, {"White", {0xFFFF}},
};
static const size_t vgm_colors_count = COUNT_OF(vgm_colors);
static void cfw_app_scene_misc_vgm_foreground_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, vgm_colors[index].name);
    cfw_settings.vgm_color_fg = vgm_colors[index].color;
    app->save_settings = true;
    if(cfw_settings.vgm_color_mode == VgmColorModeCustom) {
        expansion_disable(app->expansion);
        expansion_enable(app->expansion);
    }
}
static void cfw_app_scene_misc_vgm_background_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, vgm_colors[index].name);
    cfw_settings.vgm_color_bg = vgm_colors[index].color;
    app->save_settings = true;
    if(cfw_settings.vgm_color_mode == VgmColorModeCustom) {
        expansion_disable(app->expansion);
        expansion_enable(app->expansion);
    }
}

void cfw_app_scene_misc_vgm_on_enter(void* context) {
    CfwApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(
        var_item_list, "VGM Colors", VgmColorModeCount, cfw_app_scene_misc_vgm_colors_changed, app);
    value_index = cfw_settings.vgm_color_mode;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, colors_names[value_index]);

    item = variable_item_list_add(
        var_item_list,
        "Foreground",
        vgm_colors_count,
        cfw_app_scene_misc_vgm_foreground_changed,
        app);
    Rgb565Color color = cfw_settings.vgm_color_fg;
    bool found = false;
    for(size_t i = 0; i < vgm_colors_count; i++) {
        if(rgb565cmp(&color, &vgm_colors[i].color) != 0) continue;
        value_index = i;
        found = true;
        break;
    }
    variable_item_set_current_value_index(item, found ? value_index : vgm_colors_count);
    if(found) {
        variable_item_set_current_value_text(item, vgm_colors[value_index].name);
    } else {
        char str[5];
        snprintf(str, sizeof(str), "%04X", color.value);
        variable_item_set_current_value_text(item, str);
    }
    variable_item_set_locked(
        item, cfw_settings.vgm_color_mode != VgmColorModeCustom, "Need Custom\nColors!");

    item = variable_item_list_add(
        var_item_list,
        "Background",
        vgm_colors_count,
        cfw_app_scene_misc_vgm_background_changed,
        app);
    color = cfw_settings.vgm_color_bg;
    found = false;
    for(size_t i = 0; i < vgm_colors_count; i++) {
        if(rgb565cmp(&color, &vgm_colors[i].color) != 0) continue;
        value_index = i;
        found = true;
        break;
    }
    variable_item_set_current_value_index(item, found ? value_index : vgm_colors_count);
    if(found) {
        variable_item_set_current_value_text(item, vgm_colors[value_index].name);
    } else {
        char str[5];
        snprintf(str, sizeof(str), "%04X", color.value);
        variable_item_set_current_value_text(item, str);
    }
    variable_item_set_locked(
        item, cfw_settings.vgm_color_mode != VgmColorModeCustom, "Need Custom\nColors!");

    variable_item_list_set_enter_callback(
        var_item_list, cfw_app_scene_misc_vgm_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, CfwAppSceneMiscVgm));

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
}

bool cfw_app_scene_misc_vgm_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, CfwAppSceneMiscVgm, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexForeground:
        case VarItemListIndexBackground:
            scene_manager_set_scene_state(
                app->scene_manager,
                CfwAppSceneMiscVgmColor,
                event.event - VarItemListIndexForeground);
            scene_manager_next_scene(app->scene_manager, CfwAppSceneMiscVgmColor);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_misc_vgm_on_exit(void* context) {
    CfwApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
