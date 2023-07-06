#ifndef POKEMON_DATA_H
#define POKEMON_DATA_H

#pragma once

/* NOTE: These map to the Gen 1 character set! */
/* TODO: It may make more sense to put this in a const array as a LUT,
 * e.g. t['F'], t['l'], t['i'], t['p'], t['e'], t['r'], t['\0']
 * As this could be an easier translation for each letter to build a string
 * to set names and things on the fly in the flipper. Need to explore that.
 * once I get to that point.
 */
#define TERM_ 0x50
#define SPACE_ 0x7f
#define A_ 0x80
#define B_ 0x81
#define C_ 0x82
#define D_ 0x83
#define E_ 0x84
#define F_ 0x85
#define G_ 0x86
#define H_ 0x87
#define I_ 0x88
#define J_ 0x89
#define K_ 0x8a
#define L_ 0x8b
#define M_ 0x8c
#define N_ 0x8d
#define O_ 0x8e
#define P_ 0x8f
#define Q_ 0x90
#define R_ 0x91
#define S_ 0x92
#define T_ 0x93
#define U_ 0x94
#define V_ 0x95
#define W_ 0x96
#define X_ 0x97
#define Y_ 0x98
#define Z_ 0x99
#define O_PAREN_ 0x9a
#define C_PAREN_ 0x9b
#define COLON_ 0x9c
#define SEMI_ 0x9d
#define O_BRACKET_ 0x9e
#define C_BRACKET_ 0x9f
#define a_ 0xa0
#define b_ 0xa1
#define c_ 0xa2
#define d_ 0xa3
#define e_ 0xa4
#define f_ 0xa5
#define g_ 0xa6
#define h_ 0xa7
#define i_ 0xa8
#define j_ 0xa9
#define k_ 0xaa
#define l_ 0xab
#define m_ 0xac
#define n_ 0xad
#define o_ 0xae
#define p_ 0xaf
#define q_ 0xb0
#define r_ 0xb1
#define s_ 0xb2
#define t_ 0xb3
#define u_ 0xb4
#define v_ 0xb5
#define w_ 0xb6
#define x_ 0xb7
#define y_ 0xb8
#define z_ 0xb9
#define e_ACCENT_ 0xba
#define d_TICK_ 0xbb
#define l_TICK_ 0xbc
#define s_TICK_ 0xbd
#define t_TICK_ 0xbe
#define v_TICK_ 0xbf
#define S_QUOTE_ 0xe0
#define PK_ 0xe1
#define MN_ 0xe2
#define DASH_ 0xe3
#define r_TICK_ 0xe4
#define m_TICK_ 0xe5
#define QUESTION_ 0xe6
#define EXCLAIM_ 0xe7
#define PERIOD_ 0xe8
#define R_ARR_ 0xec
#define D_ARR_ 0xee
#define MALE_ 0xef
#define FEMALE_ 0xf5
#define _0_ 0xf6
#define _1_ 0xf7
#define _2_ 0xf8
#define _3_ 0xf9
#define _4_ 0xfa
#define _5_ 0xfb
#define _6_ 0xfc
#define _7_ 0xfd
#define _8_ 0xfe
#define _9_ 0xff

/* XXX: These are all opposite endianness on the flipper than they are in the
 * GB/Z80. e.g. a uint16_t value of 0x2c01 translates to 0x012c. Does flipper
 * API have calls to swap endianness?
 */
