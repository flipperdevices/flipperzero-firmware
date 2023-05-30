#ifndef SELECCT_POKEMON_HPP
#define SELECCT_POKEMON_HPP

#pragma once
#include <furi.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>

#include <gui/elements.h>
#include <string>

#include "../pokemon_app.h"

View* select_pokemon_alloc(PokemonFap* pokemon_fap);

void select_pokemon_free(PokemonFap* pokemon_fap);

View* select_pokemon_get_view(PokemonFap* pokemon_fap);

#endif /* SELECCT_POKEMON_HPP */
