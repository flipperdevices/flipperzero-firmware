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

#include "pokemon_data.h"

#define TAG "Pokemon"

/* #defines for the data table entries */
#define GROWTH_FAST 4
#define GROWTH_MEDIUM_FAST 0
#define GROWTH_MEDIUM_SLOW 3
#define GROWTH_SLOW 5

struct pokemon_data_table {
    const char* name;
    const Icon* icon;
    const uint8_t index;
    const uint8_t base_hp;
    const uint8_t base_atk;
    const uint8_t base_def;
    const uint8_t base_spd;
    const uint8_t base_special;
    const uint8_t type[2];
    const uint8_t move[4];
    const uint8_t growth;
};

typedef struct pokemon_data_table PokemonTable;

struct named_list {
    const char* name;
    const uint8_t index;
};

typedef struct named_list NamedList;

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

    /* Table of pokemon data for Gen I */
    const PokemonTable* pokemon_table;

    /* List of moves, alphabetically ordered */
    const NamedList* move_list;

    /* List of types, alphabetically ordered */
    const NamedList* type_list;

    /* Struct for holding trade data */
    /* NOTE: There may be some runtime memory savings by adding more intelligence
     * to views/trade and slimming down this struct to only contain the single
     * pokemon data rather than the full 6 member party data.
     */
    TradeBlock* trade_block;

    /* The currently selected pokemon */
    int curr_pokemon;

    /* TODO: Other variables will end up here, like selected level, EV/IV,
     * moveset, etc. Likely will want to be another sub struct similar to
     * the actual pokemon data structure.
     */
    int curr_stats;
};

typedef struct pokemon_fap PokemonFap;

typedef enum {
    AppViewMainMenu,
    AppViewOpts, // Generic view ID meant for module re-use
    AppViewSelectPokemon,
    AppViewTrade,
    AppViewExitConfirm,
} AppView;

int pokemon_table_get_num_from_index(const PokemonTable* table, uint8_t index);

int pokemon_named_list_get_num_elements(const NamedList* list);

int pokemon_named_list_get_list_pos_from_index(const NamedList* list, uint8_t index);

const char* pokemon_named_list_get_name_from_index(const NamedList* list, uint8_t index);

void pokemon_trade_block_set_default_name(char* dest, PokemonFap* pokemon_fap, size_t n);

void pokemon_trade_block_recalculate(PokemonFap* pokemon_fap);

void pokemon_trade_block_recalculate_stats_from_level(PokemonFap* pokemon_fap);

#endif /* POKEMON_APP_H */
