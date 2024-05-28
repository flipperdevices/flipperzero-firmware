#pragma once

#include <gui/view.h>

typedef struct AppSettings AppSettings;

AppSettings* app_settings_alloc();
void app_settings_free(AppSettings* settings);
View* app_settings_get_view(AppSettings* settings);