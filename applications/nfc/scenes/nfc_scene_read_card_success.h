#pragma once

#include <gui/scene_manager.h>

AppScene* nfc_scene_read_card_success_alloc();

void nfc_scene_read_card_success_free(AppScene* scene);
