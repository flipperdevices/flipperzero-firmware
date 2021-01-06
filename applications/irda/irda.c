#include "flipper.h"
#include "flipper_v2.h"
#include "irda_nec.h"
#include "irda_samsung.h"
#include "irda_protocols.h"
#include "irda-decoder/irda-decoder.h"

typedef enum {
    EventTypeTick,
    EventTypeKey,
    EventTypeRX,
} EventType;

typedef struct {
    bool edge;
    uint32_t lasted;
} RXValue;

typedef struct {
    union {
        InputEvent input;
        RXValue rx;
    } value;
    EventType type;
} AppEvent;

typedef struct {
    uint8_t mode_id;
    uint16_t carrier_freq;
    uint8_t carrier_duty_cycle_id;
    uint8_t nec_packet_id;
    uint8_t samsung_packet_id;
} State;

typedef void (*ModeInput)(AppEvent*, State*);
typedef void (*ModeRender)(Canvas*, State*);

void input_carrier(AppEvent* event, State* state);
void render_carrier(Canvas* canvas, State* state);
void input_nec(AppEvent* event, State* state);
void render_nec(Canvas* canvas, State* state);
void render_carrier(Canvas* canvas, State* state);
void input_samsung(AppEvent* event, State* state);
void render_samsung(Canvas* canvas, State* state);

typedef struct {
    ModeRender render;
    ModeInput input;
} Mode;

typedef struct {
    uint8_t addr;
    uint8_t data;
} NecPacket;

typedef struct {
    uint16_t addr;
    uint16_t data;
} SamsungPacket;

const Mode modes[] = {
    {.render = render_carrier, .input = input_carrier},
    {.render = render_nec, .input = input_nec},
    {.render = render_samsung, .input = input_samsung},
};

const NecPacket nec_packets[] = {
    {.addr = 0xFF, .data = 0x11},
    {.addr = 0xF7, .data = 0x59},
    {.addr = 0xFF, .data = 0x01},
    {.addr = 0xFF, .data = 0x10},
    {.addr = 0xFF, .data = 0x15},
    {.addr = 0xFF, .data = 0x25},
    {.addr = 0xFF, .data = 0xF0},
};

const SamsungPacket samsung_packets[] = {
    {.addr = 0xE0E, .data = 0xF30C},
    {.addr = 0xE0E, .data = 0xF40D},
    {.addr = 0xE0E, .data = 0xF50E},
};

const float duty_cycles[] = {0.1, 0.25, 0.333, 0.5, 1.0};

void render_carrier(Canvas* canvas, State* state) {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "carrier mode >");
    canvas_draw_str(canvas, 2, 37, "? /\\ freq | \\/ duty cycle");
    {
        char buf[24];
        sprintf(buf, "frequency: %u Hz", state->carrier_freq);
        canvas_draw_str(canvas, 2, 50, buf);
        sprintf(
            buf, "duty cycle: %d/1000", (int)(duty_cycles[state->carrier_duty_cycle_id] * 1000));
        canvas_draw_str(canvas, 2, 62, buf);
    }
}

void render_nec(Canvas* canvas, State* state) {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "< nec mode >");
    canvas_draw_str(canvas, 2, 37, "? /\\ \\/ packet");
    {
        char buf[24];
        sprintf(
            buf,
            "packet: %02X %02X",
            nec_packets[state->nec_packet_id].addr,
            nec_packets[state->nec_packet_id].data);
        canvas_draw_str(canvas, 2, 50, buf);
    }
}

void render_samsung(Canvas* canvas, State* state) {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "< samsung32 mode");
    canvas_draw_str(canvas, 2, 37, "? /\\ \\/ packet");
    {
        char buf[24];
        sprintf(
            buf,
            "packet: %02X %02X",
            samsung_packets[state->samsung_packet_id].addr,
            samsung_packets[state->samsung_packet_id].data);
        canvas_draw_str(canvas, 2, 50, buf);
    }
}

void input_carrier(AppEvent* event, State* state) {
    if(event->value.input.input == InputOk) {
        if(event->value.input.state) {
            irda_pwm_set(duty_cycles[state->carrier_duty_cycle_id], state->carrier_freq);
        } else {
            irda_pwm_stop();
        }
    }

    if(event->value.input.state && event->value.input.input == InputUp) {
        if(state->carrier_freq < 45000) {
            state->carrier_freq += 1000;
        } else {
            state->carrier_freq = 33000;
        }
    }

    if(event->value.input.state && event->value.input.input == InputDown) {
        uint8_t duty_cycles_count = sizeof(duty_cycles) / sizeof(duty_cycles[0]);
        if(state->carrier_duty_cycle_id < (duty_cycles_count - 1)) {
            state->carrier_duty_cycle_id++;
        } else {
            state->carrier_duty_cycle_id = 0;
        }
    }
}

void input_nec(AppEvent* event, State* state) {
    uint8_t packets_count = sizeof(nec_packets) / sizeof(nec_packets[0]);

    if(event->value.input.input == InputOk) {
        if(event->value.input.state) {
            vTaskSuspendAll();
            ir_nec_send(
                nec_packets[state->nec_packet_id].addr, nec_packets[state->nec_packet_id].data);
            xTaskResumeAll();
        }
    }

    if(event->value.input.state && event->value.input.input == InputUp) {
        if(state->nec_packet_id < (packets_count - 1)) {
            state->nec_packet_id++;
        } else {
            state->nec_packet_id = 0;
        }
    }

    if(event->value.input.state && event->value.input.input == InputDown) {
        if(state->nec_packet_id > 0) {
            state->nec_packet_id--;
        } else {
            state->nec_packet_id = packets_count - 1;
        }
    }
}

