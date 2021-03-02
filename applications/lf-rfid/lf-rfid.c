#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>
#include <stream_buffer.h>

#include "lf-rfid.h"
#include "em4100.h"
#include "hid.h"

typedef enum { ModeReading, ModeEmulating } Mode;

typedef enum { ProtocolEm4100, ProtocolHid } Protocol;

typedef struct {
    uint32_t freq_khz;
    Mode mode;
    Protocol protocol;
    uint8_t customer_id;
    uint32_t em_data;
    bool dirty;
    bool dirty_freq;
} State;

static void render_callback(Canvas* canvas, void* ctx) {
    State* state = (State*)acquire_mutex((ValueMutex*)ctx, 25);

    canvas_clear(canvas);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 12, "LF RFID");

    if(state->mode == ModeEmulating) canvas_draw_str(canvas, 2, 24, "Emulating");
    if(state->mode == ModeReading) canvas_draw_str(canvas, 2, 24, "Reading");

    if(state->protocol == ProtocolHid) canvas_draw_str(canvas, 2, 36, "HID");
    if(state->protocol == ProtocolEm4100) canvas_draw_str(canvas, 2, 36, "EM4100");

    char buf[14];

    sprintf(buf, "%d kHz", (int)state->freq_khz);
    canvas_draw_str(canvas, 2, 45, buf);

    sprintf(buf, "%02d:%010ld", state->customer_id, state->em_data);
    canvas_draw_str(canvas, 2, 54, buf);

    release_mutex((ValueMutex*)ctx, state);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    osMessageQueueId_t event_queue = ctx;

    AppEvent event;
    event.type = EventTypeKey;
    event.value.input = *input_event;
    osMessageQueuePut(event_queue, &event, 0, 0);
}

extern TIM_HandleTypeDef TIM_C;

static GpioPin debug_0 = {.pin = GPIO_PIN_2, .port = GPIOB};
static GpioPin debug_1 = {.pin = GPIO_PIN_3, .port = GPIOC};

extern COMP_HandleTypeDef hcomp1;

typedef struct {
    Em4100Ctx* em4100_ctx;
    HidCtx* hid_ctx;
    uint32_t prev_dwt;
} ComparatorCtx;

void comparator_trigger_callback(void* hcomp, void* comp_ctx) {
    ComparatorCtx* ctx = (ComparatorCtx*)comp_ctx;

    uint32_t dt = (DWT->CYCCNT - ctx->prev_dwt) / (SystemCoreClock / 1000000.0f);
    ctx->prev_dwt = DWT->CYCCNT;

    bool rx_value = get_rfid_in_level();

    if(!rx_value && dt < 80 && dt > 10) {
        hid_fsm(ctx->hid_ctx, DWT->CYCCNT);
    }

    if(dt > 150) {
        em4100_fsm(ctx->em4100_ctx, rx_value, dt);
    }
}

