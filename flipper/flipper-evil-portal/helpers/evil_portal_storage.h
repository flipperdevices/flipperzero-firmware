#include "../evil_portal_app_i.h"
#include <flipper_format/flipper_format_i.h>
#include <lib/toolbox/stream/file_stream.h>
#include <stdlib.h>
#include <storage/storage.h>
#include <string.h>

#define EVIL_PORTAL_SETTINGS_FILE_VERSION 1
#define PORTAL_FILE_DIRECTORY_PATH EXT_PATH("apps_data/evil_portal")
#define EVIL_PORTAL_SETTINGS_SAVE_PATH                                         \
  PORTAL_FILE_DIRECTORY_PATH "/evil_portal.conf"
#define EVIL_PORTAL_SETTINGS_SAVE_PATH_TMP EVIL_PORTAL_SETTINGS_SAVE_PATH ".tmp"
#define EVIL_PORTAL_INDEX_SAVE_PATH PORTAL_FILE_DIRECTORY_PATH "/index.html"
#define EVIL_PORTAL_INDEX_SAVE_PATH_TMP EVIL_PORTAL_INDEX_SAVE_PATH ".tmp"
#define EVIL_PORTAL_AP_SAVE_PATH PORTAL_FILE_DIRECTORY_PATH "/ap.txt"
#define EVIL_PORTAL_AP_SAVE_PATH_TMP EVIL_PORTAL_AP_SAVE_PATH ".tmp"
#define EVIL_PORTAL_LOG_SAVE_PATH PORTAL_FILE_DIRECTORY_PATH "/logs"
#define EVIL_PORTAL_LOG_SAVE_PATH_TMP EVIL_PORTAL_LOG_SAVE_PATH ".tmp"
#define EVIL_PORTAL_SETTINGS_HEADER "EvilPortal Config File"
#define EVIL_PORTAL_SETTINGS_KEY_PORTAL "PORTAL"
#define EVIL_PORTAL_SETTINGS_KEY_HAPTIC "Haptic"
#define EVIL_PORTAL_SETTINGS_KEY_LED "Led"
#define EVIL_PORTAL_SETTINGS_KEY_SPEAKER "Speaker"
#define EVIL_PORTAL_SETTINGS_KEY_SAVE_SETTINGS "SaveSettings"
#define EVIL_PORTAL_BUF_SIZE 4092

// void captive_portal_save_settings(void* context);
// void captive_portal_read_settings(void* context);
void evil_portal_read_index_html(void *context);
void evil_portal_read_ap_name(void *context);
void write_logs(char* portal_logs);
char *sequential_file_resolve_path(Storage *storage, const char *dir,
                                   const char *prefix, const char *extension);
