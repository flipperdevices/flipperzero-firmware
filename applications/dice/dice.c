#include <furi.h>
#include <furi_hal.h>
#include "furi_hal_random.h"
#include <gui/elements.h>
#include <gui/gui.h>
#include <input/input.h>

#define TAG "Dice Roller"
uint8_t diceSelect=20;
uint8_t diceQty=1;
uint8_t diceRoll=0;
char rollTime[1][12];
char diceType[1][5];
char strings[2][45];
bool letsRoll=false;

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    FuriHalRtcDateTime datetime;
} ClockState;

static void dice_input_callback(InputEvent* input_event, osMessageQueueId_t event_queue) {
    furi_assert(event_queue); 
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    osMessageQueuePut(event_queue, &event, 0, osWaitForever);
}

static void dice_render_callback(Canvas* const canvas, void* ctx) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    ClockState* state = (ClockState*)acquire_mutex((ValueMutex*)ctx, 25);
    if (letsRoll) {
        diceRoll= ((rand() % diceSelect)+1);
        sprintf(diceType[0], "%s%d", "d", diceSelect);
        sprintf(rollTime[0], "%.2d:%.2d:%.2d", state->datetime.hour, state->datetime.minute, state->datetime.second);
        sprintf(strings[0], "%d%s at %s", diceQty, diceType[0], rollTime[0]);
        if(diceQty==1) {
            sprintf(strings[1], "%d",diceRoll);
        } else if(diceQty==2) {
            sprintf(strings[1], "%d %d",diceRoll, ((rand() % diceSelect)+1));
        } else if(diceQty==3) {
            sprintf(strings[1], "%d %d %d",diceRoll, ((rand() % diceSelect)+1), ((rand() % diceSelect)+1));
        } else if(diceQty==4) {
            sprintf(strings[1], "%d %d %d %d",diceRoll, ((rand() % diceSelect)+1), ((rand() % diceSelect)+1), ((rand() % diceSelect)+1));
        } else if(diceQty==5) {
            sprintf(strings[1], "%d %d %d %d %d",diceRoll, ((rand() % diceSelect)+1), ((rand() % diceSelect)+1), ((rand() % diceSelect)+1), ((rand() % diceSelect)+1));
        } else if(diceQty==6) {
            sprintf(strings[1], "%d %d %d %d %d %d",diceRoll, ((rand() % diceSelect)+1), ((rand() % diceSelect)+1), ((rand() % diceSelect)+1), ((rand() % diceSelect)+1), ((rand() % diceSelect)+1));
        }
        letsRoll=false;
    }
    release_mutex((ValueMutex*)ctx, state);
    if(diceRoll!=0) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignCenter, strings[1]);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, strings[0]);
    }
    elements_button_center(canvas, "Roll");
    if(diceSelect==2) {
        elements_button_right(canvas, "d2");
    } else if(diceSelect==3) {
        elements_button_right(canvas, "d3");
    } else if(diceSelect==4) {
        elements_button_right(canvas, "d4");
    } else if(diceSelect==6) {
        elements_button_right(canvas, "d6");
    } else if(diceSelect==8) {
        elements_button_right(canvas, "d8");
    } else if(diceSelect==10) {
        elements_button_right(canvas, "d10");
    } else if(diceSelect==12) {
        elements_button_right(canvas, "d12");
    } else if(diceSelect==20) {
        elements_button_right(canvas, "d20");
    } else if(diceSelect==59) {
        elements_button_right(canvas, "d59");
    } else if(diceSelect==69) {
        elements_button_right(canvas, "d69");
    } else if(diceSelect==100) {
        elements_button_right(canvas, "d100");
    }
    if(diceQty==1) {
        elements_button_left(canvas, "x1");
    } else if(diceQty==2) {
        elements_button_left(canvas, "x2");
    } else if(diceQty==3) {
        elements_button_left(canvas, "x3");
    } else if(diceQty==4) {
        elements_button_left(canvas, "x4");
    } else if(diceQty==5) {
        elements_button_left(canvas, "x5");
    } else if(diceQty==6) {
        elements_button_left(canvas, "x6");
    } 
}

static void diceclock_state_init(ClockState* const state) {
    furi_hal_rtc_get_datetime(&state->datetime);
}

static void dice_tick(void* ctx) {
    furi_assert(ctx);
    osMessageQueueId_t event_queue = ctx;
    PluginEvent event = {.type = EventTypeTick};
    osMessageQueuePut(event_queue, &event, 0, 0);
}

int32_t dice_app(void* p) {
    UNUSED(p);
    letsRoll=false;
    diceSelect=20;
    diceQty=1;
    diceRoll=0;
    osMessageQueueId_t event_queue = osMessageQueueNew(8, sizeof(PluginEvent), NULL);
    ClockState* plugin_state = malloc(sizeof(ClockState));
    diceclock_state_init(plugin_state);
    ValueMutex state_mutex;
    if (!init_mutex(&state_mutex, plugin_state, sizeof(ClockState))) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(plugin_state);
        return 255;
    }
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, dice_render_callback, &state_mutex);
    view_port_input_callback_set(view_port, dice_input_callback, event_queue);
    osTimerId_t timer = osTimerNew(dice_tick, osTimerPeriodic, event_queue, NULL);
    osTimerStart(timer, osKernelGetTickFreq());
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    PluginEvent event;
    for (bool processing = true; processing;) {
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, 100);
        ClockState* plugin_state = (ClockState*)acquire_mutex_block(&state_mutex);
        if (event_status == osOK) {
            if (event.type == EventTypeKey) {
                if (event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyRight:
                        if(diceSelect==2) {
                            diceSelect=3;
                        } else if(diceSelect==3)  {
                            diceSelect=4;
                        } else if(diceSelect==4)  {
                            diceSelect=6;
                        } else if(diceSelect==6)  {
                            diceSelect=8;
                        } else if(diceSelect==8)  {
                            diceSelect=10;
                        } else if(diceSelect==10)  {
                            diceSelect=12;
                        } else if(diceSelect==12)  {
                            diceSelect=20;
                        } else if(diceSelect==20)  {
                            diceSelect=59;
                        } else if(diceSelect==59)  {
                            diceSelect=69;
                        } else if(diceSelect==69)  {
                            diceSelect=100;
                        } else {
                            diceSelect=2;
                        }
                        break;
                    case InputKeyLeft:
                        if(diceQty<=5) {
                            diceQty=diceQty+1;
                        } else {
                            diceQty=1;
                        }
                        break;
                    case InputKeyOk: 
                        letsRoll=true;
                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    }
                }
            } else if(event.type == EventTypeTick) {
                furi_hal_rtc_get_datetime(&plugin_state->datetime);
            }
        } else {
            FURI_LOG_D(TAG, "osMessageQueue: event timeout");
        }
        view_port_update(view_port);
        release_mutex(&state_mutex, plugin_state);
    }
    osTimerDelete(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    osMessageQueueDelete(event_queue);
    return 0;
}
