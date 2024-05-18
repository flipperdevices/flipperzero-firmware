#include <furi.h>

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/dialog_ex.h>

#include <notification/notification_messages.h>

#include "quac.h"
#include "scenes.h"
#include "scene_items.h"
#include "../actions/action.h"
#include "../views/action_menu.h"

#include <lib/toolbox/path.h>

static const ActionMenuItemType ItemToMenuItem[] = {
    [Item_SubGhz] = ActionMenuItemTypeSubGHz,
    [Item_RFID] = ActionMenuItemTypeRFID,
    [Item_IR] = ActionMenuItemTypeIR,
    [Item_NFC] = ActionMenuItemTypeNFC,
    [Item_Playlist] = ActionMenuItemTypePlaylist,
    [Item_Group] = ActionMenuItemTypeGroup,
    [Item_Settings] = ActionMenuItemTypeSettings,
    [Item_Unknown] = ActionMenuItemTypeUnknown,
};

void scene_items_item_callback(void* context, int32_t index, InputType type) {
    App* app = context;

    // FURI_LOG_I(TAG, "scene_items callback, type == %s", input_get_type_name(type));

    if(type == InputTypeShort) {
        app->selected_item = index;
        view_dispatcher_send_custom_event(app->view_dispatcher, Event_ButtonPressed);
    } else if(type == InputTypeLong) {
        app->selected_item = index;
        view_dispatcher_send_custom_event(app->view_dispatcher, Event_ButtonPressedLong);
    } else {
        // do nothing
    }
}

// For each scene, implement handler callbacks
void scene_items_on_enter(void* context) {
    App* app = context;

    ActionMenu* menu = app->action_menu;
    action_menu_reset(menu);
    if(app->settings.layout == QUAC_APP_LANDSCAPE)
        action_menu_set_layout(menu, ActionMenuLayoutLandscape);
    else
        action_menu_set_layout(menu, ActionMenuLayoutPortrait);
    action_menu_set_show_icons(menu, app->settings.show_icons);
    action_menu_set_show_headers(menu, app->settings.show_headers);

    ItemsView* items_view = app->items_view;
    FURI_LOG_I(
        TAG, "Generating scene: [depth=%d] %s", app->depth, furi_string_get_cstr(items_view->path));

    action_menu_set_header(menu, furi_string_get_cstr(items_view->name));

    size_t item_view_size = ItemArray_size(items_view->items);
    if(item_view_size > 0) {
        ItemArray_it_t iter;
        int32_t index = 0;
        for(ItemArray_it(iter, items_view->items); !ItemArray_end_p(iter);
            ItemArray_next(iter), ++index) {
            const char* label = furi_string_get_cstr(ItemArray_cref(iter)->name);
            ActionMenuItemType type = ItemToMenuItem[ItemArray_cref(iter)->type];
            action_menu_add_item(menu, label, index, scene_items_item_callback, type, app);
        }
    } else {
        FURI_LOG_W(TAG, "No items for: %s", furi_string_get_cstr(items_view->path));
        // Add a bogus item - this lets the user still access the Action menu to import, etc
        action_menu_add_item(
            menu,
            "<Empty>",
            EMPTY_ACTION_INDEX,
            scene_items_item_callback,
            ActionMenuItemTypeGroup,
            app);
    }

    // Always add the "Settings" item at the end of our list - but only at top level!
    if(app->depth == 0) {
        action_menu_add_item(
            menu,
            "Settings",
            item_view_size, // last item!
            scene_items_item_callback,
            ActionMenuItemTypeSettings,
            app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, QView_ActionMenu);
}
bool scene_items_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        if(event.event == Event_ButtonPressed && app->selected_item != EMPTY_ACTION_INDEX) {
            consumed = true;
            // FURI_LOG_I(TAG, "button pressed is %d", app->selected_item);
            if(app->selected_item < (int)ItemArray_size(app->items_view->items)) {
                Item* item = ItemArray_get(app->items_view->items, app->selected_item);
                if(item->type == Item_Group) {
                    app->depth++;
                    ItemsView* new_items = item_get_items_view_from_path(app, item->path);
                    item_items_view_free(app->items_view);
                    app->items_view = new_items;
                    scene_manager_next_scene(app->scene_manager, QScene_Items);
                } else {
                    FURI_LOG_I(
                        TAG, "Initiating item action: %s", furi_string_get_cstr(item->name));

                    // LED goes blinky blinky
                    App* app = context;
                    notification_message(app->notifications, &sequence_blink_start_blue);

                    // Prepare error string for action calls
                    FuriString* error;
                    error = furi_string_alloc();

                    action_tx(app, item, error);

                    if(furi_string_size(error)) {
                        FURI_LOG_E(TAG, furi_string_get_cstr(error));
                        // Fire up the LED and vibrate!
                        notification_message(app->notifications, &sequence_error);
                    }

                    furi_string_free(error);

                    // Turn off LED light
                    notification_message(app->notifications, &sequence_blink_stop);
                }
            } else {
                // FURI_LOG_I(TAG, "Selected Settings!");
                // TODO: Do we need to free this current items_view??
                scene_manager_next_scene(app->scene_manager, QScene_Settings);
            }
        } else if(event.event == Event_ButtonPressedLong) {
            if(app->selected_item < (int)ItemArray_size(app->items_view->items)) {
                consumed = true;
                scene_manager_next_scene(app->scene_manager, QScene_ActionSettings);
            }
        }
        break;
    case SceneManagerEventTypeBack:
        // FURI_LOG_I(TAG, "Back button pressed!");
        consumed = false; // Ensure Back event continues to propagate
        if(app->depth > 0) {
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
            // FURI_LOG_I(TAG, "At the root level!");
        }
        break;
    default:
        FURI_LOG_I(TAG, "Custom event not handled");
        break;
    }
    // FURI_LOG_I(TAG, "Generic event not handled");
    return consumed;
}

void scene_items_on_exit(void* context) {
    App* app = context;
    ActionMenu* menu = app->action_menu;
    action_menu_reset(menu);
}