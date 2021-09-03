#include "archive_main_view.h"
#include "../archive_i.h"
#include "../helpers/archive_files.h"
#include "../helpers/archive_favorites.h"

#include <furi.h>

static const char* ArchiveTabNames[] = {
    [ArchiveTabFavorites] = "Favorites",
    [ArchiveTabIButton] = "iButton",
    [ArchiveTabNFC] = "NFC",
    [ArchiveTabSubGhz] = "Sub-GHz",
    [ArchiveTabLFRFID] = "RFID LF",
    [ArchiveTabIrda] = "Infrared",
    [ArchiveTabBrowser] = "Browser"};

static const Icon* ArchiveItemIcons[] = {
    [ArchiveFileTypeIButton] = &I_ibutt_10px,
    [ArchiveFileTypeNFC] = &I_Nfc_10px,
    [ArchiveFileTypeSubGhz] = &I_sub1_10px,
    [ArchiveFileTypeLFRFID] = &I_125_10px,
    [ArchiveFileTypeIrda] = &I_ir_10px,
    [ArchiveFileTypeFolder] = &I_dir_10px,
    [ArchiveFileTypeUnknown] = &I_unknown_10px,
};

struct ArchiveMainView {
    View* view;
    ArchiveMainViewCallback callback;
    void* context;

    ArchiveTabEnum tab_id;
    string_t name;
    string_t path;
    char text_input_buffer[MAX_NAME_LEN];

    uint8_t depth;
    uint16_t last_idx[MAX_DEPTH];

    bool menu;
};

typedef struct {
    uint8_t tab_idx;
    uint8_t menu_idx;
    uint16_t idx;
    uint16_t list_offset;
    files_array_t files;
    bool menu;
} ArchiveMainViewModel;

size_t archive_file_array_size(ArchiveMainView* archive_main_view) {
    uint16_t size = 0;
    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            size = files_array_size(model->files);
            return true;
        });
    return size;
}

void archive_file_array_remove_selected(ArchiveMainView* archive_main_view) {
    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            files_array_remove_v(model->files, model->idx, model->idx + 1);
            model->idx = CLAMP(model->idx, files_array_size(model->files) - 1, 0);
            return true;
        });
}

void archive_file_array_clean(ArchiveMainView* archive_main_view) {
    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            files_array_clean(model->files);
            return true;
        });
}

void update_offset(ArchiveMainView* archive_main_view) {
    furi_assert(archive_main_view);

    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            size_t array_size = files_array_size(model->files);
            uint16_t bounds = array_size > 3 ? 2 : array_size;

            if(array_size > 3 && model->idx >= array_size - 1) {
                model->list_offset = model->idx - 3;
            } else if(model->list_offset < model->idx - bounds) {
                model->list_offset = CLAMP(model->list_offset + 1, array_size - bounds, 0);
            } else if(model->list_offset > model->idx - bounds) {
                model->list_offset = CLAMP(model->idx - 1, array_size - bounds, 0);
            }
            return true;
        });
}

static bool filter_by_extension(ArchiveTabEnum tab_id, FileInfo* file_info, const char* name) {
    furi_assert(file_info);
    furi_assert(name);

    bool result = false;
    const char* filter_ext_ptr = get_tab_ext(tab_id);

    if(strcmp(filter_ext_ptr, "*") == 0) {
        result = true;
    } else if(strstr(name, filter_ext_ptr) != NULL) {
        result = true;
    } else if(file_info->flags & FSF_DIRECTORY) {
        result = true;
    }

    return result;
}

static void set_file_type(ArchiveFile_t* file, FileInfo* file_info) {
    furi_assert(file);
    furi_assert(file_info);

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

void archive_view_add_item(
    ArchiveMainView* archive_main_view,
    FileInfo* file_info,
    const char* name) {
    furi_assert(archive_main_view);
    furi_assert(file_info);
    furi_assert(name);

    ArchiveFile_t item;
    ArchiveTabEnum tab_id;
    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            tab_id = model->tab_idx;
            return true;
        });

    if(filter_by_extension(tab_id, file_info, name)) {
        ArchiveFile_t_init(&item);
        string_init_set_str(item.name, name);
        set_file_type(&item, file_info);

        with_view_model(
            archive_main_view->view, (ArchiveMainViewModel * model) {
                files_array_push_back(model->files, item);
                return true;
            });

        ArchiveFile_t_clear(&item);
    }
}

