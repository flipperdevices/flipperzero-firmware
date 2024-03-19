#ifndef __POKEMON_TABLE_H__
#define __POKEMON_TABLE_H__

#include <stdint.h>

#include "stats.h"

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

typedef struct pokemon_data_table PokemonTable;

int table_pokemon_pos_get(const PokemonTable* table, uint8_t index);
uint8_t
    table_stat_base_get(const PokemonTable* table, uint8_t num, DataStat stat, DataStatSub which);
const char* table_stat_name_get(const PokemonTable* table, int num);
const PokemonTable* table_pointer_get();

#endif // __POKEMON_TABLE_H__