int32_t lf_rfid_workaround(void* p) {
    osMessageQueueId_t event_queue = osMessageQueueNew(2, sizeof(AppEvent), NULL);

    // create pin
    GpioPin pull_pin = {.pin = RFID_PULL_Pin, .port = RFID_PULL_GPIO_Port};
    // TODO open record
    GpioPin* pull_pin_record = &pull_pin;

    gpio_init(pull_pin_record, GpioModeOutputPushPull);

    gpio_init(&debug_0, GpioModeOutputPushPull);
    gpio_init(&debug_1, GpioModeOutputPushPull);

    // pulldown iBtn pin to prevent interference from ibutton
    gpio_init((GpioPin*)&ibutton_gpio, GpioModeOutputOpenDrain);
    gpio_write((GpioPin*)&ibutton_gpio, false);

    // internal buffer
    uint8_t int_bufer[64];

    // init ctx
    ComparatorCtx comp_ctx;

    Em4100Ctx em4100_ctx;
    HidCtx hid_ctx;

    em4100_ctx.stream_buffer = xStreamBufferCreate(64, 64);
    if(em4100_ctx.stream_buffer == NULL) {
        printf("cannot create stream buffer\r\n");
        return 255;
    }

    em4100_ctx.int_buffer = int_bufer;
    em4100_ctx.event_queue = event_queue;
    em4100_init(&em4100_ctx);

    hid_init(&hid_ctx);

    comp_ctx.em4100_ctx = &em4100_ctx;
    comp_ctx.hid_ctx = &hid_ctx;
    comp_ctx.prev_dwt = 0;

    // start comp
    HAL_COMP_Start(&hcomp1);

    uint8_t raw_data[96];
    for(size_t i = 0; i < 96; i++) {
        raw_data[i] = 0;
    }

    State _state;
    _state.freq_khz = 125;
    _state.mode = ModeReading;
    _state.customer_id = 00;
    _state.em_data = 4378151;
    _state.dirty = true;
    _state.protocol = ProtocolEm4100;
    _state.dirty_freq = true;

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, &_state, sizeof(State))) {
        printf("cannot create mutex\r\n");
        return 255;
    }

    ViewPort* view_port = view_port_alloc();

    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    AppEvent event;

    while(1) {
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, 1024 / 8);

        if(event.type == EventTypeRx && event_status == osOK) {
            size_t received = xStreamBufferReceive(comp_ctx.em4100_ctx->stream_buffer, raw_data, 64, 0);
            printf("received: %d\r\n", received);
            if(received == 64) {
                if(em4100_even_check(&raw_data[9])) {
                    State* state = (State*)acquire_mutex_block(&state_mutex);
                    em4100_extract_data(&raw_data[9], &state->customer_id, &state->em_data);

                    printf("customer: %02d, data: %010lu\n", state->customer_id, state->em_data);

                    release_mutex(&state_mutex, state);

                    view_port_update(view_port);

                    api_hal_light_set(LightGreen, 0xFF);
                    osDelay(50);
                    api_hal_light_set(LightGreen, 0x00);
                }
            }
        } else {
            State* state = (State*)acquire_mutex_block(&state_mutex);

            if(event_status == osOK) {
                if(event.type == EventTypeKey) {
                    // press events
                    if(event.value.input.type == InputTypePress &&
                       event.value.input.key == InputKeyBack) {
                        hal_pwmn_stop(&TIM_C, TIM_CHANNEL_1); // TODO: move to furiac_onexit
                        api_interrupt_remove(
                            comparator_trigger_callback, InterruptTypeComparatorTrigger);
                        gpio_init(pull_pin_record, GpioModeInput);
                        gpio_init((GpioPin*)&ibutton_gpio, GpioModeInput);

                        // TODO remove all view_ports create by app
                        view_port_enabled_set(view_port, false);
                        return 255;
                    }

                    if(event.value.input.type == InputTypePress &&
                       event.value.input.key == InputKeyUp) {
                        state->dirty_freq = true;
                        state->freq_khz += 10;
                    }

                    if(event.value.input.type == InputTypePress &&
                       event.value.input.key == InputKeyDown) {
                        state->dirty_freq = true;
                        state->freq_khz -= 10;
                    }

                    if(event.value.input.type == InputTypePress &&
                       event.value.input.key == InputKeyLeft) {
                        if(state->protocol == ProtocolEm4100) {
                            state->protocol = ProtocolHid;
                            state->dirty = true;
                        }
                    }

                    if(event.value.input.type == InputTypePress &&
                       event.value.input.key == InputKeyRight) {
                        if(state->protocol == ProtocolHid) {
                            state->protocol = ProtocolEm4100;
                            state->dirty = true;
                        }
                    }

                    if(event.value.input.type == InputTypePress &&
                       event.value.input.key == InputKeyOk) {
                        state->dirty = true;
                        if(state->mode == ModeEmulating) {
                            state->mode = ModeReading;
                        } else if(state->mode == ModeReading) {
                            state->mode = ModeEmulating;
                        }
                    }
                }
            } else {
                // event timeout
                printf("dt: %ld\r\n", comp_ctx.hid_ctx->dt);
            }

            if(state->dirty) {
                if(state->mode == ModeEmulating) {
                    api_interrupt_remove(
                        comparator_trigger_callback, InterruptTypeComparatorTrigger);

                    if(state->protocol == ProtocolEm4100) {
                        em4100_prepare_data(state->em_data, state->customer_id, raw_data);
                    }

                    if(state->protocol == ProtocolHid) {
                        hid_prepare_data(155, 13710, raw_data);
                    }
                }

                if(state->mode == ModeReading) {
                    gpio_write(pull_pin_record, false);

                    em4100_init(comp_ctx.em4100_ctx);
                    hid_init(comp_ctx.hid_ctx);

                    api_interrupt_add(
                        comparator_trigger_callback, InterruptTypeComparatorTrigger, &comp_ctx);
                    hal_pwmn_set(0.5, (float)(state->freq_khz * 1000), &LFRFID_TIM, LFRFID_CH);
                }

                state->dirty_freq = true; // config new PWM next

                state->dirty = false;
            }

            if(state->dirty_freq) {
                if(state->mode == ModeReading) {
                    hal_pwmn_set(0.5, (float)(state->freq_khz * 1000), &LFRFID_TIM, LFRFID_CH);
                }
                if(state->mode == ModeEmulating) {
                    hal_pwmn_set(1.0, (float)(state->freq_khz * 1000), &LFRFID_TIM, LFRFID_CH);
                }

                state->dirty_freq = false;
            }

            if(state->mode == ModeEmulating) {
                if(state->protocol == ProtocolEm4100) {
                    em4100_emulation(raw_data, pull_pin_record);
                }

                if(state->protocol == ProtocolHid) {
                    hid_emulation(raw_data, pull_pin_record);
                }
            }

            release_mutex(&state_mutex, state);
            view_port_update(view_port);
        }
    }

    return 0;
}
