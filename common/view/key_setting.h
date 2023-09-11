#pragma once

#include <furi.h>
#include <gui/modules/variable_item_list.h>

typedef struct KeySetting KeySetting;

KeySetting* key_setting_alloc();
void key_setting_free(KeySetting* key_setting);