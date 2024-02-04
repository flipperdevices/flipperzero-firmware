#ifndef __game_structs_h__
#define __game_structs_h__

#include <stdint.h>
#include <furi_hal.h>

enum ThreadsMessageType {
    SAVE_AND_EXIT, // Request to exit the program, save the state and quit the thread
    RESET_STATE, // Reset the persisted state and return to the main view
    PROCESS_CANDY, // User requested to give food
    PROCESS_PILL // User requested to give a pill
};

/* Request from main thread to be processed by secondary thread */
struct ThreadsMessage {
    enum ThreadsMessageType type;
    // Filled with other field in case there's the need
    // to pass an argument for a specific message type
};

enum LifeStage {
    EGG,
    BABY,
    CHILD,
    TEEN,
    ADULT,
    DEAD,
    LIFE_STAGES_NUM
};

struct PersistentGameState {
    enum LifeStage stage;
    // Feature XP
    uint32_t xp;
    uint32_t last_recorded_xp_update; // Timestamp converted from FuriHalRtcDateTime
    // Feature HU
    uint32_t hu;
    uint32_t last_recorded_hu_update; // Same as above
    // Feature HP
    uint32_t hp;
    uint32_t last_recorded_hp_update; // Same as above
};

struct PersistentSettings {
    uint8_t vibration;
    uint8_t sound;
};

/* Internal state of the game. Some of the info are persisted on
 * the storage */
struct GameState {
    struct PersistentGameState persistent;
    struct PersistentSettings settings;
    // Transient information
    uint32_t next_animation_index;
};

struct GameEvents {
    uint32_t xp; // How many new XP to assign
    uint32_t xp_timestamp; // New timestamp to use in the GameState
    int32_t hu; // How many HU to add or remove
    uint32_t hu_timestamp; // Timestamp to use in the GameState
    int32_t hp; // How many HP to add or remove
    uint32_t hp_timestamp; // Timestamp to use in the GameState
};
#endif
