#include <stdbool.h>

typedef enum {
    FlipBipFileDat,
    FlipBipFileKey,
    FlipBipFileOther,
} FlipBipFile;

bool flipbip_has_settings(const FlipBipFile file_type, const char* file_name);
bool flipbip_load_settings(char* settings, const FlipBipFile file_type, const char* file_name);
bool flipbip_save_settings(
    const char* settings,
    const FlipBipFile file_type,
    const char* file_name,
    const bool append);

bool flipbip_load_settings_secure(char* settings);
bool flipbip_save_settings_secure(const char* settings);