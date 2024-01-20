#ifndef POKEMON_APP_H
#define POKEMON_APP_H

#pragma once

#include <gui/scene_manager.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/icon.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>
#include <gblink.h>

#include "pokemon_data.h"

#define TAG "Pokemon"

struct pokemon_fap {
    ViewDispatcher* view_dispatcher;

    /* View ports for each of the application's steps */
    View* select_view;
    void* trade;

    /* Scene manager */
    SceneManager* scene_manager;

    /* gui modules used in the application lifetime */
    Submenu* submenu;
    TextInput* text_input;
    VariableItemList* variable_item_list;

    /* Struct for holding trade data */
    /* NOTE: There may be some runtime memory savings by adding more intelligence
     * to views/trade and slimming down this struct to only contain the single
     * pokemon data rather than the full 6 member party data.
     */
    TradeBlock* trade_block;

    /* Pin definition to actual Game Link Cable interface */
    struct gblink_pins pins;
    int malveke_detected;
};

typedef struct pokemon_fap PokemonFap;

typedef enum {
    AppViewMainMenu,
    AppViewOpts, // Generic view ID meant for module re-use
    AppViewSelectPokemon,
    AppViewTrade,
    AppViewExitConfirm,
} AppView;

int pokemon_named_list_num_elements_get(const NamedList* list);

void pokemon_trade_block_set_default_name(char* dest, PokemonFap* pokemon_fap, size_t n);

#endif /* POKEMON_APP_H */
