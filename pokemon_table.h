#ifndef __POKEMON_TABLE_H__
#define __POKEMON_TABLE_H__

#include <stdint.h>

#include <gui/icon.h>

typedef enum {
    GROWTH_MEDIUM_FAST = 0,
    GROWTH_MEDIUM_SLOW = 3,
    GROWTH_FAST = 4,
    GROWTH_SLOW = 5,
} Growth;

typedef enum {
    GENDER_F0 = 0x00,
    GENDER_F12_5 = 0x1F,
    GENDER_F25 = 0x3F,
    GENDER_F50 = 0x7F,
    GENDER_F75 = 0xBF,
    GENDER_F100 = 0xFE,
    GENDER_UNKNOWN = 0xFF,
} Gender;

/* NOTE: It seems like gen ii index is national pokedex order? */
/* Gen i and Gen ii are _almost_ the same with all stats. The big difference
 * is that while most gen i pokemon's spc matches the same gen ii spc_atk,
 * some of them do differ. Therefore, we track spc for gen i, and then spc_atk
 * and spc_def for gen ii.
 */
struct __attribute__((__packed__)) pokemon_data_table {
    const char* name;
    const Icon* icon;
    const uint8_t index;
    const uint8_t base_hp;
    const uint8_t base_atk;
    const uint8_t base_def;
    const uint8_t base_spd;
    const uint8_t base_spc;
    const uint8_t base_spc_atk;
    const uint8_t base_spc_def;
    const uint8_t type[2];
    const uint8_t move[4];
    const Growth growth;
    const Gender gender_ratio;
    /* XXX: Unsure if I want to implement this or not */
    //const uint8_t egg_cycles;
};

typedef struct pokemon_data_table PokemonTable;
extern const PokemonTable pokemon_table[];

#endif // __POKEMON_TABLE_H__
