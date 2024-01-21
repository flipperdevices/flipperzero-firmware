#ifndef POKEMON_DATA_H
#define POKEMON_DATA_H

#pragma once

#include <stdint.h>
#include <furi.h>
#include <math.h>

/* #defines for the data table entries */
#define GROWTH_FAST 4
#define GROWTH_MEDIUM_FAST 0
#define GROWTH_MEDIUM_SLOW 3
#define GROWTH_SLOW 5

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
    STAT_BASE_SPC,
    STAT_BASE_HP,
    STAT_BASE_TYPE,
    STAT_BASE_MOVE,
    STAT_BASE_GROWTH,
    STAT_BASE_END, // Sentry value

    /* XXX: Add icon/image/name ptr here? */

    /* Repeated enum numbers to allow for more programmatic access */
    /* In-party stats */
    STAT = 0, // Sentry value
    STAT_ATK = 0,
    STAT_DEF,
    STAT_SPD,
    STAT_SPC,
    STAT_HP,
    STAT_END = 5, // Sentry value
    STAT_TYPE = 5,
    STAT_MOVE,

    STAT_EV = 9, // Sentry value
    STAT_ATK_EV = 9,
    STAT_DEF_EV,
    STAT_SPD_EV,
    STAT_SPC_EV,
    STAT_HP_EV,
    STAT_EV_END, // Sentry value

    STAT_IV = 15, // Sentry value
    STAT_ATK_IV = 15,
    STAT_DEF_IV,
    STAT_SPD_IV,
    STAT_SPC_IV,
    STAT_HP_IV,
    STAT_IV_END, // Sentry value

    /* These won't ever really be needed in groups */
    STAT_LEVEL,
    STAT_INDEX,
    STAT_NUM,
    STAT_NICKNAME,
    STAT_OT_NAME,
    STAT_OT_ID,
    STAT_TRAINER_NAME,
    STAT_SEL, // which EV/IV calc to use
    STAT_GEN,
    STAT_EXP,
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

struct __attribute__((__packed__)) pokemon_data_table {
    const char* name;
    const Icon* icon;
    const uint8_t index;
    const uint8_t base_hp;
    const uint8_t base_atk;
    const uint8_t base_def;
    const uint8_t base_spd;
    const uint8_t base_spc;
    const uint8_t type[2];
    const uint8_t move[4];
    const uint8_t growth;
};

typedef struct pokemon_data_table PokemonTable;

struct __attribute__((__packed__)) named_list {
    const char* name;
    const uint8_t index;
    const uint8_t generation;
};

typedef struct named_list NamedList;

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
struct __attribute__((__packed__)) pokemon_structure {
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

/* This is 415 bytes in memory/transmitted */
struct __attribute__((__packed__)) trade_data_block {
    /* TODO: Change this to use struct name above */
    struct name trainer_name;
    uint8_t party_cnt;
    /* Only the first pokemon is ever used even though there are 7 bytes here.
     * If the remaining 6 bytes are _not_ 0xff, then the trade window renders
     * garbage for the Flipper's party.
     */
    uint8_t party_members[7];
    /* Only the first pokemon is set up, even though there are 6 total party members */
    struct pokemon_structure party[6];
    /* Only the first pokemon has an OT name and nickname even though there are 6 members */
    /* OT name should not exceed 7 chars! */
    struct name ot_name[6];
    struct name nickname[6];

    /* Extra variables needed to track tradeblock state appended here.
     * These _should_ never be transmitted.
     */
    uint8_t gen;
    uint8_t stat_sel;
};

typedef struct trade_data_block TradeBlock;

extern const PokemonTable pokemon_table[];
extern const NamedList move_list[];
extern const NamedList type_list[];
extern const NamedList stats_list[];

int named_list_pos_from_index_get(const NamedList* list, uint8_t index);
const char* named_list_name_from_index_get(const NamedList* list, uint8_t index);
int named_list_num_elements_get(const NamedList* list);
uint8_t table_stat_base_get(void* block, int gen, DataStat stat, DataStatSub num);
uint16_t pokemon_stat_get(void* block, int gen, DataStat stat, DataStatSub num);
void pokemon_stat_set(void* block, int gen, DataStat stat, DataStatSub which, uint16_t val);
uint16_t pokemon_stat_ev_get(void* block, int gen, DataStat stat);
void pokemon_stat_ev_set(void* block, int gen, DataStat stat, uint16_t val);
uint8_t pokemon_stat_iv_get(void* block, int gen, DataStat stat);
const char* table_stat_name_get(int num);
const Icon *table_icon_get(int num);
void pokemon_stat_iv_set(void* block, int gen, int val);
void pokemon_exp_set(void* block, int gen, uint32_t exp);
void pokemon_exp_calc(void* trade, int gen);
void pokemon_stat_calc(void* block, int gen, DataStat stat);
void pokemon_default_nickname_set(char* dest, void* block, int gen, size_t n);
void pokemon_name_set(void* block, int gen, DataStat stat, char* name);
void pokemon_name_get(void* block, int gen, DataStat stat, char* dest, size_t len);
#endif /* POKEMON_DATA_H */
