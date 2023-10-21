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

enum LifeStage {
    EGG,
    BABY,
    CHILD,
    TEEN,
    ADULT
};

struct PersistentGameState {
    uint32_t last_recorded_event; // Timestamp converted from FuriHalRtcDateTime
    enum LifeStage stage;
};

/* Internal state of the game. Some of the info are persisted on
 * the storage */
struct GameState {
    struct PersistentGameState persistent;
    // Transient information
    uint32_t next_animation_index;
};

#endif
