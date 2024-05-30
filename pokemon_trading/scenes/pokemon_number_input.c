#include <furi.h>
#include <gui/modules/text_input.h>
#include <gui/view_dispatcher.h>
#include <stdlib.h>

#include "../pokemon_app.h"
#include "../pokemon_data.h"
#include "pokemon_menu.h"

static char number_buf[LEN_NUM_BUF];

static bool select_number_input_validator(const char* text, FuriString* error, void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    uint32_t state = scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectLevelScene);
    int number;
    char* error_str;
    int min;
    int max;
    DataStat stat;
    bool rc = true;
    unsigned int i;

    switch(state) {
    case SelectLevelScene:
        error_str = "Level must\nbe a number\nbetween\n2-100!";
        min = 2;
        max = 100;
        stat = STAT_LEVEL;
        break;
    case SelectOTIDScene:
        error_str = "OT ID must\nbe between\n0-65535!";
        min = 0;
        max = 65535;
        stat = STAT_OT_ID;
        break;
    default:
        return false;
    }

    /* Need to check each byte to ensure is not alpha. atoi returns 0 which is
     * technically a valid OTID, so we need to separately check for alpha chars.
     */
    for(i = 0; i < sizeof(text); i++) {
        if(!isdigit((unsigned int)text[i])) {
            if(text[i] == '\0') break;
            rc = false;
            break;
        }
    }

    number = atoi(text);
    if(number < min || number > max || rc == false) {
        furi_string_printf(error, error_str);
        rc = false;
    } else {
        pokemon_stat_set(pokemon_fap->pdata, stat, NONE, number);
    }

    return rc;
}

static void select_number_input_callback(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    scene_manager_previous_scene(pokemon_fap->scene_manager);
}

void select_number_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    char* header;
    uint32_t state = scene_manager_get_scene_state(pokemon_fap->scene_manager, SelectLevelScene);
    int len;
    DataStat stat;

    switch(state) {
    case SelectLevelScene:
        header = "Enter level (numbers only)";
        len = LEN_LEVEL;
        stat = STAT_LEVEL;
        break;
    case SelectOTIDScene:
        header = "Enter OT ID (numbers only)";
        len = LEN_OT_ID;
        stat = STAT_OT_ID;
        break;
    default:
        furi_crash("Num: invalid state");
        break;
    }

    snprintf(number_buf, len, "%d", pokemon_stat_get(pokemon_fap->pdata, stat, NONE));

    text_input_reset(pokemon_fap->text_input);
    text_input_set_validator(pokemon_fap->text_input, select_number_input_validator, pokemon_fap);
    text_input_set_result_callback(
        pokemon_fap->text_input, select_number_input_callback, pokemon_fap, number_buf, len, true);
    text_input_set_header_text(pokemon_fap->text_input, header);

    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher, AppViewOpts, text_input_get_view(pokemon_fap->text_input));
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewOpts);
}
