#include "archive_views.h"

static const char* ArchiveTabNames[] =
    {"Favorites", "iButton", "NFC", "SubOne", "Rfid", "Infared", "Browser"};

static const IconName ArchiveItemIcons[] = {
    [FileTypeIButton] = I_ibutt_10px,
    [FileTypeNFC] = I_Nfc_10px,
    [FileTypeSubOne] = I_sub1_10px,
    [FileTypeLFRFID] = I_125_10px,
    [FileTypeIrda] = I_ir_10px,
    [FileTypeFolder] = I_dir_10px,
    [FileTypeUnknown] = I_unknown_10px,

};

static IconName archive_get_file_icon(ArchiveViewModelDefault* m, uint8_t idx) {
    return ArchiveItemIcons[m->files[idx].type];
}

static void draw_list(Canvas* canvas, void* model) {
    furi_assert(model);
    ArchiveViewModelDefault* m = model;

    bool scrollbar = m->file_count > 4;
    string_t short_name_buff;

    for(int i = 0; i < m->file_count; ++i) {
        uint8_t idx = i + m->list_offset;
        size_t str_len = string_size(m->files[idx].name);

        if(str_len) {
            string_init_set(short_name_buff, m->files[idx].name);

            // cut file extensions
            char* buff_ptr = stringi_get_cstr(short_name_buff);
            char* end = buff_ptr + str_len;
            while(end > buff_ptr && *end != '.' && *end != '\\' && *end != '/') {
                --end;
            }
            if((end > buff_ptr && *end == '.') && (*(end - 1) != '\\' && *(end - 1) != '/')) {
                *end = '\0';
            }

            uint16_t str_len_px = canvas_string_width(canvas, string_get_cstr(short_name_buff));
            char* short_name_ptr = stringi_get_cstr(short_name_buff);

            if(str_len_px > MAX_LEN_PX) {
                // adaptive long name shortening
                string_mid(
                    short_name_buff,
                    0,
                    str_len -
                        (size_t)((str_len_px - MAX_LEN_PX) / ((str_len_px / str_len) + 2) + 1));

                string_cat(short_name_buff, "...");
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

            canvas_draw_str(canvas, 15, 24 + i * FRAME_HEIGHT, short_name_ptr);

            string_clear(short_name_buff);
        }
    }

    if(scrollbar) {
        elements_scrollbar_pos(canvas, 126, 16, 48, m->idx, (uint8_t)m->file_count);
    }
}

static void archive_render_status_bar(Canvas* canvas, void* model) {
    furi_assert(model);
    ArchiveViewModelDefault* m = model;

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
    if(m->tab_idx < (sizeof(ArchiveTabNames) / sizeof(ArchiveTabNames[0])) - 1) {
        canvas_draw_icon_name(canvas, 114, 3, I_ButtonRight_4x7);
    }
}

void archive_view_render(Canvas* canvas, void* model) {
    ArchiveViewModelDefault* m = model;
    archive_render_status_bar(canvas, model);

    if(m->file_count > 0) {
        draw_list(canvas, model);
    } else {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_WIDTH / 2, 40, AlignCenter, AlignCenter, "No data");
    }
}
