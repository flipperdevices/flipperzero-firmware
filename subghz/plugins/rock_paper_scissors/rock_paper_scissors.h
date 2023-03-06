#pragma once

#include "rock_paper_scissors_icons.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <gui/gui.h>
#include <gui/icon.h>
#include <locale/locale.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <lib/subghz/subghz_tx_rx_worker.h>

// This is sent at the beginning of all RF messages. NOTE: It must end with the ':' character.
#define RPS_GAME_NAME "RPS:"
#define TAG "rock_paper_scissors_app"

// Name for "N", followed by your name without any spaces.
#define CONTACT_INFO "NYourNameHere"

// The message max length should be no larger than a value around 60 to 64.
#define MESSAGE_MAX_LEN 60

// How often to send a beacon.
#define BEACON_DURATION 3

// The major version must be a single character (it can be anything - like '1' or 'A' or 'a').
#define MAJOR_VERSION 'A'

// Temporary timings, since I don't have second Flipper & send commands via laptop.
#define DURATION_NO_MOVE_DETECTED_ERROR 60000
#define DURATION_SHOW_ERROR 3000
#define DURATION_SHOW_MOVES 500
#define DURATION_WIN_LOSS_TIE 10000

typedef enum {
    DolphinLocalLooking = 0,
    DolphinLocalReady,
    DolphinLocalCount,
    DolphinLocalRock,
    DolphinLocalPaper,
    DolphinLocalScissors,
    DolphinRemoteReady,
    DolphinRemoteCount,
    DolphinRemoteRock,
    DolphinRemotePaper,
    DolphinRemoteScissors,
} DolphinImageIndex;

static const uint32_t frequency_list[] = {
    /* 300 - 348 */
    300000000,
    303875000,
    304250000,
    310000000,
    315000000,
    318000000,

    /* 387 - 464 */
    390000000,
    418000000,
    433075000,
    433420000,
    433920000,
    434420000,
    434775000,
    438900000,

    /* 779 - 928 */
    868350000,
    915000000,
    925000000,
};

const Icon* images[] = {
    &I_Local_Looking,
    &I_Local_Ready,
    &I_Local_Count,
    &I_Local_Rock,
    &I_Local_Paper,
    &I_Local_Scissors,
    &I_Remote_Ready,
    &I_Remote_Count,
    &I_Remote_Rock,
    &I_Remote_Paper,
    &I_Remote_Scissors};

// The various moves a player can make.
// Some moves may be invalid depending on the current game state.
typedef enum {
    MoveUnknown = '-',
    MoveCount = 'C',
    MoveCount1 = '1',
    MoveCount2 = '2',
    MoveRock = 'R',
    MovePaper = 'P',
    MoveScissors = 'S',
} Move;

// The various states a player in the game can be in.
typedef enum {
    // ScreenHostGame states:
    StateHostingSetGameNumber,
    StateHostingSetFrequency,
    StateHostingBadFrequency,
    StateHostingLookingForPlayer = '*',
    // ScreenJoinGame states:
    StateJoiningSetGameNumber,
    StateJoiningSetFrequency,
    StateJoiningBadFrequency,
    // ScreenPlayingGame states:
    StateReady = 'G',
    StateCount1 = MoveCount1, // 1
    StateCount2 = MoveCount2, // 2
    StatePaper = MovePaper, // P
    StateRock = MoveRock, // R
    StateScissors = MoveScissors, // S
    StateLostRock = 'L',
    StateLostPaper = 'l',
    StateLostScissors = '-',
    StateTieRock = 'T',
    StateTiePaper = 't',
    StateTieScissors = 'x',
    StateWonRock = 'W',
    StateWonPaper = 'w',
    StateWonScissors = '+',
    // ScreenError states:
    StateError = 'E',
    StateErrorRemoteTimeout = '7', // Joined but didn't make any moves.
    StateErrorRemoteFast = '8', // Remote user sent moves after than local user.
    StateErrorLocalFast = '9', // Local user sent moves after than remote user.
    // ScrenMainMenu states:
    StateUnknown = '?',
    StateMainMenuHost,
    StateMainMenuJoin,
    StateMainMenuPastGames,
    StateMainMenuMessage,
} GameState;

