#pragma once
#include <furi.h>
#include "game_engine.h"
#include "level.h"
#include "level_manager.h"
#include "director.h"
#include "entity.h"

typedef struct {
    void (*start)(LevelManager* level_manager, void* context);
    void (*stop)(void* context);
    size_t context_size;
} Game;

extern const Game game;