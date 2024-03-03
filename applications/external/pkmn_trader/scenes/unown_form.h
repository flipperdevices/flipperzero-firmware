#ifndef __UNOWN_FORM_H__
#define __UNOWN_FORM_H__

#pragma once

#include "../pokemon_data.h"

/* Returns ascii char, or 0 if unown is not the current pokemon */
char unown_form_get(PokemonData* pdata);

void unown_form_set(PokemonData* pdata, char letter);

#endif // __UNOWN_FORM_H__
