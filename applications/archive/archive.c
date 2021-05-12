#include "archive.h"

bool archive_get_filenames(ArchiveState* archive);

static View* archive_get_tab_view(ArchiveState* archive) {
    View* tabs[] = {
        [ArchiveTabFavorites] = archive->view_favorite_items,
        [ArchiveTabIButton] = archive->view_ibutton_keys,
        [ArchiveTabNFC] = archive->view_nfc_keys,
        [ArchiveTabSubOne] = archive->view_subone_keys,
        [ArchiveTabLFRFID] = archive->view_lfrfid_keys,
        [ArchiveTabIrda] = archive->view_irda_keys,
    };

    return tabs[archive->tab.id];
}

static void update_offset(ArchiveState* archive) {
    View* current = archive_get_tab_view(archive);
    ArchiveViewModelDefault* model = view_get_model(current);
    uint8_t bounds = model->file_count > 3 ? 2 : model->file_count;

    if(model->list_offset < model->idx - bounds) {
        model->list_offset = CLAMP(model->list_offset + 1, model->file_count - bounds, 0);
    }

    if(model->list_offset > model->idx - bounds) {
        model->list_offset = CLAMP(model->idx - 1, model->file_count - bounds, 0);
    }

    view_commit_model(current, true);
}

void archive_switch_dir(ArchiveState* archive) {
    const char* paths[] = {
        [ArchiveTabFavorites] = "favorites",
        [ArchiveTabIButton] = "ibutton",
        [ArchiveTabNFC] = "nfc",
        [ArchiveTabSubOne] = "subone",
        [ArchiveTabLFRFID] = "lfrfid",
        [ArchiveTabIrda] = "irda",
    };

    archive->tab.extension = "*";
    archive->tab.path = paths[archive->tab.id];

    archive_get_filenames(archive);

    View* current = archive_get_tab_view(archive);
    ArchiveViewModelDefault* model = view_get_model(current);

    model->tab_idx = archive->tab.id;

    view_commit_model(current, true);
}

bool archive_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    ArchiveState* archive = context;

    if(event->type != InputTypeShort) return false;

    if(event->key == InputKeyLeft) {
        archive->tab.id = CLAMP(archive->tab.id - 1, ArchiveTabTotal, 0);
        archive_switch_dir(archive);
        view_dispatcher_switch_to_view(archive->view_dispatcher, archive->tab.id);
    } else if(event->key == InputKeyRight) {
        archive->tab.id = CLAMP(archive->tab.id + 1, ArchiveTabTotal - 1, 0);
        archive_switch_dir(archive);
        view_dispatcher_switch_to_view(archive->view_dispatcher, archive->tab.id);
    }

    if(event->key == InputKeyUp) {
        View* current = archive_get_tab_view(archive);
        ArchiveViewModelDefault* model = view_get_model(current);

        model->idx = CLAMP(model->idx - 1, model->file_count - 1, 0);
        update_offset(archive);
        view_commit_model(current, true);
    }

    if(event->key == InputKeyDown) {
        View* current = archive_get_tab_view(archive);
        ArchiveViewModelDefault* model = view_get_model(current);

        model->idx = CLAMP(model->idx + 1, model->file_count - 1, 0);
        update_offset(archive);
        view_commit_model(current, true);
    }

    return true;
}

bool filter_extension(ArchiveState* archive, FileInfo* file_info, char* name) {
    bool result = false;

    if(!(file_info->flags & FSF_DIRECTORY)) {
        if(strcmp(archive->tab.extension, "*") == 0) {
            result = true;
        } else if(strstr(name, archive->tab.extension) != NULL) {
            result = true;
        }
    }

    return result;
}

bool archive_get_filenames(ArchiveState* archive) {
    FileInfo file_info;
    File directory;
    bool result;
    FS_Dir_Api* dir_api = &archive->fs_api->dir;
    uint8_t string_counter = 0;
    uint16_t file_counter = 0;
    const uint8_t name_length = 100;
    char* name = calloc(name_length, sizeof(char));
    uint16_t first_file_index = 0;

    View* current = archive_get_tab_view(archive);
    ArchiveViewModelDefault* model = view_get_model(current);

    first_file_index = model->first_file_index;

    if(name == NULL) {
        return false;
    }

    result = dir_api->open(&directory, archive->tab.path);

    if(!result) {
        dir_api->close(&directory);
        free(name);
        return false;
    }

    while(1) {
        result = dir_api->read(&directory, &file_info, name, name_length);

        if(directory.error_id == FSE_NOT_EXIST || name[0] == 0) {
            break;
        }

        if(result) {
            if(directory.error_id == FSE_OK) {
                if(filter_extension(archive, &file_info, name)) {
                    if(file_counter >= first_file_index) {
                        string_set_str(model->filename[string_counter], name);

                        string_counter++;

                        if(string_counter >= FILENAME_COUNT) {
                            break;
                        }
                    }
                    file_counter++;
                }
            } else {
                dir_api->close(&directory);
                free(name);
                return false;
            }
        }
    }
    model->file_count = file_counter;
    view_commit_model(current, true);
    dir_api->close(&directory);
    free(name);
    return true;
}

