#include <stdbool.h>

bool flipbip_load_settings(char* settings);
bool flipbip_save_settings(const char* settings, bool append);

bool flipbip_load_settings_secure(char* settings);
bool flipbip_save_settings_secure(const char* settings);