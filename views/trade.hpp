#ifndef TRADE_HPP
#define TRADE_HPP

#pragma once

#include <gui/view.h>
#include "../pokemon_app.h"

View* trade_alloc(PokemonFap* pokemon_fap);

void trade_free(PokemonFap* pokemon_fap);

#endif /* TRADE_HPP */