typedef enum {
    ScreenMainMenu,
    ScreenHostGame,
    ScreenPlayingGame,
    ScreenError,
    ScreenJoinGame,
    ScreenEditMessage,
    ScreenPastGames,
} ScreenState;

// When an RF message is sent, it includes a purpose so the receiving application
// can decide if it should process the message.
typedef enum {
    GameRfPurposeBeacon = 'B', // Beacon.
    GameRfPurposeJoin = 'J', // Join a game.
    GameRfPurposeJoinAcknowledge = 'A', // Acknowledge a join request.
    GameRfPurposeMove = 'M', // Player move.
} GameRfPurpose;

// Messages in our event queue are one of the following types.
typedef enum {
    GameEventTypeTimer,
    GameEventTypeKey,
    GameEventDataDetected,
    GameEventRemoteBeacon,
    GameEventRemoteJoined,
    GameEventRemoteJoinAcknowledged,
    GameEventLocalMove,
    GameEventRemoteMove,
    GameEventSendMove,
    GameEventPlaySong,
} GameEventType;

// An item in the event queue has both the type and its associated data.
// Some fields may be null, they are only set for particular events.
typedef struct {
    GameEventType type; // The reason for this event.
    InputEvent input; // Key-press input events.
    Move move; // The move associated with the event.
    uint32_t tick; // The time the event originated (furi_get_tick()).
    uint16_t game_number; // The game number for the message.
    FuriString* sender_name; // If not null, be sure to release this string.
} GameEvent;

typedef struct GameInfo {
    uint16_t game_number;
    FuriString* sender_name;
    struct GameInfo* next_game;
} GameInfo;

// This is the data for our application.
typedef struct {
    FuriString* buffer;
    ScreenState screen_state;
    uint16_t game_number;
    uint8_t frequency_index;
    GameState local_player;
    GameState remote_player;
    uint32_t local_move_tick; // local & remote need to press buttons near the same time.
    uint32_t remote_move_tick;
    struct GameInfo* remote_games;
    struct GameInfo* remote_selected_game;
} GameData;

// This is our application context.
typedef struct {
    FuriMessageQueue* queue; // Message queue (GameEvent items to process).
    FuriMutex* mutex; // Used to provide thread safe access to data.
    GameData* data; // Data accessed by multiple threads (acquire the mutex before accessing!)
    SubGhzTxRxWorker* subghz_txrx;
} GameContext;

// Checks if game state is winner.
// @param state GameState to check.
// @returns true if game state is a winner.
static bool isWin(GameState state);

// Checks if game state is lost.
// @param state GameState to check.
// @returns true if game state is a loss.
static bool isLoss(GameState state);

// Checks if game state is tie.
// @param state GameState to check.
// @returns true if game state is a tie.
static bool isTie(GameState state);

// Checks if game state is result (win/loss/tie).
// @param state GameState to check.
// @returns true if game state is a win, loss or tie.
static bool isResult(GameState state);

// Checks if game state is final move (rock/paper/scissors).
// @param state GameState to check.
// @returns true if game state is a rock, paper, scissors.
static bool isFinalMove(GameState state);

static bool isError(GameState state);

// When user makes a move, we briefly pulse the vibro motor.
static void single_vibro();

// Plays a note.  You must acquire the speaker before invoking.
// @frequency the frequency of the note in Hz.
// @volume the volume of the note from 0.0 to 1.0
// @durationPlay the duration of the note in ms.
// @durationPause the duration after the note to be silent.
static void
    play_note(float frequency, float volume, uint32_t durationPlay, uint32_t durationPause);

// Play a song
static void play_song(GameState state);

// We register this callback to get invoked whenever new subghz data is received.
// Queue a GameEventDataDetected message.
// @param ctx pointer to a GameContext
static void rps_worker_update_rx_event_callback(void* ctx);

// We register this callback to get invoked whenever the timer triggers.
// Queue a GameEventTypeTimer message.
// @param ctx pointer to a GameContext
static void rps_timer_callback(void* ctx);

