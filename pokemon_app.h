#ifndef POKEMON_APP_H
#define POKEMON_APP_H

#pragma once
#include <furi.h>
#include <furi_hal_light.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/icon.h>
#include <pokemon_icons.h>

#include "views/select_pokemon.hpp"
#include "views/select_level.hpp"
#include "views/select_move1.hpp"
#include "views/select_move2.hpp"
#include "views/select_move3.hpp"
#include "views/select_move4.hpp"
#include "views/trade.hpp"

#define TAG "Pokemon"

struct pokemon_lut {
    const char* name;
    const Icon* icon;
    const uint8_t hex;
    const uint8_t type1;
    const uint8_t type2;
    const int xp_group;
    const int base_hp;
    const int base_atk;
    const int base_def;
    const int base_spd;
    const int base_special;
};

struct pokemon_mv {
    const char* name;
    const uint8_t hex;
};

typedef struct App App;
typedef enum {
    GAMEBOY_INITIAL,
    GAMEBOY_READY,
    GAMEBOY_WAITING,
    GAMEBOY_TRADE_READY,
    GAMEBOY_SEND,
    GAMEBOY_PENDING,
    GAMEBOY_TRADING
} render_gameboy_state_t;

struct App {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SelectPokemon* select_pokemon;
    SelectLevel* select_level;
    SelectMove1* select_move1;
    SelectMove2* select_move2;
    SelectMove3* select_move3;
    SelectMove4* select_move4;
    Trade* trade;
    uint32_t view_id;

    int current_pokemon = 0;
    int current_level = 3;
    int current_move = 0;
    char pokemon_hex_code = ' ';
    char move1_hex_code = ' ';
    char move2_hex_code = ' ';
    char move3_hex_code = ' ';
    char move4_hex_code = ' ';
};

typedef enum {
    AppViewSelectPokemon,
    AppViewSelectLevel,
    AppViewSelectMove1,
    AppViewSelectMove2,
    AppViewSelectMove3,
    AppViewSelectMove4,
    AppViewTrade,
    AppViewExitConfirm,
} AppView;

typedef void (*SelectPokemonCallback)(void* context, uint32_t index);
typedef struct SelectPokemonModel {
    int current_pokemon = 0;
    int current_level = 3;
    int current_move = 0;
    char pokemon_hex_code = ' ';
    char move1_hex_code = ' ';
    char move2_hex_code = ' ';
    char move3_hex_code = ' ';
    char move4_hex_code = ' ';
    bool trading = false;
    bool connected = false;
    render_gameboy_state_t gameboy_status = GAMEBOY_INITIAL;
    SelectPokemonCallback callback;
    void* callback_context;
} SelectPokemonModel;

extern struct pokemon_lut pokemon_table[];
extern struct pokemon_mv move_table[];

#endif /* POKEMON_APP_H */
