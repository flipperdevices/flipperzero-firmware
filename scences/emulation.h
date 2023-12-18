#pragma once

#include <furi.h>
#include <string.h>
#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>
#include <nfc_playlist_worker.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/popup.h>
#include <gui/modules/variable_item_list.h>
#include <nfc_playlist.h>

void nfc_playlist_emulation_scene_on_enter(void* context);
bool nfc_playlist_emulation_scene_on_event(void* context, SceneManagerEvent event);
void nfc_playlist_emulation_scene_on_exit(void* context);