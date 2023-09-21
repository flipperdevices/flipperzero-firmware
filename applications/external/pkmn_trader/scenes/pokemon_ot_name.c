#include <ctype.h>
#include <furi.h>
#include <gui/modules/text_input.h>
#include <gui/view_dispatcher.h>
#include <stdlib.h>

#include "../pokemon_app.h"
#include "../pokemon_char_encode.h"
#include "pokemon_menu.h"

static char ot_name_buf[8];

static bool select_ot_name_input_validator(const char* text, FuriString* error, void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    bool rc = true;
    unsigned int i;

    // XXX If no pokemon name, use default. How TF to have text input handle that?
    // OT name is 7 chars max on gen 1, so only take that and then fill the rest of the 11 bytes with term

    for(i = 0; i < sizeof(ot_name_buf); i++) {
        if(!isalnum((unsigned int)text[i])) {
            if(text[i] == '\0') break;
            rc = false;
            break;
        }
    }

    if(rc == false) {
        furi_string_printf(error, "Some error?");
    } else {
        /* Clear existing OT Name in trade block*/
        memset(pokemon_fap->trade_block->ot_name, TERM_, sizeof(struct name));

        /* Encode string to OT Name */
        pokemon_str_to_encoded_array(
            (uint8_t*)pokemon_fap->trade_block->ot_name, (char*)text, strlen(text));
    }

    return rc;
}

static void select_ot_name_input_callback(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    scene_manager_previous_scene(pokemon_fap->scene_manager);
}

void select_ot_name_scene_on_exit(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewMainMenu);
    view_dispatcher_remove_view(pokemon_fap->view_dispatcher, AppViewOpts);
}

void select_ot_name_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    text_input_reset(pokemon_fap->text_input);
    text_input_set_validator(pokemon_fap->text_input, select_ot_name_input_validator, pokemon_fap);
    text_input_set_result_callback(
        pokemon_fap->text_input,
        select_ot_name_input_callback,
        pokemon_fap,
        ot_name_buf,
        sizeof(ot_name_buf),
        true);
    text_input_set_header_text(pokemon_fap->text_input, "Enter OT Name");

    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher, AppViewOpts, text_input_get_view(pokemon_fap->text_input));
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewOpts);
}
