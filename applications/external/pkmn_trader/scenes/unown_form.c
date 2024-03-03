#include <stdint.h>
#include "../pokemon_data.h"

#include "unown_form.h"

/* XXX: Can improve performance and size by using a single call to get IV */
/* XXX: Also, could loop with get/set maybe? */
static uint8_t unown_ivs_get(PokemonData* pdata) {
    uint8_t atk_iv = pokemon_stat_get(pdata, STAT_ATK_IV, NONE);
    uint8_t def_iv = pokemon_stat_get(pdata, STAT_DEF_IV, NONE);
    uint8_t spd_iv = pokemon_stat_get(pdata, STAT_SPD_IV, NONE);
    uint8_t spc_iv = pokemon_stat_get(pdata, STAT_SPC_IV, NONE);
    uint8_t ivs;

    ivs =
        (((atk_iv & 0x6) << 5) | ((def_iv & 0x6) << 3) | ((spd_iv & 0x6) << 1) |
         ((spc_iv & 0x6) >> 1));

    return ivs;
}

static void unown_ivs_set(PokemonData* pdata, uint8_t ivs) {
    uint8_t atk_iv = (pokemon_stat_get(pdata, STAT_ATK_IV, NONE) & 0x9);
    uint8_t def_iv = (pokemon_stat_get(pdata, STAT_DEF_IV, NONE) & 0x9);
    uint8_t spd_iv = (pokemon_stat_get(pdata, STAT_SPD_IV, NONE) & 0x9);
    uint8_t spc_iv = (pokemon_stat_get(pdata, STAT_SPC_IV, NONE) & 0x9);

    atk_iv |= ((ivs & 0xC0) >> 5);
    def_iv |= ((ivs & 0x30) >> 3);
    spd_iv |= ((ivs & 0x0C) >> 1);
    spc_iv |= ((ivs & 0x03) << 1);

    pokemon_stat_set(pdata, STAT_ATK_IV, NONE, atk_iv);
    pokemon_stat_set(pdata, STAT_DEF_IV, NONE, def_iv);
    pokemon_stat_set(pdata, STAT_SPD_IV, NONE, spd_iv);
    pokemon_stat_set(pdata, STAT_SPC_IV, NONE, spc_iv);
}

char unown_form_get(PokemonData* pdata) {
    uint8_t form = unown_ivs_get(pdata);

    /* The forumula is specifically the center two bits of each IV slapped
     * together and floor(/10)
     */
    form /= 10;
    form += 'A';

    return form;
}

/* Try and get to the desired form by adding/subtracting the current IVs */
void unown_form_set(PokemonData* pdata, char letter) {
    uint8_t ivs = unown_ivs_get(pdata);
    uint8_t form;

    letter = toupper(letter);
    furi_check(isalpha(letter));

    while(1) {
        form = ((ivs / 10) + 'A');
        if(form == letter) break;
        if(form > letter)
            ivs--;
        else
            ivs++;
    }

    /* form is now the target letter, set IVs back up */
    unown_ivs_set(pdata, ivs);
}