static void render_item_menu(Canvas* canvas, ArchiveMainViewModel* model) {
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 71, 17, 57, 46);
    canvas_set_color(canvas, ColorBlack);
    elements_slightly_rounded_frame(canvas, 70, 16, 58, 48);

    string_t menu[MENU_ITEMS];

    string_init_set_str(menu[0], "Run in app");
    string_init_set_str(menu[1], "Pin");
    string_init_set_str(menu[2], "Rename");
    string_init_set_str(menu[3], "Delete");

    ArchiveFile_t* selected = files_array_get(model->files, model->idx);

    if(!is_known_app(selected->type)) {
        string_set_str(menu[0], "---");
        string_set_str(menu[1], "---");
        string_set_str(menu[2], "---");
    } else if(model->tab_idx == 0 || selected->fav) {
        string_set_str(menu[1], "Unpin");
    }

    for(size_t i = 0; i < MENU_ITEMS; i++) {
        canvas_draw_str(canvas, 82, 27 + i * 11, string_get_cstr(menu[i]));
        string_clear(menu[i]);
    }

    canvas_draw_icon(canvas, 74, 20 + model->menu_idx * 11, &I_ButtonRight_4x7);
}

void archive_trim_file_ext(char* name) {
    size_t str_len = strlen(name);
    char* end = name + str_len;
    while(end > name && *end != '.' && *end != '\\' && *end != '/') {
        --end;
    }
    if((end > name && *end == '.') && (*(end - 1) != '\\' && *(end - 1) != '/')) {
        *end = '\0';
    }
}

static void archive_draw_frame(Canvas* canvas, uint16_t idx, bool scrollbar) {
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_box(canvas, 0, 15 + idx * FRAME_HEIGHT, scrollbar ? 122 : 127, FRAME_HEIGHT);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_dot(canvas, 0, 15 + idx * FRAME_HEIGHT);
    canvas_draw_dot(canvas, 1, 15 + idx * FRAME_HEIGHT);
    canvas_draw_dot(canvas, 0, (15 + idx * FRAME_HEIGHT) + 1);

    canvas_draw_dot(canvas, 0, (15 + idx * FRAME_HEIGHT) + 11);
    canvas_draw_dot(canvas, scrollbar ? 121 : 126, 15 + idx * FRAME_HEIGHT);
    canvas_draw_dot(canvas, scrollbar ? 121 : 126, (15 + idx * FRAME_HEIGHT) + 11);
}

static void draw_list(Canvas* canvas, ArchiveMainViewModel* model) {
    furi_assert(model);

    size_t array_size = files_array_size(model->files);
    bool scrollbar = array_size > 4;

    for(size_t i = 0; i < MIN(array_size, MENU_ITEMS); ++i) {
        string_t str_buff;
        char cstr_buff[MAX_NAME_LEN];

        size_t idx = CLAMP(i + model->list_offset, array_size, 0);
        ArchiveFile_t* file = files_array_get(model->files, CLAMP(idx, array_size - 1, 0));

        string_init_set(str_buff, file->name);
        string_right(str_buff, string_search_rchar(str_buff, '/') + 1);
        strlcpy(cstr_buff, string_get_cstr(str_buff), string_size(str_buff) + 1);

        if(is_known_app(file->type)) archive_trim_file_ext(cstr_buff);

        string_clean(str_buff);
        string_set_str(str_buff, cstr_buff);

        elements_string_fit_width(canvas, str_buff, scrollbar ? MAX_LEN_PX - 6 : MAX_LEN_PX);

        if(model->idx == idx) {
            archive_draw_frame(canvas, i, scrollbar);
        } else {
            canvas_set_color(canvas, ColorBlack);
        }

        canvas_draw_icon(canvas, 2, 16 + i * FRAME_HEIGHT, ArchiveItemIcons[file->type]);
        canvas_draw_str(canvas, 15, 24 + i * FRAME_HEIGHT, string_get_cstr(str_buff));
        string_clear(str_buff);
    }

    if(scrollbar) {
        elements_scrollbar_pos(canvas, 126, 15, 49, model->idx, array_size);
    }

    if(model->menu) {
        render_item_menu(canvas, model);
    }
}

