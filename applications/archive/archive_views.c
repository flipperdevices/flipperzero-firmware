#include "archive_views.h"

const char* ArchiveTabNames[] = {
    "Favorites",
    "All files",
    "iButton",
    // "NFC",
    // "SubOne",
    // "Rfid",
};

static void draw_list(Canvas* canvas, void* model) {
    furi_assert(model);
    ArchiveViewModelDefault* m = model;
    uint8_t y_pos = m->idx;

    for(int i = 0; i < 4; ++i) {
        uint8_t idx = i + m->list_offset;
        canvas_draw_str(canvas, 15, 25 + i * 12, m->list[idx]);
        if(y_pos == idx) elements_frame_light(canvas, 0, 15 + i * 12, 120, 13);
    }

    elements_scrollbar_pos(canvas, 126, 53, 15, y_pos, 5);
}

void archive_render_status_bar(Canvas* canvas, void* model) {
    furi_assert(model);
    ArchiveViewModelDefault* m = model;

    const char* tab_name = ArchiveTabNames[m->tab_idx];

    canvas_draw_icon_name(canvas, 0, 0, I_Background_128x11);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, 0, 50, 13);
    canvas_draw_box(canvas, 100, 0, 28, 13);

    canvas_set_color(canvas, ColorBlack);
    elements_frame(canvas, 0, 0, 50, 13);
    canvas_draw_str(canvas, 4, 10, tab_name);

    elements_frame(canvas, 100, 0, 27, 13);

    if(m->tab_idx > 0) {
        canvas_draw_icon_name(canvas, 108, 3, I_ButtonLeft_4x7);
    }
    if(m->tab_idx < (sizeof(ArchiveTabNames) / sizeof(ArchiveTabNames[0])) - 1) {
        canvas_draw_icon_name(canvas, 116, 3, I_ButtonRight_4x7);
    }
}

void archive_view_favorites(Canvas* canvas, void* model) {
    ArchiveViewModelDefault* m = model;
    archive_render_status_bar(canvas, model);

    if(m->list[0] != 0) {
        draw_list(canvas, model);
    } else {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_WIDTH / 2, 40, AlignCenter, AlignCenter, "No data");
    }
}

void archive_view_all(Canvas* canvas, void* model) {
    ArchiveViewModelDefault* m = model;
    archive_render_status_bar(canvas, model);

    if(m->list[0] != 0) {
        draw_list(canvas, model);
    } else {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_WIDTH / 2, 40, AlignCenter, AlignCenter, "No data");
    }
}

void archive_view_ibutton(Canvas* canvas, void* model) {
    ArchiveViewModelDefault* m = model;
    archive_render_status_bar(canvas, model);

    if(m->list[0] != 0) {
        draw_list(canvas, model);
    } else {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_WIDTH / 2, 40, AlignCenter, AlignCenter, "No data");
    }
}
