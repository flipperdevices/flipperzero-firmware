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
    ChessClockStateSetup,
    ChessClockStatePlayer1,
    ChessClockStatePlayer2,
    ChessClockStateGameOver,
} ChessClockState;

typedef struct {
    ChessClockState state;
    uint32_t total_time;
    uint32_t time_left_p1;
    uint32_t time_left_p2;
    uint32_t last_tick;
} PluginState;

static void render_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    PluginState* plugin_state = ctx;

    char text[50];

    canvas_set_font(canvas, FontPrimary);
    switch(plugin_state->state) {
    case ChessClockStateSetup:
    case ChessClockStatePlayer1:
    case ChessClockStatePlayer2:
        snprintf(
            text,
            sizeof(text),
            "Player 1: %02lu:%02lu:%02lu\nPlayer 2: %02lu:%02lu:%02lu",
            plugin_state->time_left_p1 / 60000,
            (plugin_state->time_left_p1 / 1000) % 60,
            (plugin_state->time_left_p1 % 1000) / 10,
            plugin_state->time_left_p2 / 60000,
            (plugin_state->time_left_p2 / 1000) % 60,
            (plugin_state->time_left_p2 % 1000) / 10);
        break;
    case ChessClockStateGameOver:
        snprintf(text, sizeof(text), "Player %d wins!", plugin_state->time_left_p1 > 0 ? 1 : 2);
        break;
    }
    elements_multiline_text_aligned(canvas, 64, 32, AlignCenter, AlignCenter, text);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

int32_t chess_clock_app() {
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    PluginState* plugin_state = malloc(sizeof(PluginState));
    plugin_state->state = ChessClockStateSetup;
    plugin_state->total_time = 300000;
    plugin_state->time_left_p1 = plugin_state->total_time;
    plugin_state->time_left_p2 = plugin_state->total_time;
    plugin_state->last_tick = 0;

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, plugin_state);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 10);

        uint32_t now = furi_get_tick();

        if(event_status == FuriStatusOk && event.type == EventTypeKey &&
           event.input.key == InputKeyBack) {
            if(event.input.type == InputTypeShort) {
                plugin_state->state = ChessClockStateSetup;
                plugin_state->time_left_p1 = plugin_state->total_time;
                plugin_state->time_left_p2 = plugin_state->total_time;
                notification_message(notification, &sequence_reset_rgb);
            } else if(event.input.type == InputTypeLong) {
                processing = false;
            }
        }

        switch(plugin_state->state) {
        case ChessClockStateSetup:
            if(event_status == FuriStatusOk && event.type == EventTypeKey) {
                if(event.input.key == InputKeyUp && event.input.type == InputTypePress) {
                    plugin_state->total_time += 15000;
                    plugin_state->time_left_p1 = plugin_state->total_time;
                    plugin_state->time_left_p2 = plugin_state->total_time;
                } else if(event.input.key == InputKeyDown && event.input.type == InputTypePress) {
                    if(plugin_state->total_time > 15000) {
                        plugin_state->total_time -= 15000;
                        plugin_state->time_left_p1 = plugin_state->total_time;
                        plugin_state->time_left_p2 = plugin_state->total_time;
                    }
                } else if(event.input.key == InputKeyOk && event.input.type == InputTypePress) {
                    plugin_state->state = ChessClockStatePlayer1;
                    plugin_state->last_tick = now;
                }
            }
            break;

        case ChessClockStatePlayer1:
            if(event_status == FuriStatusOk && event.type == EventTypeKey &&
               event.input.key == InputKeyOk && event.input.type == InputTypePress) {
                plugin_state->state = ChessClockStatePlayer2;
                plugin_state->last_tick = now;
            } else {
                plugin_state->time_left_p1 -= now - plugin_state->last_tick;
                if(plugin_state->time_left_p1 <= 0 ||
                   plugin_state->time_left_p1 >= UINT32_MAX - 1000) {
                    plugin_state->time_left_p1 = 0;
                    plugin_state->state = ChessClockStateGameOver;
                    notification_message(notification, &sequence_set_only_blue_255);
                    notification_message(notification, &sequence_single_vibro);
                }
                plugin_state->last_tick = now;
            }
            break;

        case ChessClockStatePlayer2:
            if(event_status == FuriStatusOk && event.type == EventTypeKey &&
               event.input.key == InputKeyOk && event.input.type == InputTypePress) {
                plugin_state->state = ChessClockStatePlayer1;
                plugin_state->last_tick = now;
            } else {
                plugin_state->time_left_p2 -= now - plugin_state->last_tick;
                if(plugin_state->time_left_p2 <= 0 ||
                   plugin_state->time_left_p2 >= UINT32_MAX - 1000) {
                    plugin_state->time_left_p2 = 0;
                    plugin_state->state = ChessClockStateGameOver;
                    notification_message(notification, &sequence_set_only_green_255);
                    notification_message(notification, &sequence_single_vibro);
                }
                plugin_state->last_tick = now;
            }
            break;

        case ChessClockStateGameOver:
            if(event_status == FuriStatusOk && event.type == EventTypeKey &&
               event.input.key == InputKeyBack && event.input.type == InputTypeShort) {
                plugin_state->state = ChessClockStateSetup;
                plugin_state->time_left_p1 = plugin_state->total_time;
                plugin_state->time_left_p2 = plugin_state->total_time;
            }
            break;
        }

        view_port_update(view_port);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    free(plugin_state);

    return 0;
}
