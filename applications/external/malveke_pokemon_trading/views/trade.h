#ifndef TRADE_H
#define TRADE_H

#pragma once

#include <gui/view.h>
#include "../pokemon_app.h"

void* trade_alloc(
    TradeBlock* trade_block,
    const PokemonTable* table,
    ViewDispatcher* view_dispatcher,
    uint32_t view_id);

void trade_free(ViewDispatcher* view_dispatcher, uint32_t view_id, void* trade_ctx);

#endif /* TRADE_H */
