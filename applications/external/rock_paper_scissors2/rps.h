#ifndef RPS_H_
#define RPS_H_

typedef enum HandType {
    HandTypeRock,
    HandTypePaper,
    HandTypeScissor,
    HandTypeEmpty,
} HandType_t;

typedef enum GameState {
    GameStateEnded,
    GameStateUserSelect,
    GameStateSystemRoll,
    GameStateNone,
} GameState_t;

typedef enum HandBoxType {
    HandBoxTypeNormal,
    HandBoxTypeSelected,
    HandBoxTypeFilled,
} HandBoxType_t;

typedef enum GameResult {
    GameResultWon,
    GameResultLost,
    GameResultDraw,
    GameResultUnknown,
} GameResult_t;

typedef struct RPSApp {
    // Flipper Firmware Variables
    FuriMessageQueue* event_queue;
    ViewPort* view_port;
    Gui* gui;
    FuriMutex** mutex;
    FuriTimer* roll_timer;
    FuriTimer* roll_complete_timer;

    // App Variables
    HandType_t user_hand;
    HandType_t system_hand;
    GameState_t game_state;
    HandBoxType_t system_hand_box;
    HandBoxType_t user_hand_box;
    char* click_message;
    char* middle_message;
    uint16_t system_score;
    uint16_t user_score;
} RPSApp_t;

#endif // RPS_H_
