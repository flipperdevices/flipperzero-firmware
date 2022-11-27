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
#include "vb_migrate_icons.h"

static void vb_migrate_scene_delete_success_popup_callback(void* context) {
    VbMigrate* inst = context;
    view_dispatcher_send_custom_event(inst->view_dispatcher, 0);
}

void vb_migrate_scene_delete_success_on_enter(void* context) {
    VbMigrate* inst = context;

    // Perform your setup here
    Popup* popup = inst->popup;
    popup_set_icon(popup, 0, 2, &I_DolphinMafia_115x62);
    popup_set_header(popup, "Deleted", 83, 19, AlignLeft, AlignBottom);
    popup_set_timeout(popup, 1500);
    popup_set_context(popup, inst);
    popup_set_callback(popup, vb_migrate_scene_delete_success_popup_callback);
    popup_enable_timeout(popup);

    view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewPopup);
}

bool vb_migrate_scene_delete_success_on_event(void* context, SceneManagerEvent event) {
    VbMigrate* inst = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        uint32_t back_scenes[] = {VbMigrateSceneSelect, VbMigrateSceneMainMenu};
        consumed = scene_manager_search_and_switch_to_previous_scene_one_of(
            inst->scene_manager, back_scenes, COUNT_OF(back_scenes));
    }
    return consumed;
}

void vb_migrate_scene_delete_success_on_exit(void* context) {
    VbMigrate* inst = context;

    // Perform your cleanup here
    popup_reset(inst->popup);
}
