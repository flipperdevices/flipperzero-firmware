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

#define TAG "vb_migrate_scene_from_app"

typedef enum {
    FromAppStateInitial,
    FromAppStateInstructions,
    FromAppStateEmulateReady,
    FromAppStateEmulateCheckDim,
    FromAppStateEmulateTransferFromApp,
    FromAppStateTemplateError,
    FromAppStateSaveError,
} FromAppState;

typedef enum {
    FromAppEventTypeWidgetLeft,
    FromAppEventTypeWidgetRight,
    FromAppEventTypeTemplateLoadError,
    FromAppEventTypeTagWrite,
    FromAppEventTypeCaptureSaveError,
    FromAppEventTypeCaptureSaveSuccess,
} FromAppEventType;

static void
    vb_migrate_scene_from_app_widget_callback(GuiButtonType result, InputType type, void* context) {
    VbMigrate* inst = context;

    if(type == InputTypeShort) {
        if(result == GuiButtonTypeLeft)
            view_dispatcher_send_custom_event(inst->view_dispatcher, FromAppEventTypeWidgetLeft);
        else if(result == GuiButtonTypeRight)
            view_dispatcher_send_custom_event(inst->view_dispatcher, FromAppEventTypeWidgetRight);
    }
}

static bool vb_migrate_scene_from_app_worker_callback(NfcWorkerEvent event, void* context) {
    VbMigrate* inst = context;
    bool result = false;

    if(event == NfcWorkerEventSuccess) {
        view_dispatcher_send_custom_event(inst->view_dispatcher, FromAppEventTypeTagWrite);
        result = true;
    }

    return result;
}

static void vb_migrate_scene_from_app_set_nfc_state(VbMigrate* inst, FromAppState state) {
    BantBlock* bant = vb_tag_get_bant_block(&inst->nfc_dev->dev_data);
    if(state == FromAppStateEmulateReady) {
        vb_tag_set_status(bant, VbTagStatusReady);
        vb_tag_set_operation(bant, VbTagOperationReady);
    } else if(state == FromAppStateEmulateCheckDim) {
        vb_tag_set_status(bant, VbTagStatusReady | VbTagStatusDimReady);
        vb_tag_set_operation(bant, VbTagOperationReady);
    } else if(state == FromAppStateEmulateTransferFromApp) {
        vb_tag_set_status(bant, 0);
        vb_tag_set_operation(bant, VbTagOperationIdle);
    }

    // Override tag type
    if(inst->override_type != inst->orig_type && inst->override_type != VbTagTypeUnknown) {
        vb_tag_set_item_id_no(bant, vb_tag_get_default_product(inst->override_type));
    }
}

static bool vb_migrate_scene_from_app_is_state_changed(VbMigrate* inst, FromAppState state) {
    BantBlock* bant = vb_tag_get_bant_block(&inst->nfc_dev->dev_data);
    VbTagOperation operation = vb_tag_get_operation(bant);

    if(state == FromAppStateEmulateReady) {
        return operation == VbTagOperationCheckDim;
    } else if(state == FromAppStateEmulateCheckDim) {
        return operation == VbTagOperationCheckDim || operation == VbTagOperationReturnFromApp;
    }

    return false;
}

