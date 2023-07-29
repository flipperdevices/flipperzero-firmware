#include "../cfw_app.h"

enum VarItemListIndex {
    VarItemListIndexInterface,
    VarItemListIndexProtocols,
    VarItemListIndexMisc,
};

void cfw_app_scene_start_var_item_list_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void cfw_app_scene_start_on_enter(void* context) {
    CfwApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    variable_item_list_add(var_item_list, "Interface", 0, NULL, app);
    variable_item_list_add(var_item_list, "Protocols", 0, NULL, app);
    variable_item_list_add(var_item_list, "Misc", 0, NULL, app);
    variable_item_list_add(var_item_list, furi_string_get_cstr(app->version_tag), 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, cfw_app_scene_start_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, CfwAppSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
}

bool cfw_app_scene_start_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, CfwAppSceneStart, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexInterface:
            scene_manager_set_scene_state(app->scene_manager, CfwAppSceneInterface, 0);
            scene_manager_next_scene(app->scene_manager, CfwAppSceneInterface);
            break;
        case VarItemListIndexProtocols:
            scene_manager_set_scene_state(app->scene_manager, CfwAppSceneProtocols, 0);
            scene_manager_next_scene(app->scene_manager, CfwAppSceneProtocols);
            break;
        case VarItemListIndexMisc:
            scene_manager_set_scene_state(app->scene_manager, CfwAppSceneMisc, 0);
            scene_manager_next_scene(app->scene_manager, CfwAppSceneMisc);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_start_on_exit(void* context) {
    CfwApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
