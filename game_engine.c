#include "game_engine.h"
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include "clock_timer.h"

typedef _Atomic uint32_t AtomicUint32;

GameEngineSettings game_engine_settings_init() {
    GameEngineSettings settings;
    settings.fps = 60.0f;
    settings.show_fps = false;
    return settings;
}

struct GameEngine {
    Gui* gui;
    FuriPubSub* input_pubsub;
    FuriThreadId thread_id;
    GameEngineSettings settings;
    volatile bool running;
};

typedef enum {
    GameThreadFlagUpdate = 1 << 0,
    GameThreadFlagStop = 1 << 1,
} GameThreadFlag;

#define GameThreadFlagMask (GameThreadFlagUpdate | GameThreadFlagStop)

GameEngine* game_engine_alloc(GameEngineSettings settings) {
    GameEngine* engine = malloc(sizeof(GameEngine));
    engine->gui = furi_record_open(RECORD_GUI);
    engine->input_pubsub = furi_record_open(RECORD_INPUT_EVENTS);
    engine->thread_id = furi_thread_get_current_id();
    engine->settings = settings;
    engine->running = false;

    return engine;
}

void game_engine_free(GameEngine* engine) {
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_INPUT_EVENTS);
    free(engine);
}

static void canvas_printf(Canvas* canvas, uint8_t x, uint8_t y, const char* format, ...) {
    FuriString* string = furi_string_alloc();
    va_list args;
    va_start(args, format);
    furi_string_vprintf(string, format, args);
    va_end(args);

    canvas_draw_str(canvas, x, y, furi_string_get_cstr(string));

    furi_string_free(string);
}

static void clock_timer_callback(void* context) {
    GameEngine* engine = context;
    furi_thread_flags_set(engine->thread_id, GameThreadFlagUpdate);
}

static void input_events_callback(const void* value, void* context) {
    AtomicUint32* input_state = context;

    const InputEvent* event = value;
    if(event->type == InputTypePress) {
        *input_state |= (1 << event->key);
    } else if(event->type == InputTypeRelease) {
        *input_state &= ~(1 << event->key);
    }
}

typedef struct {
    uint32_t current;
    uint32_t pressed;
    uint32_t released;
} InputState;

void game_engine_run(GameEngine* engine) {
    engine->running = true;

    // input state
    AtomicUint32 input_state = 0;
    uint32_t input_prev_state = 0;

    // acquire gui canvas
    Canvas* canvas = gui_direct_draw_acquire(engine->gui);

    // subscribe to input events
    FuriPubSubSubscription* input_subscription =
        furi_pubsub_subscribe(engine->input_pubsub, input_events_callback, &input_state);

    // start "game update" timer
    clock_timer_init(clock_timer_callback, engine, engine->settings.fps);
    clock_timer_start();

    // init fps counter
    uint32_t time_start = DWT->CYCCNT;

    while(true) {
        uint32_t flags =
            furi_thread_flags_wait(GameThreadFlagMask, FuriFlagWaitAny, FuriWaitForever);
        furi_check((flags & FuriFlagError) == 0);

        if(flags & GameThreadFlagUpdate) {
            uint32_t input_current_state = input_state;
            InputState input = {
                .current = input_current_state,
                .pressed = input_current_state & ~input_prev_state,
                .released = ~input_current_state & input_prev_state,
            };
            input_prev_state = input_current_state;

            canvas_reset(canvas);

            uint32_t time_end = DWT->CYCCNT;
            uint32_t time_delta = time_end - time_start;
            time_start = time_end;

            if(engine->settings.show_fps) {
                float fps = 1.0f / (time_delta / (float)SystemCoreClock);
                canvas_set_color(canvas, ColorXOR);
                canvas_printf(canvas, 0, 7, "%u", (uint32_t)roundf(fps));
            }

            if(input.pressed != 0) {
                FURI_LOG_I("input", "pressed: %lu", input.pressed);
            }

            if(input.released != 0) {
                FURI_LOG_I("input", "released: %lu", input.released);
            }

            canvas_commit(canvas);
        } else if(flags & GameThreadFlagStop) {
            break;
        }
    }

    clock_timer_stop();
    clock_timer_deinit();

    gui_direct_draw_release(engine->gui);
    furi_pubsub_unsubscribe(engine->input_pubsub, input_subscription);
    engine->running = false;
}

void game_engine_stop(GameEngine* engine) {
    furi_thread_flags_set(engine->thread_id, GameThreadFlagStop);
}