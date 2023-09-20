#ifndef POKEMON_DATA_H
#define POKEMON_DATA_H

#pragma once

/* NOTE: These are all opposite endianness on the flipper than they are in the
 * GB/Z80. e.g. a uint16_t value of 0x2c01 translates to 0x012c. Does flipper
 * API have calls to swap endianness?
 */
struct __attribute__((__packed__)) pokemon_structure {
    uint8_t index;
    uint16_t hp;
    uint8_t level; // Effectively useless, recalculated based on exp at multiple points
    uint8_t status_condition; // Do you really want to trade a Poisoned pokemon?
    uint8_t type[2]; // These values are meaningless in Gen 1, they are never used
    uint8_t catch_held; // Unsure if this has any effect in Gen 1
    uint8_t move[4];
    uint16_t orig_trainer;
    uint8_t exp[3];
    uint16_t hp_ev;
    uint16_t atk_ev;
    uint16_t def_ev;
    uint16_t spd_ev;
    uint16_t special_ev;
    uint16_t iv;
    uint8_t move_pp[4];
    uint8_t level_again;
    uint16_t max_hp;
    uint16_t atk;
    uint16_t def;
    uint16_t spd;
    uint16_t special;
};

struct __attribute__((__packed__)) name {
    unsigned char str[11];
};

struct __attribute__((__packed__)) trade_data_block {
    unsigned char trainer_name[11];
    uint8_t party_cnt;
    uint8_t party_members[7]; // Unsure if last byte always needs to be 0xff for terminator
    struct pokemon_structure party[6];
    struct name ot_name[6];
    struct name nickname[6];
};

typedef struct trade_data_block TradeBlock;

#endif /* POKEMON_DATA_H */
