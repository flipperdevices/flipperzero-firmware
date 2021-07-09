#pragma once

#include <gui/scene_manager.h>

AppScene* nfc_scene_start_alloc();

void nfc_scene_start_free(AppScene* scene);
