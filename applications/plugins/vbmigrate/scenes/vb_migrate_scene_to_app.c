// SPDX-License-Identifier: GPL-3.0-or-later
//
// VB Lab Migration Assistant for Flipper Zero
// Copyright (C) 2022  cyanic
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <notification/notification_messages.h>

#include "../vb_migrate_i.h"
#include "../vb_tag.h"
#include "vb_migrate_icons.h"

#define TAG "vb_migrate_scene_to_app"

typedef enum {
    ToAppStateInitial,
    ToAppStateInstructions,
    ToAppStateEmulateReady,
    ToAppStateEmulateTransferToApp,
    ToAppStateLoadError,
    ToAppStateComplete,
} ToAppState;

typedef enum {
    ToAppEventTypeWidgetLeft,
    ToAppEventTypeWidgetRight,
    ToAppEventTypeEmulateStart,
    ToAppEventTypeCaptureLoadError,
    ToAppEventTypeTagWrite,
} ToAppEventType;

static void
    vb_migrate_scene_to_app_widget_callback(GuiButtonType result, InputType type, void* context) {
    VbMigrate* inst = context;

    if(type == InputTypeShort) {
        if(result == GuiButtonTypeLeft)
            view_dispatcher_send_custom_event(inst->view_dispatcher, ToAppEventTypeWidgetLeft);
        else if(result == GuiButtonTypeRight)
            view_dispatcher_send_custom_event(inst->view_dispatcher, ToAppEventTypeWidgetRight);
    }
}

static bool vb_migrate_scene_to_app_worker_callback(NfcWorkerEvent event, void* context) {
    VbMigrate* inst = context;
    bool result = false;

    if(event == NfcWorkerEventSuccess) {
        view_dispatcher_send_custom_event(inst->view_dispatcher, ToAppEventTypeTagWrite);
        result = true;
    }

    return result;
}

static void vb_migrate_scene_to_app_set_nfc_state(VbMigrate* inst, ToAppState state) {
    BantBlock* bant = vb_tag_get_bant_block(&inst->nfc_dev->dev_data);
    if(state == ToAppStateEmulateReady) {
        vb_tag_set_status(bant, VbTagStatusReady);
        vb_tag_set_operation(bant, VbTagOperationReady);
    }

    // Override tag type
    if(inst->override_type != inst->orig_type && inst->override_type != VbTagTypeUnknown) {
        vb_tag_set_item_id_no(bant, vb_tag_get_default_product(inst->override_type));
    }
}

static bool vb_migrate_scene_to_app_is_state_changed(VbMigrate* inst, ToAppState state) {
    BantBlock* bant = vb_tag_get_bant_block(&inst->nfc_dev->dev_data);
    VbTagOperation operation = vb_tag_get_operation(bant);

    if(state == ToAppStateEmulateReady) {
        return operation == VbTagOperationTransferToApp;
    }

    return false;
}

static void vb_migrate_scene_to_app_set_state(VbMigrate* inst, ToAppState state) {
    uint32_t curr_state = scene_manager_get_scene_state(inst->scene_manager, VbMigrateSceneToApp);
    if(state != curr_state) {
        Widget* widget = inst->widget;

        if(state == ToAppStateInstructions) {
            widget_reset(widget);
            widget_add_text_scroll_element(
                widget,
                0,
                0,
                128,
                45,
                "\e#To transfer characters\n"
                "\e#from Flipper:\n"
                "1. Put the current character\n"
                "into storage\n"
                "2. Sync character from\n"
                "Flipperas if it was a Vital\n"
                "Bracelet. Flipper will beep\n"
                "and automatically advance to\n"
                "the next captured character\n"
                "when VB Lab has transferred\n"
                "the current character\n"
                "\n"
                "-> You can press the right\n"
                "key to skip sending the\n"
                "current character\n"
                "3. Repeat the above until you\n"
                "have transferred all the\n"
                "characters you want\n"
                "\n"
                "You can cancel at any time to\n"
                "finish transferring.");
            widget_add_button_element(
                widget, GuiButtonTypeLeft, "Cancel", vb_migrate_scene_to_app_widget_callback, inst);
            widget_add_button_element(
                widget, GuiButtonTypeRight, "Next", vb_migrate_scene_to_app_widget_callback, inst);

            view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);
        } else if(state == ToAppStateEmulateReady) {
            view_dispatcher_send_custom_event(inst->view_dispatcher, ToAppEventTypeEmulateStart);
        } else if(state == ToAppStateLoadError) {
            FuriString* temp_str =
                furi_string_alloc_printf("Could not load\ncapture %03d", inst->next_id);
            widget_reset(widget);
            widget_add_string_multiline_element(
                widget,
                80,
                31,
                AlignCenter,
                AlignCenter,
                FontPrimary,
                furi_string_get_cstr(temp_str));
            widget_add_icon_element(widget, 15, 22, &I_Error_18x18);
            widget_add_button_element(
                widget, GuiButtonTypeLeft, "Cancel", vb_migrate_scene_to_app_widget_callback, inst);

            view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);
            furi_string_free(temp_str);
            notification_message(inst->notifications, &sequence_error);
            notification_message(inst->notifications, &sequence_set_red_255);
        } else if(state == ToAppStateComplete) {
            widget_reset(widget);
            widget_add_icon_element(widget, 32, 5, &I_DolphinNice_96x59);
            widget_add_string_multiline_element(
                widget, 28, 26, AlignCenter, AlignBottom, FontPrimary, "Transfers\ncomplete");
            widget_add_button_element(
                widget, GuiButtonTypeRight, "Done", vb_migrate_scene_to_app_widget_callback, inst);

            view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);
        } else {
            furi_crash("Unknown new state in vb_migrate_scene_to_app_set_state");
        }

        scene_manager_set_scene_state(inst->scene_manager, VbMigrateSceneToApp, state);
    }
}

