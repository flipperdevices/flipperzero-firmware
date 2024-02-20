#pragma once
#include <furi.h>
#include "../storage_glue.h"
#include "../storage_sd_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void storage_ext_init(StorageData* storage);
FS_Error sd_mount_card(StorageData* storage, bool notify);
FS_Error sd_unmount_card(StorageData* storage);
FS_Error sd_format_card(StorageData* storage);
FS_Error sd_card_info(StorageData* storage, SDInfo* sd_info);
void storage_mnt_init(StorageData* storage);
FS_Error storage_process_virtual_init(StorageData* image_storage, File* image);
FS_Error storage_process_virtual_format(StorageData* storage);
FS_Error storage_process_virtual_mount(StorageData* storage);
FS_Error storage_process_virtual_unmount(StorageData* storage);
FS_Error storage_process_virtual_quit(StorageData* storage);
#ifdef __cplusplus
}
#endif
