#ifndef __POKEMON_DATA_I_H__
#define __POKEMON_DATA_I_H__

#include "pokemon_data.h"

//#include "pokemon_app.h"
//#include "pokemon_char_encode.h"

static void pokemon_stat_ev_calc(PokemonData* pdata, EvIv val);
static void pokemon_stat_iv_calc(PokemonData* pdata, EvIv val);

/* The struct is laid out exactly as the data trasfer that gets sent for trade
 * information. It has to be packed in order to not have padding in the Flipper.
 * Packing is always potentially filled with pitfalls, however this has worked
 * in testing without issue and this code isn't meant to be portable.
 */

/* NOTE: These are all opposite endianness on the flipper than they are in the
 * GB/Z80. e.g. a uint16_t value of 0x2c01 translates to 0x012c.
 * Need to use __builtin_bswap16(val) to switch between Flipper and Pokemon.
 */
/* This is 44 bytes in memory */
struct __attribute__((__packed__)) pokemon_party_data_gen_i {
    uint8_t index;
    uint16_t hp; // Calculated from level
    /* Level is normally calculated from exp, however, level is more human
     * readable/digestable compared to exp. Therefore, we set legel and then
     * from that calculate, (Max)HP, ATK, DEF, SPD, SPC.
     */
    uint8_t level;
    uint8_t status_condition; // Do you really want to trade a Poisoned pokemon?
    uint8_t type[2]; // Pokemon with a single type just repeat the type twice
    uint8_t catch_held; // Unsure if this has any effect in Gen 1
    uint8_t move[4];
    uint16_t ot_id;
    uint8_t exp[3]; // Calculated from level
    uint16_t hp_ev;
    uint16_t atk_ev;
    uint16_t def_ev;
    uint16_t spd_ev;
    uint16_t spc_ev;
    uint16_t iv;
    uint8_t move_pp[4];
    uint8_t level_again; // Copy of level
    uint16_t max_hp; // Calculated from level
    uint16_t atk; // Calculated from level
    uint16_t def; // Calculated from level
    uint16_t spd; // Calculated from level
    uint16_t spc; // Calculated from level
};

struct __attribute__((__packed__)) name {
    /* Reused a few times, but in Gen I, all name strings are 11 bytes in memory.
     * At most, 10 symbols and a TERM_ byte.
     * Note that some strings must be shorter than 11.
     */
    uint8_t str[LEN_NAME_BUF];
};
typedef struct name Name;

/* This is 415 bytes in memory/transmitted */
struct __attribute__((__packed__)) trade_block_gen_i {
    Name trainer_name;
    uint8_t party_cnt;
    /* Only the first pokemon is ever used even though there are 7 bytes here.
     * If the remaining 6 bytes are _not_ 0xff, then the trade window renders
     * garbage for the Flipper's party.
     */
    uint8_t party_members[7];
    /* Only the first pokemon is set up, even though there are 6 total party members */
    PokemonPartyGenI party[6];
    /* Only the first pokemon has an OT name and nickname even though there are 6 members */
    /* OT name should not exceed 7 chars! */
    Name ot_name[6];
    Name nickname[6];
};

/* NOTE: These are all opposite endianness on the flipper than they are in the
 * GB/Z80. e.g. a uint16_t value of 0x2c01 translates to 0x012c.
 * Need to use __builtin_bswap16(val) to switch between Flipper and Pokemon.
 */
/* This is 48 bytes in memory */
struct __attribute__((__packed__)) pokemon_party_data_gen_ii {
    uint8_t index;
    uint8_t held_item;
    uint8_t move[4];
    uint16_t ot_id;
    uint8_t exp[3];
    uint16_t hp_ev;
    uint16_t atk_ev;
    uint16_t def_ev;
    uint16_t spd_ev;
    uint16_t spc_ev;
    uint16_t iv;
    uint8_t move_pp[4];
    uint8_t friendship;
    uint8_t pokerus;
    uint16_t caught_data;
    /* Level is normally calculated from exp, however, level is more human
     * readable/digestable compared to exp. Therefore, we set level and then
     * from that calculate, (Max)HP, ATK, DEF, SPD, SPC.
     */
    uint8_t level;
    uint8_t status_condition;
    uint8_t unused;
    uint16_t hp;
    uint16_t max_hp;
    uint16_t atk;
    uint16_t def;
    uint16_t spd;
    uint16_t spc_atk;
    uint16_t spc_def;
};

/* NOTE:
 * For eggs in gen ii, the handling is a bit clever. The party structure is set
 * up as normal for the pokemon that will hatch. The only difference is the
 * friendship vairable is used to denote number of egg cycles remaining.
 * Then, in the party_members array, that pokemon's index is set to 0xFD which
 * is the index for an egg. Once traded, its now an egg.
 * Creating an egg is not implemented at this time because I don't really see
 * a reason to. But, knowing some of these details makes it really easy to
 * implement later on.
 */

struct __attribute__((__packed__)) trade_block_gen_ii {
    Name trainer_name;
    uint8_t party_cnt;
    /* Only the first pokemon is ever used even though there are 7 bytes here.
     * If the remaining 6 bytes are _not_ 0xff, then the trade window renders
     * garbage for the Flipper's party.
     */
    uint8_t party_members[7];
    uint16_t trainer_id;
    /* Only the first pokemon is set up, even though there are 6 total party members */
    PokemonPartyGenII party[6];
    /* Only the first pokemon has an OT name and nickname even though there are 6 members */
    /* OT name should not exceed 7 chars! */
    Name ot_name[6];
    Name nickname[6];
};

#endif // __POKEMON_DATA_I_H__
