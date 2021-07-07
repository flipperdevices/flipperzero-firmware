#include "filesystem-i.h"
#include "filesystem-external-api.h"
#include "filesystem-message.h"

void filesystem_app_fill_api(FilesystemApp* app) {
    app->external_api.file.open = fs_api_file_open;
    app->external_api.file.close = fs_api_file_close;
    app->external_api.file.read = fs_api_file_read;
    app->external_api.file.write = fs_api_file_write;
    app->external_api.file.seek = fs_api_file_seek;
    app->external_api.file.tell = fs_api_file_tell;
    app->external_api.file.truncate = fs_api_file_truncate;
    app->external_api.file.size = fs_api_file_size;
    app->external_api.file.sync = fs_api_file_sync;
    app->external_api.file.eof = fs_api_file_eof;

    app->external_api.dir.open = fs_api_dir_open;
    app->external_api.dir.close = fs_api_dir_close;
    app->external_api.dir.read = fs_api_dir_read;
    app->external_api.dir.rewind = fs_api_dir_rewind;

    app->external_api.common.info = fs_api_common_info;
    app->external_api.common.remove = fs_api_common_remove;
    app->external_api.common.rename = fs_api_common_rename;
    app->external_api.common.mkdir = fs_api_common_mkdir;
    app->external_api.common.get_fs_info = fs_api_common_get_fs_info;

    app->external_api.error.get_desc = fs_api_error_get_desc;
    app->external_api.error.get_internal_desc = fs_api_error_get_internal_desc;
}

FilesystemApp* filesystem_app_alloc() {
    FilesystemApp* app = malloc(sizeof(FilesystemApp));
    app->message_queue = osMessageQueueNew(8, sizeof(FilesystemMessage), NULL);
    filesystem_app_fill_api(app);
    return app;
}

int32_t filesystem(void* p) {
    FilesystemApp* app = filesystem_app_alloc();

    fs_api_set_filesystem_app(app);

    return 0;
}