#include "archive_views.h"

const char* ArchiveTabNames[] = {
    "Favorites",
    "iButton",
    "NFC",
    "SubOne",
    "Rfid",
    "Infared",
};

static void draw_list(Canvas* canvas, void* model) {
    furi_assert(model);
    ArchiveViewModelDefault* m = model;

    bool scrollbar = m->file_count > 4;

    for(int i = 0; i < m->file_count; ++i) {
        uint8_t idx = i + m->list_offset;
        canvas_draw_str(canvas, 15, 25 + i * 12, m->filename[idx]->ptr);
        if(m->idx == idx) elements_frame_light(canvas, 0, 15 + i * 12, scrollbar ? 122 : 127, 13);
    }

    if(scrollbar) {
        elements_scrollbar_pos(canvas, 126, 15, 49, m->idx, m->file_count);
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
