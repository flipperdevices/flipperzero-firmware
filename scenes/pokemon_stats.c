#include <gui/modules/submenu.h>

#include <named_list.h>

#include "../pokemon_app.h"
#include "../pokemon_data.h"
#include "pokemon_menu.h"

static void select_stats_selected_callback(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    pokemon_stat_set(pokemon_fap->pdata, STAT_SEL, NONE, index);

    scene_manager_previous_scene(pokemon_fap->scene_manager);
}

void select_stats_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    int i;

    submenu_reset(pokemon_fap->submenu);
    /* TODO: This is a magic number that this scene shouldn't need to know about */
    for(i = 0; i < 6; i++) {
        submenu_add_item(
            pokemon_fap->submenu,
            namedlist_name_get_index(pokemon_fap->pdata->stat_list, i),
            i,
            select_stats_selected_callback,
            pokemon_fap);
    }
}
