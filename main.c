#include <furi.h>
#include "game_engine.h"
#include "director_i.h"
#include "level_manager_i.h"
#include "level_i.h"
#include "entity_i.h"

void game_setup(Level* level);
void game_destroy(void);

static void frame_cb(RunningGameEngine* engine, Canvas* canvas, InputState input, void* context) {
    Director* director = context;
    director_input_set(director, input);
    director_engine_set(director, engine);

    LevelManager* level_manager = director_level_manager_get(director);
    level_manager_update(level_manager, director);
    level_manager_render(level_manager, director, canvas);
}

int32_t game_app(void* p) {
    UNUSED(p);

    Director* director = director_alloc();
    LevelManager* level_manager = level_manager_alloc();
    director_level_manager_set(director, level_manager);
    Level* level = level_manager_add_level(level_manager);

    game_setup(level);

    GameEngineSettings settings = game_engine_settings_init();
    settings.fps = 60.0f;
    settings.show_fps = true;
    settings.always_backlight = true;
    settings.frame_callback = frame_cb;
    settings.context = director;

    GameEngine* engine = game_engine_alloc(settings);
    game_engine_run(engine);
    game_engine_free(engine);

    game_destroy();

    level_manager_free(level_manager);
    director_free(director);

    size_t entities = entities_get_count();
    if(entities != 0) {
        FURI_LOG_E("Game", "Memory leak detected: %d entities still allocated", entities);
        return -1;
    }

    return 0;
}