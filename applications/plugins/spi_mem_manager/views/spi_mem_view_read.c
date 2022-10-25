#include "spi_mem_view_read.h"
#include <gui/elements.h>

struct SPIMemReadView {
    View* view;
    SPIMemViewReadCallback callback;
    void* cb_ctx;
};

View* spi_mem_view_read_get_view(SPIMemReadView* app) {
    return app->view;
}

static void spi_mem_view_read_draw_progress(Canvas* canvas, float progress) {
    FuriString* progress_str = furi_string_alloc();
    furi_string_printf(progress_str, "%d %%", (int)(progress * 100));
    elements_progress_bar(canvas, 13, 35, 100, progress);
    canvas_draw_str_aligned(
        canvas, 64, 25, AlignCenter, AlignTop, furi_string_get_cstr(progress_str));
    furi_string_free(progress_str);
}

static void spi_mem_view_read_draw_callback(Canvas* canvas, void* context) {
    canvas_draw_str_aligned(canvas, 64, 4, AlignCenter, AlignTop, "Reading dump");
    spi_mem_view_read_draw_progress(canvas, 0.47);
    elements_button_left(canvas, "Cancel");
    UNUSED(context);
}

static bool spi_mem_view_read_input_callback(InputEvent* event, void* context) {
    SPIMemReadView* app = context;
    bool success = false;
    if(event->type == InputTypeShort && event->key == InputKeyLeft) {
        if(app->callback) {
            app->callback(app->cb_ctx);
        }
        success = true;
    }
    return success;
}

SPIMemReadView* spi_mem_view_read_alloc() {
    SPIMemReadView* app = malloc(sizeof(SPIMemReadView));
    app->view = view_alloc();
    view_set_context(app->view, app);
    view_set_draw_callback(app->view, spi_mem_view_read_draw_callback);
    view_set_input_callback(app->view, spi_mem_view_read_input_callback);
    return app;
}

void spi_mem_view_read_free(SPIMemReadView* app) {
    view_free(app->view);
    free(app);
}

void spi_mem_view_read_set_callback(
    SPIMemReadView* app,
    SPIMemViewReadCallback callback,
    void* cb_ctx) {
    app->callback = callback;
    app->cb_ctx = cb_ctx;
}
