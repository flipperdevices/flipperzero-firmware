#include "archive_i.h"

static bool archive_get_filenames(ArchiveApp* archive);

static void update_offset(ArchiveApp* archive) {
    ArchiveViewModel* model = view_get_model(archive->view_archive_main);
    uint8_t bounds = model->file_count > 4 ? 2 : model->file_count;

    if(model->list_offset < model->idx - bounds) {
        model->list_offset = CLAMP(model->list_offset + 1, model->file_count - bounds, 0);
    }

    if(model->list_offset > model->idx - bounds) {
        model->list_offset = CLAMP(model->idx - 1, model->file_count - bounds, 0);
    }

    view_commit_model(archive->view_archive_main, true);
}

static void archive_switch_dir(ArchiveApp* archive, const char* path) {
    // set path
    string_init(archive->tab.path[archive->tab.level]);
    if(archive->tab.level > 0) {
        string_cat(
            archive->tab.path[archive->tab.level],
            archive->tab.path[CLAMP(archive->tab.level - 1, MAX_DEPTH_LEVEL, 0)]);
        string_cat(archive->tab.path[archive->tab.level], "/");
    }

    string_cat(archive->tab.path[archive->tab.level], path);

    // set ext filter
    string_init_set_str(archive->tab.ext_filter, tab_ext_filter[archive->tab.id]);

    ArchiveViewModel* model = view_get_model(archive->view_archive_main);

    memset(model->files, 0, (sizeof(ArchiveFile_t) * FILENAME_COUNT));
    model->idx = 0;
    model->file_count = 0;
    model->first_file_index = 0;
    model->list_offset = 0;

    view_commit_model(archive->view_archive_main, true);
    model = NULL;
    archive_get_filenames(archive);
}

static void archive_switch_tab(ArchiveApp* archive) {
    ArchiveViewModel* model = view_get_model(archive->view_archive_main);
    model->tab_idx = archive->tab.id;
    view_commit_model(archive->view_archive_main, true);
    model = NULL;

    archive->tab.level = 0;
    archive_switch_dir(archive, tab_default_paths[archive->tab.id]);
}

static void archive_open_file_menu(ArchiveApp* archive) {
    archive->tab.menu = true;
    ArchiveViewModel* model = view_get_model(archive->view_archive_main);
    model->menu = true;
    model->menu_idx = 0;
    view_commit_model(archive->view_archive_main, true);
}

static void archive_close_file_menu(ArchiveApp* archive) {
    archive->tab.menu = false;
    ArchiveViewModel* model = view_get_model(archive->view_archive_main);
    model->menu = false;
    model->menu_idx = 0;
    view_commit_model(archive->view_archive_main, true);
}

static void menu_input_handler(ArchiveApp* archive, InputEvent* event) {
    ArchiveViewModel* model = view_get_model(archive->view_archive_main);

    if(event->key == InputKeyUp) {
        model->menu_idx = CLAMP(model->menu_idx - 1, MENU_ITEMS - 1, 0);
    } else if(event->key == InputKeyDown) {
        model->menu_idx = CLAMP(model->menu_idx + 1, MENU_ITEMS - 1, 0);
    } else if(event->key == InputKeyOk) {
        // menu callback
    } else if(event->key == InputKeyBack) {
        archive_close_file_menu(archive);
    }

    view_commit_model(archive->view_archive_main, true);
}

static bool archive_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    ArchiveApp* archive = context;
    ArchiveViewModel* model = view_get_model(archive->view_archive_main);
    bool in_menu = archive->tab.menu;

    if(event->type != InputTypeShort) return false;

    if(in_menu) {
        menu_input_handler(archive, event);
        return true;
    }

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

    if(event->key == InputKeyUp) {
        model->idx = CLAMP(model->idx - 1, model->file_count - 1, 0);
        update_offset(archive);
    } else if(event->key == InputKeyDown) {
        model->idx = CLAMP(model->idx + 1, model->file_count - 1, 0);
        update_offset(archive);
    } else if(event->key == InputKeyOk) {
        if(model->files[model->idx].type == ArchiveFileTypeFolder) {
            archive->tab.level = CLAMP(archive->tab.level + 1, MAX_DEPTH_LEVEL, 0);
            archive_switch_dir(archive, string_get_cstr(model->files[model->idx].name));
        } else {
            archive_open_file_menu(archive);
        }
    }

    model = NULL;
    return true;
}

static bool filter_by_extension(ArchiveApp* archive, FileInfo* file_info, char* name) {
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
    for(size_t i = 0; i < SIZEOF_ARRAY(known_ext); i++) {
        if(string_search_str(file->name, known_ext[i], 0) != STRING_FAILURE) {
            file->type = i;
            return;
        }
    }

    if(file_info->flags & FSF_DIRECTORY) {
        file->type = ArchiveFileTypeFolder;
    } else {
        file->type = ArchiveFileTypeUnknown;
    }
}

static bool archive_get_filenames(ArchiveApp* archive) {
    FileInfo file_info;
    File directory;
    bool result;
    FS_Dir_Api* dir_api = &archive->fs_api->dir;
    uint8_t string_counter = 0;
    uint16_t file_counter = 0;
    uint16_t first_file_index = 0;
    const uint8_t name_length = 100;

    string_t name;
    string_init_printf(name, "%0*d\n", name_length, 0); // is there a better way?

    char* name_ptr = stringi_get_cstr(name);

    ArchiveViewModel* model = view_get_model(archive->view_archive_main);

    first_file_index = model->first_file_index;
    result = dir_api->open(&directory, string_get_cstr(archive->tab.path[archive->tab.level]));

    if(!result) {
        dir_api->close(&directory);
        string_clear(name);
        return false;
    }

    while(1) {
        result = dir_api->read(&directory, &file_info, name_ptr, name_length);

        if(directory.error_id == FSE_NOT_EXIST || name_ptr[0] == 0) {
            break;
        }

        if(result) {
            if(directory.error_id == FSE_OK) {
                if(filter_by_extension(archive, &file_info, name_ptr)) {
                    if(file_counter >= first_file_index) {
                        string_set(model->files[string_counter].name, name);
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
                string_clear(name);
                return false;
            }
        }
    }
    model->file_count = file_counter;
    view_commit_model(archive->view_archive_main, true);
    model = NULL;

    dir_api->close(&directory);
    string_clear(name);
    return true;
}

void archive_free(ArchiveApp* archive) {
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

ArchiveApp* archive_alloc() {
    ArchiveApp* archive = furi_alloc(sizeof(ArchiveApp));
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
        archive->view_archive_main, ViewModelTypeLockFree, sizeof(ArchiveViewModel));
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
    ArchiveApp* archive = archive_alloc();

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
