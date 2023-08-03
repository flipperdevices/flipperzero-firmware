#include "../mass_storage_app_i.h"
#include <lib/toolbox/value_index.h>

enum VarItemListIndex {
    VarItemListIndexImageSize,
    VarItemListIndexSizeUnit,
    VarItemListIndexName,
    VarItemListIndexCreate,
};

void mass_storage_scene_create_image_var_item_list_callback(void* context, uint32_t index) {
    MassStorageApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

const uint32_t image_size_values[] = {1,  2,  4,  8,  12,  16,  20,  25,  30,  35, 40,
                                      45, 50, 69, 75, 100, 150, 200, 250, 500, 750};
static void mass_storage_scene_create_image_image_size_changed(VariableItem* item) {
    MassStorageApp* app = variable_item_get_context(item);
    app->create_image_size = image_size_values[variable_item_get_current_value_index(item)];
    char str[4];
    snprintf(str, sizeof(str), "%lu", app->create_image_size);
    variable_item_set_current_value_text(item, str);
}

const char* const size_unit_names[] = {
    [SizeUnitBytes] = "Bytes",
    [SizeUnitKb] = "Kb",
    [SizeUnitMb] = "Mb",
    [SizeUnitGb] = "Gb",
};
static void mass_storage_scene_create_image_size_unit_changed(VariableItem* item) {
    MassStorageApp* app = variable_item_get_context(item);
    app->create_size_unit = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, size_unit_names[app->create_size_unit]);
}

void mass_storage_scene_create_image_on_enter(void* context) {
    MassStorageApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    value_index =
        value_index_uint32(app->create_image_size, image_size_values, COUNT_OF(image_size_values));
    app->create_image_size = image_size_values[value_index];
    char str[4];
    snprintf(str, sizeof(str), "%lu", app->create_image_size);
    item = variable_item_list_add(
        var_item_list,
        "Image Size",
        COUNT_OF(image_size_values),
        mass_storage_scene_create_image_image_size_changed,
        app);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, str);

    app->create_size_unit = CLAMP(app->create_size_unit, SizeUnitCount - 1, 0);
    item = variable_item_list_add(
        var_item_list,
        "Size Unit",
        SizeUnitCount,
        mass_storage_scene_create_image_size_unit_changed,
        app);
    variable_item_set_current_value_index(item, app->create_size_unit);
    variable_item_set_current_value_text(item, size_unit_names[app->create_size_unit]);

    item = variable_item_list_add(var_item_list, "Name", 0, NULL, app);
    variable_item_set_current_value_text(item, app->create_name);

    variable_item_list_add(var_item_list, "Create", 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, mass_storage_scene_create_image_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, MassStorageSceneCreateImage));

    view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewVarItemList);
}

static void popup_callback_ok(void* context) {
    MassStorageApp* app = context;
    scene_manager_previous_scene(app->scene_manager);
}

static void popup_callback_error(void* context) {
    MassStorageApp* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewVarItemList);
}

bool mass_storage_scene_create_image_on_event(void* context, SceneManagerEvent event) {
    MassStorageApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, MassStorageSceneCreateImage, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexName:
            scene_manager_next_scene(app->scene_manager, MassStorageSceneCreateImageName);
            break;
        case VarItemListIndexCreate: {
            popup_set_header(app->popup, "Creating Image...", 64, 32, AlignCenter, AlignCenter);
            popup_set_text(app->popup, "", 0, 0, AlignLeft, AlignBottom);
            popup_set_callback(app->popup, NULL);
            popup_set_context(app->popup, NULL);
            popup_set_timeout(app->popup, 0);
            popup_disable_timeout(app->popup);
            view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewPopup);

            bool default_name = !strnlen(app->create_name, sizeof(app->create_name));
            if(default_name) {
                snprintf(
                    app->create_name,
                    sizeof(app->create_name),
                    "%lu%s",
                    app->create_image_size,
                    size_unit_names[app->create_size_unit]);
            }
            furi_string_printf(app->file_path, APP_DATA_PATH("%s.img"), app->create_name);

            app->file = storage_file_alloc(app->fs_api);
            const char* error = NULL;
            if(storage_file_open(
                   app->file, furi_string_get_cstr(app->file_path), FSAM_WRITE, FSOM_CREATE_NEW)) {
                uint64_t size = app->create_image_size;
                for(size_t i = app->create_size_unit; i > 0; i--) size *= 1024;
                if(!storage_file_expand(app->file, size)) {
                    error = "Can't allocate data";
                }
            } else {
                if(storage_file_exists(app->fs_api, furi_string_get_cstr(app->file_path))) {
                    error = "File already exists";
                } else {
                    error = "Can't open file";
                }
            }
            storage_file_free(app->file);

            if(error) {
                popup_set_header(
                    app->popup, "Error Creating Image!", 64, 26, AlignCenter, AlignCenter);
                popup_set_text(app->popup, error, 64, 40, AlignCenter, AlignCenter);
                popup_set_callback(app->popup, popup_callback_error);
            } else {
                popup_set_header(app->popup, "Image Created!", 64, 32, AlignCenter, AlignCenter);
                popup_set_text(app->popup, "", 0, 0, AlignLeft, AlignBottom);
                popup_set_callback(app->popup, popup_callback_ok);
            }
            popup_set_context(app->popup, app);
            popup_set_timeout(app->popup, 0);
            popup_disable_timeout(app->popup);
            view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewPopup);

            if(default_name) strcpy(app->create_name, "");
            break;
        }
        default:
            break;
        }
    }

    return consumed;
}

void mass_storage_scene_create_image_on_exit(void* context) {
    MassStorageApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
