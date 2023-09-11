#include <ctype.h>
#include <furi.h>
#include <gui/modules/text_input.h>
#include <gui/view_dispatcher.h>
#include <stdlib.h>

#include "../pokemon_app.h"
#include "../pokemon_char_encode.h"
#include "pokemon_menu.h"

static char name_buf[11];

/* NOTE:
 * It would be nice if we could cleanly default to the pokemon's name as their
 * nickname. The issue is that if you enter a blank line to text input, it does
 * call this function, but returning true does nothing. However, I've found that
 * if you check for the first char of the buffer being \0, you can then set the
 * buffer and then return true. This has the effect of staying in the text_input
 * screen, but, prepopulating the text entry with the buffer AND staying on the
 * save button.
 */
static bool select_nickname_input_validator(const char* text, FuriString* error, void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    bool rc = true;

    if(text[0] == '\0') {
        /* Get the pokemon's name and populate our buffer with it */
        /* XXX: Nidoran M/F are still a problem with this. */
        pokemon_trade_block_set_default_name(name_buf, pokemon_fap, sizeof(name_buf));
        return true;
    }

    if(rc == false) {
        furi_string_printf(error, "Some error?");
    } else {
        /* Clear existing nickname in trade block*/
        memset(pokemon_fap->trade_block->nickname, TERM_, sizeof(struct name));

        /* Encode string to nickname */
        pokemon_str_to_encoded_array(
            (uint8_t*)pokemon_fap->trade_block->nickname, (char*)text, strlen(text));
    }

    return rc;
}

static void select_nickname_input_callback(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    scene_manager_previous_scene(pokemon_fap->scene_manager);
}

void select_nickname_scene_on_exit(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewMainMenu);
    view_dispatcher_remove_view(pokemon_fap->view_dispatcher, AppViewOpts);
}

void select_nickname_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    text_input_reset(pokemon_fap->text_input);
    text_input_set_validator(
        pokemon_fap->text_input, select_nickname_input_validator, pokemon_fap);
    text_input_set_result_callback(
        pokemon_fap->text_input,
        select_nickname_input_callback,
        pokemon_fap,
        name_buf,
        sizeof(name_buf),
        true);
    text_input_set_header_text(pokemon_fap->text_input, "Nickname (none for default)");

    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher, AppViewOpts, text_input_get_view(pokemon_fap->text_input));
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewOpts);
}
