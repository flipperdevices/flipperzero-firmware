#include <furi.h>
#include <gui/view.h>
#include "spi_mem_read.h"

View* spi_mem_read_get_view(SPIMemRead* instance) {
    return instance->view;
}

static bool spi_mem_read_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    if(event->key == InputKeyBack) return false;
    return true;
}

static void spi_mem_read_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "DrunkBatya was here");
}

SPIMemRead* spi_mem_read_alloc(void) {
    SPIMemRead* instance = malloc(sizeof(SPIMemRead));
    instance->view = view_alloc();
    view_set_input_callback(instance->view, spi_mem_read_input_callback);
    view_set_draw_callback(instance->view, spi_mem_read_draw_callback);
    return instance;
}

void spi_mem_read_free(SPIMemRead* instance) {
    view_free(instance->view);
    free(instance);
}
