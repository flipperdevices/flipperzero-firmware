#include <furi.h>
#include <gui/modules/text_input.h>
#include <gui/view_dispatcher.h>
#include <stdlib.h>

#include "../pokemon_app.h"
#include "pokemon_menu.h"

static char level_buf[4];

static bool select_level_input_validator(const char* text, FuriString* error, void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    int level_val;
    bool rc = true;

    level_val = atoi(text);
    if(level_val < 2 || level_val > 100) {
        furi_string_printf(error, "Level must\nbe a number\nbetween\n2-100!\n");
        rc = false;
    } else {
        pokemon_fap->trade_block->party[0].level = level_val;
        /* XXX: Need to recalculate other stats with level updated! */
    }

    return rc;
}

static void select_level_input_callback(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    pokemon_trade_block_recalculate_stats_from_level(pokemon_fap);
    scene_manager_previous_scene(pokemon_fap->scene_manager);
}

void select_level_scene_on_exit(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewMainMenu);
    view_dispatcher_remove_view(pokemon_fap->view_dispatcher, AppViewOpts);
}

void select_level_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    text_input_reset(pokemon_fap->text_input);
    text_input_set_validator(pokemon_fap->text_input, select_level_input_validator, pokemon_fap);
    text_input_set_result_callback(
        pokemon_fap->text_input,
        select_level_input_callback,
        pokemon_fap,
        level_buf,
        sizeof(level_buf),
        true);
    text_input_set_header_text(pokemon_fap->text_input, "Enter level (numbers only):");

    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher, AppViewOpts, text_input_get_view(pokemon_fap->text_input));
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewOpts);
}
