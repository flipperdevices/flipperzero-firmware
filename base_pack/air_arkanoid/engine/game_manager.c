#include "game_manager.h"
#include "level_i.h"
#include <furi.h>
#include <m-list.h>
#include <storage/storage.h>

typedef struct {
    Sprite* sprite;
    FuriString* path;
} SpriteCache;

LIST_DEF(LevelList, Level*, M_POD_OPLIST);
LIST_DEF(SpriteCacheList, SpriteCache, M_POD_OPLIST);

struct GameManager {
    LevelList_t levels;
    Level* current_level;
    Level* next_level;

    GameEngine* engine;
    InputState input;
    void* game_context;

    SpriteCacheList_t sprites;
};

GameManager* game_manager_alloc() {
    GameManager* manager = malloc(sizeof(GameManager));
    LevelList_init(manager->levels);
    manager->current_level = NULL;
    manager->next_level = NULL;
    manager->engine = NULL;
    manager->game_context = NULL;
    memset(&manager->input, 0, sizeof(InputState));
    SpriteCacheList_init(manager->sprites);
    return manager;
}

void game_manager_free(GameManager* manager) {
    level_call_stop(manager->current_level);

    // Free all levels
    {
        LevelList_it_t it;
        LevelList_it(it, manager->levels);
        while(!LevelList_end_p(it)) {
            level_call_free(*LevelList_cref(it));
            level_free(*LevelList_cref(it));
            LevelList_next(it);
        }
    }

    LevelList_clear(manager->levels);

    // Free all sprites
    {
        SpriteCacheList_it_t it;
        SpriteCacheList_it(it, manager->sprites);
        while(!SpriteCacheList_end_p(it)) {
            furi_string_free(SpriteCacheList_cref(it)->path);
            sprite_free(SpriteCacheList_cref(it)->sprite);
            SpriteCacheList_next(it);
        }
    }
    SpriteCacheList_clear(manager->sprites);
    free(manager);
}

Level* game_manager_add_level(GameManager* manager, const LevelBehaviour* behaviour) {
    UNUSED(manager);
    Level* level = level_alloc(behaviour, manager);
    LevelList_push_back(manager->levels, level);
    level_call_alloc(level);
    if(!manager->current_level) {
        manager->current_level = level;
        level_call_start(level);
    }
    return level;
}

void game_manager_next_level_set(GameManager* manager, Level* next_level) {
    manager->next_level = next_level;
}

void game_manager_game_stop(GameManager* manager) {
    GameEngine* engine = game_manager_engine_get(manager);
    game_engine_stop(engine);
}

Level* game_manager_current_level_get(GameManager* manager) {
    return manager->current_level;
}

void game_manager_update(GameManager* manager) {
    if(manager->next_level) {
        level_call_stop(manager->current_level);
        manager->current_level = manager->next_level;
        level_call_start(manager->current_level);
        manager->next_level = NULL;
    }

    level_update(manager->current_level, manager);
}

void game_manager_render(GameManager* manager, Canvas* canvas) {
    level_render(manager->current_level, manager, canvas);
}

void game_manager_engine_set(GameManager* manager, GameEngine* engine) {
    manager->engine = engine;
}

void game_manager_input_set(GameManager* manager, InputState input) {
    manager->input = input;
}

void game_manager_game_context_set(GameManager* manager, void* context) {
    manager->game_context = context;
}

GameEngine* game_manager_engine_get(GameManager* manager) {
    return manager->engine;
}

InputState game_manager_input_get(GameManager* manager) {
    return manager->input;
}

void* game_manager_game_context_get(GameManager* manager) {
    return manager->game_context;
}

void game_manager_show_fps_set(GameManager* manager, bool show_fps) {
    GameEngine* engine = game_manager_engine_get(manager);
    game_engine_show_fps_set(engine, show_fps);
}

Sprite* game_manager_sprite_load(GameManager* manager, const char* path) {
    SpriteCacheList_it_t it;
    SpriteCacheList_it(it, manager->sprites);
    while(!SpriteCacheList_end_p(it)) {
        if(furi_string_cmp(SpriteCacheList_cref(it)->path, path) == 0) {
            return SpriteCacheList_cref(it)->sprite;
        }
        SpriteCacheList_next(it);
    }

    FuriString* path_full = furi_string_alloc_set(APP_ASSETS_PATH("sprites/"));
    furi_string_cat(path_full, path);

    Sprite* sprite = sprite_alloc(furi_string_get_cstr(path_full));
    if(sprite) {
        SpriteCache cache = {
            .sprite = sprite,
            .path = furi_string_alloc_set(path),
        };
        SpriteCacheList_push_back(manager->sprites, cache);
    }
    furi_string_free(path_full);

    return sprite;
}