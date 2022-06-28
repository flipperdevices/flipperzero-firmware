#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>

#include <furi_hal.h>

typedef struct {
    uint8_t status;
} SentryState;

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} Event;


const char* status_texts[3] = { "[Press OK to open safe]", "Sending...", "Done !" };


static void sentry_safe_render_callback(Canvas* const canvas, void* ctx) {

    const SentryState* sentry_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(sentry_state == NULL) {
        return;
    }

    // Before the function is called, the state is set with the canvas_reset(canvas)

    // Frame
    canvas_draw_frame(canvas, 0, 0, 128, 64);

    // Message
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_frame(canvas, 28, 4, 73, 24);
    canvas_draw_str_aligned(canvas, 64, 15, AlignCenter, AlignBottom, "BLACK <-> GND");
    canvas_draw_str_aligned(canvas, 64, 25, AlignCenter, AlignBottom, "GREEN <-> C1");
    canvas_draw_str_aligned(canvas, 64, 50, AlignCenter, AlignBottom, status_texts[sentry_state->status]);

    release_mutex((ValueMutex*)ctx, sentry_state);
}

static void sentry_safe_input_callback(InputEvent* input_event, osMessageQueueId_t event_queue) {
    furi_assert(event_queue);

    Event event = {.type = EventTypeKey, .input = *input_event};
    osMessageQueuePut(event_queue, &event, 0, osWaitForever);
}

void send_request(int command, int a, int b, int c, int d, int e){
    int checksum = (command + a + b + c + d + e);

    furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeOutputPushPull);
    furi_hal_gpio_write(&gpio_ext_pc1, false);
    osDelay(3.4);
    furi_hal_gpio_write(&gpio_ext_pc1, true);
    
    furi_hal_uart_init(FuriHalUartIdLPUART1, 4800);
    //furi_hal_uart_set_br(FuriHalUartIdLPUART1, 4800);
    //furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, usb_uart_on_irq_cb, usb_uart);

    uint8_t data[8] = {0x0, command, a, b, c, d, e, checksum};
    furi_hal_uart_tx(FuriHalUartIdLPUART1, data, 8);
    
    osDelay(100);

    furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, NULL, NULL);
    furi_hal_uart_deinit(FuriHalUartIdLPUART1);
}

void reset_code(int a, int b, int c, int d, int e) {
  send_request(0x75, a, b, c, d, e);
}

void try_code(int a, int b, int c, int d, int e) {
  send_request(0x71, a, b, c, d, e);
}

int32_t sentry_safe_app(void* p) {

    UNUSED(p);

    osMessageQueueId_t event_queue = osMessageQueueNew(8, sizeof(Event), NULL);

    SentryState* sentry_state = malloc(sizeof(SentryState));

    sentry_state->status = 0;

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, sentry_state, sizeof(SentryState))) {
        FURI_LOG_E("SentrySafe", "cannot create mutex\r\n");
        free(sentry_state);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, sentry_safe_render_callback, &state_mutex);
    view_port_input_callback_set(view_port, sentry_safe_input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    Event event;
    for(bool processing = true; processing;) {
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, 100);

        SentryState* sentry_state = (SentryState*)acquire_mutex_block(&state_mutex);

        if(event_status == osOK) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {

                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyRight:
                        break;
                    case InputKeyLeft:
                        break;
                    
                    case InputKeyOk:

                        if(sentry_state->status == 2){

                            sentry_state->status = 0;

                        }else if(sentry_state->status == 0){
                            
                            sentry_state->status = 1;

                            reset_code(1,2,3,4,5);
                            osDelay(500);
                            try_code(1,2,3,4,5);

                            sentry_state->status = 2;

                        }

                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    }
                }
            }
        } else {
            // event timeout
        }

        view_port_update(view_port);
        release_mutex(&state_mutex, sentry_state);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    osMessageQueueDelete(event_queue);
    delete_mutex(&state_mutex);
    free(sentry_state);

    return 0;
}