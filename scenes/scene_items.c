#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_items.h"
#include "../actions/action.h"
#include <lib/toolbox/path.h>

void scene_items_item_callback(void* context, int32_t index, InputType type) {
    App* app = context;

    // FURI_LOG_I(TAG, "item_callback: %ld, %s", index, input_get_type_name(type));
    if(type == InputTypeShort || type == InputTypeRelease) {
        // FURI_LOG_I(TAG, "You clicked button %li", index);
        app->selected_item = index;
        view_dispatcher_send_custom_event(app->view_dispatcher, Event_ButtonPressed);
    } else {
        // FURI_LOG_I(TAG, "[Ignored event of type %i]", type);
    }
}

// For each scene, implement handler callbacks
void scene_items_on_enter(void* context) {
    App* app = context;
    ButtonMenu* menu = app->btn_menu;
    button_menu_reset(menu);

    ItemsView* items_view = app->items_view;
    FURI_LOG_I(TAG, "items on_enter: [%d] %s", app->depth, furi_string_get_cstr(items_view->path));

    const char* header = furi_string_get_cstr(items_view->name);
    button_menu_set_header(menu, header);

    if(ItemArray_size(items_view->items)) {
        ItemArray_it_t iter;
        int32_t index = 0;
        for(ItemArray_it(iter, items_view->items); !ItemArray_end_p(iter);
            ItemArray_next(iter), ++index) {
            const char* label = furi_string_get_cstr(ItemArray_cref(iter)->name);
            ButtonMenuItemType type = ItemArray_cref(iter)->type == Item_Action ?
                                          ButtonMenuItemTypeCommon :
                                          ButtonMenuItemTypeControl;
            button_menu_add_item(menu, label, index, scene_items_item_callback, type, app);
        }
    } else {
        FURI_LOG_W(TAG, "No items for: %s", furi_string_get_cstr(items_view->path));
        // TODO: Display Error popup?
    }
    // ...

    view_dispatcher_switch_to_view(app->view_dispatcher, SR_ButtonMenu);
}
bool scene_items_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;

    FURI_LOG_I(TAG, "device on_event");
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        if(event.event == Event_ButtonPressed) {
            consumed = true;
            FURI_LOG_I(TAG, "button pressed is %d", app->selected_item);
            Item* item = ItemArray_get(app->items_view->items, app->selected_item);
            if(item->type == Item_Group) {
                ItemsView* new_items = item_get_items_view_from_path(app, item->path);
                FURI_LOG_I(TAG, "calling item_items_view_free");
                item_items_view_free(app->items_view);
                app->items_view = new_items;
                app->depth++;
                scene_manager_next_scene(app->scene_manager, SR_Scene_Items);
            } else {
                FURI_LOG_I(TAG, "Initiating item action: %s", furi_string_get_cstr(item->name));

                // LED goes blinky blinky
                App* app = context;
                notification_message(app->notifications, &sequence_blink_start_green);

                action_tx(app, item);

                // Turn off LED light
                notification_message(app->notifications, &sequence_blink_stop);
            }
        }
        break;
    case SceneManagerEventTypeBack:
        FURI_LOG_I(TAG, "Back button pressed!");
        if(app->depth) {
            // take our current ItemsView path, and back it up a level
            FuriString* new_path;
            new_path = furi_string_alloc();
            path_extract_dirname(furi_string_get_cstr(app->items_view->path), new_path);

            ItemsView* new_items = item_get_items_view_from_path(app, new_path);
            item_items_view_free(app->items_view);
            app->items_view = new_items;
            app->depth--;

            furi_string_free(new_path);
        } else {
            FURI_LOG_W(TAG, "At the root level!");
        }

        break;
    default:
        break;
    }
    return consumed;
}

void scene_items_on_exit(void* context) {
    App* app = context;
    ButtonMenu* menu = app->btn_menu;
    button_menu_reset(menu);
    FURI_LOG_I(TAG, "on_exit. depth = %d", app->depth);
}