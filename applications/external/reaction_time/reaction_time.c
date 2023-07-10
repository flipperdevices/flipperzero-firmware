#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>
#include <furi_hal.h>

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef enum {
    GameStateStart,
    GameStateReady,
    GameStateWait,
    GameStatePress,
    GameStateShowTime,
    GameStateLoss,
} GameState;

typedef struct {
    GameState state;
    uint32_t start_tick;
    uint32_t reaction_time;
} PluginState;

static void render_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    PluginState* plugin_state = ctx;

    canvas_set_font(canvas, FontPrimary);
    switch(plugin_state->state) {
    case GameStateStart:
        elements_multiline_text_aligned(canvas, 64, 28, AlignCenter, AlignTop, "Press to start");
        break;
    case GameStateReady:
    case GameStateWait:
        elements_multiline_text_aligned(canvas, 64, 28, AlignCenter, AlignTop, "Get ready...");
        break;
    case GameStatePress:
        elements_multiline_text_aligned(canvas, 64, 28, AlignCenter, AlignTop, "PRESS");
        break;
    case GameStateShowTime: {
        char text[50];
        snprintf(text, sizeof(text), "Reaction time: %lu ms", plugin_state->reaction_time);
        elements_multiline_text_aligned(canvas, 64, 28, AlignCenter, AlignTop, text);
        break;
    }
    case GameStateLoss:
        elements_multiline_text_aligned(canvas, 64, 28, AlignCenter, AlignTop, "Too soon!");
        break;
    }
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

int32_t reaction_time_app() {
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    PluginState* plugin_state = malloc(sizeof(PluginState));
    plugin_state->state = GameStateStart;

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, plugin_state);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);

    uint32_t wait_time = 2000 + (rand() % 18000);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 10);

        if(event_status == FuriStatusOk && event.type == EventTypeKey &&
           event.input.key == InputKeyBack) {
            break;
        }

        switch(plugin_state->state) {
        case GameStateStart:
            if(event_status == FuriStatusOk && event.type == EventTypeKey &&
               event.input.type == InputTypePress) {
                plugin_state->start_tick = furi_get_tick();
                plugin_state->state = GameStateReady;
            }
            break;
        case GameStateReady:
            plugin_state->state = GameStateWait;
            notification_message(notification, &sequence_set_only_red_255);
            break;
        case GameStateWait:
            if(event_status == FuriStatusOk && event.type == EventTypeKey &&
               event.input.type == InputTypePress) {
                plugin_state->state = GameStateLoss;
                notification_message(notification, &sequence_single_vibro);
            } else if((furi_get_tick() - plugin_state->start_tick) > wait_time) {
                plugin_state->start_tick = furi_get_tick();
                plugin_state->state = GameStatePress;
                notification_message(notification, &sequence_set_only_blue_255);
                notification_message(notification, &sequence_single_vibro);
            }
            break;
        case GameStatePress:
            if(event_status == FuriStatusOk && event.type == EventTypeKey &&
               event.input.type == InputTypePress) {
                plugin_state->reaction_time = (furi_get_tick() - plugin_state->start_tick);
                plugin_state->state = GameStateShowTime;
            }
            break;
        case GameStateShowTime:
            if(event_status == FuriStatusOk && event.type == EventTypeKey &&
               event.input.type == InputTypePress) {
                plugin_state->state = GameStateStart;
                notification_message(notification, &sequence_reset_rgb);
            }
            break;
        case GameStateLoss:
            if(event_status == FuriStatusOk && event.type == EventTypeKey &&
               event.input.type == InputTypePress) {
                plugin_state->state = GameStateStart;
                notification_message(notification, &sequence_reset_rgb);
            }
            break;
        }

        view_port_update(view_port);
    }

    notification_message(notification, &sequence_reset_rgb);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    free(plugin_state);

    return 0;
}
