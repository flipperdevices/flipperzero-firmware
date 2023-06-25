#ifndef SELECCT_POKEMON_HPP
#define SELECCT_POKEMON_HPP

#pragma once

#include <gui/view.h>
#include "../pokemon_app.h"

View* select_pokemon_alloc(PokemonFap* pokemon_fap);

void select_pokemon_free(PokemonFap* pokemon_fap);

#endif /* SELECCT_POKEMON_HPP */
