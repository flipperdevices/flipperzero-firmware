#ifndef __game_structs_h__
#define __game_structs_h__

#include <stdint.h>

enum GameEventType {
    IDLE_TIMEOUT, // No user input received, perform background work
    SAVE_AND_EXIT // Request to exit the program, save the state and quit the thread
};

/* Event to be processed by secondary thread to update the game state */
struct GameEvent {
    enum GameEventType type;
};

/* Which animation to display in the main screen */
enum Animation {
    INVALID, // Used to distinguish an uninitialized value
    EGG
};

/* Internal state of the game. Some of the info are persisted on
 * the storage */
struct GameState {
    // Transient information
    enum Animation animation;
    uint32_t next_animation_index;
};

#endif
