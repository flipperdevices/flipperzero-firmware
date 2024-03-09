#include <furi.h>

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/dialog_ex.h>

#include <notification/notification_messages.h>

#include "quac.h"
#include "scenes.h"
#include "scene_items.h"
#include "../actions/action.h"
#include <lib/toolbox/path.h>

void scene_items_item_callback(void* context, int32_t index, InputType type) {
    App* app = context;

    if(type == InputTypeShort || type == InputTypeRelease) {
        app->selected_item = index;
        view_dispatcher_send_custom_event(app->view_dispatcher, Event_ButtonPressed);
    } else {
        // do nothing
    }
}

// For each scene, implement handler callbacks
void scene_items_on_enter(void* context) {
    App* app = context;
    ButtonMenu* menu = app->btn_menu;
    button_menu_reset(menu);
    DialogEx* dialog = app->dialog;
    dialog_ex_reset(dialog);

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
        // TODO: Display Error popup? Empty folder?
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
                app->depth++;
                ItemsView* new_items = item_get_items_view_from_path(app, item->path);
                item_items_view_free(app->items_view);
                app->items_view = new_items;
                scene_manager_next_scene(app->scene_manager, SR_Scene_Items);
            } else {
                FURI_LOG_I(TAG, "Initiating item action: %s", furi_string_get_cstr(item->name));

                // LED goes blinky blinky
                App* app = context;
                notification_message(app->notifications, &sequence_blink_start_blue);

                // Prepare error string for action calls
                FuriString* error;
                error = furi_string_alloc();

                action_tx(app, item, error);

                if(furi_string_size(error)) {
                    FURI_LOG_E(TAG, furi_string_get_cstr(error));
                    // Change LED to Red and Vibrate!
                    notification_message(app->notifications, &sequence_error);

                    // Display DialogEx popup or something?
                }

                furi_string_free(error);

                // Turn off LED light
                notification_message(app->notifications, &sequence_blink_stop);
            }
        }
        break;
    case SceneManagerEventTypeBack:
        FURI_LOG_I(TAG, "Back button pressed!");
        consumed = false; // Ensure Back event continues to propagate
        if(app->depth >= 0) {
            // take our current ItemsView path, and go back up a level
            FuriString* parent_path;
            parent_path = furi_string_alloc();
            path_extract_dirname(furi_string_get_cstr(app->items_view->path), parent_path);

            app->depth--;
            ItemsView* new_items = item_get_items_view_from_path(app, parent_path);
            item_items_view_free(app->items_view);
            app->items_view = new_items;

            furi_string_free(parent_path);
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
    DialogEx* dialog = app->dialog;
    dialog_ex_reset(dialog);

    FURI_LOG_I(TAG, "on_exit. depth = %d", app->depth);
}