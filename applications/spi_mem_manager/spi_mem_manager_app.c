#include <furi.h>
#include <gui/gui.h>
#include "spi_mem_manager_app.h"

#define TAG "SPIMemManager"

static void spi_mem_manager_input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void spi_mem_manager_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "DrunkBatya was here");
}

SPIMemManager* spi_mem_manager_alloc(void) {
    SPIMemManager* instance = malloc(sizeof(SPIMemManager));
    instance->view_port = view_port_alloc();
    instance->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(instance->gui, instance->view_port, GuiLayerFullscreen);
    instance->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    view_port_input_callback_set(
        instance->view_port, spi_mem_manager_input_callback, instance->event_queue);
    view_port_draw_callback_set(instance->view_port, spi_mem_manager_draw_callback, instance);
    return instance;
}

void spi_mem_manager_free(SPIMemManager* instance) {
    gui_remove_view_port(instance->gui, instance->view_port);
    view_port_free(instance->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(instance->event_queue);
    free(instance);
}

void spi_mem_manager_loop(SPIMemManager* SPIMemManagerApp) {
    InputEvent event;
    while(furi_message_queue_get(SPIMemManagerApp->event_queue, &event, FuriWaitForever) ==
          FuriStatusOk) {
        if(event.key == InputKeyBack) {
            if(event.type == InputTypeLong) {
                break;
            }
        }
        view_port_update(SPIMemManagerApp->view_port);
    }
}

int32_t spi_mem_manager_app(void* p) {
    UNUSED(p);
    SPIMemManager* SPIMemManagerApp = spi_mem_manager_alloc();
    spi_mem_manager_loop(SPIMemManagerApp);
    spi_mem_manager_free(SPIMemManagerApp);
    return 0;
}
