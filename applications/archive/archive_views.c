#include "archive_views.h"

static const char* ArchiveTabNames[] =
    {"Favorites", "iButton", "NFC", "SubOne", "Rfid", "Infared", "Browser"};

static const IconName ArchiveItemIcons[] = {
    [ArchiveFileTypeIButton] = I_ibutt_10px,
    [ArchiveFileTypeNFC] = I_Nfc_10px,
    [ArchiveFileTypeSubOne] = I_sub1_10px,
    [ArchiveFileTypeLFRFID] = I_125_10px,
    [ArchiveFileTypeIrda] = I_ir_10px,
    [ArchiveFileTypeFolder] = I_dir_10px,
    [ArchiveFileTypeUnknown] = I_unknown_10px,

};

static const char* test_menu[] = {"Activate", "Open app", "Move", "Delete"};

static IconName archive_get_file_icon(ArchiveViewModel* m, uint8_t idx) {
    return ArchiveItemIcons[m->files[idx].type];
}

static void trim_file_ext(string_t name) {
    size_t str_len = string_size(name);
    char* buff_ptr = stringi_get_cstr(name);
    char* end = buff_ptr + str_len;
    while(end > buff_ptr && *end != '.' && *end != '\\' && *end != '/') {
        --end;
    }
    if((end > buff_ptr && *end == '.') && (*(end - 1) != '\\' && *(end - 1) != '/')) {
        *end = '\0';
    }
}

static void render_item_menu(Canvas* canvas, ArchiveViewModel* model) {
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 61, 17, 60, 46);
    canvas_set_color(canvas, ColorBlack);
    elements_slightly_rounded_frame(canvas, 60, 16, 62, 48);

    for(size_t i = 0; i < MENU_ITEMS; i++) {
        canvas_draw_str(canvas, 72, 27 + i * 11, test_menu[i]);
    }

    canvas_draw_icon_name(canvas, 64, 20 + model->menu_idx * 11, I_ButtonRight_4x7);
}

static void draw_list(Canvas* canvas, void* model) {
    furi_assert(model);
    ArchiveViewModel* m = model;

    bool scrollbar = m->file_count > 4;
    string_t str_buff;

    for(int i = 0; i < m->file_count; ++i) {
        uint8_t idx = i + m->list_offset;
        size_t s_len = string_size(m->files[idx].name);

        if(s_len) {
            string_init_set(str_buff, m->files[idx].name);
            trim_file_ext(str_buff);

            uint16_t len_px = canvas_string_width(canvas, string_get_cstr(str_buff));
            char* str_ptr = stringi_get_cstr(str_buff);

            if(len_px > MAX_LEN_PX) {
                string_mid(
                    str_buff,
                    0,
                    s_len - (size_t)((len_px - MAX_LEN_PX) / ((len_px / s_len) + 2) + 1));
                string_cat(str_buff, "...");
            }

            if(m->idx == idx) {
                canvas_set_color(canvas, ColorBlack);
                canvas_draw_box(
                    canvas, 0, 15 + i * FRAME_HEIGHT, scrollbar ? 122 : 127, FRAME_HEIGHT);

                canvas_set_color(canvas, ColorWhite);
                canvas_draw_dot(canvas, 0, 15 + i * FRAME_HEIGHT);
                canvas_draw_dot(canvas, 1, 15 + i * FRAME_HEIGHT);
                canvas_draw_dot(canvas, 0, (15 + i * FRAME_HEIGHT) + 1);

                canvas_draw_dot(canvas, 0, (15 + i * FRAME_HEIGHT) + 11);
                canvas_draw_dot(canvas, scrollbar ? 121 : 126, 15 + i * FRAME_HEIGHT);
                canvas_draw_dot(canvas, scrollbar ? 121 : 126, (15 + i * FRAME_HEIGHT) + 11);

            } else {
                canvas_set_color(canvas, ColorBlack);
            }

            canvas_draw_icon_name(canvas, 2, 16 + i * FRAME_HEIGHT, archive_get_file_icon(m, idx));
            canvas_draw_str(canvas, 15, 24 + i * FRAME_HEIGHT, str_ptr);

            string_clear(str_buff);
        }
    }

    if(m->menu) {
        render_item_menu(canvas, m);
    }

    if(scrollbar) {
        elements_scrollbar_pos(canvas, 126, 16, 48, m->idx, (uint8_t)m->file_count);
    }
}

static void archive_render_status_bar(Canvas* canvas, void* model) {
    furi_assert(model);
    ArchiveViewModel* m = model;

    const char* tab_name = ArchiveTabNames[m->tab_idx];

    canvas_draw_icon_name(canvas, 0, 0, I_Background_128x11);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, 0, 50, 13);
    canvas_draw_box(canvas, 100, 0, 28, 13);

    canvas_set_color(canvas, ColorBlack);
    elements_frame(canvas, 0, 0, 50, 13);
    canvas_draw_str_aligned(canvas, 25, 10, AlignCenter, AlignBottom, tab_name);

    elements_frame(canvas, 100, 0, 24, 13);

    if(m->tab_idx > 0) {
        canvas_draw_icon_name(canvas, 106, 3, I_ButtonLeft_4x7);
    }
    if(m->tab_idx < SIZEOF_ARRAY(ArchiveTabNames) - 1) {
        canvas_draw_icon_name(canvas, 114, 3, I_ButtonRight_4x7);
    }
}

void archive_view_render(Canvas* canvas, void* model) {
    ArchiveViewModel* m = model;
    archive_render_status_bar(canvas, model);

    if(m->file_count > 0) {
        draw_list(canvas, model);
    } else {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_WIDTH / 2, 40, AlignCenter, AlignCenter, "No data");
    }
}
