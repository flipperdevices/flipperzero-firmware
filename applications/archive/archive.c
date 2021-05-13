#include "archive.h"

bool archive_get_filenames(ArchiveState* archive);

static void update_offset(ArchiveState* archive) {
    ArchiveViewModelDefault* model = view_get_model(archive->view_archive_main);
    uint8_t bounds = model->file_count > 3 ? 2 : model->file_count;

    if(model->list_offset < model->idx - bounds) {
        model->list_offset = CLAMP(model->list_offset + 1, model->file_count - bounds, 0);
    }

    if(model->list_offset > model->idx - bounds) {
        model->list_offset = CLAMP(model->idx - 1, model->file_count - bounds, 0);
    }

    view_commit_model(archive->view_archive_main, true);
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

    ArchiveViewModelDefault* model = view_get_model(archive->view_archive_main);

    memset(model->filename, 0, (sizeof(string_t) * FILENAME_COUNT));
    model->idx = 0;
    model->file_count = 0;
    model->first_file_index = 0;
    model->list_offset = 0;

    model->tab_idx = archive->tab.id;
    view_commit_model(archive->view_archive_main, true);
    archive_get_filenames(archive);
}

bool archive_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    ArchiveState* archive = context;

    if(event->type != InputTypeShort) return false;

    if(event->key == InputKeyLeft) {
        archive->tab.id = CLAMP(archive->tab.id - 1, ArchiveTabTotal, 0);
        archive_switch_dir(archive);
    } else if(event->key == InputKeyRight) {
        archive->tab.id = CLAMP(archive->tab.id + 1, ArchiveTabTotal - 1, 0);
        archive_switch_dir(archive);
    }

    if(event->key == InputKeyUp) {
        ArchiveViewModelDefault* model = view_get_model(archive->view_archive_main);
        model->idx = CLAMP(model->idx - 1, model->file_count - 1, 0);
        update_offset(archive);

        view_commit_model(archive->view_archive_main, true);
    } else if(event->key == InputKeyDown) {
        ArchiveViewModelDefault* model = view_get_model(archive->view_archive_main);
        model->idx = CLAMP(model->idx + 1, model->file_count - 1, 0);
        update_offset(archive);
        view_commit_model(archive->view_archive_main, true);
    }

    if(event->key == InputKeyBack) {
        AppEvent event;
        event.type = EventTypeExit;
        furi_check(osMessageQueuePut(archive->event_queue, &event, 0, osWaitForever) == osOK);
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

    ArchiveViewModelDefault* model = view_get_model(archive->view_archive_main);

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
    view_commit_model(archive->view_archive_main, true);
    dir_api->close(&directory);
    free(name);
    return true;
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

    archive->view_archive_main = view_alloc();
    view_allocate_model(
        archive->view_archive_main, ViewModelTypeLockFree, sizeof(ArchiveViewModelDefault));
    view_set_context(archive->view_archive_main, archive);
    view_set_draw_callback(archive->view_archive_main, archive_view_render);
    view_set_input_callback(archive->view_archive_main, archive_view_input);
    view_dispatcher_add_view(
        archive->view_dispatcher, ArchiveTabFavorites, archive->view_archive_main);

    view_dispatcher_attach_to_gui(
        archive->view_dispatcher, archive->gui, ViewDispatcherTypeFullscreen);

    return archive;
}

int32_t app_archive(void* p) {
    ArchiveState* archive = archive_alloc();

    // default tab
    archive_switch_dir(archive);
    view_dispatcher_switch_to_view(archive->view_dispatcher, archive->tab.id);

    AppEvent event;
    while(1) {
        furi_check(osMessageQueueGet(archive->event_queue, &event, NULL, osWaitForever) == osOK);
        if(event.type == EventTypeExit) {
            break;
        }
    }

    archive_free(archive);
    return 0;
}
