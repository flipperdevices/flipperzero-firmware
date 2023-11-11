#ifndef __game_structs_h__
#define __game_structs_h__

#include <stdint.h>
#include <furi_hal.h>

enum ThreadsMessageType {
    SAVE_AND_EXIT, // Request to exit the program, save the state and quit the thread
    RESET_STATE // Reset the persisted state and return to the main view
};

/* Request from main thread to be processed by secondary thread */
struct ThreadsMessage {
    enum ThreadsMessageType type;
    // Filled with other field in case there's the need
    // to pass an argument for a specific message type
};

enum LifeStage {
    EGG = 0,
    BABY = 1,
    CHILD = 2,
    TEEN = 3,
    ADULT = 4,
    DEAD = 5
};

struct PersistentGameState {
    enum LifeStage stage;
    // Feature XP
    uint32_t xp;
    uint32_t last_recorded_xp_update; // Timestamp converted from FuriHalRtcDateTime
};

/* Internal state of the game. Some of the info are persisted on
 * the storage */
struct GameState {
    struct PersistentGameState persistent;
    // Transient information
    uint32_t next_animation_index;
};

struct GameEvents {
    uint32_t xp; // How many new XP to assign
    uint32_t xp_timestamp; // New timestamp to use in the GameState
};
#endif
