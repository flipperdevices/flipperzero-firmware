#include <gui/modules/dialog_ex.h>

#include <pokemon_icons.h>
#include "pokemon_menu.h"
#include "../pokemon_app.h"
#include "../pokemon_data.h"

#include "../views/select_pokemon.h"
#include "../views/trade.h"

static bool pokemon_exit_confirm_back_event_callback(void* context) {
    UNUSED(context);

    return true;
}

void pokemon_exit_confirm_dialog_callback(DialogExResult result, void* context) {
    PokemonFap* pokemon_fap = context;

    scene_manager_handle_custom_event(pokemon_fap->scene_manager, result);
}

void pokemon_exit_confirm_on_enter(void* context) {
    PokemonFap* pokemon_fap = context;
    DialogEx* dialog_ex = pokemon_fap->dialog_ex;

    // Clean view
    dialog_ex_reset(pokemon_fap->dialog_ex);

    dialog_ex_set_left_button_text(dialog_ex, "Exit");
    dialog_ex_set_right_button_text(dialog_ex, "Stay");
    dialog_ex_set_header(dialog_ex, "Exit to Main Menu?", 64, 0, AlignCenter, AlignTop);
    dialog_ex_set_text(
        dialog_ex,
        "Current configuration and/or\ntrade status will be lost!",
        64,
        12,
        AlignCenter,
        AlignTop);
    dialog_ex_set_icon(dialog_ex, 44, 32, &I_surprised_pika);
    dialog_ex_set_context(dialog_ex, pokemon_fap);
    dialog_ex_set_result_callback(dialog_ex, pokemon_exit_confirm_dialog_callback);

    /* Disable the existing navigation event handler to prevent handling further
     * back events. Going back to the main menu as well as going back to the
     * gen menu will re-enable the proper navigation handler.
     */
    view_dispatcher_set_navigation_event_callback(
        pokemon_fap->view_dispatcher, pokemon_exit_confirm_back_event_callback);

    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher, AppViewOpts, dialog_ex_get_view(pokemon_fap->dialog_ex));
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewOpts);
}

bool pokemon_exit_confirm_on_event(void* context, SceneManagerEvent event) {
    PokemonFap* pokemon_fap = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DialogExResultRight) {
            consumed = scene_manager_previous_scene(pokemon_fap->scene_manager);
        } else if(event.event == DialogExResultLeft) {
            consumed = scene_manager_search_and_switch_to_previous_scene(
                pokemon_fap->scene_manager, MainMenuScene);
            /* NOTE: The above should never fail */
            furi_check(consumed);

            /* NOTE: The following might need to change when adding more 
	     * feature support.
	     */
            /* Clean up PokemonData */
            pokemon_data_free(pokemon_fap->pdata);

            // Free views
            /* These each remove themselves from the view_dispatcher */
            select_pokemon_free(
                pokemon_fap->view_dispatcher, AppViewSelectPokemon, pokemon_fap->select);
            trade_free(pokemon_fap->view_dispatcher, AppViewTrade, pokemon_fap->trade);

            pokemon_fap->pdata = NULL;
            pokemon_fap->select = NULL;
            pokemon_fap->trade = NULL;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}
