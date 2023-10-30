#pragma once

#include "flipboard_model.h"
#include "key_setting_model.h"

bool flipboard_model_save(FlipboardModel* model, KeySettingModelFields fields);
bool flipboard_model_load(FlipboardModel* model);