#ifndef POKEMON_DATA_H
#define POKEMON_DATA_H

#pragma once

#include <furi.h>
#include <gui/icon.h>
#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>

#include <math.h>
#include <stdint.h>

#include <named_list.h>
#include <stat_nl.h>
#include <pokemon_table.h>
#include "stats.h"

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

typedef struct pokemon_party_data_gen_i PokemonPartyGenI;
typedef struct trade_block_gen_i TradeBlockGenI;
typedef struct pokemon_party_data_gen_ii PokemonPartyGenII;
typedef struct trade_block_gen_ii TradeBlockGenII;

/* Based on the flipperzero-game-engine sprite structure */
struct fxbm_sprite {
    uint32_t width;
    uint32_t height;
    uint8_t data[];
};

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
    size_t party_sz;

    /* Current EV/IV stat selection */
    EvIv stat_sel;

    /* Current generation */
    uint8_t gen;

    /* 0 indexed max pokedex number */
    uint8_t dex_max;

    /* These are private to pokemon_data */
    Storage* storage;
    struct fxbm_sprite* bitmap;
    uint8_t bitmap_num;
    FuriString* asset_path;
};
typedef struct pokemon_data PokemonData;

PokemonData* pokemon_data_alloc(uint8_t gen);
void pokemon_data_free(PokemonData* pdata);

uint8_t* pokemon_icon_get(PokemonData* pdata, int num);

void pokemon_stat_memcpy(PokemonData* dst, PokemonData* src, uint8_t which);
uint16_t pokemon_stat_get(PokemonData* pdata, DataStat stat, DataStatSub num);
void pokemon_stat_set(PokemonData* pdata, DataStat stat, DataStatSub which, uint16_t val);
uint16_t pokemon_stat_ev_get(PokemonData* pdata, DataStat stat);
void pokemon_stat_ev_set(PokemonData* pdata, DataStat stat, uint16_t val);
void pokemon_stat_iv_set(PokemonData* pdata, int val);
void pokemon_exp_set(PokemonData* pdata, uint32_t exp);
void pokemon_exp_calc(PokemonData* pdata);
void pokemon_stat_calc(PokemonData* pdata, DataStat stat);
void pokemon_default_nickname_set(char* dest, PokemonData* pdata, size_t n);
void pokemon_name_set(PokemonData* pdata, DataStat stat, char* name);
void pokemon_name_get(PokemonData* pdata, DataStat stat, char* dest, size_t len);
#endif /* POKEMON_DATA_H */
