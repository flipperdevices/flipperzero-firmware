#include <storage\storage-sd-api.h>

int32_t storage_settings(void* p) {
    StorageSDApi* sd_api = furi_record_open("storage-sd");

    furi_record_close("storage-sd");
    return 0;
}