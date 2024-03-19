#include <gui/modules/submenu.h>

#include "../pokemon_app.h"
#include "../pokemon_data.h"
#include "pokemon_menu.h"

static const char* gender_str[] = {
    "Unknown",
    "Female",
    "Male",
};

/* This returns a string pointer if the gender is static, NULL if it is not and
 * the gender needs to be calculated.
 */
const char* select_gender_is_static(PokemonData* pdata, uint8_t ratio) {
    switch(ratio) {
    case 0xFF:
        return gender_str[0];
    case 0xFE:
        return gender_str[1];
    case 0x00:
        if(pokemon_stat_get(pdata, STAT_NUM, NONE) != 0xEB) { // Tyrogue can be either gender
            return gender_str[2];
        }
        break;
    default:
        break;
    }

    return NULL;
}

const char* select_gender_get(PokemonData* pdata) {
    uint8_t ratio = table_stat_base_get(
        pdata->pokemon_table,
        pokemon_stat_get(pdata, STAT_NUM, NONE),
        STAT_BASE_GENDER_RATIO,
        NONE);
    uint8_t atk_iv;
    const char* rc;

    rc = select_gender_is_static(pdata, ratio);
    if(rc) return rc;

    /* Falling through here means now we need to calculate the gender from
     * its ratio and ATK_IV.
     */
    atk_iv = pokemon_stat_get(pdata, STAT_ATK_IV, NONE);
    if(atk_iv * 17 <= ratio)
        return gender_str[1];
    else
        return gender_str[2];
}

static void select_gender_selected_callback(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    PokemonData* pdata = pokemon_fap->pdata;
    uint8_t ratio = table_stat_base_get(
        pdata->pokemon_table,
        pokemon_stat_get(pdata, STAT_NUM, NONE),
        STAT_BASE_GENDER_RATIO,
        NONE);
    uint8_t atk_iv = pokemon_stat_get(pdata, STAT_ATK_IV, NONE);

    /* If we need to make the pokemon a male, increase atk IV until it exceeds
     * the gender ratio.
     *
     * Note that, there is no checking here for impossible situations as the
     * scene enter function will immediately quit if its not possible to change
     * the gender (the extremes of gender_ratio value).
     *
     * The check for gender is a percentage, if ATK_IV*(255/15) <= the ratio,
     * then the pokemon is a female. The gender ratio values end up being:
     * DEF GENDER_F0      EQU   0 percent
     * DEF GENDER_F12_5   EQU  12 percent + 1
     * DEF GENDER_F25     EQU  25 percent
     * DEF GENDER_F50     EQU  50 percent
     * DEF GENDER_F75     EQU  75 percent
     * DEF GENDER_F100    EQU 100 percent - 1
     * Where percent is (255/100)
     */
    if(index) {
        while((atk_iv * 17) <= ratio) atk_iv++;
    } else {
        while((atk_iv * 17) > ratio) atk_iv--;
    }

    pokemon_stat_set(pdata, STAT_ATK_IV, NONE, atk_iv);

    scene_manager_previous_scene(pokemon_fap->scene_manager);
}

void select_gender_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    submenu_reset(pokemon_fap->submenu);

    submenu_add_item(
        pokemon_fap->submenu, "Female", 0, select_gender_selected_callback, pokemon_fap);

    submenu_add_item(
        pokemon_fap->submenu, "Male", 1, select_gender_selected_callback, pokemon_fap);
}