static void vb_migrate_scene_to_app_load_capture(VbMigrate* inst, bool go_next) {
    if(go_next) {
        nfc_worker_stop(inst->worker);
        vb_migrate_blink_stop(inst);
        ++inst->next_id;
        ++inst->num_sent;
    }

    if(inst->num_sent == inst->num_captured) {
        vb_migrate_scene_to_app_set_state(inst, ToAppStateComplete);
    } else {
        uint32_t state = scene_manager_get_scene_state(inst->scene_manager, VbMigrateSceneToApp);
        inst->next_id = vb_migrate_get_next_id(inst, inst->text_store, inst->next_id, true);
        FuriString* temp_str =
            furi_string_alloc_printf(VB_MIGRATE_CAPTURE_FORMAT, inst->next_id, NFC_APP_EXTENSION);

        vb_migrate_show_loading_popup(inst, true);
        if(vb_migrate_load_nfc(inst, inst->text_store, furi_string_get_cstr(temp_str))) {
            Widget* widget = inst->widget;

            widget_reset(widget);
            widget_add_string_multiline_element(
                widget,
                80,
                20,
                AlignCenter,
                AlignCenter,
                FontPrimary,
                "Ready, waiting\nfor transfer");
            furi_string_printf(
                temp_str, "Progress: %d/%d", inst->num_sent + 1, inst->num_captured);
            widget_add_string_element(
                widget,
                63,
                43,
                AlignCenter,
                AlignCenter,
                FontSecondary,
                furi_string_get_cstr(temp_str));
            widget_add_icon_element(widget, 7, 7, &I_Touch_26x26);
            widget_add_button_element(
                widget, GuiButtonTypeLeft, "Cancel", vb_migrate_scene_to_app_widget_callback, inst);
            widget_add_button_element(
                widget, GuiButtonTypeRight, "Skip", vb_migrate_scene_to_app_widget_callback, inst);

            view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);

            vb_migrate_scene_to_app_set_nfc_state(inst, state);
            nfc_worker_start(
                inst->worker,
                NfcWorkerStateMfUltralightEmulate,
                &inst->nfc_dev->dev_data,
                vb_migrate_scene_to_app_worker_callback,
                inst);
            vb_migrate_blink_emulate(inst);
        } else {
            view_dispatcher_send_custom_event(
                inst->view_dispatcher, ToAppEventTypeCaptureLoadError);
        }
        vb_migrate_show_loading_popup(inst, false);
        furi_string_free(temp_str);
    }
}

void vb_migrate_scene_to_app_on_enter(void* context) {
    VbMigrate* inst = context;

    // Perform your setup here
    inst->next_id = 0;
    inst->num_sent = 0;
    scene_manager_set_scene_state(inst->scene_manager, VbMigrateSceneToApp, ToAppStateInitial);
    vb_migrate_scene_to_app_set_state(inst, ToAppStateInstructions);
}

bool vb_migrate_scene_to_app_on_event(void* context, SceneManagerEvent event) {
    VbMigrate* inst = context;
    uint32_t state = scene_manager_get_scene_state(inst->scene_manager, VbMigrateSceneToApp);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == ToAppEventTypeWidgetLeft) {
            consumed = scene_manager_previous_scene(inst->scene_manager);
        } else if(event.event == ToAppEventTypeWidgetRight) {
            if(state == ToAppStateInstructions) {
                vb_migrate_scene_to_app_set_state(inst, ToAppStateEmulateReady);
                consumed = true;
            } else if(state == ToAppStateEmulateReady) {
                vb_migrate_scene_to_app_load_capture(inst, true);
                consumed = true;
            } else if(state == ToAppStateComplete) {
                consumed = scene_manager_previous_scene(inst->scene_manager);
            }
        } else if(event.event == ToAppEventTypeEmulateStart) {
            vb_migrate_scene_to_app_load_capture(inst, false);
        } else if(event.event == ToAppEventTypeTagWrite) {
            if(vb_migrate_scene_to_app_is_state_changed(inst, state)) {
                if(state == ToAppStateEmulateReady) {
                    notification_message(inst->notifications, &sequence_success);
                    vb_migrate_scene_to_app_load_capture(inst, true);
                    consumed = true;
                }
            }
        } else if(event.event == ToAppEventTypeCaptureLoadError) {
            vb_migrate_scene_to_app_set_state(inst, ToAppStateLoadError);
            consumed = true;
        } else {
            furi_crash("Unknown event in vb_migrate_scene_to_app_on_event");
        }
    }
    return consumed;
}

void vb_migrate_scene_to_app_on_exit(void* context) {
    VbMigrate* inst = context;

    // Perform your cleanup here
    widget_reset(inst->widget);
    nfc_worker_stop(inst->worker);
    vb_migrate_blink_stop(inst);
    notification_message_block(inst->notifications, &sequence_reset_red);
}