static void vb_migrate_scene_from_app_set_state(VbMigrate* inst, FromAppState state) {
    uint32_t curr_state =
        scene_manager_get_scene_state(inst->scene_manager, VbMigrateSceneFromApp);
    if(state != curr_state) {
        Widget* widget = inst->widget;

        if(state == FromAppStateInstructions) {
            widget_reset(widget);
            widget_add_text_scroll_element(
                widget,
                0,
                0,
                128,
                45,
                "\e#To transfer characters\n"
                "\e#to Flipper:\n"
                "1. Wake up character from\n"
                "storage\n"
                "2. Sync character to Flipper\n"
                "as if it was a Vital Bracelet.\n"
                "Flipper will beep when it is\n"
                "ready for the next step\n"
                "3. The character is\n"
                "automatically saved when the\n"
                "transfer is complete, and will\n"
                "be ready for another transfer\n"
                "4. Repeat the above until you\n"
                "have transferred all the\n"
                "characters you want\n"
                "\n"
                "You can cancel at any time to\n"
                "finish transferring.");
            widget_add_button_element(
                widget,
                GuiButtonTypeLeft,
                "Cancel",
                vb_migrate_scene_from_app_widget_callback,
                inst);
            widget_add_button_element(
                widget,
                GuiButtonTypeRight,
                "Next",
                vb_migrate_scene_from_app_widget_callback,
                inst);

            view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);
        } else if(state == FromAppStateEmulateReady) {
            vb_migrate_show_loading_popup(inst, true);
            if(vb_migrate_load_nfc(inst, inst->text_store, VB_MIGRATE_TEMPLATE_NAME)) {
                widget_reset(widget);
                widget_add_string_multiline_element(
                    widget,
                    80,
                    20,
                    AlignCenter,
                    AlignCenter,
                    FontPrimary,
                    "Ready, waiting\nfor Dim check");
                FuriString* temp_str =
                    furi_string_alloc_printf("Charas. captured: %d", inst->num_captured);
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
                    widget,
                    GuiButtonTypeLeft,
                    "Cancel",
                    vb_migrate_scene_from_app_widget_callback,
                    inst);

                view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);
                furi_string_free(temp_str);

                vb_migrate_scene_from_app_set_nfc_state(inst, state);
                nfc_worker_start(
                    inst->worker,
                    NfcWorkerStateMfUltralightEmulate,
                    &inst->nfc_dev->dev_data,
                    vb_migrate_scene_from_app_worker_callback,
                    inst);
                vb_migrate_blink_emulate(inst);
            } else {
                view_dispatcher_send_custom_event(
                    inst->view_dispatcher, FromAppEventTypeTemplateLoadError);
            }
            vb_migrate_show_loading_popup(inst, false);
        } else if(state == FromAppStateEmulateCheckDim) {
            widget_reset(widget);
            widget_add_string_multiline_element(
                widget,
                80,
                31,
                AlignCenter,
                AlignCenter,
                FontPrimary,
                "Waiting for\nsecond Dim\ncheck/transfer");
            widget_add_icon_element(widget, 7, 18, &I_Touch_26x26);
            widget_add_button_element(
                widget,
                GuiButtonTypeLeft,
                "Cancel",
                vb_migrate_scene_from_app_widget_callback,
                inst);

            view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);
            notification_message(inst->notifications, &sequence_success);
            vb_migrate_scene_from_app_set_nfc_state(inst, state);
            nfc_worker_start(
                inst->worker,
                NfcWorkerStateMfUltralightEmulate,
                &inst->nfc_dev->dev_data,
                vb_migrate_scene_from_app_worker_callback,
                inst);
            vb_migrate_blink_emulate(inst);
        } else if(state == FromAppStateEmulateTransferFromApp) {
            widget_reset(widget);
            widget_add_string_multiline_element(
                widget,
                80,
                31,
                AlignCenter,
                AlignCenter,
                FontPrimary,
                "Transfer captured,\nsaving...");
            widget_add_button_element(
                widget,
                GuiButtonTypeLeft,
                "Cancel",
                vb_migrate_scene_from_app_widget_callback,
                inst);

            view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);
            nfc_worker_stop(inst->worker);
            vb_migrate_blink_stop(inst);
            vb_migrate_scene_from_app_set_nfc_state(inst, state);
            notification_message(inst->notifications, &sequence_success);

            // Restore original tag type if necessary
            if(inst->override_type != inst->orig_type && inst->override_type != VbTagTypeUnknown) {
                BantBlock* bant = vb_tag_get_bant_block(&inst->nfc_dev->dev_data);
                vb_tag_set_item_id_no(bant, inst->orig_product);
            }

            // Save the tag
            inst->next_id = vb_migrate_get_next_id(inst, inst->text_store, inst->next_id, false);
            FuriString* save_path = furi_string_alloc_printf(
                VB_MIGRATE_CAPTURE_FORMAT, inst->next_id, NFC_APP_EXTENSION);
            if(vb_migrate_save_nfc(inst, inst->text_store, furi_string_get_cstr(save_path))) {
                view_dispatcher_send_custom_event(
                    inst->view_dispatcher, FromAppEventTypeCaptureSaveSuccess);
            } else {
                view_dispatcher_send_custom_event(
                    inst->view_dispatcher, FromAppEventTypeCaptureSaveError);
            }
            furi_string_free(save_path);
        } else if(state == FromAppStateTemplateError) {
            widget_reset(widget);
            widget_add_string_multiline_element(
                widget, 80, 31, AlignCenter, AlignCenter, FontPrimary, "Could not\nload template");
            widget_add_icon_element(widget, 15, 22, &I_Error_18x18);
            widget_add_button_element(
                widget,
                GuiButtonTypeLeft,
                "Cancel",
                vb_migrate_scene_from_app_widget_callback,
                inst);

            view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);
            notification_message(inst->notifications, &sequence_error);
            notification_message(inst->notifications, &sequence_set_red_255);
        } else if(state == FromAppStateSaveError) {
            widget_reset(widget);
            widget_add_string_multiline_element(
                widget, 80, 31, AlignCenter, AlignCenter, FontPrimary, "Could not\nsave capture");
            widget_add_icon_element(widget, 15, 22, &I_Error_18x18);
            widget_add_button_element(
                widget,
                GuiButtonTypeLeft,
                "Cancel",
                vb_migrate_scene_from_app_widget_callback,
                inst);

            view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);
            notification_message(inst->notifications, &sequence_error);
            notification_message(inst->notifications, &sequence_set_red_255);
        } else {
            furi_crash("Unknown new state in vb_migrate_scene_from_app_set_state");
        }

        scene_manager_set_scene_state(inst->scene_manager, VbMigrateSceneFromApp, state);
    }
}

