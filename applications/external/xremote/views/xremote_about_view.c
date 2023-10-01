/*!
 *  @file flipper-xremote/views/xremote_about_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief View functionality for version and additional information.
 */

#include "xremote_about_view.h"
#include "../xremote.h"

static void xremote_about_view_draw_vertical(Canvas* canvas, XRemoteViewModel* model) {
    UNUSED(model);
    char version[32];

    canvas_set_font(canvas, FontSecondary);
    xremote_get_version(version, sizeof(version));

    canvas_draw_str_aligned(canvas, 0, 30, AlignLeft, AlignTop, "Version:");
    canvas_draw_str_aligned(canvas, 35, 30, AlignLeft, AlignTop, version);

    canvas_draw_str_aligned(canvas, 0, 40, AlignLeft, AlignTop, "License: GPLv3");
    canvas_draw_str_aligned(canvas, 0, 50, AlignLeft, AlignTop, "Author: kala13x");

    elements_slightly_rounded_frame(canvas, 9, 78, 45, 33);
    canvas_draw_str_aligned(canvas, 0, 69, AlignLeft, AlignTop, "Contact:");
    canvas_draw_str_aligned(canvas, 13, 80, AlignLeft, AlignTop, "s.kalatoz");
    canvas_draw_str_aligned(canvas, 28, 91, AlignLeft, AlignTop, "@");
    canvas_draw_str_aligned(canvas, 11, 100, AlignLeft, AlignTop, "gmail.com");
}

static void xremote_about_view_draw_horizontal(Canvas* canvas, XRemoteViewModel* model) {
    UNUSED(model);
    char version[32];

    canvas_set_font(canvas, FontSecondary);
    xremote_get_version(version, sizeof(version));

    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "Version:");
    canvas_draw_str_aligned(canvas, 35, 0, AlignLeft, AlignTop, version);

    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "License: GPLv3");
    canvas_draw_str_aligned(canvas, 0, 20, AlignLeft, AlignTop, "Author: kala13x");

    elements_slightly_rounded_frame(canvas, 8, 30, 45, 33);
    canvas_draw_str_aligned(canvas, 0, 69, AlignLeft, AlignTop, "Contact:");
    canvas_draw_str_aligned(canvas, 12, 32, AlignLeft, AlignTop, "s.kalatoz");
    canvas_draw_str_aligned(canvas, 27, 43, AlignLeft, AlignTop, "@");
    canvas_draw_str_aligned(canvas, 10, 52, AlignLeft, AlignTop, "gmail.com");
}

static void xremote_about_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    XRemoteViewModel* model = context;
    XRemoteAppContext* app_ctx = model->context;
    XRemoteViewDrawFunction xremote_about_view_draw_body;

    ViewOrientation orientation = app_ctx->app_settings->orientation;
    xremote_about_view_draw_body = orientation == ViewOrientationVertical ?
                                       xremote_about_view_draw_vertical :
                                       xremote_about_view_draw_horizontal;

    xremote_canvas_draw_header(canvas, orientation, "About");
    xremote_about_view_draw_body(canvas, model);
    xremote_canvas_draw_exit_footer(canvas, orientation, "Press to exit");
}

XRemoteView* xremote_about_view_alloc(void* app_ctx) {
    XRemoteView* view = xremote_view_alloc(app_ctx, NULL, xremote_about_view_draw_callback);

    with_view_model(
        xremote_view_get_view(view), XRemoteViewModel * model, { model->context = app_ctx; }, true);

    return view;
}