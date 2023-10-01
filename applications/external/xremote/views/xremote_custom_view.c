/*!
 *  @file flipper-xremote/views/xremote_custom_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Custom layout UI and view functionality for remote buttons.
 */

#include "xremote_custom_view.h"
#include "../xremote_app.h"

static void xremote_custom_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    XRemoteViewModel* model = context;
    XRemoteAppContext* app_ctx = model->context;

    ViewOrientation orientation = app_ctx->app_settings->orientation;
    uint64_t x = orientation == ViewOrientationVertical ? 70 : 34;

    xremote_canvas_draw_header(canvas, orientation, "Custom");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, x, "Coming Soon.");
    xremote_canvas_draw_exit_footer(canvas, orientation, "Press to exit");
}

XRemoteView* xremote_custom_view_alloc(void* app_ctx) {
    XRemoteView* view = xremote_view_alloc(app_ctx, NULL, xremote_custom_view_draw_callback);

    with_view_model(
        xremote_view_get_view(view), XRemoteViewModel * model, { model->context = app_ctx; }, true);

    return view;
}