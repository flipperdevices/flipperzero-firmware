#include <furi.h>
#include "engine.h"
#include "game_engine.h"
#include "game_manager_i.h"
#include "level_i.h"
#include "entity_i.h"

static void frame_cb(GameEngine* engine, Canvas* canvas, InputState input, void* context) {
    UNUSED(engine);
    GameManager* game_manager = context;
    game_manager_input_set(game_manager, input);
    game_manager_update(game_manager);
    game_manager_render(game_manager, canvas);
}

int32_t game_app(void* p) {
    UNUSED(p);
    GameManager* game_manager = game_manager_alloc();

    GameEngineSettings settings = game_engine_settings_init();
    settings.target_fps = game.target_fps;
    settings.show_fps = game.show_fps;
    settings.always_backlight = game.always_backlight;
    settings.frame_callback = frame_cb;
    settings.context = game_manager;

    GameEngine* engine = game_engine_alloc(settings);
    game_manager_engine_set(game_manager, engine);

    void* game_context = NULL;
    if(game.context_size > 0) {
        game_context = malloc(game.context_size);
        game_manager_game_context_set(game_manager, game_context);
    }
    game.start(game_manager, game_context);

    game_engine_run(engine);
    game_engine_free(engine);

    game_manager_free(game_manager);

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