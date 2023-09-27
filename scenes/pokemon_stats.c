#include <gui/modules/submenu.h>

#include "../pokemon_app.h"
#include "pokemon_menu.h"

const char* stats_text[6] = {
    "Random IV, Zero EV",
    "Random IV, Max EV / Level",
    "Random IV, Max EV",
    "Max IV, Zero EV",
    "Max IV, Max EV / Level",
    "Max IV, Max EV",
};

static void select_stats_selected_callback(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    pokemon_fap->curr_stats = index;

    pokemon_trade_block_recalculate_stats_from_level(pokemon_fap);

    scene_manager_previous_scene(pokemon_fap->scene_manager);
}

void select_stats_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    submenu_reset(pokemon_fap->submenu);
    submenu_add_item(
        pokemon_fap->submenu, stats_text[0], 0, select_stats_selected_callback, pokemon_fap);
    submenu_add_item(
        pokemon_fap->submenu, stats_text[1], 1, select_stats_selected_callback, pokemon_fap);
    submenu_add_item(
        pokemon_fap->submenu, stats_text[2], 2, select_stats_selected_callback, pokemon_fap);
    submenu_add_item(
        pokemon_fap->submenu, stats_text[3], 3, select_stats_selected_callback, pokemon_fap);
    submenu_add_item(
        pokemon_fap->submenu, stats_text[4], 4, select_stats_selected_callback, pokemon_fap);
    submenu_add_item(
        pokemon_fap->submenu, stats_text[5], 5, select_stats_selected_callback, pokemon_fap);
}
