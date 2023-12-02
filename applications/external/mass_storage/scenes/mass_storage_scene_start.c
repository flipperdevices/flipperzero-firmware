#include "../mass_storage_app_i.h"

static const struct {
    char* name;
    uint32_t value;
} image_size[] = {
    {"1.44M", 1440 * 1024},
    {"2M", 2 * 1024 * 1024},
    {"4M", 4 * 1024 * 1024},
    {"8M", 8 * 1024 * 1024},
    {"16M", 16 * 1024 * 1024},
    {"32M", 32 * 1024 * 1024},
    {"64M", 64 * 1024 * 1024},
    {"128M", 128 * 1024 * 1024},
    {"256M", 256 * 1024 * 1024},
    {"512M", 512 * 1024 * 1024},
    {"700M", 700 * 1024 * 1024},
    {"1G", 1024 * 1024 * 1024},
    {"2G", 2u * 1024 * 1024 * 1024},
};

static void mass_storage_item_select(void* context, uint32_t index) {
    MassStorageApp* app = context;
    if(index == 0) {
        view_dispatcher_send_custom_event(app->view_dispatcher, MassStorageCustomEventFileSelect);
    } else {
        view_dispatcher_send_custom_event(app->view_dispatcher, MassStorageCustomEventNewImage);
    }
}

static void mass_storage_image_size(VariableItem* item) {
    MassStorageApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, image_size[index].name);
    app->new_file_size = image_size[index].value;
}

void mass_storage_scene_start_on_enter(void* context) {
    MassStorageApp* app = context;

    VariableItem* item =
        variable_item_list_add(app->variable_item_list, "Select disk image", 0, NULL, NULL);

    item = variable_item_list_add(
        app->variable_item_list, "New image", COUNT_OF(image_size), mass_storage_image_size, app);

    variable_item_list_set_enter_callback(app->variable_item_list, mass_storage_item_select, app);

    variable_item_set_current_value_index(item, 2);
    variable_item_set_current_value_text(item, image_size[2].name);
    app->new_file_size = image_size[2].value;
    view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewStart);
}

bool mass_storage_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    MassStorageApp* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MassStorageCustomEventFileSelect) {
            scene_manager_next_scene(app->scene_manager, MassStorageSceneFileSelect);
        } else if(event.event == MassStorageCustomEventNewImage) {
            scene_manager_next_scene(app->scene_manager, MassStorageSceneFileName);
        }
    }
    return false;
}

void mass_storage_scene_start_on_exit(void* context) {
    UNUSED(context);
    MassStorageApp* app = context;
    variable_item_list_reset(app->variable_item_list);
}
