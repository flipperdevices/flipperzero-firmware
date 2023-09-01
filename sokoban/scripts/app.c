/*
Sokoban, by Racso.
https://rac.so

Licensed under the GNU General Public License v3.0: https://www.gnu.org/licenses/gpl-3.0.en.html

Copyright (C) 2023 Óscar F. Gómez

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "app.h"
#include "app_gameplay.h"
#include "scene_menu.h"
#include "scene_game.h"
#include "scene_credits.h"
#include "save_data_manager.h"
#include "wave/scene_management.h"
#include "wave/exception_manager.h"

#include <dolphin/dolphin.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <storage/storage.h>

AppContext* app_alloc()
{
    AppContext* app = malloc(sizeof(AppContext));
    app->gameplay = malloc(sizeof(AppGameplayState));

    app->sceneManager = scene_manager_alloc_auto();
    scene_manager_register_scene(app->sceneManager, SceneType_Menu, scene_alloc(menu_render_callback, NULL, menu_input_callback, menu_transition_callback, app));
    scene_manager_register_scene(app->sceneManager, SceneType_Credits, scene_alloc(credits_render_callback, NULL, credits_input_callback, credits_transition_callback, app));
    scene_manager_register_scene(app->sceneManager, SceneType_Game, scene_alloc(game_render_callback, game_tick_callback, game_handle_input, game_transition_callback, app));

    app->database = levels_database_load();
    levels_database_load_player_progress(app->database);

    return app;
}

void app_free(AppContext* app)
{
    levels_database_free(app->database);

    scene_manager_free(app->sceneManager);

    free(app->gameplay);
    free(app);
}

int32_t app_main(void* p)
{
    UNUSED(p);

    FURI_LOG_D("SOKOBAN", "App started.");

    dolphin_deed(DolphinDeedPluginGameStart);

    furi_assert(false);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message_block(notification, &sequence_display_backlight_enforce_on);

    AppContext* app = app_alloc();

    scene_manager_set_scene(app->sceneManager, SceneType_Menu);

    while (scene_manager_has_scene(app->sceneManager))
    {
        exception_manager_handle_exceptions(app->sceneManager);
        scene_manager_tick(app->sceneManager);
        furi_delay_ms(100);
    }

    app_free(app);

    notification_message(notification, &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);

    return 0;
}