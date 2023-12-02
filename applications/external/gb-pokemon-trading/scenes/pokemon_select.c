#include "../pokemon_app.h"

void select_pokemon_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    // switch to select pokemon scene
    // Note for the future, this might make sense to setup and teardown each view
    // at runtime rather than at the start of the whole application
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewSelectPokemon);
}

void select_pokemon_scene_on_exit(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    /* If a new pokemon was selected, then recalculate all of the trade_block
     * values for the first pokemon in the party.
     */
    /* XXX: Find a way to see if exit was caused by an OK or a Back input? */
    if(pokemon_fap->pokemon_table[pokemon_fap->curr_pokemon].index !=
       pokemon_fap->trade_block->party[0].index) {
        pokemon_trade_block_recalculate(pokemon_fap);
    }
}
