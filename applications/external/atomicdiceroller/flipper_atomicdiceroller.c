// CC0 1.0 Universal (CC0 1.0)
// Public Domain Dedication
// https://github.com/nmrr

#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <furi_hal_power.h>
#include <locale/locale.h>
#include <toolbox/crc32_calc.h>
#include <lib/toolbox/md5.h>

#define SCREEN_SIZE_X 128
#define SCREEN_SIZE_Y 64

typedef enum {
    EventTypeInput,
    ClockEventTypeTick,
    ClockEventTypeTickPause,
    EventGPIO,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} EventApp;

#define lineArraySize 128

typedef struct {
    FuriMutex* mutex;
    uint32_t cps;
    uint32_t diceAvailiable;
    uint8_t dice;
    uint8_t method;
    uint8_t pause;
} mutexStruct;

static void draw_callback(Canvas* canvas, void* ctx) {
    mutexStruct* mutexVal = ctx;
    mutexStruct mutexDraw;
    furi_mutex_acquire(mutexVal->mutex, FuriWaitForever);
    memcpy(&mutexDraw, mutexVal, sizeof(mutexStruct));
    furi_mutex_release(mutexVal->mutex);

    canvas_set_font(canvas, FontPrimary);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%ld cps", mutexDraw.cps);
    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignBottom, buffer);

    snprintf(buffer, sizeof(buffer), "%lu/64", mutexDraw.diceAvailiable);
    canvas_draw_str_aligned(canvas, SCREEN_SIZE_X, 10, AlignRight, AlignBottom, buffer);

    if(mutexDraw.method == 0)
        canvas_draw_str_aligned(canvas, 0, 20, AlignLeft, AlignBottom, "Hash: CRC32");
    else
        canvas_draw_str_aligned(canvas, 0, 20, AlignLeft, AlignBottom, "Hash: MD5");

    if(mutexDraw.dice != 0 && mutexDraw.pause == 0) {
        canvas_set_font(canvas, FontBigNumbers);
        snprintf(buffer, sizeof(buffer), "%u", mutexDraw.dice);
        canvas_draw_str_aligned(canvas, SCREEN_SIZE_X / 2, 50, AlignCenter, AlignBottom, buffer);
    }
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    EventApp event = {.type = EventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void clock_tick(void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* queue = ctx;
    EventApp event = {.type = ClockEventTypeTick};
    furi_message_queue_put(queue, &event, 0);
}

static void clock_tick_pause(void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* queue = ctx;
    EventApp event = {.type = ClockEventTypeTickPause};
    furi_message_queue_put(queue, &event, 0);
}

static void gpiocallback(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* queue = ctx;
    EventApp event = {.type = EventGPIO};
    furi_message_queue_put(queue, &event, 0);
}

int32_t flipper_atomicdiceroller_app() {
    furi_hal_bus_enable(FuriHalBusTIM2);
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetPrescaler(TIM2, 0);
    LL_TIM_SetAutoReload(TIM2, 0xFFFFFFFF);
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);

    EventApp event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(EventApp));

    furi_hal_gpio_init(&gpio_ext_pa7, GpioModeInterruptFall, GpioPullUp, GpioSpeedVeryHigh);

    mutexStruct mutexVal;
    mutexVal.cps = 0;
    mutexVal.dice = 0;
    mutexVal.diceAvailiable = 0;
    mutexVal.method = 0;
    uint32_t counter = 0;

    mutexVal.mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!mutexVal.mutex) {
        furi_message_queue_free(event_queue);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, &mutexVal.mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    furi_hal_gpio_add_int_callback(&gpio_ext_pa7, gpiocallback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FuriTimer* timer = furi_timer_alloc(clock_tick, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, 1000);

    FuriTimer* timerPause = furi_timer_alloc(clock_tick_pause, FuriTimerTypePeriodic, event_queue);

    // ENABLE 5V pin
    furi_hal_power_enable_otg();

    uint8_t diceBuffer[64];
    for(uint8_t i = 0; i < 64; i++) diceBuffer[i] = 0;

    uint8_t diceBufferCounter = 0;
    uint8_t diceBufferPositionWrite = 0;
    uint8_t diceBufferPositionRead = 0;
    uint8_t tickCounter = 0;
    uint32_t CRC32 = 0;
    uint8_t method = 0;

    // MD5
    md5_context* md5_ctx = malloc(sizeof(md5_context));
    uint8_t* hash = malloc(sizeof(uint8_t) * 16);
    uint8_t* bufferTim2 = malloc(4);
    md5_starts(md5_ctx);

    uint8_t pause = 0;

    while(1) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, FuriWaitForever);

        uint8_t screenRefresh = 0;

        if(event_status == FuriStatusOk) {
            if(event.type == EventTypeInput) {
                if(event.input.key == InputKeyBack && event.input.type == InputTypeLong) {
                    break;
                } else if(pause == 0) {
                    if(event.input.key == InputKeyOk && event.input.type == InputTypeShort) {
                        if(diceBufferCounter > 0) {
                            furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                            mutexVal.dice = diceBuffer[diceBufferPositionRead];
                            mutexVal.diceAvailiable = --diceBufferCounter;
                            mutexVal.pause = 1;
                            furi_mutex_release(mutexVal.mutex);

                            if(diceBufferPositionRead != 63)
                                diceBufferPositionRead++;
                            else
                                diceBufferPositionRead = 0;

                            pause = 1;
                            furi_timer_start(timerPause, 500);
                            screenRefresh = 1;
                        }
                    } else if(event.input.key == InputKeyLeft && event.input.type == InputTypeLong) {
                        if(method == 1) {
                            method = 0;
                            diceBufferPositionWrite = 0;
                            diceBufferPositionRead = 0;
                            diceBufferCounter = 0;
                            CRC32 = 0;
                            tickCounter = 0;
                            furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                            mutexVal.method = 0;
                            mutexVal.dice = 0;
                            mutexVal.diceAvailiable = 0;
                            furi_mutex_release(mutexVal.mutex);
                            screenRefresh = 1;
                        }
                    } else if(event.input.key == InputKeyRight && event.input.type == InputTypeLong) {
                        if(method == 0) {
                            method = 1;
                            diceBufferPositionWrite = 0;
                            diceBufferPositionRead = 0;
                            diceBufferCounter = 0;
                            md5_starts(md5_ctx);
                            tickCounter = 0;
                            furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                            mutexVal.method = 1;
                            mutexVal.dice = 0;
                            mutexVal.diceAvailiable = 0;
                            furi_mutex_release(mutexVal.mutex);
                            screenRefresh = 1;
                        }
                    }
                }
            } else if(event.type == ClockEventTypeTick) {
                furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                mutexVal.cps = counter;
                furi_mutex_release(mutexVal.mutex);

                counter = 0;
                screenRefresh = 1;
            } else if(event.type == ClockEventTypeTickPause) {
                furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                mutexVal.pause = 0;
                furi_mutex_release(mutexVal.mutex);

                furi_timer_stop(timerPause);

                pause = 0;
                screenRefresh = 1;
            } else if(event.type == EventGPIO) {
                if(diceBufferCounter < 64) {
                    // CRC32
                    if(method == 0) {
                        uint32_t TIM2Tick = TIM2->CNT;
                        bufferTim2[0] = (uint8_t)(TIM2Tick >> 24);
                        bufferTim2[1] = (uint8_t)(TIM2Tick >> 16);
                        bufferTim2[2] = (uint8_t)(TIM2Tick >> 8);
                        bufferTim2[3] = (uint8_t)TIM2Tick;
                        CRC32 = crc32_calc_buffer(CRC32, bufferTim2, 4);
                        tickCounter++;

                        if(tickCounter == 8) {
                            uint8_t localDice = CRC32 & 0b111;

                            if(localDice == 0 || localDice == 7) {
                                localDice = (diceBuffer[diceBufferPositionRead] >> 3) & 0b111;
                            }

                            if(localDice >= 1 && localDice <= 6) {
                                diceBuffer[diceBufferPositionWrite] = localDice;
                                diceBufferCounter++;
                                if(diceBufferPositionWrite != 63)
                                    diceBufferPositionWrite++;
                                else
                                    diceBufferPositionWrite = 0;

                                furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                                mutexVal.diceAvailiable = diceBufferCounter;
                                furi_mutex_release(mutexVal.mutex);

                                screenRefresh = 1;
                            }

                            CRC32 = 0;
                            tickCounter = 0;
                        }
                    }
                    // MD5
                    else {
                        uint32_t tick = TIM2->CNT;
                        bufferTim2[0] = (uint8_t)(tick >> 24);
                        bufferTim2[1] = (uint8_t)(tick >> 16);
                        bufferTim2[2] = (uint8_t)(tick >> 8);
                        bufferTim2[3] = (uint8_t)tick;
                        md5_update(md5_ctx, bufferTim2, 4);

                        tickCounter++;

                        if(tickCounter == 32) {
                            md5_finish(md5_ctx, hash);
                            uint8_t localDice = 0;

                            for(uint8_t i = 0; i < 16; i++) {
                                localDice = hash[i] & 0b111;
                                if(localDice >= 1 && localDice <= 6) {
                                    diceBuffer[diceBufferPositionWrite] = localDice;
                                    diceBufferCounter++;
                                    if(diceBufferPositionWrite != 63)
                                        diceBufferPositionWrite++;
                                    else
                                        diceBufferPositionWrite = 0;

                                    furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                                    mutexVal.diceAvailiable = diceBufferCounter;
                                    furi_mutex_release(mutexVal.mutex);

                                    screenRefresh = 1;
                                    break;
                                }
                            }

                            md5_starts(md5_ctx);
                            tickCounter = 0;
                        }
                    }
                }

                counter++;
            }
        }

        if(screenRefresh == 1) view_port_update(view_port);
    }

    LL_TIM_DisableCounter(TIM2);
    furi_hal_bus_disable(FuriHalBusTIM2);

    free(md5_ctx);
    free(bufferTim2);
    free(hash);

    furi_record_close(RECORD_NOTIFICATION);

    furi_hal_power_disable_otg();

    furi_hal_gpio_disable_int_callback(&gpio_ext_pa7);
    furi_hal_gpio_remove_int_callback(&gpio_ext_pa7);

    furi_message_queue_free(event_queue);
    furi_mutex_free(mutexVal.mutex);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_timer_free(timer);
    furi_timer_free(timerPause);
    furi_record_close(RECORD_GUI);

    return 0;
}