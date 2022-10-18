#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include "coffee.h"
#include <math.h>

#define TAG "COFFEE EEPROM"

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
   double credit;
   char status[20];
} PluginState;


static void coffee_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void coffee_render_callback(Canvas* const canvas, void* ctx) {

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    const PluginState* plugin_state = acquire_mutex((ValueMutex*)ctx, 25);
    canvas_set_font(canvas, FontSecondary);
    elements_button_left(canvas, "Virgin");
    elements_button_right(canvas, "10E");
    elements_button_center(canvas, "Dump");
    canvas_set_font(canvas, FontPrimary);
    char str[340];
    snprintf(str, 340, "Credit: %.2f EUR", plugin_state->credit);
    canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, str);
    canvas_draw_str_aligned(canvas, 64, 26, AlignCenter, AlignCenter, plugin_state->status);
    release_mutex((ValueMutex*)ctx, plugin_state);
}

static void coffee_eeprom_state_init(PluginState* const plugin_state) {
    plugin_state->credit = read_credit();
}

int32_t coffee_eeprom_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    PluginState* plugin_state = malloc(sizeof(PluginState));
    coffee_eeprom_state_init(plugin_state);
    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, plugin_state, sizeof(PluginState))) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(plugin_state);
        return 255;
    }
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, coffee_render_callback, &state_mutex);
    view_port_input_callback_set(view_port, coffee_input_callback, event_queue);
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        PluginState* plugin_state = (PluginState*)acquire_mutex_block(&state_mutex);
        if(event_status == FuriStatusOk) {
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyRight:
                        write_10_eur();
                        plugin_state->credit = read_credit();
                        break;
                    case InputKeyLeft:
                        virgin();
                        plugin_state->credit = read_credit();
                        snprintf(plugin_state->status, sizeof(plugin_state->status), "Virgin done!");
                        break;
                    case InputKeyOk:
                        dump();
                        snprintf(plugin_state->status, sizeof(plugin_state->status), "Dumped to logs");
                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    }
                }
            }
        } else {
            //FURI_LOG_D(TAG, "osMessageQueue: event timeout");
        }
        view_port_update(view_port);
        release_mutex(&state_mutex, plugin_state);
    }
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    return 0;
}