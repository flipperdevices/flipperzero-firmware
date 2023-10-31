#include "../evil_portal_app_i.h"
#include <flipper_format/flipper_format_i.h>
#include <lib/toolbox/stream/file_stream.h>
#include <stdlib.h>
#include <storage/storage.h>
#include <string.h>

#define PORTAL_FILE_DIRECTORY_PATH EXT_PATH("apps_data/evil_portal")
#define EVIL_PORTAL_INDEX_SAVE_PATH PORTAL_FILE_DIRECTORY_PATH "/index.html"
#define EVIL_PORTAL_AP_SAVE_PATH PORTAL_FILE_DIRECTORY_PATH "/ap.config.txt"
#define EVIL_PORTAL_LOG_SAVE_PATH PORTAL_FILE_DIRECTORY_PATH "/logs"

void evil_portal_read_ap_name(void* context);
void evil_portal_write_ap_name(void* context);
void evil_portal_replace_index_html(Storage* storage, FuriString* path);
void evil_portal_create_html_folder_if_not_exists(Storage* storage);
void write_logs(Storage* storage, FuriString* portal_logs);