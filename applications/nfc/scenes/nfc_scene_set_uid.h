#pragma once

#include <gui/scene_manager.h>

AppScene* nfc_scene_set_uid_alloc();

void nfc_scene_set_uid_free(AppScene* scene);
