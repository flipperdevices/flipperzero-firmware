#pragma once
#include <furi.h>
#include "game_engine.h"
#include "level.h"
#include "entity.h"
#include "game_manager.h"

typedef struct {
    void (*start)(GameManager* game_manager, void* context);
    void (*stop)(void* context);
    size_t context_size;
} Game;

extern const Game game;