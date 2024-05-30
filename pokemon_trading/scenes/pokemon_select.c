#include "../pokemon_app.h"

void select_pokemon_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    // switch to select pokemon scene
    // Note for the future, this might make sense to setup and teardown each view
    // at runtime rather than at the start of the whole application
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewSelectPokemon);
}
