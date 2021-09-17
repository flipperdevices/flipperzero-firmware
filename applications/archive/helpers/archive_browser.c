#include "archive_browser.h"

void update_offset(ArchiveMainView* main_view) {
    furi_assert(main_view);
    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            size_t array_size = files_array_size(model->files);
            uint16_t bounds = array_size > 3 ? 2 : array_size;

            if(array_size > 3 && model->idx >= array_size - 1) {
                model->list_offset = model->idx - 3;
            } else if(model->list_offset < model->idx - bounds) {
                model->list_offset = CLAMP(model->idx - 2, array_size - bounds, 0);
            } else if(model->list_offset > model->idx - bounds) {
                model->list_offset = CLAMP(model->idx - 1, array_size - bounds, 0);
            }
            return true;
        });
}

size_t archive_file_array_size(ArchiveMainView* main_view) {
    uint16_t size = 0;
    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            size = files_array_size(model->files);
            return true;
        });
    return size;
}

void archive_file_array_remove_selected(ArchiveMainView* main_view) {
    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            files_array_remove_v(model->files, model->idx, model->idx + 1);
            model->idx = CLAMP(model->idx, files_array_size(model->files) - 1, 0);
            return true;
        });

    update_offset(main_view);
}

void archive_file_array_clean(ArchiveMainView* main_view) {
    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            files_array_clean(model->files);
            return true;
        });
}

ArchiveFile_t* archive_get_current_file(ArchiveMainView* main_view) {
    ArchiveFile_t* selected;
    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            selected = files_array_size(model->files) > 0 ?
                           files_array_get(model->files, model->idx) :
                           NULL;
            return true;
        });
    return selected;
}

ArchiveTabEnum archive_get_tab(ArchiveMainView* main_view) {
    ArchiveTabEnum tab_id;
    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            tab_id = model->tab_idx;
            return true;
        });
    return tab_id;
}

void archive_set_tab(ArchiveMainView* main_view, ArchiveTabEnum tab) {
    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            model->tab_idx = tab;
            return true;
        });
}

uint8_t archive_get_depth(ArchiveMainView* main_view) {
    uint8_t depth;
    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            depth = model->depth;
            return true;
        });

    return depth;
}

const char* archive_get_path(ArchiveMainView* main_view) {
    return string_get_cstr(main_view->path);
}

const char* archive_get_name(ArchiveMainView* main_view) {
    ArchiveFile_t* selected = archive_get_current_file(main_view);
    return string_get_cstr(selected->name);
}

void archive_set_name(ArchiveMainView* main_view, const char* name) {
    furi_assert(main_view);
    furi_assert(name);

    string_set(main_view->name, name);
}

void archive_view_add_item(ArchiveMainView* main_view, FileInfo* file_info, const char* name) {
    furi_assert(main_view);
    furi_assert(file_info);
    furi_assert(name);

    ArchiveFile_t item;

    if(filter_by_extension(file_info, get_tab_ext(archive_get_tab(main_view)), name)) {
        ArchiveFile_t_init(&item);
        string_init_set_str(item.name, name);
        set_file_type(&item, file_info);

        with_view_model(
            main_view->view, (ArchiveMainViewModel * model) {
                files_array_push_back(model->files, item);
                return true;
            });

        ArchiveFile_t_clear(&item);
    }
}

void archive_show_file_menu(ArchiveMainView* main_view, bool show) {
    furi_assert(main_view);
    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            model->menu = show;
            model->menu_idx = 0;

            if(show) {
                ArchiveFile_t* selected;
                selected = files_array_get(model->files, model->idx);
                selected->fav =
                    is_known_app(selected->type) ?
                        archive_is_favorite(
                            string_get_cstr(main_view->path), string_get_cstr(selected->name)) :
                        false;
            }

            return true;
        });
}

bool archive_in_file_menu(ArchiveMainView* main_view) {
    furi_assert(main_view);
    bool in_menu;
    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            in_menu = model->menu;
            return true;
        });

    return in_menu;
}

void archive_switch_dir(ArchiveMainView* main_view, const char* path) {
    furi_assert(main_view);
    furi_assert(path);

    string_set(main_view->path, path);
    archive_get_filenames(main_view, archive_get_tab(main_view), string_get_cstr(main_view->path));

    if(archive_file_array_size(main_view)) {
        update_offset(main_view);
    }
}

void archive_switch_tab(ArchiveMainView* main_view, InputKey key) {
    furi_assert(main_view);

    ArchiveTabEnum tab = archive_get_tab(main_view);

    if(key == InputKeyLeft) {
        tab = ((tab - 1) + ArchiveTabTotal) % ArchiveTabTotal;
    } else if(key == InputKeyRight) {
        tab = (tab + 1) % ArchiveTabTotal;
    }
    archive_set_tab(main_view, tab);

    if((tab != ArchiveTabFavorites && !archive_dir_empty(main_view, tab_default_paths[tab])) ||
       (tab == ArchiveTabFavorites && !archive_favorites_count(main_view))) {
        archive_switch_tab(main_view, key);
    } else {
        archive_switch_dir(main_view, tab_default_paths[archive_get_tab(main_view)]);
        with_view_model(
            main_view->view, (ArchiveMainViewModel * model) {
                model->idx = 0;
                model->depth = 0;
                return true;
            });
    }
}
void archive_enter_dir(ArchiveMainView* main_view, string_t name) {
    furi_assert(main_view);
    furi_assert(name);

    // update last index
    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            model->last_idx[model->depth] =
                CLAMP(model->idx, files_array_size(model->files) - 1, 0);
            model->idx = 0;
            model->depth = CLAMP(model->depth + 1, MAX_DEPTH, 0);
            return true;
        });

    string_cat(main_view->path, "/");
    string_cat(main_view->path, main_view->name);

    archive_switch_dir(main_view, string_get_cstr(main_view->path));
}

void archive_leave_dir(ArchiveMainView* main_view) {
    furi_assert(main_view);

    char* last_char_ptr = strrchr(string_get_cstr(main_view->path), '/');

    if(last_char_ptr) {
        size_t pos = last_char_ptr - string_get_cstr(main_view->path);
        string_left(main_view->path, pos);
    }

    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            model->depth = CLAMP(model->depth - 1, MAX_DEPTH, 0);
            model->idx = model->last_idx[model->depth];
            return true;
        });

    archive_switch_dir(main_view, string_get_cstr(main_view->path));
}

void archive_browser_update(ArchiveMainView* main_view) {
    furi_assert(main_view);

    archive_get_filenames(main_view, archive_get_tab(main_view), string_get_cstr(main_view->path));

    if(!archive_file_array_size(main_view) && !archive_get_depth(main_view)) {
        archive_switch_tab(main_view, DEFAULT_TAB_DIR);
    } else {
        with_view_model(
            main_view->view, (ArchiveMainViewModel * model) {
                uint16_t idx = 0;
                while(idx < files_array_size(model->files)) {
                    ArchiveFile_t* current = files_array_get(model->files, idx);
                    if(!string_search(current->name, string_get_cstr(main_view->name))) {
                        model->idx = idx;
                        break;
                    }
                    ++idx;
                }
                return true;
            });

        update_offset(main_view);
    }
}