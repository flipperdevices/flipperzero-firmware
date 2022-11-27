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

void vb_migrate_scene_about_on_enter(void* context) {
    VbMigrate* inst = context;

    // Perform your setup here
    FuriString* temp_str = furi_string_alloc_printf(
        "\e#Information\n"
        "Version: %s\n"
        "Developed by: cyanic\n"
        "GitHub: https://github.com/GMMan/flipperzero-vb-migrate\n"
        "\n"
        "\e#Description\n"
        "Makes transferring\n"
        "characters with VB Lab less\n"
        "cumbersome",
        VB_MIGRATE_VERSION);

    Widget* widget = inst->widget;
    widget_add_text_box_element(
        widget,
        0,
        0,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!                                                      \e!\n",
        false);
    widget_add_text_box_element(
        widget,
        0,
        2,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!  VB Migration Assistant \e!\n",
        false);
    widget_add_text_scroll_element(widget, 0, 16, 128, 50, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(inst->view_dispatcher, VbMigrateViewWidget);
}

bool vb_migrate_scene_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void vb_migrate_scene_about_on_exit(void* context) {
    VbMigrate* inst = context;

    // Perform your cleanup here
    widget_reset(inst->widget);
}
