#ifndef SELECCT_POKEMON_HPP
#define SELECCT_POKEMON_HPP

#pragma once
#include <furi.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>

#include <gui/elements.h>
#include <string>

typedef struct App App;

typedef struct {
    View* view;
    App* app;
} SelectPokemon;

SelectPokemon* select_pokemon_alloc(App* app);

void select_pokemon_free(App* app);

View* select_pokemon_get_view(App* app);

#endif /* SELECCT_POKEMON_HPP */