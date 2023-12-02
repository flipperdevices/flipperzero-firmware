#ifndef TRADE_H
#define TRADE_H

#pragma once

#include <gui/view.h>
#include "../pokemon_app.h"

View* trade_alloc(PokemonFap* pokemon_fap);

void trade_free(PokemonFap* pokemon_fap);

#endif /* TRADE_H */
