#include <ctype.h>
#include <furi.h>
#include <gui/modules/text_input.h>
#include <gui/view_dispatcher.h>
#include <stdlib.h>

#include "../pokemon_app.h"
#include "../pokemon_data.h"
#include "../pokemon_char_encode.h"
#include "pokemon_menu.h"
#include "unown_form.h"

static char name_buf[LEN_NAME_BUF];

/* NOTE:
 * It would be nice if we could cleanly default to the pokemon's name as their
 * name. The issue is that if you enter a blank line to text input, it does
 * call this function, but returning true does nothing. However, I've found that
 * if you check for the first char of the buffer being \0, you can then set the
 * buffer and then return true. This has the effect of staying in the text_input
 * screen, but, prepopulating the text entry with the buffer AND staying on the
 * save button.
 */
static bool select_name_input_validator(const char* text, FuriString* error, void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    uint32_t state =
        scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectNicknameScene);
    unsigned int i;

    /* A blank field for the pokemon nickname means revert to default name */
    if(text[0] == '\0' && state == SelectNicknameScene) {
        /* Get the pokemon's name and populate our buffer with it */
        /* TODO: Nidoran M/F are still a problem with this. */
        pokemon_default_nickname_set(name_buf, pokemon_fap->pdata, sizeof(name_buf));
        return true;
    }

    /* Check to ensure no digits in OT name/nickname */
    for(i = 0; i < strlen(text); i++) {
        if(isdigit((unsigned int)text[i])) {
            furi_string_printf(error, "Name cannot\ncontain\nnumbers!");
            return false;
        }
    }

    /* Check for Unown setting is a character */
    if(state == SelectUnownFormScene) {
        if(!isalpha((int)text[0])) {
            furi_string_printf(error, "Form must\nbe a single\nletter!");
            return false;
        }
    }

    switch(state) {
    case SelectNicknameScene:
        pokemon_name_set(pokemon_fap->pdata, STAT_NICKNAME, (char*)text);
        break;
    case SelectOTNameScene:
        pokemon_name_set(pokemon_fap->pdata, STAT_OT_NAME, (char*)text);
        break;
    case SelectUnownFormScene:
        unown_form_set(pokemon_fap->pdata, text[0]);
        break;
    default:
        furi_crash("Invalid scene");
        break;
    }

    return true;
}

static void select_name_input_callback(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    scene_manager_previous_scene(pokemon_fap->scene_manager);
}

void select_name_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    uint32_t state =
        scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectNicknameScene);
    char* header;
    int len;
    DataStat stat;

    switch(state) {
    case SelectNicknameScene:
        header = "Nickname (none for default)";
        len = LEN_NICKNAME;
        stat = STAT_NICKNAME;
        break;
    case SelectOTNameScene:
        header = "Enter OT Name";
        len = LEN_OT_NAME;
        stat = STAT_OT_NAME;
        break;
    case SelectUnownFormScene:
        header = "Enter Unown Letter Form";
        len = 2;
        stat = STAT_OT_NAME;
        break;
    default:
        furi_crash("Name: invalid state");
        break;
    }

    if(state == SelectUnownFormScene) {
        /* Put the current letter in the buffer */
        name_buf[0] = unown_form_get(pokemon_fap->pdata);
        name_buf[1] = '\0';
    } else {
        pokemon_name_get(pokemon_fap->pdata, stat, name_buf, len);
    }

    text_input_reset(pokemon_fap->text_input);
    text_input_set_validator(pokemon_fap->text_input, select_name_input_validator, pokemon_fap);
    text_input_set_result_callback(
        pokemon_fap->text_input, select_name_input_callback, pokemon_fap, name_buf, len, true);
    text_input_set_header_text(pokemon_fap->text_input, header);

    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher, AppViewOpts, text_input_get_view(pokemon_fap->text_input));
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewOpts);
}
