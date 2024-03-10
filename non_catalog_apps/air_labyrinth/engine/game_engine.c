#include "game_engine.h"
#include <furi.h>
#include <furi_hal_rtc.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include "clock_timer.h"

typedef _Atomic uint32_t AtomicUint32;

typedef struct {
    bool lefty;
    AtomicUint32 state;
} InputHolder;

GameEngineSettings game_engine_settings_init() {
    GameEngineSettings settings;
    settings.target_fps = 30.0f;
    settings.show_fps = false;
    settings.always_backlight = true;
    settings.start_callback = NULL;
    settings.frame_callback = NULL;
    settings.stop_callback = NULL;
    settings.context = NULL;
    return settings;
}

struct GameEngine {
    Gui* gui;
    NotificationApp* notifications;
    FuriPubSub* input_pubsub;
    FuriThreadId thread_id;
    GameEngineSettings settings;
    float fps;
};

typedef enum {
    GameThreadFlagUpdate = 1 << 0,
    GameThreadFlagStop = 1 << 1,
} GameThreadFlag;

#define GameThreadFlagMask (GameThreadFlagUpdate | GameThreadFlagStop)

GameEngine* game_engine_alloc(GameEngineSettings settings) {
    furi_check(settings.frame_callback != NULL);

    GameEngine* engine = malloc(sizeof(GameEngine));
    engine->gui = furi_record_open(RECORD_GUI);
    engine->notifications = furi_record_open(RECORD_NOTIFICATION);
    engine->input_pubsub = furi_record_open(RECORD_INPUT_EVENTS);
    engine->thread_id = furi_thread_get_current_id();
    engine->settings = settings;
    engine->fps = 1.0f;

    return engine;
}

void game_engine_free(GameEngine* engine) {
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_INPUT_EVENTS);
    free(engine);
}

static void clock_timer_callback(void* context) {
    GameEngine* engine = context;
    furi_thread_flags_set(engine->thread_id, GameThreadFlagUpdate);
}

static const GameKey keys_right_hand[] = {
    [InputKeyUp] = GameKeyUp,
    [InputKeyDown] = GameKeyDown,
    [InputKeyRight] = GameKeyRight,
    [InputKeyLeft] = GameKeyLeft,
    [InputKeyOk] = GameKeyOk,
    [InputKeyBack] = GameKeyBack,
};

static const GameKey keys_left_hand[] = {
    [InputKeyUp] = GameKeyDown,
    [InputKeyDown] = GameKeyUp,
    [InputKeyRight] = GameKeyLeft,
    [InputKeyLeft] = GameKeyRight,
    [InputKeyOk] = GameKeyOk,
    [InputKeyBack] = GameKeyBack,
};
static_assert(
    sizeof(keys_right_hand) == sizeof(keys_left_hand),
    "keys_right_hand and keys_left_hand do not match!");

static const size_t keys_count = sizeof(keys_right_hand) / sizeof(keys_right_hand[0]);

static void input_events_callback(const void* value, void* context) {
    InputHolder* holder = context;
    const InputEvent* event = value;
    const GameKey* keys = holder->lefty ? keys_left_hand : keys_right_hand;

    if(event->key < keys_count) {
        switch(event->type) {
        case InputTypePress:
            holder->state |= (keys[event->key]);
            break;
        case InputTypeRelease:
            holder->state &= ~(keys[event->key]);
            break;
        default:
            break;
        }
    }
}

void game_engine_run(GameEngine* engine) {
    // input state
    InputHolder input_state = {
        .lefty = furi_hal_rtc_is_flag_set(FuriHalRtcFlagHandOrient),
        .state = 0,
    };
    uint32_t input_prev_state = 0;

    // set backlight if needed
    if(engine->settings.always_backlight) {
        notification_message(engine->notifications, &sequence_display_backlight_enforce_on);
    }

    // acquire gui canvas
    Canvas* canvas = gui_direct_draw_acquire(engine->gui);

    // subscribe to input events
    FuriPubSubSubscription* input_subscription =
        furi_pubsub_subscribe(engine->input_pubsub, input_events_callback, &input_state);

    // call start callback, if any
    if(engine->settings.start_callback) {
        engine->settings.start_callback(engine, engine->settings.context);
    }

    // start "game update" timer
    clock_timer_start(clock_timer_callback, engine, engine->settings.target_fps);

    // init fps counter
    uint32_t time_start = DWT->CYCCNT;

    while(true) {
        uint32_t flags =
            furi_thread_flags_wait(GameThreadFlagMask, FuriFlagWaitAny, FuriWaitForever);
        furi_check((flags & FuriFlagError) == 0);

        if(flags & GameThreadFlagUpdate) {
            // update fps counter
            uint32_t time_end = DWT->CYCCNT;
            uint32_t time_delta = time_end - time_start;
            time_start = time_end;

            // update input state
            uint32_t input_current_state = input_state.state;
            InputState input = {
                .held = input_current_state,
                .pressed = input_current_state & ~input_prev_state,
                .released = ~input_current_state & input_prev_state,
            };
            input_prev_state = input_current_state;

            // clear screen
            canvas_reset(canvas);

            // calculate actual fps
            engine->fps = (float)SystemCoreClock / time_delta;

            // do the work
            engine->settings.frame_callback(engine, canvas, input, engine->settings.context);

            // show fps if needed
            if(engine->settings.show_fps) {
                canvas_set_color(canvas, ColorXOR);
                canvas_printf(canvas, 0, 7, "%lu", (uint32_t)roundf(engine->fps));
            }

            // and output screen buffer
            canvas_commit(canvas);

            // throttle a bit
            furi_delay_tick(2);
        }

        if(flags & GameThreadFlagStop) {
            break;
        }
    }

    // stop timer
    clock_timer_stop();

    // call stop callback, if any
    if(engine->settings.stop_callback) {
        engine->settings.stop_callback(engine, engine->settings.context);
    }

    // release gui canvas and unsubscribe from input events
    gui_direct_draw_release(engine->gui);
    furi_pubsub_unsubscribe(engine->input_pubsub, input_subscription);

    if(engine->settings.always_backlight) {
        notification_message(engine->notifications, &sequence_display_backlight_enforce_auto);
    }
}

void game_engine_stop(GameEngine* engine) {
    furi_thread_flags_set(engine->thread_id, GameThreadFlagStop);
}

float game_engine_get_delta_time(GameEngine* engine) {
    return 1.0f / engine->fps;
}

float game_engine_get_delta_frames(GameEngine* engine) {
    return engine->fps / engine->settings.target_fps;
}

void game_engine_show_fps_set(GameEngine* engine, bool show_fps) {
    engine->settings.show_fps = show_fps;
}