void input_samsung(AppEvent* event, State* state) {
    uint8_t packets_count = sizeof(samsung_packets) / sizeof(samsung_packets[0]);

    if(event->value.input.input == InputOk) {
        if(event->value.input.state) {
            vTaskSuspendAll();
            ir_samsung_send(
                samsung_packets[state->samsung_packet_id].addr,
                samsung_packets[state->samsung_packet_id].data);
            xTaskResumeAll();
        }
    }

    if(event->value.input.state && event->value.input.input == InputUp) {
        if(state->samsung_packet_id < (packets_count - 1)) {
            state->samsung_packet_id++;
        } else {
            state->samsung_packet_id = 0;
        }
    }

    if(event->value.input.state && event->value.input.input == InputDown) {
        if(state->samsung_packet_id > 0) {
            state->samsung_packet_id--;
        } else {
            state->samsung_packet_id = packets_count - 1;
        }
    }
}

static void render_callback(Canvas* canvas, void* ctx) {
    State* state = (State*)acquire_mutex((ValueMutex*)ctx, 25);

    if(state != NULL) {
        canvas_clear(canvas);
        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 2, 12, "irda test");

        modes[state->mode_id].render(canvas, state);

        release_mutex((ValueMutex*)ctx, state);
    }
}

static void input_callback(InputEvent* input_event, void* ctx) {
    osMessageQueueId_t event_queue = (QueueHandle_t)ctx;

    AppEvent event;
    event.type = EventTypeKey;
    event.value.input = *input_event;
    osMessageQueuePut(event_queue, &event, 0, 0);
}

void irda_timer_capture_callback(void* htim, void* comp_ctx) {
    TIM_HandleTypeDef* _htim = (TIM_HandleTypeDef*)htim;
    osMessageQueueId_t event_queue = (osMessageQueueId_t)comp_ctx;

    if(_htim->Instance == TIM2) {
        AppEvent event;
        event.type = EventTypeRX;
        uint32_t channel;

        if(_htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            // falling event
            event.value.rx.edge = false;
            channel = TIM_CHANNEL_1;
        } else if(_htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            // rising event
            event.value.rx.edge = true;
            channel = TIM_CHANNEL_2;
        } else {
            // not our event
            return;
        }

        event.value.rx.lasted = HAL_TIM_ReadCapturedValue(_htim, channel);
        __HAL_TIM_SET_COUNTER(_htim, 0);

        osMessageQueuePut(event_queue, &event, 0, 0);
    }
}

void irda(void* p) {
    osMessageQueueId_t event_queue = osMessageQueueNew(32, sizeof(AppEvent), NULL);

    State _state;
    uint8_t mode_count = sizeof(modes) / sizeof(modes[0]);
    uint8_t duty_cycles_count = sizeof(duty_cycles) / sizeof(duty_cycles[0]);

    _state.carrier_duty_cycle_id = duty_cycles_count - 2;
    _state.carrier_freq = 36000;
    _state.mode_id = 0;
    _state.nec_packet_id = 0;
    _state.samsung_packet_id = 0;

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, &_state, sizeof(State))) {
        printf("cannot create mutex\n");
        furiac_exit(NULL);
    }

    Widget* widget = widget_alloc();

    widget_draw_callback_set(widget, render_callback, &state_mutex);
    widget_input_callback_set(widget, input_callback, event_queue);

    // Open GUI and register widget
    Gui* gui = (Gui*)furi_open("gui");
    if(gui == NULL) {
        printf("gui is not available\n");
        furiac_exit(NULL);
    }
    gui_add_widget(gui, widget, GuiLayerFullscreen);

    // Red LED
    // TODO open record
    const GpioPin* led_record = &led_gpio[0];

    // configure pin
    gpio_init(led_record, GpioModeOutputOpenDrain);

    // setup irda rx timer
    tim_irda_rx_init();

    // add timer capture interrupt
    api_interrupt_add(irda_timer_capture_callback, InterruptTypeTimerCapture, event_queue);

    // receive array
    const uint8_t arr_size = 128;
    uint32_t* arr = malloc(arr_size * sizeof(uint32_t));
    uint8_t arr_index = 0;
    bool start_from = false;

    IrDADecoder* decoder = alloc_decoder();

    AppEvent event;
    while(1) {
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, 500);
        State* state = (State*)acquire_mutex_block(&state_mutex);

        if(event_status == osOK) {
            if(event.type == EventTypeKey) {
                // press events
                if(event.value.input.state && event.value.input.input == InputBack) {
                    printf("[irda] bye!\n");
                    // TODO remove all widgets create by app
                    widget_enabled_set(widget, false);
                    furiac_exit(NULL);
                }

                if(event.value.input.state && event.value.input.input == InputLeft) {
                    if(state->mode_id > 0) {
                        state->mode_id--;
                    }
                }

                if(event.value.input.state && event.value.input.input == InputRight) {
                    if(state->mode_id < (mode_count - 1)) {
                        state->mode_id++;
                    }
                }

                modes[state->mode_id].input(&event, state);
            } else if(event.type == EventTypeRX) {
                
                gpio_write(led_record, event.value.rx.edge);

                if(arr_index < arr_size) {
                    if(arr_index == 0) {
                        start_from = event.value.rx.edge;
                    }
                    arr[arr_index] = event.value.rx.lasted;
                    arr_index++;
                } else {
                    // dump array
                    process_decoder(decoder, start_from, arr, arr_size);

                    if(start_from) {
                        printf(" true: ");
                    } else {
                        printf("false: ");
                    }

                    for(uint8_t i = 0; i < arr_size; i++) {
                        printf("%d, ", arr[i]);
                    }
                    printf("\r\n");
                    arr_index = 0;
                }
            }

        } else {
            // event timeout
            arr_index = 0;
        }

        release_mutex(&state_mutex, state);
        widget_update(widget);
    }
}