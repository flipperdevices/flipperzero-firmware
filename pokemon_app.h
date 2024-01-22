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

#define TAG "Pokemon"

struct pokemon_fap {
    /* Various anonymous pointers for flipper UI/navigation */
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    void* select;
    void* trade;
    Submenu* submenu;
    TextInput* text_input;
    VariableItemList* variable_item_list;

    /* Runtime information that may be needed by any view or scene */
    int generation;

    /* Struct for holding trade data */
    /* NOTE: There may be some runtime memory savings by adding more intelligence
     * to views/trade and slimming down this struct to only contain the single
     * pokemon data rather than the full 6 member party data.
     */
    /* This could be one of a number of different formats which is determined
     * based on the generation variable.
     */
    void* trade_block;
    void *party; // Pointer to the whole data struct for the first pokemon. This
		 // shortcuts some calculations.
    /* These are anonymous in case at some point in the future these change
     * per generation.
     */
    void *move_list;
    void *type_list;
    void *stats_list;
    int current_stat;

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
