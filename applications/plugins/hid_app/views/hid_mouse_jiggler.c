#include "hid_mouse_jiggler.h"
#include <gui/elements.h>
#include "../hid.h"

#include "hid_icons.h"

#define TAG "HidMouseJiggler"

struct HidMouseJiggler {
    View* view;
    Hid* hid;
};

typedef struct {
    bool connected;
} HidMouseJigglerModel;

bool running = false;
FuriTimer* timer;

static void hid_mouse_jiggler_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidMouseJigglerModel* model = context;

    // Header
    if(model->connected) {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
    } else {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
    }
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 17, 3, AlignLeft, AlignTop, "Mouse Jiggler");

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text(canvas, AlignLeft, 35, "Press Start\nto jiggle");
    canvas_set_font(canvas, FontSecondary);

    // Ok
    canvas_draw_icon(canvas, 63, 25, &I_Space_65x18);
    if(running) {
        elements_slightly_rounded_box(canvas, 66, 27, 60, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 74, 29, &I_Ok_btn_9x9);
    if(running) {
        elements_multiline_text_aligned(canvas, 91, 36, AlignLeft, AlignBottom, "Stop");
    } else {
        elements_multiline_text_aligned(canvas, 91, 36, AlignLeft, AlignBottom, "Start");
    }
    canvas_set_color(canvas, ColorBlack);

    // Back
    canvas_draw_icon(canvas, 74, 49, &I_Pin_back_arrow_10x8);
    elements_multiline_text_aligned(canvas, 91, 57, AlignLeft, AlignBottom, "Quit");
}

static void hid_mouse_jiggler_process(void* context) {
    furi_assert(context);
    Hid* hid = context;
    if(running) {
        hid_hal_mouse_move(hid, MOUSE_MOVE_SHORT, 0);
        furi_delay_ms(500);
        hid_hal_mouse_move(hid, -MOUSE_MOVE_SHORT, 0);
        furi_delay_ms(500);
    }
}

static bool hid_mouse_jiggler_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidMouseJiggler* hid_mouse_jiggler = context;

    bool consumed = false;

    if(event->key == InputKeyBack) {
        running = false;
        hid_hal_mouse_release_all(hid_mouse_jiggler->hid);
    } else if(event->key == InputKeyOk) {
        running = !running;
        consumed = true;
    }

    return consumed;
}

HidMouseJiggler* hid_mouse_jiggler_alloc(Hid* hid) {
    HidMouseJiggler* hid_mouse_jiggler = malloc(sizeof(HidMouseJiggler));
    hid_mouse_jiggler->view = view_alloc();
    hid_mouse_jiggler->hid = hid;
    view_set_context(hid_mouse_jiggler->view, hid_mouse_jiggler);
    view_allocate_model(
        hid_mouse_jiggler->view, ViewModelTypeLocking, sizeof(HidMouseJigglerModel));
    view_set_draw_callback(hid_mouse_jiggler->view, hid_mouse_jiggler_draw_callback);
    view_set_input_callback(hid_mouse_jiggler->view, hid_mouse_jiggler_input_callback);
    timer = furi_timer_alloc(hid_mouse_jiggler_process, FuriTimerTypePeriodic, hid);

    return hid_mouse_jiggler;
}

void hid_mouse_jiggler_free(HidMouseJiggler* hid_mouse_jiggler) {
    furi_assert(hid_mouse_jiggler);
    furi_timer_stop(timer);
    furi_timer_free(timer);
    view_free(hid_mouse_jiggler->view);
    free(hid_mouse_jiggler);
}

View* hid_mouse_jiggler_get_view(HidMouseJiggler* hid_mouse_jiggler) {
    furi_assert(hid_mouse_jiggler);
    furi_timer_start(timer, furi_kernel_get_tick_frequency());
    return hid_mouse_jiggler->view;
}

void hid_mouse_jiggler_set_connected_status(HidMouseJiggler* hid_mouse_jiggler, bool connected) {
    furi_assert(hid_mouse_jiggler);
    with_view_model(
        hid_mouse_jiggler->view,
        HidMouseJigglerModel * model,
        { model->connected = connected; },
        true);
}
