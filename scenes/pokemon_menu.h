#ifndef POKEMON_MENU_H
#define POKEMON_MENU_H

#pragma once

#include <gui/scene_manager.h>

typedef enum {
    MainMenuScene,
    GenITradeScene, // Formerly main menu scene
    GenIITradeScene,
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
    SelectPinsScene,
    SceneCount,
} AppScene;

extern const SceneManagerHandlers pokemon_scene_manager_handlers;

#endif // POKEMON_MENU_H