static void archive_render_status_bar(Canvas* canvas, ArchiveMainViewModel* model) {
    furi_assert(model);

    const char* tab_name = ArchiveTabNames[model->tab_idx];

    canvas_draw_icon(canvas, 0, 0, &I_Background_128x11);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, 0, 50, 13);
    canvas_draw_box(canvas, 107, 0, 20, 13);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, 1, 0, 50, 12);
    canvas_draw_line(canvas, 0, 1, 0, 11);
    canvas_draw_line(canvas, 1, 12, 49, 12);
    canvas_draw_str_aligned(canvas, 26, 9, AlignCenter, AlignBottom, tab_name);

    canvas_draw_frame(canvas, 108, 0, 20, 12);
    canvas_draw_line(canvas, 107, 1, 107, 11);
    canvas_draw_line(canvas, 108, 12, 126, 12);

    if(model->tab_idx > 0) {
        canvas_draw_icon(canvas, 112, 2, &I_ButtonLeft_4x7);
    }
    if(model->tab_idx < SIZEOF_ARRAY(ArchiveTabNames) - 1) {
        canvas_draw_icon(canvas, 120, 2, &I_ButtonRight_4x7);
    }

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_dot(canvas, 50, 0);
    canvas_draw_dot(canvas, 127, 0);

    canvas_set_color(canvas, ColorBlack);
}

void archive_view_render(Canvas* canvas, void* model) {
    ArchiveMainViewModel* m = model;

    archive_render_status_bar(canvas, model);

    if(files_array_size(m->files) > 0) {
        draw_list(canvas, m);
    } else {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_WIDTH / 2, 40, AlignCenter, AlignCenter, "Empty");
    }
}

View* archive_main_get_view(ArchiveMainView* archive_main_view) {
    furi_assert(archive_main_view);
    return archive_main_view->view;
}

static void archive_text_input_callback(void* context) {
    furi_assert(context);
    ArchiveMainView* archive_main_view = context;
    Storage* fs_api = furi_record_open("storage");

    string_t buffer_src;
    string_t buffer_dst;

    string_init_printf(
        buffer_src,
        "%s/%s",
        string_get_cstr(archive_main_view->path),
        string_get_cstr(archive_main_view->name));
    string_init_printf(
        buffer_dst,
        "%s/%s",
        string_get_cstr(archive_main_view->path),
        archive_main_view->text_input_buffer);

    string_set(archive_main_view->name, archive_main_view->text_input_buffer);
    // append extension

    ArchiveFile_t* file;

    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            file = files_array_get(
                model->files, CLAMP(model->idx, files_array_size(model->files) - 1, 0));
            file->fav = archive_is_favorite(archive_main_view->path, file->name);

            return true;
        });

    string_cat(buffer_dst, known_ext[file->type]);
    storage_common_rename(fs_api, string_get_cstr(buffer_src), string_get_cstr(buffer_dst));

    if(file->fav) {
        archive_favorites_rename(archive_main_view->path, file->name, buffer_dst);
    }

    // view_dispatcher_switch_to_view(archive->view_dispatcher, ArchiveViewMain);
    archive_get_filenames(archive_main_view, archive_main_view->tab_id, archive_main_view->path);

    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            model->idx = 0;
            while(model->idx < files_array_size(model->files)) {
                ArchiveFile_t* current = files_array_get(model->files, model->idx);
                if(!string_search(current->name, archive_main_view->text_input_buffer)) {
                    break;
                }
                ++model->idx;
            }
            return true;
        });
    furi_record_close("storage");
    update_offset(archive_main_view);

    string_clear(buffer_src);
    string_clear(buffer_dst);
}

static void archive_enter_text_input(void* context) {
    furi_assert(context);
    ArchiveApp* archive = context;
    ArchiveMainView* archive_main_view = archive->archive_main_view;

    *archive_main_view->text_input_buffer = '\0';

    strlcpy(
        archive_main_view->text_input_buffer,
        string_get_cstr(archive_main_view->name),
        MAX_NAME_LEN);

    archive_trim_file_ext(archive_main_view->text_input_buffer);

    text_input_set_header_text(archive->text_input, "Rename:");

    text_input_set_result_callback(
        archive->text_input,
        archive_text_input_callback,
        archive,
        archive_main_view->text_input_buffer,
        MAX_NAME_LEN,
        false);

    // view_dispatcher_switch_to_view(archive->view_dispatcher, ArchiveViewTextInput);
}

static void archive_show_file_menu(ArchiveMainView* archive_main_view) {
    furi_assert(archive_main_view);

    archive_main_view->menu = true;

    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            ArchiveFile_t* selected;
            selected = files_array_get(model->files, model->idx);
            model->menu = true;
            model->menu_idx = 0;
            selected->fav = is_known_app(selected->type) ?
                                archive_is_favorite(archive_main_view->path, selected->name) :
                                false;

            return true;
        });
}

static void archive_close_file_menu(ArchiveMainView* archive_main_view) {
    furi_assert(archive_main_view);

    archive_main_view->menu = false;

    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            model->menu = false;
            model->menu_idx = 0;
            return true;
        });
}

