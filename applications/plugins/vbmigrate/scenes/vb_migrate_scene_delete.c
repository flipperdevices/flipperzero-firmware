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

#include "../vb_migrate_i.h"
#include "../vb_tag.h"

static void
    vb_migrate_scene_delete_widget_callback(GuiButtonType result, InputType type, void* context) {
    VbMigrate* inst = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(inst->view_dispatcher, result);
    }
}

void vb_migrate_scene_delete_on_enter(void* context) {
    VbMigrate* inst = context;
    FuriString* temp_str;

    // Perform your setup here
    BantBlock* bant = vb_tag_get_bant_block(&inst->nfc_dev->dev_data);
    const VbTagProduct* product = vb_tag_find_product(bant);
    if(product)
        temp_str = furi_string_alloc_printf("\e#Delete %s?\e#", product->short_name);
    else
        temp_str = furi_string_alloc_set_str("\e#Delete VB?\e#");

    widget_add_text_box_element(
        inst->widget,
        0,
        0,
        128,
        23,
        AlignCenter,
        AlignCenter,
        furi_string_get_cstr(temp_str),
        false);
    widget_add_button_element(
        inst->widget, GuiButtonTypeLeft, "Cancel", vb_migrate_scene_delete_widget_callback, inst);
    widget_add_button_element(
        inst->widget, GuiButtonTypeRight, "Delete", vb_migrate_scene_delete_widget_callback, inst);

    furi_string_printf(temp_str, "%s\nCharas. captured: %d", inst->text_store, inst->num_captured);
    widget_add_string_multiline_element(
        inst->widget, 64, 24, AlignCenter, AlignTop, FontSecondary, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);
}

bool vb_migrate_scene_delete_on_event(void* context, SceneManagerEvent event) {
    VbMigrate* inst = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = scene_manager_previous_scene(inst->scene_manager);
        } else if(event.event == GuiButtonTypeRight) {
            if(vb_migrate_delete(inst, inst->text_store, true)) {
                scene_manager_next_scene(inst->scene_manager, VbMigrateSceneDeleteSuccess);
                consumed = true;
            } else {
                consumed = scene_manager_search_and_switch_to_previous_scene(
                    inst->scene_manager, VbMigrateSceneSelect);
            }
        }
    }
    return consumed;
}

void vb_migrate_scene_delete_on_exit(void* context) {
    VbMigrate* inst = context;

    // Perform your cleanup here
    widget_reset(inst->widget);
}
