#ifndef POKEMON_MENU_H
#define POKEMON_MENU_H

#pragma once

#include <gui/scene_manager.h>

typedef enum {
    MainMenuScene,
    SelectPokemonScene,
    SelectNicknameScene,
    SelectLevelScene,
    SelectMoveScene,
    SelectMoveIndexScene,
    SelectMoveSetScene,
    SelectTypeScene,
    SelectStatsScene,
    SelectOTIDScene,
    SelectOTNameScene,
    TradeScene,
    SceneCount,
} AppScene;

extern const SceneManagerHandlers pokemon_scene_manager_handlers;

#endif // POKEMON_MENU_H
