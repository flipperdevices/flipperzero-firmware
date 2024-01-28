#include "fgeo.h"
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>

#include "clock_timer.h"

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

static void clock_timer_tick(void* context) {
    GameEngine* engine = context;
    furi_thread_flags_set(engine->thread_id, GameThreadFlagUpdate);
}

static void input_events_callback(const void* value, void* context) {
    GameEngine* engine = context;

    const InputEvent* event = value;
    if(event->key == InputKeyBack && event->type == InputTypePress) {
        furi_thread_flags_set(engine->thread_id, GameThreadFlagStop);
    }
}

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

void game_engine_run(GameEngine* engine) {
    engine->running = true;
    Canvas* canvas = gui_direct_draw_acquire(engine->gui);
    FuriPubSubSubscription* input_subscription =
        furi_pubsub_subscribe(engine->input_pubsub, input_events_callback, engine);

    clock_timer_init(clock_timer_tick, engine, engine->settings.fps);
    clock_timer_start();

    uint32_t time_start = DWT->CYCCNT;

    while(true) {
        uint32_t flags =
            furi_thread_flags_wait(GameThreadFlagMask, FuriFlagWaitAny, FuriWaitForever);
        furi_check((flags & FuriFlagError) == 0);

        if(flags & GameThreadFlagUpdate) {
            canvas_reset(canvas);

            uint32_t time_end = DWT->CYCCNT;
            uint32_t time_delta = time_end - time_start;
            time_start = time_end;

            if(engine->settings.show_fps) {
                float fps = 1.0f / (time_delta / (float)SystemCoreClock);
                canvas_set_color(canvas, ColorXOR);
                canvas_printf(canvas, 0, 7, "%u", (uint32_t)roundf(fps));
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
