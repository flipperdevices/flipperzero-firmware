#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <stdio.h>

#include "../pokemon_app.h"
#include "pokemon_menu.h"

static void select_move_selected_callback(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    uint32_t move = scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectMoveScene);

    if(index == UINT32_MAX) {
        pokemon_fap->trade_block->party[0].move[move] =
            pokemon_fap->pokemon_table[pokemon_fap->curr_pokemon].move[move];
    } else {
        pokemon_fap->trade_block->party[0].move[move] = (uint8_t)index;
    }

    /* Move back to move menu */
    scene_manager_search_and_switch_to_previous_scene(pokemon_fap->scene_manager, SelectMoveScene);
}

static void select_move_index_callback(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    /* Move to next scene */
    scene_manager_set_scene_state(pokemon_fap->scene_manager, SelectMoveIndexScene, index);
    scene_manager_next_scene(pokemon_fap->scene_manager, SelectMoveSetScene);
}

static void select_move_number_callback(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    /* Move to move index scene, save which move number we're selecting,
     * This doubles as the move slot we're going to write to later.
     */
    scene_manager_set_scene_state(pokemon_fap->scene_manager, SelectMoveScene, index);
    scene_manager_next_scene(pokemon_fap->scene_manager, SelectMoveIndexScene);
}

void select_move_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    uint8_t* pkmn_move = pokemon_fap->trade_block->party[0].move;
    char buf[64];

    submenu_reset(pokemon_fap->submenu);

    snprintf(
        buf,
        sizeof(buf),
        "Move 1:         %s",
        pokemon_named_list_get_name_from_index(pokemon_fap->move_list, pkmn_move[0]));
    submenu_add_item(pokemon_fap->submenu, buf, 0, select_move_number_callback, pokemon_fap);
    snprintf(
        buf,
        sizeof(buf),
        "Move 2:        %s",
        pokemon_named_list_get_name_from_index(pokemon_fap->move_list, pkmn_move[1]));
    submenu_add_item(pokemon_fap->submenu, buf, 1, select_move_number_callback, pokemon_fap);
    snprintf(
        buf,
        sizeof(buf),
        "Move 3:        %s",
        pokemon_named_list_get_name_from_index(pokemon_fap->move_list, pkmn_move[2]));
    submenu_add_item(pokemon_fap->submenu, buf, 2, select_move_number_callback, pokemon_fap);
    snprintf(
        buf,
        sizeof(buf),
        "Move 4:        %s",
        pokemon_named_list_get_name_from_index(pokemon_fap->move_list, pkmn_move[3]));
    submenu_add_item(pokemon_fap->submenu, buf, 3, select_move_number_callback, pokemon_fap);

    /* TODO: Add a "Default all moves" item? */

    submenu_set_selected_item(
        pokemon_fap->submenu,
        scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectMoveScene));

    /* Clear cursor position on MoveIndex */
    scene_manager_set_scene_state(pokemon_fap->scene_manager, SelectMoveIndexScene, 0);
}

void select_move_index_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    int i;
    char letter[2] = {'\0'};
    char buf[32];
    int curr_pokemon = pokemon_fap->curr_pokemon;
    uint32_t move_num = scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectMoveScene);
    uint8_t default_move = pokemon_fap->pokemon_table[curr_pokemon].move[move_num];
    const NamedList* move_list = pokemon_fap->move_list;

    submenu_reset(pokemon_fap->submenu);
    /* The move list should always start with No Move, put that at the start
     * for quick access.
     */
    submenu_add_item(
        pokemon_fap->submenu,
        move_list[0].name,
        move_list[0].index,
        select_move_selected_callback,
        pokemon_fap);

    /* Option to set move back to default */
    snprintf(
        buf,
        sizeof(buf),
        "Default [%s]",
        pokemon_named_list_get_name_from_index(pokemon_fap->move_list, default_move));
    submenu_add_item(
        pokemon_fap->submenu, buf, UINT32_MAX, select_move_selected_callback, pokemon_fap);

    /* Now, walk through the list and make a submenu item for each move's starting letter */
    for(i = 1;; i++) {
        if(move_list[i].name == NULL) break;
        if(toupper(move_list[i].name[0]) != toupper(letter[0])) {
            letter[0] = toupper(move_list[i].name[0]);
            submenu_add_item(
                pokemon_fap->submenu, letter, letter[0], select_move_index_callback, pokemon_fap);
        }
    }

    submenu_set_selected_item(
        pokemon_fap->submenu,
        scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectMoveIndexScene));
}

void select_move_set_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    int i;
    char letter =
        (char)scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectMoveIndexScene);

    /* Populate submenu with all moves that start with `letter` */
    /* NOTE! Start with index of 1 in the move list since 0 should always be no move! */
    submenu_reset(pokemon_fap->submenu);
    for(i = 1;; i++) {
        if(pokemon_fap->move_list[i].name == NULL) break;
        if(toupper(pokemon_fap->move_list[i].name[0]) == toupper(letter)) {
            submenu_add_item(
                pokemon_fap->submenu,
                pokemon_fap->move_list[i].name,
                pokemon_fap->move_list[i].index,
                select_move_selected_callback,
                pokemon_fap);
        }
    }
}
