#ifndef TRADE_H
#define TRADE_H

#pragma once

#include <gui/view.h>
#include "../pokemon_app.h"

void* trade_alloc(TradeBlock* trade_block, View* view);

void trade_free(PokemonFap* pokemon_fap);

#endif /* TRADE_H */
