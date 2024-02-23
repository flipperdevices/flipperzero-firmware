#ifndef __FEATURE_MANAGEMENT_H__
#define __FEATURE_MANAGEMENT_H__

#include <stdint.h>

#include "game_structs.h"

/* Every feature has to implement a function returning the following Feat struct.
 * After defining it, you need to call that function to initialize a corresponding entry
 * in the features array in state_management.c, within the init_state function.
 *
 * The purpose of the methods of that struct are described here:
 *
 * void init(struct GameState *game_state, uint32_t current_timestamp)
 *   Initializes the feature variables inside the @game_state.
 *   The feature may needs the last update of the params, it can use @current_timestamp
 *   as init value.
 *
 * void check(const struct GameState *game_state, uint32_t current_timestamp, struct GameEvents *game_events)
 *   Verifies if at @current_timestamp there are updates to do to the params of the feature.
 *   It should use the @game_state only as reference. It should update @game_events instead
 *   to notify eventual updates to apply.
 *
 * bool apply(struct GameState *game_state, struct GameEvents game_events)
 *   Applies the updates calculated by the previous function and passed here by @game_events
 *   to the @game_state.
 *   @return: true if changes have been made to @game_state, false otherwise
 *
 * int get_text(const struct GameState *game_state, char *str, size_t size)
 *   Write in @str the text that represent the status of this feature.
 *   @return: the number of character written excluding the trailing \0
 */

struct Feat {
    void (*init)(struct GameState*, uint32_t);
    void (*check)(const struct GameState*, uint32_t, struct GameEvents*);
    bool (*apply)(struct GameState*, struct GameEvents);
    int (*get_text)(const struct GameState*, char*, size_t);
};

/* correct_state
 * It is use to verify that the updates
 * done by the individual features doesn't
 * corrupt the other features. */
void correct_state(struct GameState*);

/* Special actions */

/* generate_hu is used to restore hunger.
 * Takes in input the @game_state, the @current_timestamp and
 * the @game_events struct where to put the hu that are restored. */
void generate_hu(struct GameState*, uint32_t, struct GameEvents*);

/* generate_hp is used to restore health.
 * Takes in input the @game_state, the @current_timestamp and
 * the @game_events struct where to put the hp that are restored. */
void generate_hp(struct GameState*, uint32_t, struct GameEvents*);

#endif
