#pragma once
#include <furi.h>
#include <string.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/file_browser.h>
#include <gui/modules/text_input.h>
#include <storage/storage.h>

void nfc_playlist_text_input_scene_on_enter(void* context);
bool nfc_playlist_text_input_scene_on_event(void* context, SceneManagerEvent event);
void nfc_playlist_text_input_scene_on_exit(void* context);