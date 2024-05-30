#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <stdio.h>

#include <named_list.h>

#include "../pokemon_app.h"
#include "../pokemon_data.h"
#include "pokemon_menu.h"

static void select_item_selected_callback(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    uint32_t item = scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectItemSetScene);

    pokemon_stat_set(pokemon_fap->pdata, STAT_HELD_ITEM, item, index);

    FURI_LOG_D(
        TAG,
        "[item] Set item %s",
        namedlist_name_get_index(
            pokemon_fap->pdata->item_list,
            pokemon_stat_get(pokemon_fap->pdata, STAT_HELD_ITEM, item)));

    /* Move back to Gen menu. This assumes this submenu is only ever used in Gen II */
    scene_manager_search_and_switch_to_previous_scene(pokemon_fap->scene_manager, GenIITradeScene);
}

static void select_item_index_callback(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    /* Move to next scene */
    scene_manager_set_scene_state(pokemon_fap->scene_manager, SelectItemSetScene, index);
    scene_manager_next_scene(pokemon_fap->scene_manager, SelectItemSetScene);
}

void select_item_scene_on_enter(void* context) {
    furi_assert(context);
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    int i;
    const char* name;
    char letter[2] = {'\0'};

    submenu_reset(pokemon_fap->submenu);

    /* The item list should always start with No Item, put that at the start
     * for quick access.
     */
    submenu_add_item(
        pokemon_fap->submenu,
        namedlist_name_get_index(pokemon_fap->pdata->item_list, 0),
        0,
        select_item_selected_callback,
        pokemon_fap);

    for(i = 1;; i++) {
        name = namedlist_name_get_pos(pokemon_fap->pdata->item_list, i);
        if(name == NULL) break;
        if(name[0] != letter[0]) {
            letter[0] = name[0];
            submenu_add_item(
                pokemon_fap->submenu, letter, letter[0], select_item_index_callback, pokemon_fap);
        }
    }

    submenu_set_selected_item(
        pokemon_fap->submenu,
        scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectItemSetScene));
    scene_manager_set_scene_state(pokemon_fap->scene_manager, SelectItemSetScene, 0);
}

void select_item_set_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    int i;
    const char* name;
    char letter =
        (char)scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectItemSetScene);

    /* Populate submenu with all items that start with `letter` */
    /* NOTE! Start with pos of 1 in the item list since 0 should always be no item! */
    submenu_reset(pokemon_fap->submenu);
    for(i = 1;; i++) {
        name = namedlist_name_get_pos(pokemon_fap->pdata->item_list, i);
        if(name == NULL) break;
        if(name[0] == letter &&
           (pokemon_fap->pdata->gen & namedlist_gen_get_pos(pokemon_fap->pdata->item_list, i))) {
            submenu_add_item(
                pokemon_fap->submenu,
                name,
                namedlist_index_get(pokemon_fap->pdata->item_list, i),
                select_item_selected_callback,
                pokemon_fap);
        }
    }
}
