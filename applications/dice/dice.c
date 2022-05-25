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
uint8_t playerOneScore=0;
uint8_t playerTwoScore=0;
char rollTime[1][12];
char diceType[1][5];
char strings[2][45];
char theScores[1][45];
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
    if(diceSelect<230) {
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
    ClockState* state = (ClockState*)acquire_mutex((ValueMutex*)ctx, 25);
            const char* diceOne[] = {
                "Nibble", "Fondle", "Massage", "Touch",
                "Suck", "Lick", "Blow", "Kiss", "???",
            };
            const char* diceTwo[] = {
                "Navel", "Ears", "Lips", "Neck",
                "Thigh", "Hand", "Breasts", "Genitals",
            };
            const char* deckOne[] = {
                "2S", "2D", "2C", "2H", "3S", "3D", "3C", "3H", "4S", "4D", "4C", "4H", 
                "5S", "5D", "5C", "5H", "6S", "6D", "6C", "6H", "7S", "7D", "7C", "7H", 
                "8S", "8D", "8C", "8H", "9S", "9D", "9C", "9H", "10S", "10D", "10C", "10H", 
                "JS", "JD", "JC", "JH", "KS", "KD", "KC", "KH", "QS", "QD", "QC", "QH", "AS", "AD", "AC", "AH"
            };
            char* deckTwo[] = {
                "2S", "2D", "2C", "2H", "3S", "3D", "3C", "3H", "4S", "4D", "4C", "4H", 
                "5S", "5D", "5C", "5H", "6S", "6D", "6C", "6H", "7S", "7D", "7C", "7H", 
                "8S", "8D", "8C", "8H", "9S", "9D", "9C", "9H", "10S", "10D", "10C", "10H", 
                "JS", "JD", "JC", "JH", "KS", "KD", "KC", "KH", "QS", "QD", "QC", "QH", "AS", "AD", "AC"
            }; // ONE LESS SINCE ONE WILL BE REMOVED
    if (letsRoll) {
        static bool rand_generator_inited = false;
        if(!rand_generator_inited) {
            srand(DWT->CYCCNT);
            rand_generator_inited = true;
        }
        sprintf(rollTime[0], "%.2d:%.2d:%.2d", state->datetime.hour, state->datetime.minute, state->datetime.second);
        if(diceSelect==230) {
            diceRoll= ((rand() % diceSelect)+1); // JUST TO GET IT GOING? AND FIX BUG
            sprintf(diceType[0], "%s", "SEX?");
            sprintf(strings[0], "%s at %s", diceType[0], rollTime[0]);
            uint8_t d1_i = rand() % COUNT_OF(diceOne);
            uint8_t d2_i = rand() % COUNT_OF(diceTwo);
            sprintf(strings[1], "%s %s", diceOne[d1_i], diceTwo[d2_i]);
        } else if(diceSelect==231) {
            diceRoll= ((rand() % diceSelect)+1); // JUST TO GET IT GOING? AND FIX BUG
            sprintf(diceType[0], "%s", "WAR!");
            sprintf(strings[0], "%s at %s", diceType[0], rollTime[0]);
            uint8_t d1_i = rand() % COUNT_OF(deckOne);
            // INITIALIZE WITH PLACEHOLDERS TO AVOID MAYBE UNINITIALIZED ERROR
            for (int i = 0; i < COUNT_OF(deckOne); i++ ) {
                if(i<d1_i) {
                    sprintf(deckTwo[i], "%s", deckOne[i]);
                } else if(i>d1_i) {
                    sprintf(deckTwo[i-1], "%s", deckOne[i]);
                }
            }
            uint8_t d2_i = rand() % COUNT_OF(deckTwo);
            if(d1_i>d2_i) {
                playerOneScore++;
            } else {
                playerTwoScore++;
            }
            sprintf(strings[1], "%s <> %s", deckOne[d1_i], deckTwo[d2_i]);
        } else {
            diceRoll= ((rand() % diceSelect)+1);
            sprintf(diceType[0], "%s%d", "d", diceSelect);
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
        }
        letsRoll=false;
    }
    release_mutex((ValueMutex*)ctx, state);
    if(diceRoll!=0) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignCenter, strings[1]);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, strings[0]);
        if(diceSelect==231 && !(playerOneScore==0 && playerTwoScore==0)) {
            canvas_set_font(canvas, FontSecondary);
            sprintf(theScores[0], "%d                                   %d", playerOneScore, playerTwoScore);
            canvas_draw_str_aligned(canvas, 64, 34, AlignCenter, AlignCenter, theScores[0]);
        }
    }
    if(diceSelect==231) {
        elements_button_center(canvas, "Draw");
    } else {
        elements_button_center(canvas, "Roll");
    }
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
    } else if(diceSelect==230) {
        elements_button_right(canvas, "SEX");
    } else if(diceSelect==231) {
        elements_button_right(canvas, "WAR");
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
    playerOneScore=0;
    playerTwoScore=0;
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
                            diceSelect=100;
                        } else if(diceSelect==100)  {
                            diceSelect=230;
                        } else if(diceSelect==230)  {
                            playerOneScore=0;
                            playerTwoScore=0;
                            diceSelect=231;
                        } else if(diceSelect==231)  {
                            diceSelect=59;
                        } else if(diceSelect==59)  {
                            diceSelect=69;
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
