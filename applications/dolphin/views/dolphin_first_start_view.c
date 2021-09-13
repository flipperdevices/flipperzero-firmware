#include <furi.h>
#include "../dolphin_i.h"
#include "dolphin_first_start_view.h"

void dolphin_first_start_set_callback(
    DolphinFirstStartView* first_start_view,
    DolphinFirstStartViewCallback callback,
    void* context) {
    furi_assert(first_start_view);
    furi_assert(callback);
    first_start_view->callback = callback;
    first_start_view->context = context;
}

void dolphin_first_start_view_render(Canvas* canvas, void* model) {
    DolphinFirstStartViewModel* m = model;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    uint8_t width = canvas_width(canvas);
    uint8_t height = canvas_height(canvas);
    const char* my_name = furi_hal_version_get_name_ptr();
    if(m->page == 0) {
        canvas_draw_icon(canvas, 0, height - 48, &I_DolphinFirstStart0_70x53);
        elements_multiline_text_framed(canvas, 75, 20, "Hey m8,\npress > to\ncontinue");
    } else if(m->page == 1) {
        canvas_draw_icon(canvas, 0, height - 48, &I_DolphinFirstStart1_59x53);
        elements_multiline_text_framed(canvas, 64, 20, "First Of All,\n...      >");
    } else if(m->page == 2) {
        canvas_draw_icon(canvas, 0, height - 48, &I_DolphinFirstStart2_59x51);
        elements_multiline_text_framed(canvas, 64, 20, "Thank you\nfor your\nsupport! >");
    } else if(m->page == 3) {
        canvas_draw_icon(canvas, width - 57, height - 48, &I_DolphinFirstStart3_57x48);
        elements_multiline_text_framed(canvas, 0, 20, "Kickstarter\ncampaign\nwas INSANE! >");
    } else if(m->page == 4) {
        canvas_draw_icon(canvas, width - 67, height - 50, &I_DolphinFirstStart4_67x53);
        elements_multiline_text_framed(canvas, 0, 17, "Now\nallow me\nto introduce\nmyself >");
    } else if(m->page == 5) {
        char buf[64];
        snprintf(
            buf,
            64,
            "%s %s%s",
            "I am",
            my_name ? my_name : "Unknown",
            ",\ncyberdolphin\nliving in your\npocket >");
        canvas_draw_icon(canvas, 0, height - 48, &I_DolphinFirstStart5_54x49);
        elements_multiline_text_framed(canvas, 60, 17, buf);
    } else if(m->page == 6) {
        canvas_draw_icon(canvas, 0, height - 48, &I_DolphinFirstStart6_58x54);
        elements_multiline_text_framed(
            canvas, 63, 17, "I can grow\nsmart'n'cool\nif you use me\noften >");
    } else if(m->page == 7) {
        canvas_draw_icon(canvas, width - 61, height - 48, &I_DolphinFirstStart7_61x51);
        elements_multiline_text_framed(
            canvas, 0, 17, "As long as\nyou read, write\nand emulate >");
    } else if(m->page == 8) {
        canvas_draw_icon(canvas, width - 56, height - 48, &I_DolphinFirstStart8_56x51);
        elements_multiline_text_framed(
            canvas, 0, 17, "You can check\nmy level and\nmood in the\nPassport menu");
    }
}

View* dolphin_first_start_get_view(DolphinFirstStartView* first_start_view) {
    furi_assert(first_start_view);
    return first_start_view->view;
}

bool dolphin_first_start_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    DolphinFirstStartView* first_start_view = context;

    if(event->type == InputTypeShort) {
        DolphinFirstStartViewModel* model = view_get_model(first_start_view->view);
        if(event->key == InputKeyLeft) {
            if(model->page > 0) model->page--;
        } else if(event->key == InputKeyRight) {
            uint32_t page = ++model->page;
            if(page > 8) {
                first_start_view->callback(DolphinFirstStartCompleted, first_start_view->context);
            }
        }
        view_commit_model(first_start_view->view, true);
    }

    return true;
}

DolphinFirstStartView* dolphin_first_start_view_alloc() {
    DolphinFirstStartView* first_start_view = furi_alloc(sizeof(DolphinFirstStartView));
    first_start_view->view = view_alloc();
    view_allocate_model(
        first_start_view->view, ViewModelTypeLocking, sizeof(DolphinFirstStartViewModel));
    view_set_context(first_start_view->view, first_start_view);
    view_set_draw_callback(
        first_start_view->view, (ViewDrawCallback)dolphin_first_start_view_render);
    view_set_input_callback(first_start_view->view, dolphin_first_start_view_input);

    return first_start_view;
}

void dolphin_first_start_view_free(DolphinFirstStartView* first_start_view) {
    furi_assert(first_start_view);

    view_free(first_start_view->view);
    free(first_start_view);
}
