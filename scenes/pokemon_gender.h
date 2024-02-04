#ifndef POKEMON_GENDER_H
#define POKEMON_GENDER_H

#pragma once

/* The gender ratio is a bit value, and if the ATK_IV is less than or equal to
 * the gender ratio, the gender is female.
 *
 * A ratio of 0xff means gender is unknown.
 * A ratio of 0x00 is annoyingly special. It either means that pokemon can be
 *   male only -OR- there is a very small chance the pokemon is female. The
 *   male only pokemon need to be specifically checked for.
 */

/* This will return a pointer to a string of the pokemon's current gender */
char *select_gender_get_gender(PokemonData* pdata);

void select_gender_scene_on_enter(void* context);

#endif // POKEMON_GENDER_H
