#ifndef __POKEMON_EXIT_CONFIRM_H__
#define __POKEMON_EXIT_CONFIRM_H__

#pragma once

#include <gui/modules/dialog_ex.h>
#include <gui/scene_manager.h>

void pokemon_exit_confirm_dialog_callback(DialogExResult result, void* context);

void pokemon_exit_confirm_on_enter(void* context);

bool pokemon_exit_confirm_on_event(void* context, SceneManagerEvent event);

#endif // __POKEMON_EXIT_CONFIRM_H__
