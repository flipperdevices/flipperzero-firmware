#include "file_select.h"
#include <m-string.h>

#define FILENAME_COUNT 4

struct FileSelect {
    // public
    View* view;
    void* context;
    FS_Api* fs_api;
    char* path;

    // private
    uint16_t first_file_index;
};

typedef struct {
    string_t filename[FILENAME_COUNT];
    uint8_t position;
} FileSelectModel;

static void file_select_draw_callback(Canvas* canvas, void* _model) {
}

static bool file_select_input_callback(InputEvent* event, void* context) {
}

FileSelect* file_select_alloc() {
    FileSelect* file_select = furi_alloc(sizeof(FileSelect));
    file_select->view = view_alloc();
    view_set_context(file_select->view, file_select);
    view_allocate_model(file_select->view, ViewModelTypeLockFree, sizeof(FileSelectModel));
    view_set_draw_callback(file_select->view, file_select_draw_callback);
    view_set_input_callback(file_select->view, file_select_input_callback);

    file_select->first_file_index = 0;

    with_view_model(
        file_select->view, (FileSelectModel * model) {
            for(uint8_t i = 0; i < FILENAME_COUNT; i++) {
                string_init(model->filename[i]);
            }
            return false;
        });

    return file_select;
}

void file_select_free(FileSelect* file_select) {
}

void file_select_set_api(FileSelect* file_select, FS_Api* fs_api) {
}

void file_select_set_filter(FileSelect* file_select, char* path, char* extension) {
}

bool file_select_fill_strings(FileSelect* file_select) {
    FileInfo file_info;
    File directory;
    bool result;
    FS_Dir_Api* dir_api = &file_select->fs_api->dir;
    uint8_t string_counter = 0;
    uint16_t file_counter = 0;
    const uint8_t name_length = 100;
    char* name = calloc(name_length, sizeof(char));

    result = dir_api->open(&directory, file_select->path);

    if(!result) {
        dir_api->close(&directory);
        free(name);
        return false;
    }

    while(1) {
        result = dir_api->read(&directory, &file_info, name, name_length);

        if(result) {
            if(directory.error_id == FSE_NOT_EXIST || name[0] == 0) {
                break;
            }

            if(directory.error_id == FSE_OK) {
                if(file_info.flags & FSF_DIRECTORY) {
                } else {
                    if(file_counter >= file_select->first_file_index) {
                        with_view_model(
                            file_select->view, (FileSelectModel * model) {
                                string_set_str(model->filename[string_counter], name);
                                return true;
                            });
                        string_counter++;

                        if(string_counter >= FILENAME_COUNT) {
                            break;
                        }
                    }
                }
                file_counter++;
            } else {
                dir_api->close(&directory);
                free(name);
                return false;
            }
        }
    }

    dir_api->close(&directory);
    free(name);
    return true;
}