#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <furi_hal_rfid.h>

typedef enum {
    EventTypeInput,
    ClockEventTypeTick,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} EventApp;

const char CW_char[54] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',  'M', 'N',
                          'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',  '0', '1',
                          '2', '3', '4', '5', '6', '7', '8', '9', '.', ',', '?', '\'', '!', '/',
                          '(', ')', '&', ':', ';', '=', '+', '-', '_', '"', '$', '@'};
const uint8_t CW_size[54] = {2, 4, 4, 3, 1, 4, 3, 4, 2, 4, 3, 4, 2, 2, 3, 4, 4, 3,
                             3, 1, 3, 4, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                             6, 6, 6, 6, 6, 5, 5, 6, 5, 6, 6, 5, 5, 6, 6, 6, 7, 6};
const uint8_t CW_value[54] = {
    0b01000000, 0b10000000, 0b10100000, 0b10000000, 0b00000000, 0b00100000, 0b11000000, 0b00000000,
    0b00000000, 0b01110000, 0b10100000, 0b01000000, 0b11000000, 0b10000000, 0b11100000, 0b01100000,
    0b11010000, 0b01000000, 0b00000000, 0b10000000, 0b00100000, 0b00010000, 0b01100000, 0b10010000,
    0b10110000, 0b11000000, 0b11111000, 0b01111000, 0b00111000, 0b00011000, 0b00001000, 0b00000000,
    0b10000000, 0b11000000, 0b11100000, 0b11110000, 0b01010100, 0b11001100, 0b00110000, 0b01111000,
    0b10101100, 0b10010000, 0b10110000, 0b10110100, 0b01000000, 0b11100000, 0b10101000, 0b10001000,
    0b01010000, 0b10000100, 0b00110100, 0b01001000, 0b00010010, 0b01101000};

typedef struct {
    FuriMutex* mutex;
    uint8_t status;
    uint8_t enableCW_mutex;
} mutexStruct;

static void draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);

    mutexStruct* mutexVal = ctx;
    mutexStruct mutexDraw;
    furi_mutex_acquire(mutexVal->mutex, FuriWaitForever);
    memcpy(&mutexDraw, mutexVal, sizeof(mutexStruct));
    furi_mutex_release(mutexVal->mutex);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 5, AlignCenter, AlignCenter, "RFID Beacon");

    char buffer[24];
    uint8_t positionBuffer = 0;
    buffer[positionBuffer++] = CW_char[mutexVal->status];
    buffer[positionBuffer++] = ' ';
    buffer[positionBuffer++] = ' ';
    buffer[positionBuffer++] = ' ';
    buffer[positionBuffer++] = ' ';

    uint8_t maskMorse = 0b10000000;
    for(uint8_t i = 0; i < CW_size[mutexVal->status]; i++) {
        if(i != 0) {
            buffer[positionBuffer++] = ' ';
            maskMorse >>= 1;
        }

        if((CW_value[mutexVal->status] & maskMorse) != 0) {
            buffer[positionBuffer++] = '_';
        } else {
            buffer[positionBuffer++] = '.';
        }
    }

    buffer[positionBuffer++] = '\0';

    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, buffer);

    if(mutexVal->enableCW_mutex == 0)
        canvas_draw_str_aligned(canvas, 64, 59, AlignCenter, AlignCenter, "OFF");
    else
        canvas_draw_str_aligned(canvas, 64, 59, AlignCenter, AlignCenter, "On the air");
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

static uint8_t RFID_STATUS = 0;
void RFID_ON(NotificationApp* notification) {
    if(RFID_STATUS == 0) {
        notification_message(notification, &sequence_set_only_red_255);
        furi_hal_rfid_tim_read_start(125000, 0.5);
        RFID_STATUS = 1;
    }
}

void RFID_OFF(NotificationApp* notification) {
    if(RFID_STATUS == 1) {
        notification_message(notification, &sequence_reset_red);
        furi_hal_rfid_tim_read_stop();
        RFID_STATUS = 0;
    }
}

int32_t flipper_rfidbeacon_app() {
    EventApp event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(EventApp));

    mutexStruct mutexVal;
    mutexVal.status = 0;
    mutexVal.enableCW_mutex = 0;

    mutexVal.mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!mutexVal.mutex) {
        furi_message_queue_free(event_queue);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, &mutexVal.mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    FuriTimer* timer = furi_timer_alloc(clock_tick, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, 100);

    uint8_t enableCW = 0;
    uint8_t letterState = 0;
    uint8_t letterPosition = 0;
    uint8_t letterChosen = 0;

    // 1 : pause, 2 : dot, 3 : dash
    uint8_t draw = 0;

    while(1) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);
        uint8_t screenRefresh = 0;

        if(event.type == EventTypeInput) {
            if(event.input.key == InputKeyBack && event.input.type == InputTypeLong) {
                break;
            } else if(event.input.key == InputKeyUp && event.input.type == InputTypeLong) {
                furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);

                if(enableCW == 0) {
                    enableCW = 1;
                    letterPosition = 0;
                    draw = 0;
                } else {
                    RFID_OFF(notification);
                    enableCW = 0;
                }

                mutexVal.enableCW_mutex = enableCW;
                screenRefresh = 1;
                furi_mutex_release(mutexVal.mutex);
            } else if(event.input.key == InputKeyLeft && event.input.type == InputTypeShort) {
                furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);

                if(mutexVal.status != 0)
                    mutexVal.status--;
                else
                    mutexVal.status = 53;

                screenRefresh = 1;
                furi_mutex_release(mutexVal.mutex);
            } else if(event.input.key == InputKeyRight && event.input.type == InputTypeShort) {
                furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);

                if(mutexVal.status != 53)
                    mutexVal.status++;
                else
                    mutexVal.status = 0;

                screenRefresh = 1;
                furi_mutex_release(mutexVal.mutex);
            }
        } else if(event.type == ClockEventTypeTick) {
            if(enableCW == 1) {
                if(draw == 0) {
                    if(letterPosition == 0) {
                        furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                        letterChosen = mutexVal.status;
                        furi_mutex_release(mutexVal.mutex);
                    }

                    if(letterPosition == CW_size[letterChosen]) {
                        draw = 1;
                        letterPosition = 0;
                    } else {
                        uint8_t mask = 0b10000000;
                        mask >>= letterPosition;
                        if((CW_value[letterChosen] & mask) != 0)
                            draw = 3;
                        else
                            draw = 2;
                        letterState = 0;
                        letterPosition++;
                    }
                }

                // PAUSE
                if(draw == 1) {
                    letterState++;
                    if(letterState == 30) {
                        letterState = 0;
                        draw = 0;
                    }
                }
                // DOT
                else if(draw == 2) {
                    if(letterState == 0) {
                        RFID_ON(notification);
                        letterState = 1;
                    } else {
                        RFID_OFF(notification);
                        draw = 0;
                        letterState = 0;
                    }
                }
                // DASH
                else if(draw == 3) {
                    if(letterState == 0) {
                        RFID_ON(notification);
                        letterState = 1;
                    } else {
                        if(letterState != 3)
                            letterState++;
                        else {
                            RFID_OFF(notification);
                            draw = 0;
                            letterState = 0;
                        }
                    }
                }
            }
        }

        if(screenRefresh == 1) view_port_update(view_port);
    }

    RFID_OFF(notification);

    furi_timer_free(timer);
    furi_message_queue_free(event_queue);
    furi_record_close(RECORD_NOTIFICATION);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    return 0;
}