void vb_migrate_scene_from_app_on_enter(void* context) {
    VbMigrate* inst = context;

    // Perform your setup here
    inst->next_id = 0;
    scene_manager_set_scene_state(inst->scene_manager, VbMigrateSceneFromApp, FromAppStateInitial);
    vb_migrate_scene_from_app_set_state(inst, FromAppStateInstructions);
}

bool vb_migrate_scene_from_app_on_event(void* context, SceneManagerEvent event) {
    VbMigrate* inst = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == FromAppEventTypeWidgetLeft) {
            consumed = scene_manager_previous_scene(inst->scene_manager);
        } else if(event.event == FromAppEventTypeWidgetRight) {
            vb_migrate_scene_from_app_set_state(inst, FromAppStateEmulateReady);
            consumed = true;
        } else if(event.event == FromAppEventTypeTagWrite) {
            uint32_t state =
                scene_manager_get_scene_state(inst->scene_manager, VbMigrateSceneFromApp);
            if(vb_migrate_scene_from_app_is_state_changed(inst, state)) {
                if(state == FromAppStateEmulateReady) {
                    nfc_worker_stop(inst->worker);
                    vb_migrate_blink_stop(inst);
                    vb_migrate_scene_from_app_set_state(inst, FromAppStateEmulateCheckDim);
                    consumed = true;
                } else if(state == FromAppStateEmulateCheckDim) {
                    BantBlock* bant = vb_tag_get_bant_block(&inst->nfc_dev->dev_data);
                    VbTagOperation operation = vb_tag_get_operation(bant);

                    if(operation == VbTagOperationReturnFromApp) {
                        nfc_worker_stop(inst->worker);
                        vb_migrate_blink_stop(inst);
                        vb_migrate_scene_from_app_set_state(
                            inst, FromAppStateEmulateTransferFromApp);
                        consumed = true;
                    } else if(operation == VbTagOperationCheckDim) {
                        // Don't need to reset tag, but should make a beep
                        vb_migrate_blink_stop(inst);
                        notification_message_block(inst->notifications, &sequence_success);
                        vb_migrate_blink_emulate(inst);
                    }
                }
            }
        } else if(event.event == FromAppEventTypeTemplateLoadError) {
            vb_migrate_scene_from_app_set_state(inst, FromAppStateTemplateError);
            consumed = true;
        } else if(event.event == FromAppEventTypeCaptureSaveError) {
            vb_migrate_scene_from_app_set_state(inst, FromAppStateSaveError);
            consumed = true;
        } else if(event.event == FromAppEventTypeCaptureSaveSuccess) {
            ++inst->num_captured;
            ++inst->next_id;
            vb_migrate_scene_from_app_set_state(inst, FromAppStateEmulateReady);
            consumed = true;
        } else {
            furi_crash("Unknown event in vb_migrate_scene_from_app_on_event");
        }
    }
    return consumed;
}

void vb_migrate_scene_from_app_on_exit(void* context) {
    VbMigrate* inst = context;

    // Perform your cleanup here
    widget_reset(inst->widget);
    nfc_worker_stop(inst->worker);
    vb_migrate_blink_stop(inst);
    notification_message_block(inst->notifications, &sequence_reset_red);
}
