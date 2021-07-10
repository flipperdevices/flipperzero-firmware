#include "storage-i.h"
#include "storage-external-api.h"
#include "storage-message.h"
#include "storage-processing.h"
#include "storages/storage-int.h"
#include "storages/storage-ext.h"

#define STORAGE_TICK 1000

void storage_app_fill_api(StorageApp* app) {
    app->external_api.file.open = s_api_file_open;
    app->external_api.file.close = s_api_file_close;
    app->external_api.file.read = s_api_file_read;
    app->external_api.file.write = s_api_file_write;
    app->external_api.file.seek = s_api_file_seek;
    app->external_api.file.tell = s_api_file_tell;
    app->external_api.file.truncate = s_api_file_truncate;
    app->external_api.file.size = s_api_file_size;
    app->external_api.file.sync = s_api_file_sync;
    app->external_api.file.eof = s_api_file_eof;

    app->external_api.dir.open = s_api_dir_open;
    app->external_api.dir.close = s_api_dir_close;
    app->external_api.dir.read = s_api_dir_read;
    app->external_api.dir.rewind = s_api_dir_rewind;

    app->external_api.common.info = s_api_common_info;
    app->external_api.common.remove = s_api_common_remove;
    app->external_api.common.rename = s_api_common_rename;
    app->external_api.common.mkdir = s_api_common_mkdir;
    app->external_api.common.get_fs_info = s_api_common_get_fs_info;

    app->external_api.error.get_desc = s_api_error_get_desc;
    app->external_api.error.get_internal_desc = s_api_error_get_internal_desc;

    app->external_api.context = app;
}

StorageApp* storage_app_alloc() {
    StorageApp* app = malloc(sizeof(StorageApp));
    app->message_queue = osMessageQueueNew(8, sizeof(StorageMessage), NULL);
    storage_app_fill_api(app);

    for(uint8_t i = 0; i < STORAGE_COUNT; i++) {
        storage_data_init(&app->storage[i]);
    }

    storage_int_init(&app->storage[ST_INT]);
    storage_ext_init(&app->storage[ST_EXT]);

    return app;
}

void storage_tick(StorageApp* app) {
    for(uint8_t i = 0; i < STORAGE_COUNT; i++) {
        StorageApi api = app->storage[i].api;
        if(api.tick != NULL) {
            api.tick(&app->storage[i]);
        }
    }
}

int32_t storage_app(void* p) {
    StorageApp* app = storage_app_alloc();
    furi_record_create("storage", &app->external_api);

    StorageMessage message;
    while(1) {
        if(osMessageQueueGet(app->message_queue, &message, NULL, STORAGE_TICK) == osOK) {
            storage_process_message(app, &message);
        } else {
            storage_tick(app);
        }
    }

    return 0;
}