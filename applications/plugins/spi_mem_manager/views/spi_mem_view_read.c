#include "spi_mem_view_read.h"
#include <gui/elements.h>

struct SPIMemReadView {
    View* view;
    SPIMemViewReadCallback callback;
    void* cb_ctx;
};

typedef struct {
    size_t chip_size;
    size_t blocks_written;
    size_t block_size;
    float progress;
} SPIMemReadViewModel;

View* spi_mem_view_read_get_view(SPIMemReadView* app) {
    return app->view;
}

static void spi_mem_view_read_draw_progress(Canvas* canvas, float progress) {
    FuriString* progress_str = furi_string_alloc();
    if(progress > 1.0) progress = 1.0;
    furi_string_printf(progress_str, "%d %%", (int)(progress * 100));
    elements_progress_bar(canvas, 13, 35, 100, progress);
    canvas_draw_str_aligned(
        canvas, 64, 25, AlignCenter, AlignTop, furi_string_get_cstr(progress_str));
    furi_string_free(progress_str);
}

static void spi_mem_view_read_draw_callback(Canvas* canvas, void* model) {
    SPIMemReadViewModel* app = model;
    canvas_draw_str_aligned(canvas, 64, 4, AlignCenter, AlignTop, "Reading dump");
    spi_mem_view_read_draw_progress(canvas, app->progress);
    elements_button_left(canvas, "Cancel");
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
    view_allocate_model(app->view, ViewModelTypeLocking, sizeof(SPIMemReadViewModel));
    view_set_context(app->view, app);
    view_set_draw_callback(app->view, spi_mem_view_read_draw_callback);
    view_set_input_callback(app->view, spi_mem_view_read_input_callback);
    spi_mem_view_read_reset(app);
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

void spi_mem_view_read_set_chip_size(SPIMemReadView* app, size_t chip_size) {
    with_view_model(
        app->view, SPIMemReadViewModel * model, { model->chip_size = chip_size; }, true);
    ;
}

void spi_mem_view_read_set_block_size(SPIMemReadView* app, size_t block_size) {
    with_view_model(
        app->view, SPIMemReadViewModel * model, { model->block_size = block_size; }, true);
}

void spi_mem_view_read_inc_progress(SPIMemReadView* app) {
    with_view_model(
        app->view,
        SPIMemReadViewModel * model,
        {
            model->blocks_written++;
            model->progress = ((float)model->block_size * (float)model->blocks_written) /
                              ((float)model->chip_size * (float)1000);
        },
        true);
}

void spi_mem_view_read_reset(SPIMemReadView* app) {
    with_view_model(
        app->view,
        SPIMemReadViewModel * model,
        {
            model->blocks_written = 0;
            model->block_size = 0;
            model->chip_size = 0;
            model->progress = 0;
        },
        true);
}
