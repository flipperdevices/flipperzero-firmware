#include <stdbool.h>

bool flipbip_has_settings(bool key_file);
bool flipbip_load_settings(char* settings, bool key_file);
bool flipbip_save_settings(const char* settings, bool key_file, bool append);

bool flipbip_load_settings_secure(char* settings);
bool flipbip_save_settings_secure(const char* settings);