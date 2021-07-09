#pragma once

#include <gui/scene_manager.h>

AppScene* nfc_scene_read_mifare_ul_success_alloc();

void nfc_scene_read_mifare_ul_success_free(AppScene* scene);
