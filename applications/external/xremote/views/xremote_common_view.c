/*!
 *  @file flipper-xremote/views/xremote_common_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Common view and canvas functionality shared between the pages.
 */

#include "xremote_common_view.h"
#include "../xremote_app.h"

typedef struct {
    int index;
    const char* name;
} XRemoteButton;

static const XRemoteButton g_buttons[XREMOTE_BUTTON_COUNT + 1] = {
    {0, XREMOTE_COMMAND_POWER},
    {1, XREMOTE_COMMAND_SETUP},
    {2, XREMOTE_COMMAND_INPUT},
    {3, XREMOTE_COMMAND_MENU},
    {4, XREMOTE_COMMAND_LIST},
    {5, XREMOTE_COMMAND_INFO},
    {6, XREMOTE_COMMAND_BACK},
    {7, XREMOTE_COMMAND_OK},
    {8, XREMOTE_COMMAND_UP},
    {9, XREMOTE_COMMAND_DOWN},
    {10, XREMOTE_COMMAND_LEFT},
    {11, XREMOTE_COMMAND_RIGHT},
    {12, XREMOTE_COMMAND_JUMP_FORWARD},
    {13, XREMOTE_COMMAND_JUMP_BACKWARD},
    {14, XREMOTE_COMMAND_FAST_FORWARD},
    {15, XREMOTE_COMMAND_FAST_BACKWARD},
    {16, XREMOTE_COMMAND_PLAY_PAUSE},
    {17, XREMOTE_COMMAND_PAUSE},
    {18, XREMOTE_COMMAND_PLAY},
    {19, XREMOTE_COMMAND_STOP},
    {20, XREMOTE_COMMAND_MUTE},
    {21, XREMOTE_COMMAND_MODE},
    {22, XREMOTE_COMMAND_VOL_UP},
    {23, XREMOTE_COMMAND_VOL_DOWN},
    {24, XREMOTE_COMMAND_NEXT_CHAN},
    {25, XREMOTE_COMMAND_PREV_CHAN},
    {-1, NULL}};

const char* xremote_button_get_name(int index) {
    if(index > XREMOTE_BUTTON_COUNT) return NULL;
    return g_buttons[index].name;
}

struct XRemoteView {
    XRemoteClearCallback on_clear;
    XRemoteAppContext* app_ctx;
    View* view;
    void* context;
};

XRemoteView*
    xremote_view_alloc(void* app_ctx, ViewInputCallback input_cb, ViewDrawCallback draw_cb) {
    XRemoteView* remote_view = malloc(sizeof(XRemoteView));
    remote_view->app_ctx = app_ctx;
    remote_view->view = view_alloc();

    remote_view->context = NULL;
    remote_view->on_clear = NULL;

    view_set_orientation(
        remote_view->view, ((XRemoteAppContext*)app_ctx)->app_settings->orientation);
    view_allocate_model(remote_view->view, ViewModelTypeLocking, sizeof(XRemoteViewModel));

    view_set_input_callback(remote_view->view, input_cb);
    view_set_draw_callback(remote_view->view, draw_cb);
    view_set_context(remote_view->view, remote_view);

    return remote_view;
}

void xremote_view_clear_context(XRemoteView* rview) {
    furi_assert(rview);

    if(rview->context && rview->on_clear) rview->on_clear(rview->context);

    rview->context = NULL;
}

void xremote_view_set_context(XRemoteView* rview, void* context, XRemoteClearCallback on_clear) {
    xremote_view_clear_context(rview);
    rview->context = context;
    rview->on_clear = on_clear;
}

void* xremote_view_get_context(XRemoteView* rview) {
    furi_assert(rview);
    return rview->context;
}

void* xremote_view_get_app_context(XRemoteView* rview) {
    furi_assert(rview);
    return rview->app_ctx;
}

void xremote_view_free(XRemoteView* rview) {
    furi_assert(rview);
    xremote_view_clear_context(rview);
    view_free(rview->view);
    free(rview);
}

View* xremote_view_get_view(XRemoteView* rview) {
    furi_assert(rview);
    return rview->view;
}

