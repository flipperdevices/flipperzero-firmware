#include <stdint.h>
#include "../pokemon_data.h"

#include "unown_form.h"

/* This is used to get the current IVs from the trade struct.
 * Unown form is calculated by taking the middle bytes of each nibble of IV,
 * pressing them in order to a single byte, and dividing that by 10 (rounded
 * down/floor). This will create a value from 0 to 25 that is a 1:1 mapping
 * of the English alphabet and is how Unown forms are represented.
 *
 * C integer division truncates to 0 rather than does any proper rounding.
 *
 * https://bulbapedia.bulbagarden.net/wiki/Individual_values#Unown's_letter
 */
static uint8_t unown_ivs_get(PokemonData* pdata) {
    furi_assert(pdata);
    uint16_t ivs = pokemon_stat_get(pdata, STAT_IV, NONE);
    uint8_t ivs_mid;

    ivs_mid =
        (((ivs & 0x6000) >> 7) | ((ivs & 0x0600) >> 5) | ((ivs & 0x0060) >> 3) |
         ((ivs & 0x0006) >> 1));

    return ivs_mid;
}

static void unown_ivs_set(PokemonData* pdata, uint8_t ivs_mid) {
    furi_assert(pdata);
    uint16_t ivs = pokemon_stat_get(pdata, STAT_IV, NONE);

    /* Clear the middle bits of each nibble */
    ivs &= ~(0x6666);

    /* Set the updated ivs_mid in to those cleared bits */
    ivs |=
        (((ivs_mid & 0xC0) << 7) | ((ivs_mid & 0x30) << 5) | ((ivs_mid & 0x0C) << 3) |
         ((ivs_mid & 0x03) << 1));
    pokemon_stat_set(pdata, STAT_IV, NONE, ivs);
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
