#include <furi.h>
#include "../archive_i.h"
#include "archive_main_view.h"
#include "../helpers/archive_browser.h"

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

void archive_browser_set_callback(
    ArchiveMainView* main_view,
    ArchiveMainViewCallback callback,
    void* context) {
    furi_assert(main_view);
    furi_assert(callback);
    main_view->callback = callback;
    main_view->context = context;
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
    } else if(selected->fav) {
        string_set_str(menu[1], "Unpin");
    } else if(model->tab_idx == ArchiveTabFavorites) {
        string_set_str(menu[1], "Unpin");
        string_set_str(menu[2], "---");
    }

    for(size_t i = 0; i < MENU_ITEMS; i++) {
        canvas_draw_str(canvas, 82, 27 + i * 11, string_get_cstr(menu[i]));
        string_clear(menu[i]);
    }

    canvas_draw_icon(canvas, 74, 20 + model->menu_idx * 11, &I_ButtonRight_4x7);
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

View* archive_main_get_view(ArchiveMainView* main_view) {
    furi_assert(main_view);
    return main_view->view;
}

bool archive_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    ArchiveMainView* main_view = context;

    if(archive_in_file_menu(main_view)) {
        if(event->type == InputTypeShort) {
            if(event->key == InputKeyUp || event->key == InputKeyDown) {
                with_view_model(
                    main_view->view, (ArchiveMainViewModel * model) {
                        if(event->key == InputKeyUp) {
                            model->menu_idx = ((model->menu_idx - 1) + MENU_ITEMS) % MENU_ITEMS;
                        } else if(event->key == InputKeyDown) {
                            model->menu_idx = (model->menu_idx + 1) % MENU_ITEMS;
                        }
                        return true;
                    });
            }

            if(event->key == InputKeyOk) {
                uint8_t idx;
                with_view_model(
                    main_view->view, (ArchiveMainViewModel * model) {
                        idx = model->menu_idx;
                        return true;
                    });

                main_view->callback(file_menu_actions[idx], main_view->context);
            } else if(event->key == InputKeyBack) {
                main_view->callback(ArchiveBrowserEventFileMenuClose, main_view->context);
            }
        }

    } else {
        if(event->type == InputTypeShort) {
            if(event->key == InputKeyLeft || event->key == InputKeyRight) {
                archive_switch_tab(main_view, event->key);
            } else if(event->key == InputKeyBack) {
                main_view->callback(ArchiveBrowserEventExit, main_view->context);
            }
        }
        if(event->key == InputKeyUp || event->key == InputKeyDown) {
            with_view_model(
                main_view->view, (ArchiveMainViewModel * model) {
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
            update_offset(main_view);
        }

        if(event->key == InputKeyOk) {
            ArchiveFile_t* selected = archive_get_current_file(main_view);

            if(selected) {
                archive_set_name(main_view, string_get_cstr(selected->name));

                if(event->type == InputTypeShort) {
                    if(archive_get_tab(main_view) == ArchiveTabFavorites) {
                        main_view->callback(ArchiveBrowserEventFileMenuRun, main_view->context);
                    } else if(selected->type == ArchiveFileTypeFolder) {
                        main_view->callback(ArchiveBrowserEventEnterDir, main_view->context);
                    } else {
                        main_view->callback(ArchiveBrowserEventFileMenuOpen, main_view->context);
                    }
                } else if(event->type == InputTypeLong) {
                    if(archive_get_tab(main_view) == ArchiveTabFavorites) {
                        main_view->callback(ArchiveBrowserEventFileMenuOpen, main_view->context);
                    } else if(selected->type == ArchiveFileTypeFolder) {
                        main_view->callback(ArchiveBrowserEventFileMenuOpen, main_view->context);
                    }
                }
            }
        }
    }

    return true;
}

ArchiveMainView* main_view_alloc() {
    ArchiveMainView* main_view = furi_alloc(sizeof(ArchiveMainView));
    main_view->view = view_alloc();
    view_allocate_model(main_view->view, ViewModelTypeLocking, sizeof(ArchiveMainViewModel));
    view_set_context(main_view->view, main_view);
    view_set_draw_callback(main_view->view, (ViewDrawCallback)archive_view_render);
    view_set_input_callback(main_view->view, archive_view_input);

    string_init(main_view->name);
    string_init(main_view->path);

    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            files_array_init(model->files);
            return true;
        });

    return main_view;
}

void main_view_free(ArchiveMainView* main_view) {
    furi_assert(main_view);

    with_view_model(
        main_view->view, (ArchiveMainViewModel * model) {
            files_array_clear(model->files);
            return false;
        });

    string_clear(main_view->name);
    string_clear(main_view->path);

    view_free(main_view->view);
    free(main_view);
}
