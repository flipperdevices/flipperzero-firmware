#include <stdbool.h>
#include <stddef.h>

typedef enum {
    FlipBipFileDat,
    FlipBipFileKey,
    FlipBipFileOther,
} FlipBipFile;

bool flipbip_has_file(const FlipBipFile file_type, const char* file_name, const bool remove);
bool flipbip_load_file(
    char* settings,
    size_t slen,
    const FlipBipFile file_type,
    const char* file_name);
bool flipbip_save_file(
    const char* settings,
    const FlipBipFile file_type,
    const char* file_name,
    const bool append);

bool flipbip_save_qrfile(
    const char* qr_msg_prefix,
    const char* qr_msg_content,
    const char* file_name);

bool flipbip_load_file_secure(char* settings);
bool flipbip_save_file_secure(const char* settings);
