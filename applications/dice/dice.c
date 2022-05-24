#include "furi_hal_random.h"
#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include <gui/gui.h>
#include <input/input.h>
#define TAG "Dice Roller"
uint8_t diceSelect=7;
uint8_t diceRoll=0;
char rollTime[1][12];
char diceType[1][5];
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
    char strings[2][25];
    if (letsRoll) {
        if(diceSelect==0) {
            sprintf(diceType[0], "%s", "d2");
            diceRoll = rand() % 2;
        } else if(diceSelect==1) {
            sprintf(diceType[0], "%s", "d3");
            diceRoll = rand() % 3;
        } else if(diceSelect==2) {
            sprintf(diceType[0], "%s", "d4");
            diceRoll = rand() % 4;
        } else if(diceSelect==3) {
            sprintf(diceType[0], "%s", "d6");
            diceRoll = rand() % 6;
        } else if(diceSelect==4) {
            sprintf(diceType[0], "%s", "d8");
            diceRoll = rand() % 8;
        } else if(diceSelect==5) {
            sprintf(diceType[0], "%s", "d10");
            diceRoll = rand() % 10;
        } else if(diceSelect==6) {
            sprintf(diceType[0], "%s", "d12");
            diceRoll = rand() % 12;
        } else if(diceSelect==7) {
            sprintf(diceType[0], "%s", "d20");
            diceRoll = rand() % 20;
        } else if(diceSelect==8) {
            sprintf(diceType[0], "%s", "d100");
            diceRoll = rand() % 100;
        }
        diceRoll = diceRoll + 1;
        ClockState* state = (ClockState*)acquire_mutex((ValueMutex*)ctx, 25);
        sprintf(rollTime[0], "%.2d:%.2d:%.2d", state->datetime.hour, state->datetime.minute, state->datetime.second);
        release_mutex((ValueMutex*)ctx, state);
        letsRoll=false;
    }
	sprintf(strings[0], "%s at %s", diceType[0], rollTime[0]);
	sprintf(strings[1], "%d", diceRoll);
    if(diceRoll!=0) {
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignCenter, strings[1]);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, strings[0]);
	}
    elements_button_center(canvas, "Roll");
    if(diceSelect==0) {
        elements_button_right(canvas, "d2");
    } else if(diceSelect==1) {
        elements_button_right(canvas, "d3");
    } else if(diceSelect==2) {
        elements_button_right(canvas, "d4");
    } else if(diceSelect==3) {
        elements_button_right(canvas, "d6");
    } else if(diceSelect==4) {
        elements_button_right(canvas, "d8");
    } else if(diceSelect==5) {
        elements_button_right(canvas, "d10");
    } else if(diceSelect==6) {
        elements_button_right(canvas, "d12");
    } else if(diceSelect==7) {
        elements_button_right(canvas, "d20");
    } else if(diceSelect==8) {
        elements_button_right(canvas, "d100");
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
    diceSelect=7;
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
                        if(diceSelect==0) {
                            diceSelect=1;
                        } else if(diceSelect==1)  {
                            diceSelect=2;
                        } else if(diceSelect==2)  {
                            diceSelect=3;
                        } else if(diceSelect==3)  {
                            diceSelect=4;
                        } else if(diceSelect==4)  {
                            diceSelect=5;
                        } else if(diceSelect==5)  {
                            diceSelect=6;
                        } else if(diceSelect==6)  {
                            diceSelect=7;
                        } else if(diceSelect==7)  {
                            diceSelect=8;
                        } else {
                            diceSelect=0;
                        }
                        break;
                    case InputKeyLeft:
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
