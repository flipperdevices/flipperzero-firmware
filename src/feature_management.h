#ifndef __FEATURE_MANAGEMENT_H__
#define __FEATURE_MANAGEMENT_H__

#include <stdint.h>

#include "game_structs.h"

/* Every feature has to implement 3 methods:
 *
 * void init_<FEATURE>(struct GameState *game_state, uint32_t current_timestamp)
 *   Initializes the feature variables inside the @game_state.
 *   The feature may needs the last update of the params, it can use @current_timestamp
 *   as init value.
 *
 * void check_<FEATURE>(const struct GameState *game_state, uint32_t current_timestamp, struct GameEvents *game_events)
 *   Verifies if at @current_timestamp there are updates to do to the params of the feature.
 *   It should use the @game_state only as reference. It should update @game_events instead
 *   to notify eventual updates to apply.
 *
 * bool apply_<FEATURE>(struct GameState *game_state, struct GameEvents game_events)
 *   Applies the updates calculated by the previous function and passed here by @game_events
 *   to the @game_state.
 *   @return: true if changes have been made to @game_state, false otherwise
 *
 * int get_text_<FEATURE>(const struct GameState *game_state, char *str, size_t size)
 *   Write in @str the text that represent the status of this feature.
 *   @return: the number of character written excluding the trailing \0
 */

// Experience
void init_xp(struct GameState *, uint32_t);
void check_xp(const struct GameState *, uint32_t, struct GameEvents *);
bool apply_xp(struct GameState *, struct GameEvents);
int get_text_xp(const struct GameState *, char *, size_t);

#endif
