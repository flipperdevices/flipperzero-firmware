#include <furi.h>
#include <gui/view.h>
#include "spi_mem_chipinfo.h"

View* spi_mem_chipinfo_get_view(SPIMemChipinfo* instance) {
    return instance->view;
}

static void spi_mem_chipinfo_draw_info(Canvas* canvas) {
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_icon(canvas, 5, 15, &I_Dip8_32x36);
    canvas_draw_frame(canvas, 42, 5, 81, 54);
    canvas_draw_str(canvas, 60, 8, "Chip info");
    canvas_draw_str(canvas, 45, 17, "Atmel");
    canvas_draw_str(canvas, 45, 26, "AT25DF16");
    canvas_draw_str(canvas, 45, 35, "65536 KB");
}

static bool spi_mem_chipinfo_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    if(event->key == InputKeyBack) return false;
    return true;
}

static void spi_mem_chipinfo_draw_callback(Canvas* canvas, void* context) {
    spi_mem_chipinfo_draw_info(canvas);
    UNUSED(context);
}

SPIMemChipinfo* spi_mem_chipinfo_alloc(void) {
    SPIMemChipinfo* instance = malloc(sizeof(SPIMemChipinfo));
    instance->view = view_alloc();
    view_set_input_callback(instance->view, spi_mem_chipinfo_input_callback);
    view_set_draw_callback(instance->view, spi_mem_chipinfo_draw_callback);
    return instance;
}

void spi_mem_chipinfo_free(SPIMemChipinfo* instance) {
    view_free(instance->view);
    free(instance);
}