// This gets invoked when we process a GameEventDataDetected event.
// Read the message using subghz_tx_rx_worker_read & determine if valid format.
// If valid, we queue a message for further processing.
// @param game_context pointer to a GameContext
// @param time (furi_get_tick) when event was initially made
static void rps_receive_data(GameContext* game_context, uint32_t tick);

// This gets invoked when input (button press) is detected.
// We queue a GameEventTypeKey message with the input event data.
// @param input_event event information, such as key that was pressed.
// @param ctx_q message queue.
static void rps_input_callback(InputEvent* input_event, void* ctx_q);

// Render UI when we are hosting the game.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_host_game(Canvas* canvas, void* ctx);

// Render UI when we are joining a game.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_join_game(Canvas* canvas, void* ctx);

// Render UI when we are playing the game.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_playing_game(Canvas* canvas, void* ctx);

// Render UI when we encounter an error in the game.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_error(Canvas* canvas, void* ctx);

// Render UI when we are hosting the game.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_main_menu(Canvas* canvas, void* ctx);

// We register this callback to get invoked whenever we need to render the screen.
// We render the UI on this callback thread.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_callback(Canvas* canvas, void* ctx);

// This is a helper method that broadcasts a buffer.
// If the message is too large, the message will get truncated.
// @param game_context pointer to a GameContext.
// @param buffer string to broadcast.
static void rps_broadcast(GameContext* game_context, FuriString* buffer);

// Our GameEventSendCounter handler invokes this method.
// We broadcast - "RPS:" + move"M" + version"A" + game"###" + move"R" + ":" + "YourFlip" + "\r\n"
// @param game_context pointer to a GameContext.
// @param moveToSend the move to send to the remote player.
static void rps_broadcast_move(GameContext* game_context, Move moveToSend);

// Our GameEventTypeTimer handler invokes this method.
// We broadcast - "RPS:" + beacon"B" + version"A" + game"###" + ":" + "YourFlip" + "\r\n"
// @param game_context pointer to a GameContext.
static void rps_broadcast_beacon(GameContext* game_context);

// Temporary - the KeyLeft button handler invokes this method.
// We broadcast - "RPS:" + join"J" + version"A" + game"###" + "NYourNameHere" + " :" + "YourFlip" + "\r\n"
// @param game_context pointer to a GameContext.
static void rps_broadcast_join(GameContext* game_context);

// Send message that acknowledges Flipper joining a specific game.
// We broadcast - "RPS:" + joinAck"A" + version"A" + game"###" + " :" + "YourFlip" + "\r\n"
// @param game_context pointer to a GameContext.
static void rps_broadcast_join_acknowledge(GameContext* game_context);

// Calculates the elapsed duration (in ticks) since a previous tick.
// @param tick previous tick obtained from furi_get_tick().
static uint32_t duration(uint32_t tick);

// Updates the state machine, if needed.
// @param game_context pointer to a GameContext.
static void rps_state_machine_update(GameContext* game_context);

// Update the state machine to reflect that a remote user joined the game.
// @param game_context pointer to a GameContext.
static void rps_state_machine_remote_joined(GameContext* game_context);

// Update the state machine to reflect the local user's move.
// @param game_context pointer to a GameContext.
// @param move local user move.
static bool rps_state_machine_local_moved(GameContext* game_context, Move move);

// Update the state machine to reflect the remote user's move.
// @param game_context pointer to a GameContext.
// @param move remote user move.
static bool rps_state_machine_remote_moved(GameContext* game_context, Move move);

static bool update_frequency(GameContext* game_context);

static void remote_games_clear(GameContext* game_context);
static GameInfo* remote_games_current(GameContext* game_context);
static GameInfo* remote_games_find(GameContext* game_context, uint16_t game_number);
static bool remote_games_has_next(GameContext* game_context);
static bool remote_games_has_previous(GameContext* game_context);
static void remote_games_next(GameContext* game_context);
static void remote_games_previous(GameContext* game_context);
static void remote_games_add(GameContext* game_context, GameEvent* game_event);

// This is the entry point for our application, which should match the application.fam file.
int32_t rock_paper_scissors_app(void* p);