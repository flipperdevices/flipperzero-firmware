#pragma once
#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>

void nfc_playlist_file_edit_scene_on_enter(void* context);
bool nfc_playlist_file_edit_scene_on_event(void* context, SceneManagerEvent event);
void nfc_playlist_file_edit_scene_on_exit(void* context);