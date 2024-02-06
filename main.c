#include <furi.h>
#include "engine.h"
#include "game_engine.h"
#include "director_i.h"
#include "level_manager_i.h"
#include "level_i.h"
#include "entity_i.h"

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

    void* game_context = NULL;
    if(game.context_size > 0) {
        game_context = malloc(game.context_size);
        director_game_context_set(director, game_context);
    }
    game.start(level_manager, NULL);

    GameEngineSettings settings = game_engine_settings_init();
    settings.fps = 60.0f;
    settings.show_fps = true;
    settings.always_backlight = true;
    settings.frame_callback = frame_cb;
    settings.context = director;

    GameEngine* engine = game_engine_alloc(settings);
    game_engine_run(engine);
    game_engine_free(engine);

    level_manager_free(level_manager);
    director_free(director);

    game.stop(game_context);
    if(game_context) {
        free(game_context);
    }

    int32_t entities = entities_get_count();
    if(entities != 0) {
        FURI_LOG_E("Game", "Memory leak detected: %ld entities still allocated", entities);
        return -1;
    }

    return 0;
}