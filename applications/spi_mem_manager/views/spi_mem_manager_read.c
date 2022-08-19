#include <furi.h>
#include <gui/view.h>
#include "spi_mem_manager_read.h"

View* spi_mem_manager_read_get_view(SPIMemManagerRead* instance) {
    return instance->view;
}

static bool spi_mem_manager_read_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    if(event->type == InputTypeLong && event->key == InputKeyBack) return false;
    return true;
}

SPIMemManagerRead* spi_mem_manager_read_alloc(void) {
    SPIMemManagerRead* instance = malloc(sizeof(SPIMemManagerRead));
    instance->view = view_alloc();
    view_set_input_callback(instance->view, spi_mem_manager_read_input_callback);
    return instance;
}

void spi_mem_manager_read_free(SPIMemManagerRead* instance) {
    view_free(instance->view);
    free(instance);
}