InfraredRemoteButton* xremote_view_get_button_by_name(XRemoteView* rview, const char* name) {
    xremote_app_assert(rview->context, NULL);
    InfraredRemote* remote = (InfraredRemote*)rview->context;
    return infrared_remote_get_button_by_name(remote, name);
}

bool xremote_view_press_button(XRemoteView* rview, InfraredRemoteButton* button) {
    xremote_app_assert(button, false);
    XRemoteAppSettings* settings = rview->app_ctx->app_settings;

    InfraredSignal* signal = infrared_remote_button_get_signal(button);
    xremote_app_assert(signal, false);

    infrared_signal_transmit_times(signal, settings->repeat_count);
    xremote_app_context_notify_led(rview->app_ctx);

    return true;
}

bool xremote_view_send_ir_msg_by_name(XRemoteView* rview, const char* name) {
    InfraredRemoteButton* button = xremote_view_get_button_by_name(rview, name);
    return (button != NULL) ? xremote_view_press_button(rview, button) : false;
}

void xremote_canvas_draw_icon(Canvas* canvas, uint8_t x, uint8_t y, XRemoteIcon icon) {
    if(icon == XRemoteIconEnter) {
        canvas_draw_circle(canvas, x - 2, y, 4);
        canvas_draw_disc(canvas, x - 2, y, 2);
    } else if(icon == XRemoteIconBack) {
        canvas_draw_triangle(canvas, x - 4, y - 2, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_line(canvas, x + 2, y + 1, x + 1, y + 2);
        canvas_draw_line(canvas, x + 2, y, x + 1, y - 1);
        canvas_draw_line(canvas, x, y - 2, x - 5, y - 2);
        canvas_draw_line(canvas, x, y + 3, x - 3, y + 3);
    } else if(icon == XRemoteIconArrowUp) {
        canvas_draw_triangle(canvas, x - 2, y - 2, 5, 3, CanvasDirectionBottomToTop);
        canvas_draw_line(canvas, x - 2, y - 3, x - 2, y + 4);
    } else if(icon == XRemoteIconArrowDown) {
        canvas_draw_triangle(canvas, x - 2, y + 2, 5, 3, CanvasDirectionTopToBottom);
        canvas_draw_line(canvas, x - 2, y - 4, x - 2, y + 3);
    } else if(icon == XRemoteIconArrowLeft) {
        canvas_draw_triangle(canvas, x - 4, y, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_line(canvas, x + 2, y, x - 5, y);
    } else if(icon == XRemoteIconArrowRight) {
        canvas_draw_triangle(canvas, x, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_line(canvas, x - 6, y, x + 1, y);
    } else if(icon == XRemoteIconJumpForward) {
        canvas_draw_triangle(canvas, x - 2, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_triangle(canvas, x - 5, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_line(canvas, x + 1, y - 2, x + 1, y + 2);
        canvas_draw_line(canvas, x - 4, y, x, y);
    } else if(icon == XRemoteIconJumpBackward) {
        canvas_draw_triangle(canvas, x - 2, y, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_triangle(canvas, x + 1, y, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_line(canvas, x - 5, y - 2, x - 5, y + 2);
        canvas_draw_line(canvas, x, y, x - 4, y);
    } else if(icon == XRemoteIconFastForward) {
        canvas_draw_triangle(canvas, x - 1, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_triangle(canvas, x - 4, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_line(canvas, x - 3, y, x, y);
    } else if(icon == XRemoteIconFastBackward) {
        canvas_draw_triangle(canvas, x - 3, y, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_triangle(canvas, x, y, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_line(canvas, x - 1, y, x - 4, y);
    } else if(icon == XRemoteIconPlayPause) {
        canvas_draw_triangle(canvas, x - 5, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_dot(canvas, x - 4, y);
        canvas_draw_line(canvas, x - 1, y - 2, x - 1, y + 2);
        canvas_draw_line(canvas, x + 1, y - 2, x + 1, y + 2);
    } else if(icon == XRemoteIconPlay) {
        canvas_draw_triangle(canvas, x - 3, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_dot(canvas, x - 2, y);
    } else if(icon == XRemoteIconPause) {
        canvas_draw_line(canvas, x - 3, y - 2, x - 3, y + 2);
        canvas_draw_line(canvas, x - 1, y - 2, x - 1, y + 2);
    } else if(icon == XRemoteIconStop) {
        canvas_draw_box(canvas, x - 4, y - 2, 5, 5);
    } else if(icon == XRemoteIconOk) {
        canvas_draw_str(canvas, x - 7, y + 4, "OK");
    }
}

void xremote_canvas_draw_header(Canvas* canvas, ViewOrientation orient, const char* section) {
    Align align = AlignLeft;
    uint8_t x = 0;

    if(orient == ViewOrientationHorizontal) {
        align = AlignRight;
        x = 128;
    }

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, x, 0, align, AlignTop, "XRemote");
    canvas_set_font(canvas, FontSecondary);

    if(section != NULL) elements_multiline_text_aligned(canvas, x, 12, align, AlignTop, section);
}

void xremote_canvas_draw_exit_footer(Canvas* canvas, ViewOrientation orient, const char* text) {
    canvas_set_font(canvas, FontSecondary);

    if(orient == ViewOrientationVertical) {
        xremote_canvas_draw_icon(canvas, 6, 124, XRemoteIconBack);
        elements_multiline_text_aligned(canvas, 12, 128, AlignLeft, AlignBottom, text);
    } else {
        uint8_t x = strncmp(text, "Hold", 4) ? 71 : 76;
        xremote_canvas_draw_icon(canvas, x, 60, XRemoteIconBack);
        elements_multiline_text_aligned(canvas, 128, 64, AlignRight, AlignBottom, text);
    }
}

void xremote_canvas_draw_button(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    XRemoteIcon icon) {
    canvas_draw_icon(canvas, x, y, &I_Button_18x18);

    if(pressed) {
        elements_slightly_rounded_box(canvas, x + 3, y + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }

    xremote_canvas_draw_icon(canvas, x + 11, y + 8, icon);
    canvas_set_color(canvas, ColorBlack);
}

void xremote_canvas_draw_button_png(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    const Icon* icon) {
    canvas_draw_icon(canvas, x, y, &I_Button_18x18);

    if(pressed) {
        elements_slightly_rounded_box(canvas, x + 3, y + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }

    canvas_draw_icon(canvas, x + 4, y + 3, icon);
    canvas_set_color(canvas, ColorBlack);
}

void xremote_canvas_draw_button_wide(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    char* text,
    XRemoteIcon icon) {
    elements_slightly_rounded_frame(canvas, x + 4, y, 56, 15);

    if(pressed) {
        elements_slightly_rounded_box(canvas, x + 6, y + 2, 52, 11);
        canvas_set_color(canvas, ColorWhite);
    }

    xremote_canvas_draw_icon(canvas, x + 15, y + 7, icon);
    elements_multiline_text_aligned(canvas, x + 22, y + 10, AlignLeft, AlignBottom, text);
    canvas_set_color(canvas, ColorBlack);
}

void xremote_canvas_draw_button_size(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    uint8_t xy,
    char* text,
    XRemoteIcon icon) {
    elements_slightly_rounded_frame(canvas, x + 4, y, xy, 15);

    if(pressed) {
        elements_slightly_rounded_box(canvas, x + 6, y + 2, xy - 4, 11);
        canvas_set_color(canvas, ColorWhite);
    }

    xremote_canvas_draw_icon(canvas, x + 15, y + 7, icon);
    elements_multiline_text_aligned(canvas, x + 22, y + 10, AlignLeft, AlignBottom, text);
    canvas_set_color(canvas, ColorBlack);
}

void xremote_canvas_draw_frame(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    uint8_t xl,
    const char* text) {
    elements_slightly_rounded_frame(canvas, x, y, xl, 15);

    if(pressed) {
        elements_slightly_rounded_box(canvas, x + 2, y + 2, xl - 4, 11);
        canvas_set_color(canvas, ColorWhite);
    }

    canvas_draw_str(canvas, x + 3, y + 11, text);
    canvas_set_color(canvas, ColorBlack);
}
