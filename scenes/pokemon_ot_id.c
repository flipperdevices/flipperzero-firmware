#include <ctype.h>
#include <furi.h>
#include <gui/modules/text_input.h>
#include <gui/view_dispatcher.h>
#include <stdlib.h>

#include "../pokemon_app.h"
#include "pokemon_menu.h"

static char ot_id_buf[6];

static bool select_ot_id_input_validator(const char* text, FuriString* error, void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    int ot_id;
    uint16_t ot_id_16;
    bool rc = true;
    unsigned int i;

    /* Need to check each byte to ensure is not alpha. atoi returns 0 which is
     * technically a valid ID, so we need to separately check for alpha chars.
     */
    for(i = 0; i < sizeof(ot_id_buf); i++) {
        if(!isdigit((unsigned int)text[i])) {
            if(text[i] == '\0') break;
            rc = false;
            break;
        }
    }

    ot_id = atoi(text);
    if(ot_id < 0 || ot_id > 65535 || rc == false) {
        furi_string_printf(error, "OT ID must\nbe between\n0-65535!");
        rc = false;
    } else {
        ot_id_16 = __builtin_bswap16((uint16_t)ot_id);
        pokemon_fap->trade_block->party[0].ot_id = ot_id_16;
    }

    FURI_LOG_D(TAG, "[ot_id] Set OT ID to %05d", (uint16_t)ot_id);

    return rc;
}

static void select_ot_id_input_callback(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    scene_manager_previous_scene(pokemon_fap->scene_manager);
}

void select_ot_id_scene_on_exit(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewMainMenu);
    view_dispatcher_remove_view(pokemon_fap->view_dispatcher, AppViewOpts);
}

void select_ot_id_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    text_input_reset(pokemon_fap->text_input);
    text_input_set_validator(pokemon_fap->text_input, select_ot_id_input_validator, pokemon_fap);
    text_input_set_result_callback(
        pokemon_fap->text_input,
        select_ot_id_input_callback,
        pokemon_fap,
        ot_id_buf,
        sizeof(ot_id_buf),
        true);
    text_input_set_header_text(pokemon_fap->text_input, "Enter OT ID (numbers only):");

    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher, AppViewOpts, text_input_get_view(pokemon_fap->text_input));
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewOpts);
}
