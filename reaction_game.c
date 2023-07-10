#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <stdbool.h> 

typedef struct {
    FuriMutex* mutex;
    bool screen_white;
    uint32_t counter;
    uint32_t random_wait_time;
} GameData;

void draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    GameData* game_state = ctx;
    furi_mutex_acquire(game_state->mutex, FuriWaitForever);

    if(game_state->screen_white) {
        canvas_set_color(canvas, ColorWhite);
    } else {
        canvas_set_color(canvas, ColorBlack);
    }

    canvas_clear(canvas);

    furi_mutex_release(game_state->mutex);
}

void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void game_timer_callback(void* ctx) {
    furi_assert(ctx);
    GameData* game_state = ctx;

    if(game_state->counter >= game_state->random_wait_time) {
        game_state->screen_white = !game_state->screen_white;
        game_state->counter = 0;
        game_state->random_wait_time = (rand() % 5) + 1;
    } else {
        game_state->counter++;
    }
}

int32_t reaction_game_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    GameData* game_state = malloc(sizeof(GameData));

    game_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!game_state->mutex) {
        FURI_LOG_E("game", "cannot create mutex\r\n");
        free(game_state);
        return -1;
    }

    game_state->screen_white = false;
    game_state->counter = 0;
    game_state->random_wait_time = (rand() % 5) + 1;

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, game_state);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FuriTimer* timer = furi_timer_alloc(game_timer_callback, FuriTimerTypePeriodic, game_state);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 25);

    InputEvent event;

    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        if(event.type == InputTypeShort && event.key == InputKeyBack) {
            break;
        }

        if(event.type == InputTypeShort && event.key == InputKeyOk && game_state->screen_white == true) {
            NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
            notification_message(notification, &sequence_success);
            furi_record_close(RECORD_NOTIFICATION);

            game_state->screen_white = false;
            game_state->counter = 0;
            game_state->random_wait_time = (rand() % 5) + 1;
        }
    }

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_mutex_free(game_state->mutex);
    furi_timer_free(timer);
    furi_record_close(RECORD_GUI);
    free(game_state);

    return 0;
}
