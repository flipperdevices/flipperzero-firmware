#ifndef POKEMON_DATA_H
#define POKEMON_DATA_H

#pragma once

#include <stdint.h>
#include <furi.h>
#include <math.h>

/* Generation defines */
#define GEN_I 0x01
#define GEN_II 0x02

/* Some length macros */
#define LEN_NAME_BUF 11
#define LEN_NICKNAME 11 // Max 10 chars
#define LEN_OT_NAME 8 // Max 7 chars
#define LEN_NUM_BUF 6
#define LEN_LEVEL 4 // Max 3 digits
#define LEN_OT_ID 6 // Max 5 digits

typedef enum {
    /* Base stats */
    STAT_BASE = 0, // Sentry value
    STAT_BASE_ATK = 0,
    STAT_BASE_DEF,
    STAT_BASE_SPD,
    /* NOTE! While accessing SPC/APC_AT will do the correct thing for both
     * Gen I and Gen II, accessing SPC_DEF for Gen I will return a value
     * that is not used in Gen I games. This normally isn't an issue, but
     * is a potential gotcha to be aware of.
     */
    STAT_BASE_SPC = 3,
    STAT_BASE_SPC_ATK = 3,
    STAT_BASE_SPC_DEF,
    STAT_BASE_HP,
    STAT_BASE_TYPE,
    STAT_BASE_MOVE,
    STAT_BASE_GROWTH,
    STAT_BASE_GENDER_RATIO,
    //STAT_BASE_EGG_CYCLES,
    STAT_BASE_END, // Sentry value

    /* XXX: Add icon/image/name ptr here? */

    /* Repeated enum numbers to allow for more programmatic access */
    /* In-party stats */
    STAT = 0, // Sentry value
    STAT_ATK = 0,
    STAT_DEF,
    STAT_SPD,
    /* Gen I uses SPC, Gen II uses SPC_ATK and SPC_DEF */
    STAT_SPC = 3,
    STAT_SPC_ATK = 3,
    STAT_SPC_DEF,
    STAT_HP,
    STAT_END = 6, // Sentry value
    STAT_TYPE = 6,
    STAT_MOVE,

    STAT_EV = 10, // Sentry value
    STAT_ATK_EV = 10,
    STAT_DEF_EV,
    STAT_SPD_EV,
    STAT_SPC_EV,
    STAT_HP_EV,
    STAT_EV_END = 15, // Sentry value

    STAT_IV = 15, // Sentry value
    STAT_ATK_IV = 15,
    STAT_DEF_IV,
    STAT_SPD_IV,
    STAT_SPC_IV,
    STAT_HP_IV,
    STAT_IV_END = 20, // Sentry value

    /* These won't ever really be needed in groups */
    STAT_LEVEL = 20,
    STAT_INDEX,
    STAT_NUM,
    STAT_CONDITION,
    STAT_NICKNAME,
    STAT_OT_NAME,
    STAT_OT_ID,
    STAT_TRAINER_NAME,
    STAT_SEL, // which EV/IV calc to use
    STAT_EXP,
    STAT_HELD_ITEM,
} DataStat;

typedef enum {
    MOVE_0 = 0,
    MOVE_1,
    MOVE_2,
    MOVE_3,

    TYPE_0 = 0,
    TYPE_1,

    EXP_0 = 0,
    EXP_1,
    EXP_2,

    NONE = 0, // Just a filler value
} DataStatSub;

typedef enum {
    RANDIV_ZEROEV,
    RANDIV_LEVELEV,
    RANDIV_MAXEV,
    MAXIV_ZEROEV,
    MAXIV_LEVELEV,
    MAXIV_MAXEV,
} EvIv;

typedef struct pokemon_party_data_gen_i PokemonPartyGenI;
typedef struct trade_block_gen_i TradeBlockGenI;
typedef struct pokemon_party_data_gen_ii PokemonPartyGenII;
typedef struct trade_block_gen_ii TradeBlockGenII;

typedef struct named_list NamedList;
typedef struct pokemon_data_table PokemonTable;

struct pokemon_data {
    const NamedList* move_list;
    const NamedList* stat_list;
    const NamedList* type_list;
    const NamedList* item_list;
    const PokemonTable* pokemon_table;
    /* Pointer to the live trade_block */
    void* trade_block;
    /* The length of the current trade_block. */
    size_t trade_block_sz;
    /* Shortcut pointer to the actual party data in the trade block */
    void* party;

    /* Current EV/IV stat selection */
    EvIv stat_sel;

    /* Current generation */
    uint8_t gen;
    /* 0 indexed max pokedex number */
    uint8_t dex_max;
};
typedef struct pokemon_data PokemonData;

PokemonData* pokemon_data_alloc(uint8_t gen);
void pokemon_data_free(PokemonData* pdata);

int namelist_pos_get(const NamedList* list, uint8_t index);
int namelist_index_get(const NamedList* list, uint8_t pos);
const char* namelist_name_get_index(const NamedList* list, uint8_t index);
const char* namelist_name_get_pos(const NamedList* list, uint8_t pos);
uint8_t namelist_gen_get_pos(const NamedList* list, uint8_t pos);
int namelist_cnt(const NamedList* list);

uint8_t table_stat_base_get(const PokemonTable* table, PokemonData *pdata, DataStat stat, DataStatSub num);
const char* table_stat_name_get(const PokemonTable* table, int num);
const Icon *table_icon_get(const PokemonTable* table, int num);

void pokemon_stat_memcpy(PokemonData* dst, PokemonData* src, uint8_t which);
uint16_t pokemon_stat_get(PokemonData *pdata, DataStat stat, DataStatSub num);
void pokemon_stat_set(PokemonData *pdata, DataStat stat, DataStatSub which, uint16_t val);
uint16_t pokemon_stat_ev_get(PokemonData *pdata, DataStat stat);
void pokemon_stat_ev_set(PokemonData *pdata, DataStat stat, uint16_t val);
uint8_t pokemon_stat_iv_get(PokemonData *pdata, DataStat stat);
void pokemon_stat_iv_set(PokemonData *pdata, int val);
void pokemon_exp_set(PokemonData *pdata, uint32_t exp);
void pokemon_exp_calc(PokemonData *pdata);
void pokemon_stat_calc(PokemonData *pdata, DataStat stat);
void pokemon_default_nickname_set(char* dest, PokemonData *pdata, size_t n);
void pokemon_name_set(PokemonData *pdata, DataStat stat, char* name);
void pokemon_name_get(PokemonData *pdata, DataStat stat, char* dest, size_t len);
#endif /* POKEMON_DATA_H */