// static void archive_open_app(ArchiveApp* archive, const char* app_name, const char* args) {
//     furi_assert(archive);
//     furi_assert(app_name);

//     loader_start(archive->loader, app_name, args);
// }

// static void
//     archive_run_in_app(ArchiveMainView* archive_main_view, ArchiveFile_t* selected, bool full_path_provided) {
//     string_t full_path;

//     if(!full_path_provided) {
//         string_init_printf(
//             full_path,
//             "%s/%s",
//             string_get_cstr(archive->browser.path),
//             string_get_cstr(selected->name));
//     } else {
//         string_init_set(full_path, selected->name);
//     }

//     archive_open_app(archive_main_view, flipper_app_name[selected->type], string_get_cstr(full_path));
//     string_clear(full_path);
// }

static void archive_file_menu_callback(ArchiveMainView* archive_main_view) {
    furi_assert(archive_main_view);

    ArchiveFile_t* selected;
    uint8_t idx = 0;

    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            selected = files_array_get(model->files, model->idx);
            idx = model->menu_idx;
            return true;
        });

    switch(idx) {
    case 0:
        if(is_known_app(selected->type)) {
            // archive_run_in_app(archive, selected, false);
        }
        break;
    case 1:
        if(is_known_app(selected->type)) {
            if(!archive_is_favorite(archive_main_view->path, selected->name)) {
                string_set(archive_main_view->name, selected->name);
                archive_add_to_favorites(archive_main_view->path, selected->name);
            } else {
                // delete from favorites
                archive_favorites_delete(archive_main_view->path, selected->name);
            }
            archive_close_file_menu(archive_main_view);
        }
        break;
    case 2:
        // open rename view
        if(is_known_app(selected->type)) {
            archive_enter_text_input(archive_main_view);
        }
        break;
    case 3:
        // confirmation?
        archive_delete_file(archive_main_view, archive_main_view->path, selected->name);
        archive_close_file_menu(archive_main_view);

        break;

    default:
        archive_close_file_menu(archive_main_view);
        break;
    }
    selected = NULL;
}

static void menu_input_handler(ArchiveMainView* archive_main_view, InputEvent* event) {
    furi_assert(archive_main_view);

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyUp || event->key == InputKeyDown) {
            with_view_model(
                archive_main_view->view, (ArchiveMainViewModel * model) {
                    if(event->key == InputKeyUp) {
                        model->menu_idx = ((model->menu_idx - 1) + MENU_ITEMS) % MENU_ITEMS;
                    } else if(event->key == InputKeyDown) {
                        model->menu_idx = (model->menu_idx + 1) % MENU_ITEMS;
                    }
                    return true;
                });
        }

        if(event->key == InputKeyOk) {
            archive_file_menu_callback(archive_main_view);
        } else if(event->key == InputKeyBack) {
            archive_close_file_menu(archive_main_view);
        }
    }
}

static void archive_switch_dir(ArchiveMainView* archive_main_view, const char* path) {
    furi_assert(archive_main_view);
    furi_assert(path);

    string_set(archive_main_view->path, path);
    archive_get_filenames(archive_main_view, archive_main_view->tab_id, archive_main_view->path);
    update_offset(archive_main_view);
}

static void archive_switch_tab(ArchiveMainView* archive_main_view) {
    furi_assert(archive_main_view);

    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            model->tab_idx = archive_main_view->tab_id;
            model->idx = 0;

            return true;
        });

    archive_main_view->depth = 0;
    archive_switch_dir(archive_main_view, tab_default_paths[archive_main_view->tab_id]);
}

static void archive_enter_dir(ArchiveMainView* archive_main_view, string_t name) {
    furi_assert(archive_main_view);
    furi_assert(name);

    // update last index
    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            archive_main_view->last_idx[archive_main_view->depth] =
                CLAMP(model->idx, files_array_size(model->files) - 1, 0);
            model->idx = 0;
            return true;
        });

    archive_main_view->depth = CLAMP(archive_main_view->depth + 1, MAX_DEPTH, 0);

    string_cat(archive_main_view->path, "/");
    string_cat(archive_main_view->path, archive_main_view->name);

    archive_switch_dir(archive_main_view, string_get_cstr(archive_main_view->path));
}