struct __attribute__((__packed__)) pokemon_structure {
    uint8_t species;
    uint16_t hp;
    uint8_t level;
    uint8_t status_condition;
    uint8_t type[2];
    uint8_t catch_held;
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

struct trade_data_block DATA_BLOCK2 =
    {.trainer_name = {F_, l_, i_, p_, p_, e_, r_, TERM_, 0x00, 0x00, 0x00},
     .party_cnt = 1,
     .party_members = {0x15, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
     .party =
         {
             {.species = 0x4a,
              .hp = 0x2c01,
              .level = 0x4a,
              .status_condition = 0x0,
              .type = {0x14, 0x08},
              .catch_held = 0x1f,
              .move = {0x7e, 0x38, 0x09, 0x19},
              .orig_trainer = 0xd204,
              .exp = {0x3, 0xd, 0x40},
              .hp_ev = 0xffff,
              .atk_ev = 0xffff,
              .def_ev = 0xffff,
              .spd_ev = 0xffff,
              .special_ev = 0xffff,
              .iv = 0xffff,
              .move_pp = {0xc0, 0xc0, 0xc0, 0xc0},
              .level_again = 0x4a,
              .max_hp = 0x2c01,
              .atk = 0x9600,
              .def = 0x9700,
              .spd = 0x9800,
              .special = 0x9900},
             {.species = 0x4a,
              .hp = 0x2c01,
              .level = 0x4a,
              .status_condition = 0x0,
              .type = {0x14, 0x08},
              .catch_held = 0x1f,
              .move = {0x7e, 0x38, 0x09, 0x19},
              .orig_trainer = 0xd204,
              .exp = {0x3, 0xd, 0x40},
              .hp_ev = 0xffff,
              .atk_ev = 0xffff,
              .def_ev = 0xffff,
              .spd_ev = 0xffff,
              .special_ev = 0xffff,
              .iv = 0xffff,
              .move_pp = {0xc0, 0xc0, 0xc0, 0xc0},
              .level_again = 0x4a,
              .max_hp = 0x2c01,
              .atk = 0x9600,
              .def = 0x9700,
              .spd = 0x9800,
              .special = 0x9900},
             {.species = 0x4a,
              .hp = 0x2c01,
              .level = 0x4a,
              .status_condition = 0x0,
              .type = {0x14, 0x08},
              .catch_held = 0x1f,
              .move = {0x7e, 0x38, 0x09, 0x19},
              .orig_trainer = 0xd204,
              .exp = {0x3, 0xd, 0x40},
              .hp_ev = 0xffff,
              .atk_ev = 0xffff,
              .def_ev = 0xffff,
              .spd_ev = 0xffff,
              .special_ev = 0xffff,
              .iv = 0xffff,
              .move_pp = {0xc0, 0xc0, 0xc0, 0xc0},
              .level_again = 0x4a,
              .max_hp = 0x2c01,
              .atk = 0x9600,
              .def = 0x9700,
              .spd = 0x9800,
              .special = 0x9900},
             {.species = 0x4a,
              .hp = 0x2c01,
              .level = 0x4a,
              .status_condition = 0x0,
              .type = {0x14, 0x08},
              .catch_held = 0x1f,
              .move = {0x7e, 0x38, 0x09, 0x19},
              .orig_trainer = 0xd204,
              .exp = {0x3, 0xd, 0x40},
              .hp_ev = 0xffff,
              .atk_ev = 0xffff,
              .def_ev = 0xffff,
              .spd_ev = 0xffff,
              .special_ev = 0xffff,
              .iv = 0xffff,
              .move_pp = {0xc0, 0xc0, 0xc0, 0xc0},
              .level_again = 0x4a,
              .max_hp = 0x2c01,
              .atk = 0x9600,
              .def = 0x9700,
              .spd = 0x9800,
              .special = 0x9900},
             {.species = 0x4a,
              .hp = 0x2c01,
              .level = 0x4a,
              .status_condition = 0x0,
              .type = {0x14, 0x08},
              .catch_held = 0x1f,
              .move = {0x7e, 0x38, 0x09, 0x19},
              .orig_trainer = 0xd204,
              .exp = {0x3, 0xd, 0x40},
              .hp_ev = 0xffff,
              .atk_ev = 0xffff,
              .def_ev = 0xffff,
              .spd_ev = 0xffff,
              .special_ev = 0xffff,
              .iv = 0xffff,
              .move_pp = {0xc0, 0xc0, 0xc0, 0xc0},
              .level_again = 0x4a,
              .max_hp = 0x2c01,
              .atk = 0x9600,
              .def = 0x9700,
              .spd = 0x9800,
              .special = 0x9900},
             {.species = 0x4a,
              .hp = 0x2c01,
              .level = 0x4a,
              .status_condition = 0x0,
              .type = {0x14, 0x08},
              .catch_held = 0x1f,
              .move = {0x7e, 0x38, 0x09, 0x19},
              .orig_trainer = 0xd204,
              .exp = {0x3, 0xd, 0x40},
              .hp_ev = 0xffff,
              .atk_ev = 0xffff,
              .def_ev = 0xffff,
              .spd_ev = 0xffff,
              .special_ev = 0xffff,
              .iv = 0xffff,
              .move_pp = {0xc0, 0xc0, 0xc0, 0xc0},
              .level_again = 0x4a,
              .max_hp = 0x2c01,
              .atk = 0x9600,
              .def = 0x9700,
              .spd = 0x9800,
              .special = 0x9900},
         },
     /* NOTE: I think this shouldn't exceed 7 chars */
     .ot_name =
         {
             {.str = {F_, l_, i_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
             {.str = {F_, l_, i_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
             {.str = {F_, l_, i_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
             {.str = {F_, l_, i_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
             {.str = {F_, l_, i_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
             {.str = {F_, l_, i_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
         },
     .nickname = {
         {.str = {F_, l_, o_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
         {.str = {F_, l_, o_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
         {.str = {F_, l_, o_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
         {.str = {F_, l_, o_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
         {.str = {F_, l_, o_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
         {.str = {F_, l_, o_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
     }};

unsigned char INPUT_BLOCK[405];
unsigned char* DATA_BLOCK = (unsigned char*)&DATA_BLOCK2;

#endif /* POKEMON_DATA_H */
