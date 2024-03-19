#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <stdio.h>

#include <named_list.h>

#include "../pokemon_app.h"
#include "../pokemon_data.h"
#include "pokemon_menu.h"

static void select_move_selected_callback(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    uint32_t move = scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectMoveScene);
    uint8_t num = pokemon_stat_get(pokemon_fap->pdata, STAT_NUM, NONE);

    if(index == UINT32_MAX) {
        pokemon_stat_set(
            pokemon_fap->pdata,
            STAT_MOVE,
            move,
            table_stat_base_get(pokemon_fap->pdata->pokemon_table, num, STAT_MOVE, move));
    } else {
        pokemon_stat_set(pokemon_fap->pdata, STAT_MOVE, move, index);
    }
    FURI_LOG_D(
        TAG,
        "[move] Set move %s to %d",
        namedlist_name_get_index(
            pokemon_fap->pdata->move_list, pokemon_stat_get(pokemon_fap->pdata, STAT_MOVE, move)),
        (int)move);

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
    furi_assert(context);
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    char buf[64];
    int i;

    submenu_reset(pokemon_fap->submenu);

    for(i = 0; i < 4; i++) {
        snprintf(
            buf,
            sizeof(buf),
            "Move %d:         %s",
            i + 1,
            namedlist_name_get_index(
                pokemon_fap->pdata->move_list,
                pokemon_stat_get(pokemon_fap->pdata, STAT_MOVE, i)));
        submenu_add_item(pokemon_fap->submenu, buf, i, select_move_number_callback, pokemon_fap);
    }

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
    const char* name;
    uint32_t move_num = scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectMoveScene);

    submenu_reset(pokemon_fap->submenu);
    /* The move list should always start with No Move, put that at the start
     * for quick access.
     */
    submenu_add_item(
        pokemon_fap->submenu,
        namedlist_name_get_index(pokemon_fap->pdata->move_list, 0),
        0,
        select_move_selected_callback,
        pokemon_fap);

    /* Option to set move back to default */
    snprintf(
        buf,
        sizeof(buf),
        "Default [%s]",
        namedlist_name_get_index(
            pokemon_fap->pdata->move_list,
            table_stat_base_get(
                pokemon_fap->pdata->pokemon_table,
                pokemon_stat_get(pokemon_fap->pdata, STAT_NUM, NONE),
                STAT_MOVE,
                move_num)));
    submenu_add_item(
        pokemon_fap->submenu, buf, UINT32_MAX, select_move_selected_callback, pokemon_fap);

    /* Now, walk through the list and make a submenu item for each move's starting letter */
    for(i = 1;; i++) {
        name = namedlist_name_get_pos(pokemon_fap->pdata->move_list, i);
        if(name == NULL) break;
        /* TODO: Add check here for generation match. Currently, this will populate
	 * the letters that have any move associated with them, even if not for the
	 * generation currently being used.
	 */
        if(name[0] != letter[0]) {
            letter[0] = name[0];
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
    const char* name;
    char letter =
        (char)scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectMoveIndexScene);

    /* Populate submenu with all moves that start with `letter` */
    /* NOTE! Start with index of 1 in the move list since 0 should always be no move! */
    submenu_reset(pokemon_fap->submenu);
    for(i = 1;; i++) {
        name = namedlist_name_get_pos(pokemon_fap->pdata->move_list, i);
        if(name == NULL) break;
        if(name[0] == letter &&
           (pokemon_fap->pdata->gen & namedlist_gen_get_pos(pokemon_fap->pdata->move_list, i))) {
            submenu_add_item(
                pokemon_fap->submenu,
                name,
                namedlist_index_get(pokemon_fap->pdata->move_list, i),
                select_move_selected_callback,
                pokemon_fap);
        };
    }
}
