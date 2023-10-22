#ifndef __game_structs_h__
#define __game_structs_h__

#include <stdint.h>
#include <furi_hal.h>

enum ThreadsMessageType {
    IDLE_TIMEOUT, // No user input received, perform background work
    SAVE_AND_EXIT, // Request to exit the program, save the state and quit the thread
    BUTTON_PRESSED
};

/* Request from main thread to be processed by secondary thread */
struct ThreadsMessage {
    enum ThreadsMessageType type;
    InputKey key; // Filled if type == BUTTON_PRESSED
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

struct GameEvents {
    uint32_t unused;
};
#endif