static void archive_leave_dir(ArchiveMainView* archive_main_view) {
    furi_assert(archive_main_view);

    char* last_char_ptr = strrchr(string_get_cstr(archive_main_view->path), '/');

    if(last_char_ptr) {
        size_t pos = last_char_ptr - string_get_cstr(archive_main_view->path);
        string_left(archive_main_view->path, pos);
    }

    archive_main_view->depth = CLAMP(archive_main_view->depth - 1, MAX_DEPTH, 0);

    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            model->idx = archive_main_view->last_idx[archive_main_view->depth];
            return true;
        });

    archive_switch_dir(archive_main_view, string_get_cstr(archive_main_view->path));
}

bool archive_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    // ArchiveApp* archive = context;
    ArchiveMainView* archive_main_view = context;

    bool in_menu = archive_main_view->menu;

    if(in_menu) {
        menu_input_handler(archive_main_view, event);
        return true;
    }

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            if(archive_main_view->tab_id > 0) {
                archive_main_view->tab_id =
                    CLAMP(archive_main_view->tab_id - 1, ArchiveTabTotal, 0);
                archive_switch_tab(archive_main_view);
                return true;
            }
        } else if(event->key == InputKeyRight) {
            if(archive_main_view->tab_id < ArchiveTabTotal - 1) {
                archive_main_view->tab_id =
                    CLAMP(archive_main_view->tab_id + 1, ArchiveTabTotal - 1, 0);
                archive_switch_tab(archive_main_view);
                return true;
            }

        } else if(event->key == InputKeyBack) {
            if(archive_main_view->depth == 0) {
                // view_dispatcher_stop(archive->view_dispatcher);
            } else {
                archive_leave_dir(archive_main_view);
            }

            return true;
        }
    }
    if(event->key == InputKeyUp || event->key == InputKeyDown) {
        with_view_model(
            archive_main_view->view, (ArchiveMainViewModel * model) {
                uint16_t num_elements = (uint16_t)files_array_size(model->files);
                if((event->type == InputTypeShort || event->type == InputTypeRepeat)) {
                    if(event->key == InputKeyUp) {
                        model->idx = ((model->idx - 1) + num_elements) % num_elements;
                    } else if(event->key == InputKeyDown) {
                        model->idx = (model->idx + 1) % num_elements;
                    }
                }

                return true;
            });
        update_offset(archive_main_view);
    }

    if(event->key == InputKeyOk) {
        ArchiveFile_t* selected;

        with_view_model(
            archive_main_view->view, (ArchiveMainViewModel * model) {
                selected = files_array_size(model->files) > 0 ?
                               files_array_get(model->files, model->idx) :
                               NULL;
                return true;
            });

        if(selected) {
            string_set(archive_main_view->name, selected->name);

            if(selected->type == ArchiveFileTypeFolder) {
                if(event->type == InputTypeShort) {
                    archive_enter_dir(archive_main_view, archive_main_view->name);
                } else if(event->type == InputTypeLong) {
                    archive_show_file_menu(archive_main_view);
                }
            } else {
                if(event->type == InputTypeShort) {
                    if(archive_main_view->tab_id == ArchiveTabFavorites) {
                        if(is_known_app(selected->type)) {
                            // archive_run_in_app(archive, selected, true);
                        }
                    } else {
                        archive_show_file_menu(archive_main_view);
                    }
                }
            }
        }
    }

    update_offset(archive_main_view);

    return true;
}

ArchiveMainView* archive_main_view_alloc() {
    ArchiveMainView* archive_main_view = furi_alloc(sizeof(ArchiveMainView));

    // View allocation and configuration
    archive_main_view->view = view_alloc();
    view_allocate_model(
        archive_main_view->view, ViewModelTypeLocking, sizeof(ArchiveMainViewModel));
    view_set_context(archive_main_view->view, archive_main_view);
    view_set_draw_callback(archive_main_view->view, (ViewDrawCallback)archive_view_render);
    view_set_input_callback(archive_main_view->view, archive_view_input);
    // view_set_enter_callback(archive_main_view->view,  );
    // view_set_exit_callback(archive_main_view->view,  );
    string_init(archive_main_view->name);
    string_init(archive_main_view->path);

    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            files_array_init(model->files);
            return true;
        });

    archive_switch_tab(archive_main_view);

    return archive_main_view;
}

void archive_main_view_free(ArchiveMainView* archive_main_view) {
    furi_assert(archive_main_view);

    with_view_model(
        archive_main_view->view, (ArchiveMainViewModel * model) {
            files_array_clear(model->files);
            return false;
        });

    string_clear(archive_main_view->name);
    string_clear(archive_main_view->path);

    view_free(archive_main_view->view);
    free(archive_main_view);
}