ArchiveState* archive_alloc() {
    ArchiveState* archive = furi_alloc(sizeof(ArchiveState));
    // Message queue
    archive->event_queue = osMessageQueueNew(2, sizeof(AppEvent), NULL);

    furi_check(archive->event_queue);
    // Scene thread
    archive->app_thread = furi_thread_alloc();
    // GUI
    archive->gui = furi_record_open("gui");
    // Dispatcher
    archive->view_dispatcher = view_dispatcher_alloc();

    archive->fs_api = furi_record_open("sdcard");

    archive->view_favorite_items = view_alloc();
    view_allocate_model(
        archive->view_favorite_items, ViewModelTypeLockFree, sizeof(ArchiveViewModelDefault));
    view_set_context(archive->view_favorite_items, archive);
    view_set_draw_callback(archive->view_favorite_items, archive_view_render);
    view_set_input_callback(archive->view_favorite_items, archive_view_input);
    view_dispatcher_add_view(
        archive->view_dispatcher, ArchiveTabFavorites, archive->view_favorite_items);

    archive->view_ibutton_keys = view_alloc();
    view_set_context(archive->view_ibutton_keys, archive);
    view_allocate_model(
        archive->view_ibutton_keys, ViewModelTypeLockFree, sizeof(ArchiveViewModelDefault));
    view_set_draw_callback(archive->view_ibutton_keys, archive_view_render);
    view_set_input_callback(archive->view_ibutton_keys, archive_view_input);
    view_dispatcher_add_view(
        archive->view_dispatcher, ArchiveTabIButton, archive->view_ibutton_keys);

    archive->view_nfc_keys = view_alloc();
    view_set_context(archive->view_nfc_keys, archive);
    view_allocate_model(
        archive->view_nfc_keys, ViewModelTypeLockFree, sizeof(ArchiveViewModelDefault));
    view_set_draw_callback(archive->view_nfc_keys, archive_view_render);
    view_set_input_callback(archive->view_nfc_keys, archive_view_input);
    view_dispatcher_add_view(archive->view_dispatcher, ArchiveTabNFC, archive->view_nfc_keys);

    archive->view_subone_keys = view_alloc();
    view_set_context(archive->view_subone_keys, archive);
    view_allocate_model(
        archive->view_subone_keys, ViewModelTypeLockFree, sizeof(ArchiveViewModelDefault));
    view_set_draw_callback(archive->view_subone_keys, archive_view_render);
    view_set_input_callback(archive->view_subone_keys, archive_view_input);
    view_dispatcher_add_view(
        archive->view_dispatcher, ArchiveTabSubOne, archive->view_subone_keys);

    archive->view_lfrfid_keys = view_alloc();
    view_set_context(archive->view_lfrfid_keys, archive);
    view_allocate_model(
        archive->view_lfrfid_keys, ViewModelTypeLockFree, sizeof(ArchiveViewModelDefault));
    view_set_draw_callback(archive->view_lfrfid_keys, archive_view_render);
    view_set_input_callback(archive->view_lfrfid_keys, archive_view_input);
    view_dispatcher_add_view(
        archive->view_dispatcher, ArchiveTabLFRFID, archive->view_lfrfid_keys);

    archive->view_irda_keys = view_alloc();
    view_set_context(archive->view_irda_keys, archive);
    view_allocate_model(
        archive->view_irda_keys, ViewModelTypeLockFree, sizeof(ArchiveViewModelDefault));
    view_set_draw_callback(archive->view_irda_keys, archive_view_render);
    view_set_input_callback(archive->view_irda_keys, archive_view_input);
    view_dispatcher_add_view(archive->view_dispatcher, ArchiveTabIrda, archive->view_irda_keys);

    view_dispatcher_attach_to_gui(
        archive->view_dispatcher, archive->gui, ViewDispatcherTypeFullscreen);

    return archive;
}

void archive_free(ArchiveState* archive) {
    furi_assert(archive);

    furi_record_close("sdcard");
    archive->fs_api = NULL;

    view_dispatcher_free(archive->view_dispatcher);

    furi_record_close("gui");
    archive->gui = NULL;

    furi_thread_free(archive->app_thread);

    osMessageQueueDelete(archive->event_queue);

    free(archive);
}

static void event_cb(const void* value, void* ctx) {
    furi_assert(value);
    furi_assert(ctx);
    const InputEvent* event = value;
    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        osThreadFlagsSet((osThreadId_t)ctx, EXIT_FLAG);
    }
}

int32_t app_archive(void* p) {
    ArchiveState* archive = archive_alloc();

    PubSub* event_record = furi_record_open("input_events");
    PubSubItem* event_pubsub = subscribe_pubsub(event_record, event_cb, (void*)osThreadGetId());

    // default tab
    archive_switch_dir(archive);
    ArchiveViewModelDefault* model = view_get_model(archive->view_favorite_items);
    model->tab_idx = archive->tab.id;
    view_commit_model(archive->view_favorite_items, true);

    view_dispatcher_switch_to_view(archive->view_dispatcher, archive->tab.id);

    while(1) {
        if(osThreadFlagsWait(EXIT_FLAG, osFlagsWaitAny, osWaitForever)) {
            break;
        }
    }

    unsubscribe_pubsub(event_pubsub);
    furi_record_close("input_events");
    event_record = NULL;
    event_pubsub = NULL;

    archive_free(archive);
    return 0;
}
