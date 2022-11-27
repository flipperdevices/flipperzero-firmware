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

typedef enum {
    SaveSuccessEventPopup,
} SaveSuccessEvent;

static void vb_migrate_scene_save_success_popup_callback(void* context) {
    VbMigrate* inst = context;

    view_dispatcher_send_custom_event(inst->view_dispatcher, SaveSuccessEventPopup);
}

void vb_migrate_scene_save_success_on_enter(void* context) {
    VbMigrate* inst = context;

    // Setup view
    Popup* popup = inst->popup;
    popup_set_icon(popup, 32, 5, &I_DolphinNice_96x59);
    popup_set_header(popup, "Saved!", 13, 22, AlignLeft, AlignBottom);
    popup_set_timeout(popup, 1500);
    popup_set_context(popup, inst);
    popup_set_callback(popup, vb_migrate_scene_save_success_popup_callback);
    popup_enable_timeout(popup);

    view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewPopup);
}

bool vb_migrate_scene_save_success_on_event(void* context, SceneManagerEvent event) {
    VbMigrate* inst = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SaveSuccessEventPopup) {
            scene_manager_next_scene(inst->scene_manager, VbMigrateSceneInfo);
            consumed = true;
        }
    }
    return consumed;
}

void vb_migrate_scene_save_success_on_exit(void* context) {
    VbMigrate* inst = context;

    // Clear view
    popup_reset(inst->popup);
}
