#ifndef TRADE_H
#define TRADE_H

#pragma once

#include <gui/view.h>
#include "../pokemon_app.h"

void* trade_alloc(TradeBlock* trade_block, const PokemonTable* table, View* view);

void trade_free(void* trade_ctx);

#endif /* TRADE_H */
