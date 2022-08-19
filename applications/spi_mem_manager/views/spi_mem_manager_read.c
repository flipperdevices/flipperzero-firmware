#include <furi.h>
#include <gui/view.h>
#include "spi_mem_manager_read.h"

View* spi_mem_manager_read_get_view(SPIMemManagerRead* instance) {
    return instance->view;
}

static bool spi_mem_manager_read_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    if(event->key == InputKeyBack) return false;
    return true;
}

static void spi_mem_manager_read_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "DrunkBatya was here");
}

SPIMemManagerRead* spi_mem_manager_read_alloc(void) {
    SPIMemManagerRead* instance = malloc(sizeof(SPIMemManagerRead));
    instance->view = view_alloc();
    view_set_input_callback(instance->view, spi_mem_manager_read_input_callback);
    view_set_draw_callback(instance->view, spi_mem_manager_read_draw_callback);
    return instance;
}

void spi_mem_manager_read_free(SPIMemManagerRead* instance) {
    view_free(instance->view);
    free(instance);
}
