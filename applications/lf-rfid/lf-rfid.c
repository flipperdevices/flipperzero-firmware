#include <furi.h>
#include <gui/gui.h>
#include <stream_buffer.h>

typedef enum { EventTypeTick, EventTypeKey, EventTypeRx } EventType;

typedef struct {
    uint8_t dummy;
} RxEvent;

typedef struct {
    union {
        InputEvent input;
        RxEvent rx;
    } value;
    EventType type;
} AppEvent;

typedef enum { ModeReading, ModeEmulating } Mode;

typedef enum { ProtocolEm4100, ProtocolHid } Protocol;

typedef struct {
    uint32_t freq_khz;
    Mode mode;
    Protocol protocol;
    uint8_t customer_id;
    uint32_t em_data;
    bool dirty;
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
void em4100_emulation(uint8_t* data, GpioPin* pin);
void em4100_prepare_data(uint32_t ID, uint32_t VENDOR, uint8_t* data);

GpioPin debug_0 = {.pin = GPIO_PIN_2, .port = GPIOB};
GpioPin debug_1 = {.pin = GPIO_PIN_3, .port = GPIOC};

extern COMP_HandleTypeDef hcomp1;

typedef struct {
    osMessageQueueId_t event_queue;
    uint32_t prev_dwt;
    int8_t symbol;
    bool center;
    size_t symbol_cnt;
    GpioPin* led_record;
    StreamBufferHandle_t stream_buffer;
    uint8_t* int_buffer;
} ComparatorCtx;


bool preamble_buffer[8];
uint16_t hid_symbol_counter = 0;

uint8_t HID_PREAMBLE[] = {false, false, false, true, true, true, false, true};

bool is_preamble = true;

void hid_push_bit(bool bit) {
    if(!is_preamble) {
        // finding preamble

        preamble_buffer[hid_symbol_counter % 8] = bit;

        is_preamble = true;
        if(hid_symbol_counter > 8) {
            for(uint8_t i = 0; i < 8; i++) {
                if(preamble_buffer[(i + hid_symbol_counter - 7) % 8] != HID_PREAMBLE[i]) {
                    is_preamble = false;
                    break;
                }
            }
        } else {
            is_preamble = false;
        }

        if(is_preamble) {
            gpio_write(&debug_0, true);
            delay_us(5);
            gpio_write(&debug_0, false);

            hid_symbol_counter = 0;
        }
    } else {
        if(hid_symbol_counter == 88) {
            hid_symbol_counter = 0;
            is_preamble = false;
        }
    }

    hid_symbol_counter++;
}

uint32_t hid_prev_dwt = 0;
uint8_t pulse_cnt = 0;

void hid_fsm(uint32_t dwt) {
    uint32_t dt = (dwt - hid_prev_dwt) / (SystemCoreClock / 1000000.0f);
    hid_prev_dwt = dwt;

    
    if(hid_symbol_counter > 500) {
        // try to move sampling frame
        pulse_cnt = (pulse_cnt + 2) % 6;
        hid_symbol_counter = 0;
    } else {
        pulse_cnt++;
    }
    
    if(pulse_cnt == 6) {
        if(dt < 72 && dt > 40) {
            gpio_write(&debug_0, true);
            delay_us(3);
            gpio_write(&debug_0, false);

            hid_push_bit(false);
        } else if(dt >= 72 && dt < 100) {
            gpio_write(&debug_0, true);
            delay_us(15);
            gpio_write(&debug_0, false);

            hid_push_bit(true);
        }

        pulse_cnt = 0;
    }
}

void comparator_trigger_callback(void* hcomp, void* comp_ctx) {
    if((COMP_HandleTypeDef*)hcomp != &hcomp1) return;

    ComparatorCtx* ctx = (ComparatorCtx*)comp_ctx;

    uint32_t dt = (DWT->CYCCNT - ctx->prev_dwt) / (SystemCoreClock / 1000000.0f);
    ctx->prev_dwt = DWT->CYCCNT;

    // TOOD F4 and F5 differ
    bool rx_value = (HAL_COMP_GetOutputLevel(hcomp) == COMP_OUTPUT_LEVEL_LOW);

    if(!rx_value) {
        hid_fsm(DWT->CYCCNT);
    }

    if(dt < 150) return; // supress noise

    // wait message will be consumed
    if(xStreamBufferBytesAvailable(ctx->stream_buffer) == 64) return;

    // gpio_write(&debug_0, true);

    if(dt > 384) {
        // change symbol 0->1 or 1->0
        ctx->symbol = rx_value;
        ctx->center = true;
    } else {
        // same symbol as prev or center
        ctx->center = !ctx->center;
    }

    /*
    gpio_write(&debug_1, true);
    delay_us(center ? 10 : 30);
    gpio_write(&debug_1, false);
    */

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(ctx->center && ctx->symbol != -1) {
        /*
        gpio_write(&debug_0, true);
        delay_us(symbol ? 10 : 30);
        gpio_write(&debug_0, false);
        */

        ctx->int_buffer[ctx->symbol_cnt] = ctx->symbol;
        ctx->symbol_cnt++;
    }

    // check preamble
    if(ctx->symbol_cnt <= 9 && ctx->symbol == 0) {
        ctx->symbol_cnt = 0;
        ctx->symbol = -1;
    }

    // check stop bit
    if(ctx->symbol_cnt == 64 && ctx->symbol == 1) {
        ctx->symbol_cnt = 0;
        ctx->symbol = -1;
    }

    // TODO
    // write only 9..64 symbols directly to streambuffer

    if(ctx->symbol_cnt == 64) {
        if(xStreamBufferSendFromISR(
               ctx->stream_buffer, ctx->int_buffer, 64, &xHigherPriorityTaskWoken) == 64) {
            AppEvent event;
            event.type = EventTypeRx;
            osMessageQueuePut(ctx->event_queue, &event, 0, 0);
        }

        ctx->symbol_cnt = 0;
    }

    // gpio_write(&debug_0, false);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

bool em4100_even_check(uint8_t* buf);
void em4100_extract_data(uint8_t* buf, uint8_t* customer, uint32_t* em_data);

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

    // init ctx
    ComparatorCtx comp_ctx;

    // internal buffer
    uint8_t int_bufer[64];
    for(size_t i = 0; i < 64; i++) {
        int_bufer[i] = 0;
    }

    comp_ctx.prev_dwt = 0;
    comp_ctx.symbol = -1; // init state
    comp_ctx.center = false;
    comp_ctx.symbol_cnt = 0;
    comp_ctx.led_record = (GpioPin*)&led_gpio[1];
    comp_ctx.stream_buffer = xStreamBufferCreate(64, 64);
    comp_ctx.int_buffer = int_bufer;
    comp_ctx.event_queue = event_queue;

    if(comp_ctx.stream_buffer == NULL) {
        printf("cannot create stream buffer\r\n");
        return 255;
    }

    // start comp
    HAL_COMP_Start(&hcomp1);

    uint8_t raw_data[64];
    for(size_t i = 0; i < 64; i++) {
        raw_data[i] = 0;
    }

    State _state;
    _state.freq_khz = 125;
    _state.mode = ModeReading;
    _state.customer_id = 00;
    _state.em_data = 4378151;
    _state.dirty = true;
    _state.protocol = ProtocolEm4100;

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

    GpioPin* led_record = (GpioPin*)&led_gpio[1];
    gpio_init(led_record, GpioModeOutputOpenDrain);

    while(1) {
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, 1024 / 8);

        if(event.type == EventTypeRx && event_status == osOK) {
            size_t received = xStreamBufferReceive(comp_ctx.stream_buffer, raw_data, 64, 0);
            printf("received: %d\r\n", received);
            if(received == 64) {
                if(em4100_even_check(&raw_data[9])) {
                    State* state = (State*)acquire_mutex_block(&state_mutex);
                    em4100_extract_data(&raw_data[9], &state->customer_id, &state->em_data);

                    printf("customer: %02d, data: %010lu\n", state->customer_id, state->em_data);

                    release_mutex(&state_mutex, state);
                    view_port_update(view_port);

                    gpio_write(led_record, false);
                    delay(50);
                    gpio_write(led_record, true);
                }

                /*
                gpio_write(led_record, false);
                osDelay(10);
                gpio_write(led_record, true);
                */
            }

            /*
            gpio_write(led_record, false);
            osDelay(10);
            gpio_write(led_record, true);
            */

        } else {
            State* state = (State*)acquire_mutex_block(&state_mutex);

            if(event_status == osOK) {
                if(event.type == EventTypeKey) {
                    // press events
                    if(event.value.input.type == InputTypePress &&
                       event.value.input.key == InputKeyBack) {
                        hal_pwmn_stop(&TIM_C, TIM_CHANNEL_1); // TODO: move to furiac_onexit
                        gpio_init(pull_pin_record, GpioModeInput);
                        gpio_init((GpioPin*)&ibutton_gpio, GpioModeInput);

                        // TODO remove all view_ports create by app
                        view_port_enabled_set(view_port, false);
                        return 255;
                    }

                    if(event.value.input.type == InputTypePress &&
                       event.value.input.key == InputKeyUp) {
                        state->dirty = true;
                        state->freq_khz += 10;
                    }

                    if(event.value.input.type == InputTypePress &&
                       event.value.input.key == InputKeyDown) {
                        state->dirty = true;
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
            }

            if(state->dirty) {
                if(state->mode == ModeEmulating) {
                    api_interrupt_remove(
                        comparator_trigger_callback, InterruptTypeComparatorTrigger);
                    hal_pwmn_set(1.0, (float)(state->freq_khz * 1000), &LFRFID_TIM, LFRFID_CH);

                    em4100_prepare_data(state->em_data, state->customer_id, raw_data);
                }

                if(state->mode == ModeReading) {
                    gpio_write(pull_pin_record, false);
                    api_interrupt_add(
                        comparator_trigger_callback, InterruptTypeComparatorTrigger, &comp_ctx);
                    hal_pwmn_set(0.5, (float)(state->freq_khz * 1000), &LFRFID_TIM, LFRFID_CH);
                }

                state->dirty = false;
            }

            if(state->mode == ModeEmulating) {
                em4100_emulation(raw_data, pull_pin_record);
            }
            release_mutex(&state_mutex, state);
            view_port_update(view_port);
        }
    }

    return 0;
}
