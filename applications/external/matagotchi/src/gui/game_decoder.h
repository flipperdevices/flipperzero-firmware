#ifndef __GAME_DECODER_H__
#define __GAME_DECODER_H__

#include <gui/icon.h>

#include "../game_structs.h"

/* Return the pointer to the icon to display given the
 * current life stage and the index of the animation taken
 * from the game_state object */
const Icon* decode_image(const struct GameState*);

#endif
