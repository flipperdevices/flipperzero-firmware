#include "../pokemon_app.h"

void trade_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    // switch to select pokemon scene
    // Note for the future, this might make sense to setup and teardown each view
    // at runtime rather than at the start?
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewTrade);
}
