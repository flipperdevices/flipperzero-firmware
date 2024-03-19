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
    SelectItemScene,
    SelectItemSetScene,
    SelectTypeScene,
    SelectStatsScene,
    SelectShinyScene,
    SelectGenderScene,
    SelectPokerusScene,
    SelectUnownFormScene,
    SelectOTIDScene,
    SelectOTNameScene,
    TradeScene,
    SelectPinsScene,
    ConfirmExitScene,
    SceneCount,
} AppScene;

extern const SceneManagerHandlers pokemon_scene_manager_handlers;

bool main_menu_back_event_callback(void* context);

#endif // POKEMON_MENU_H
