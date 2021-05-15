#include "archive.h"

static bool archive_get_filenames(ArchiveState* archive);

static void update_offset(ArchiveState* archive) {
    ArchiveViewModelDefault* model = view_get_model(archive->view_archive_main);
    uint8_t bounds = model->file_count > 4 ? 2 : model->file_count;

    if(model->list_offset < model->idx - bounds) {
        model->list_offset = CLAMP(model->list_offset + 1, model->file_count - bounds, 0);
    }

    if(model->list_offset > model->idx - bounds) {
        model->list_offset = CLAMP(model->idx - 1, model->file_count - bounds, 0);
    }

    view_commit_model(archive->view_archive_main, true);
}

static void archive_switch_dir(ArchiveState* archive, const char* path) {
    const char* tab_ext_filter[] = {
        [ArchiveTabFavorites] = "*",
        [ArchiveTabIButton] = ".ibtn",
        [ArchiveTabNFC] = ".nfc",
        [ArchiveTabSubOne] = ".sub1",
        [ArchiveTabLFRFID] = ".rfid",
        [ArchiveTabIrda] = ".irda",
        [ArchiveTabBrowser] = "*",
    };

    string_init_set_str(archive->tab.ext_filter, tab_ext_filter[archive->tab.id]);
    string_init(archive->tab.path[archive->tab.level]);

    if(archive->tab.level > 0) {
        string_cat(
            archive->tab.path[archive->tab.level],
            archive->tab.path[CLAMP(archive->tab.level - 1, MAX_DEPTH_LEVEL, 0)]);
        string_cat(archive->tab.path[archive->tab.level], "/");
    }

    string_cat(archive->tab.path[archive->tab.level], path);

    ArchiveViewModelDefault* model = view_get_model(archive->view_archive_main);

    memset(model->files, 0, (sizeof(ArchiveFile_t) * FILENAME_COUNT));
    model->idx = 0;
    model->file_count = 0;
    model->first_file_index = 0;
    model->list_offset = 0;

    view_commit_model(archive->view_archive_main, true);
    model = NULL;
    archive_get_filenames(archive);
}

static void archive_switch_tab(ArchiveState* archive) {
    const char* tab_default_paths[] = {
        [ArchiveTabFavorites] = "favorites",
        [ArchiveTabIButton] = "ibutton",
        [ArchiveTabNFC] = "nfc",
        [ArchiveTabSubOne] = "subone",
        [ArchiveTabLFRFID] = "lfrfid",
        [ArchiveTabIrda] = "irda",
        [ArchiveTabBrowser] = "/",
    };

    ArchiveViewModelDefault* model = view_get_model(archive->view_archive_main);
    model->tab_idx = archive->tab.id;
    view_commit_model(archive->view_archive_main, true);
    model = NULL;

    archive->tab.level = 0;
    archive_switch_dir(archive, tab_default_paths[archive->tab.id]);
}

static bool archive_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    ArchiveState* archive = context;

    if(event->type != InputTypeShort) return false;

    if(event->key == InputKeyLeft) {
        archive->tab.id = CLAMP(archive->tab.id - 1, ArchiveTabTotal, 0);
        archive_switch_tab(archive);
        return true;
    } else if(event->key == InputKeyRight) {
        archive->tab.id = CLAMP(archive->tab.id + 1, ArchiveTabTotal - 1, 0);
        archive_switch_tab(archive);
        return true;
    } else if(event->key == InputKeyBack) {
        if(archive->tab.level == 0) {
            AppEvent event;
            event.type = EventTypeExit;
            furi_check(osMessageQueuePut(archive->event_queue, &event, 0, osWaitForever) == osOK);
        } else {
            archive->tab.level = CLAMP(archive->tab.level - 1, MAX_DEPTH_LEVEL, 0);
            archive_switch_dir(archive, string_get_cstr(archive->tab.path[archive->tab.level]));
        }
        return true;
    }

    ArchiveViewModelDefault* model = view_get_model(archive->view_archive_main);

    if(event->key == InputKeyUp) {
        model->idx = CLAMP(model->idx - 1, model->file_count - 1, 0);
        update_offset(archive);
    } else if(event->key == InputKeyDown) {
        model->idx = CLAMP(model->idx + 1, model->file_count - 1, 0);
        update_offset(archive);
    } else if(event->key == InputKeyOk) {
        if(model->files[model->idx].type == FileTypeFolder) {
            archive->tab.level = CLAMP(archive->tab.level + 1, MAX_DEPTH_LEVEL, 0);
            archive_switch_dir(archive, string_get_cstr(model->files[model->idx].name));
        } else {
            // 2do: file interaction menu
        }
    }

    model = NULL;
    return true;
}

static bool filter_extension(ArchiveState* archive, FileInfo* file_info, char* name) {
    bool result = false;
    const char* filter_ext_ptr = string_get_cstr(archive->tab.ext_filter);
    if(strcmp(filter_ext_ptr, "*") == 0) {
        result = true;
    } else if(strstr(name, filter_ext_ptr) != NULL) {
        result = true;
    }
    return result;
}

static void set_file_type(ArchiveFile_t* file, FileInfo* file_info) {
    const char* known_ext[] = {
        [FileTypeIButton] = ".ibtn",
        [FileTypeNFC] = ".nfc",
        [FileTypeSubOne] = ".sub1",
        [FileTypeLFRFID] = ".rfid",
        [FileTypeIrda] = ".irda",
    };

    for(size_t i = 0; i < SIZEOF_ARRAY(known_ext); i++) {
        if(string_search_str(file->name, known_ext[i], 0) != STRING_FAILURE) {
            file->type = i;
            return;
        }
    }

    if(file_info->flags & FSF_DIRECTORY) {
        file->type = FileTypeFolder;
    } else {
        file->type = FileTypeUnknown;
    }
}

static bool archive_get_filenames(ArchiveState* archive) {
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

    result = dir_api->open(&directory, string_get_cstr(archive->tab.path[archive->tab.level]));

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
                        string_set_str(model->files[string_counter].name, name);
                        set_file_type(&model->files[string_counter], &file_info);

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
    model = NULL;

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
    archive_switch_tab(archive);
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
