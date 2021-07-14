#include "storage-i.h"
#include "storage-external-api.h"
#include "storage-message.h"
#include "storage-processing.h"
#include "storages/storage-int.h"
#include "storages/storage-ext.h"

#define STORAGE_TICK 1000

static void storage_app_sd_icon_draw_callback(Canvas* canvas, void* context) {
    furi_assert(canvas);
    furi_assert(context);
    StorageApp* app = context;

    switch(app->storage[ST_EXT].status) {
    case StorageStatusNotReady:
        break;
    case StorageStatusOK:
        canvas_draw_icon(canvas, 0, 0, app->sd_gui.icon_mounted);
        break;
    default:
        canvas_draw_icon(canvas, 0, 0, app->sd_gui.icon_error);
        break;
    }
}

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

    app->external_api.common.stat = s_api_common_stat;
    app->external_api.common.remove = s_api_common_remove;
    app->external_api.common.rename = s_api_common_rename;
    app->external_api.common.mkdir = s_api_common_mkdir;
    app->external_api.common.fs_info = s_api_common_fs_info;

    app->external_api.error.get_desc = s_api_error_get_desc;

    app->external_api.context = app;

    app->sd_api.format = s_api_sd_format;
    app->sd_api.unmount = s_api_sd_unmount;
    app->sd_api.info = s_api_sd_info;
    app->sd_api.status = s_api_sd_status;
    app->sd_api.context = app;
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

    // sd icon gui
    app->sd_gui.enabled = false;
    app->sd_gui.view_port = view_port_alloc();
    app->sd_gui.icon_mounted = assets_icons_get(I_SDcardMounted_11x8);
    app->sd_gui.icon_error = assets_icons_get(I_SDcardFail_11x8);
    view_port_set_width(app->sd_gui.view_port, icon_get_width(app->sd_gui.icon_mounted));
    view_port_draw_callback_set(app->sd_gui.view_port, storage_app_sd_icon_draw_callback, app);
    view_port_enabled_set(app->sd_gui.view_port, false);

    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, app->sd_gui.view_port, GuiLayerStatusBarLeft);
    furi_record_close("gui");

    return app;
}

void storage_tick(StorageApp* app) {
    for(uint8_t i = 0; i < STORAGE_COUNT; i++) {
        StorageApi api = app->storage[i].api;
        if(api.tick != NULL) {
            api.tick(&app->storage[i]);
        }
    }

    if(app->storage[ST_EXT].status == StorageStatusNotReady && app->sd_gui.enabled == true) {
        app->sd_gui.enabled = false;
        view_port_enabled_set(app->sd_gui.view_port, false);
    }

    if((app->storage[ST_EXT].status == StorageStatusOK ||
        app->storage[ST_EXT].status == StorageStatusNotMounted ||
        app->storage[ST_EXT].status == StorageStatusNoFS ||
        app->storage[ST_EXT].status == StorageStatusNotAccessible ||
        app->storage[ST_EXT].status == StorageStatusErrorInternal) &&
       app->sd_gui.enabled == false) {
        app->sd_gui.enabled = true;
        view_port_enabled_set(app->sd_gui.view_port, true);
    }
}

int32_t storage_app(void* p) {
    StorageApp* app = storage_app_alloc();
    furi_record_create("storage", &app->external_api);
    furi_record_create("storage-sd", &app->sd_api);

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