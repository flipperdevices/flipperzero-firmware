#include "../mass_storage_app_i.h"

enum VarItemListIndex {
    VarItemListIndexSelectDiskImage,
    VarItemListIndexCreateDiskImage,
};

static void mass_storage_scene_start_variable_item_list_callback(void* context, uint32_t index) {
    MassStorageApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void mass_storage_scene_start_on_enter(void* context) {
    MassStorageApp* app = context;
    VariableItemList* variable_item_list = app->variable_item_list;

    variable_item_list_add(variable_item_list, "Select Disk Image", 0, NULL, app);

    variable_item_list_add(variable_item_list, "Create Disk Image", 0, NULL, app);

    variable_item_list_set_enter_callback(
        variable_item_list, mass_storage_scene_start_variable_item_list_callback, app);

    variable_item_list_set_header(variable_item_list, "USB Mass Storage");

    variable_item_list_set_selected_item(
        variable_item_list,
        scene_manager_get_scene_state(app->scene_manager, MassStorageSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewStart);
}

bool mass_storage_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    MassStorageApp* app = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, MassStorageSceneStart, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexSelectDiskImage:
            scene_manager_next_scene(app->scene_manager, MassStorageSceneFileSelect);
            break;
        case VarItemListIndexCreateDiskImage:
            scene_manager_set_scene_state(app->scene_manager, MassStorageSceneCreateImage, 0);
            scene_manager_next_scene(app->scene_manager, MassStorageSceneCreateImage);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void mass_storage_scene_start_on_exit(void* context) {
    UNUSED(context);
    MassStorageApp* app = context;
    variable_item_list_reset(app->variable_item_list);
}
