#ifndef POKEMON_APP_H
#define POKEMON_APP_H

#pragma once
#include <furi.h>
#include <furi_hal_light.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/icon.h>
#include "pokemon_icons.h"

#include "views/select_pokemon.hpp"
#include "views/trade.hpp"

#define TAG "Pokemon"

struct pokemon_lut {
    const char* name;
    const Icon* icon;
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
    Trade* trade;
    uint32_t view_id;

    int current_pokemon = 0;
    char pokemon_hex_code = ' ';
};

typedef enum {
    AppViewSelectPokemon,
    AppViewTrade,
    AppViewExitConfirm,
} AppView;

typedef void (*SelectPokemonCallback)(void* context, uint32_t index);
typedef struct SelectPokemonModel {
    int current_pokemon = 0;
    char pokemon_hex_code = ' ';
    bool trading = false;
    bool connected = false;
    render_gameboy_state_t gameboy_status = GAMEBOY_INITIAL;
    SelectPokemonCallback callback;
    void* callback_context;
} SelectPokemonModel;

extern struct pokemon_lut pokemon_table[];

#endif /* POKEMON_APP_H */
