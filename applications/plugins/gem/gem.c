#include "gem.h"
#include "Gem_icons.h"

static void draw_interface(Canvas* const canvas) {
    // border around the edge of the screen
    //canvas_draw_frame(canvas, 0, 0, FLIPPER_LCD_WIDTH, FLIPPER_LCD_HEIGHT);

    canvas_draw_icon(canvas, TITLE_OFFSET, TITLE_OFFSET, &I_gem_10px);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, (TITLE_OFFSET * 2) + LOGO_WIDTH, TITLE_OFFSET, AlignLeft, AlignTop, TITLE);

    canvas_draw_line(
        canvas,
        0,
        TITLE_OFFSET + TEXT_OFFSET + 1,
        FLIPPER_LCD_WIDTH,
        TITLE_OFFSET + TEXT_OFFSET + 1);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas, FLIPPER_LCD_WIDTH - TITLE_OFFSET, TITLE_OFFSET, AlignRight, AlignTop, SUBTITLE);
}

static void render_callback(Canvas* const canvas, void* ctx) {
    const PluginState* plugin_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(plugin_state == NULL) {
        return;
    }

    //Draw static interface
    draw_interface(canvas);

    //Draw interactive elements
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, plugin_state->x, plugin_state->y, AlignLeft, AlignTop, TAG);

    release_mutex((ValueMutex*)ctx, plugin_state);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void gem_state_init(PluginState* const plugin_state) {
    plugin_state->x = TITLE_OFFSET;
    plugin_state->y = START_Y + TITLE_OFFSET;
}

int32_t gem_app(void* p) {
    UNUSED(p);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    PluginState* plugin_state = malloc(sizeof(PluginState));
    gem_state_init(plugin_state);
    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, plugin_state, sizeof(PluginState))) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(plugin_state);
        return 255;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        PluginState* plugin_state = (PluginState*)acquire_mutex_block(&state_mutex);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        plugin_state->y--;
                        break;
                    case InputKeyDown:
                        plugin_state->y++;
                        break;
                    case InputKeyRight:
                        plugin_state->x++;
                        break;
                    case InputKeyLeft:
                        plugin_state->x--;
                        break;
                    case InputKeyOk:
                    case InputKeyBack:
                        processing = false;
                        break;
                    case InputKeyMAX:
                        break;
                    }
                }
            } else if(event.type == EventTypeTick) {
                //Tick
            }
        } else {
            FURI_LOG_D("gem", "osMessageQueue: event timeout");
            // event timeout
        }

        view_port_update(view_port);
        release_mutex(&state_mutex, plugin_state);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    free(plugin_state);

    return 0;
}