/*
@CodeAllNight
https://github.com/jamisonderek/flipper-zero-tutorials

This is a two person game of Rock, Paper, Scissors.  It uses the subghz_tx_rx worker library to 
send and receive messages.

Features:
OK - 1
OK - 2
UP - Rock
Right - Paper
Down - Scissors

*/

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
    StateLookingForPlayer = '*',
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
    StateErrorRemoteTimeout = '7', // Joined but didn't make any moves.
    StateErrorRemoteFast = '8', // Remote user sent moves after than local user.
    StateErrorLocalFast = '9', // Local user sent moves after than remote user.
    StateError = 'E',
} GameState;

// When an RF message is sent, it includes a purpose so the receiving application
// can decide if it should process the message.
typedef enum {
    GameRfPurposeBeacon = 'B', // Beacon.
    GameRfPurposeJoin = 'J', // Join a game.
    GameRfPurposeMove = 'M', // Player move.
} GameRfPurpose;

// Messages in our event queue are one of the following types.
typedef enum {
    GameEventTypeTimer,
    GameEventTypeKey,
    GameEventDataDetected,
    GameEventRemoteBeacon,
    GameEventRemoteJoined,
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
    unsigned int gameNumber; // The game number for the message.
    FuriString* senderName; // If not null, be sure to release this string.
} GameEvent;

// This is the data for our application.
typedef struct {
    FuriString* buffer;
    unsigned int gameNumber;
    GameState localPlayer;
    GameState remotePlayer;
    uint32_t localMoveTick; // local & remote need to press buttons near the same time.
    uint32_t remoteMoveTick;
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
static bool isWin(GameState state) {
    return (StateWonPaper == state) || (StateWonRock == state) || (StateWonScissors == state);
}

// Checks if game state is lost.
// @param state GameState to check.
// @returns true if game state is a loss.
static bool isLoss(GameState state) {
    return (StateLostPaper == state) || (StateLostRock == state) || (StateLostScissors == state);
}

// Checks if game state is tie.
// @param state GameState to check.
// @returns true if game state is a tie.
static bool isTie(GameState state) {
    return (StateTiePaper == state) || (StateTieRock == state) || (StateTieScissors == state);
}

// When user makes a move, we briefly pulse the vibro motor.
static void single_vibro() {
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_single_vibro);
    furi_record_close(RECORD_NOTIFICATION);
}

// Plays a note.  You must acquire the speaker before invoking.
// @frequency the frequency of the note in Hz.
// @volume the volume of the note from 0.0 to 1.0
// @durationPlay the duration of the note in ms.
// @durationPause the duration after the note to be silent.
static void
    play_note(float frequency, float volume, uint32_t durationPlay, uint32_t durationPause) {
    furi_hal_speaker_start(frequency, volume);
    uint32_t n = furi_get_tick();
    while(furi_get_tick() < n + durationPlay) {
        furi_thread_yield();
    }
    furi_hal_speaker_stop();
    n = furi_get_tick();
    while(furi_get_tick() < n + durationPause) {
        furi_thread_yield();
    }
}

// Play a song
static void play_song(GameState state) {
    if(furi_hal_speaker_acquire(1000)) {
        const float volume = 1.0f;
        const uint32_t playQtr = 500;
        const uint32_t delayQtr = 100;

        if(isWin(state)) {
            play_note(523.25f, volume, playQtr, delayQtr);
            play_note(659.25f, volume, playQtr, delayQtr);
            play_note(783.99f, volume, playQtr, delayQtr);
        } else if(isLoss(state)) {
            play_note(783.99f, volume, playQtr * 2, delayQtr);
            play_note(523.25f, volume, playQtr, delayQtr);
        } else if(isTie(state)) {
            play_note(783.99f, volume, playQtr, delayQtr);
            play_note(523.25f, volume, playQtr, delayQtr);
            play_note(783.99f, volume, playQtr, delayQtr);
        }

        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}

// We register this callback to get invoked whenever new subghz data is received.
// Queue a GameEventDataDetected message.
// @param ctx pointer to a GameContext
static void rps_worker_update_rx_event_callback(void* ctx) {
    furi_assert(ctx);
    GameContext* game_context = ctx;
    GameEvent event = {.type = GameEventDataDetected, .tick = furi_get_tick()};
    furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
}

//
// We register this callback to get invoked whenever the timer triggers.
// Queue a GameEventTypeTimer message.
// @param ctx pointer to a GameContext
static void rps_timer_callback(void* ctx) {
    furi_assert(ctx);
    GameContext* game_context = ctx;
    GameEvent event = {.type = GameEventTypeTimer};
    furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
}

// This gets invoked when we process a GameEventDataDetected event.
// Read the message using subghz_tx_rx_worker_read & determine if valid format.
// If valid, we queue a message for further processing.
// @param game_context pointer to a GameContext
// @param time (furi_get_tick) when event was initially made
static void rps_receive_data(GameContext* game_context, uint32_t tick) {
    uint8_t message[MESSAGE_MAX_LEN] = {0};
    memset(message, 0x00, MESSAGE_MAX_LEN);
    size_t len = subghz_tx_rx_worker_read(game_context->subghz_txrx, message, MESSAGE_MAX_LEN);
    size_t game_name_len = strlen(RPS_GAME_NAME);
    if(len < (game_name_len + 2)) {
        FURI_LOG_D(TAG, "Message not long enough. >%s<", message);
        return;
    }

    // The message for a move (M) (like 'R' for Rock) using version (A) should be "RPS:" + "M" + "A" + game"###" + move"R" + ":" + "YourFlip" + "\r\n"
    if(strcmp(RPS_GAME_NAME, (const char*)message)) {
        FURI_LOG_D(TAG, "Got message >%s<", message);

        // The purpose immediately follows the game name.
        GameRfPurpose purpose = message[game_name_len];
        // The version follows the purpose.
        uint8_t version = message[game_name_len + 1];
        FURI_LOG_T(TAG, "Purpose is %c and version is %c", purpose, version);

        // Null terminate buffer at the end of message so we can't overrun the buffer.
        message[MESSAGE_MAX_LEN - 1] = 0;

        unsigned int gameNumber;
        char randomInfo[MESSAGE_MAX_LEN];
        char senderName[9];
        char tmp;
        Move move = MoveUnknown;
        switch(purpose) {
        case GameRfPurposeMove:
            // We expect this mesage to the game number, move and sender name.
            if(sscanf(
                   (const char*)message + game_name_len + 2,
                   "%03u%c:%8s",
                   &gameNumber,
                   &tmp,
                   senderName) == 3) {
                move = (Move)tmp;
                // IMPORTANT: The code processing the event needs to furi_string_free the senderName!
                FuriString* name = furi_string_alloc();
                furi_string_set(name, senderName);

                GameEvent event = {
                    .type = GameEventRemoteMove,
                    .move = move,
                    .tick = tick,
                    .senderName = name,
                    .gameNumber = gameNumber};
                furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
            } else {
                FURI_LOG_W(TAG, "Failed to parse move message. >%s<", message);
            }
            break;

        case GameRfPurposeBeacon:
            // We expect this mesage to the game number, move and sender name.
            if(sscanf(
                   (const char*)message + game_name_len + 2, "%03u:%8s", &gameNumber, senderName) ==
               2) {
                // IMPORTANT: The code processing the event needs to furi_string_free the senderName!
                FuriString* name = furi_string_alloc();
                furi_string_set(name, senderName);

                GameEvent event = {
                    .type = GameEventRemoteBeacon, .senderName = name, .gameNumber = gameNumber};
                furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
            } else {
                FURI_LOG_W(TAG, "Failed to parse beacon message. >%s<", message);
            }
            break;

        case GameRfPurposeJoin:
            // We expect this mesage to the game number, move and sender name.
            if(sscanf(
                   (const char*)message + game_name_len + 2,
                   "%03u%s :%8s",
                   &gameNumber,
                   randomInfo,
                   senderName) == 3) {
                FURI_LOG_T(TAG, "Join had randomInfo of >%s<", randomInfo);

                // IMPORTANT: The code processing the event needs to furi_string_free the senderName!
                FuriString* name = furi_string_alloc();
                furi_string_set(name, senderName);

                GameEvent event = {
                    .type = GameEventRemoteJoined, .senderName = name, .gameNumber = gameNumber};
                furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
            } else {
                FURI_LOG_W(TAG, "Failed to parse join message. >%s<", message);
            }
            break;

        default:
            if(version <= MAJOR_VERSION) {
                // The version is same or less than ours, so we should know about the message purpose.
                FURI_LOG_E(TAG, "Message purpose not handled for known version. >%s<", message);
            } else {
                // The version is newer, so it's not surprising we don't know about the purpose.
                FURI_LOG_T(TAG, "Message purpose not handled. >%s<", message);
            }
            break;
        }
    } else {
        FURI_LOG_D(TAG, "Message not for our application. >%s<", message);
    }
}

// This gets invoked when input (button press) is detected.
// We queue a GameEventTypeKey message with the input event data.
// @param input_event event information, such as key that was pressed.
// @param ctx_q message queue.
static void rps_input_callback(InputEvent* input_event, void* ctx_q) {
    furi_assert(ctx_q);
    FuriMessageQueue* queue = ctx_q;
    GameEvent event = {.type = GameEventTypeKey, .tick = furi_get_tick(), .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

// We register this callback to get invoked whenever we need to render the screen.
// We render the UI on this callback thread.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    GameContext* game_context = ctx;

    // Attempt to aquire context, so we can read the data.
    if(furi_mutex_acquire(game_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    GameData* data = game_context->data;
    GameState localPlayer = data->localPlayer;
    GameState remotePlayer = data->remotePlayer;

    canvas_set_font(canvas, FontSecondary);

    switch(remotePlayer) {
    case StateReady:
        canvas_draw_icon(canvas, 64, 0, images[DolphinRemoteReady]);
        break;

    case StateCount1:
        canvas_draw_icon(canvas, 64, 0, images[DolphinRemoteCount]);
        canvas_draw_str_aligned(canvas, 70, 15, AlignLeft, AlignTop, "1");
        break;

    case StateCount2:
        canvas_draw_icon(canvas, 64, 0, images[DolphinRemoteCount]);
        canvas_draw_str_aligned(canvas, 70, 15, AlignLeft, AlignTop, "2");
        break;

    case StateRock:
    case StateTieRock:
    case StateWonRock:
    case StateLostRock:
        if(StateCount2 != localPlayer) {
            canvas_draw_icon(canvas, 64, 0, images[DolphinRemoteRock]);
            canvas_draw_str_aligned(canvas, 70, 55, AlignLeft, AlignTop, "Rock");
        } else {
            canvas_draw_icon(canvas, 64, 0, images[DolphinRemoteCount]);
        }
        break;

    case StatePaper:
    case StateTiePaper:
    case StateWonPaper:
    case StateLostPaper:
        if(StateCount2 != localPlayer) {
            canvas_draw_icon(canvas, 64, 0, images[DolphinRemotePaper]);
            canvas_draw_str_aligned(canvas, 70, 55, AlignLeft, AlignTop, "Paper");
        } else {
            canvas_draw_icon(canvas, 64, 0, images[DolphinRemoteCount]);
        }
        break;

    case StateScissors:
    case StateTieScissors:
    case StateWonScissors:
    case StateLostScissors:
        if(StateCount2 != localPlayer) {
            canvas_draw_icon(canvas, 64, 0, images[DolphinRemoteScissors]);
            canvas_draw_str_aligned(canvas, 70, 55, AlignLeft, AlignTop, "Scissors");
        } else {
            canvas_draw_icon(canvas, 64, 0, images[DolphinRemoteCount]);
        }
        break;

    default:
        break;
    }

    switch(localPlayer) {
    case StateLookingForPlayer:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalLooking]);
        furi_string_printf(data->buffer, "Waiting for player, game %03d.", data->gameNumber);
        canvas_draw_str_aligned(
            canvas, 0, 55, AlignLeft, AlignTop, furi_string_get_cstr(data->buffer));
        break;

    case StateReady:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalReady]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Press OK    for 1.");
        break;

    case StateCount1:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalCount]);
        canvas_draw_str_aligned(canvas, 50, 15, AlignLeft, AlignTop, "1");
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Press OK for 2.");
        break;

    case StateCount2:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalCount]);
        canvas_draw_str_aligned(canvas, 50, 15, AlignLeft, AlignTop, "2");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "^Rock >Paper vScissor");
        break;

    case StateRock:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalRock]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Rock");
        break;

    case StatePaper:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalPaper]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Paper");
        break;

    case StateScissors:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalScissors]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Scissors");
        break;

    case StateWonRock:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalRock]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Rock");
        canvas_draw_str_aligned(canvas, 38, 5, AlignLeft, AlignTop, "You won!!!");
        break;
    case StateWonPaper:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalPaper]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Paper");
        canvas_draw_str_aligned(canvas, 38, 5, AlignLeft, AlignTop, "You won!!!");
        break;
    case StateWonScissors:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalScissors]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Scissors");
        canvas_draw_str_aligned(canvas, 38, 5, AlignLeft, AlignTop, "You won!!!");
        break;

    case StateTieRock:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalRock]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Rock");
        canvas_draw_str_aligned(canvas, 38, 5, AlignLeft, AlignTop, "You tied!");
        break;
    case StateTiePaper:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalPaper]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Paper");
        canvas_draw_str_aligned(canvas, 38, 5, AlignLeft, AlignTop, "You tied!");
        break;
    case StateTieScissors:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalScissors]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Scissors");
        canvas_draw_str_aligned(canvas, 38, 5, AlignLeft, AlignTop, "You tied!");
        break;

    case StateLostRock:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalRock]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Rock");
        canvas_draw_str_aligned(canvas, 38, 5, AlignLeft, AlignTop, "You lost.");
        break;
    case StateLostPaper:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalPaper]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Paper");
        canvas_draw_str_aligned(canvas, 38, 5, AlignLeft, AlignTop, "You lost.");
        break;
    case StateLostScissors:
        canvas_draw_icon(canvas, 0, 0, images[DolphinLocalScissors]);
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Scissors");
        canvas_draw_str_aligned(canvas, 38, 5, AlignLeft, AlignTop, "You lost.");
        break;

    case StateError:
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Unknown error");
        break;

    case StateErrorLocalFast:
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Too fast!");
        break;

    case StateErrorRemoteFast:
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Remote too fast!");
        break;

    case StateErrorRemoteTimeout:
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Remote timeout.");
        break;

    default:
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Unexpected.");
        break;
    }

    furi_mutex_release(game_context->mutex);
}

// This is a helper method that broadcasts a buffer.
// If the message is too large, the message will get truncated.
// @param game_context pointer to a GameContext.
// @param buffer string to broadcast.
static void rps_broadcast(GameContext* game_context, FuriString* buffer) {
    uint8_t* message = (uint8_t*)furi_string_get_cstr(buffer);
    FURI_LOG_I(TAG, "Broadcast message >%s<", message);

    // Make sure our message will fit into a packet; if not truncate it.
    size_t length = strlen((char*)message);
    if(length > MESSAGE_MAX_LEN) {
        // SECURITY REVIEW - Is it okay to log, or do we need to truncate first?
        FURI_LOG_E(
            TAG, "Outgoing message bigger than %d bytes! >%s<", MESSAGE_MAX_LEN, (char*)message);

        // Add \r\n(null) to the end of the 0-indexed string.
        message[MESSAGE_MAX_LEN - 1] = 0;
        message[MESSAGE_MAX_LEN - 2] = '\n';
        message[MESSAGE_MAX_LEN - 3] = '\r';
        length = MESSAGE_MAX_LEN;
    }

    while(!subghz_tx_rx_worker_write(game_context->subghz_txrx, message, length)) {
        // Wait a few milliseconds on failure before trying to send again.
        furi_delay_ms(20);
    }
}

// Our GameEventSendCounter handler invokes this method.
// We broadcast - "RPS:" + move"M" + version"A" + game"###" + move"R" + ":" + "YourFlip" + "\r\n"
// @param game_context pointer to a GameContext.
// @param moveToSend the move to send to the remote player.
static void rps_broadcast_move(GameContext* game_context, Move moveToSend) {
    GameData* data = game_context->data;
    FURI_LOG_I(TAG, "Sending move %c", moveToSend);

    // The message for game 42 with a move with value Rock should look like...  "RPS:MA042R:YourFlip\r\n"
    furi_string_printf(
        data->buffer,
        "%s%c%c%03u%c:%s\r\n",
        RPS_GAME_NAME,
        GameRfPurposeMove,
        MAJOR_VERSION,
        data->gameNumber,
        moveToSend,
        furi_hal_version_get_name_ptr());
    rps_broadcast(game_context, data->buffer);
}

// Our GameEventTypeTimer handler invokes this method.
// We broadcast - "RPS:" + beacon"B" + version"A" + game"###" + ":" + "YourFlip" + "\r\n"
// @param game_context pointer to a GameContext.
static void rps_broadcast_beacon(GameContext* game_context) {
    GameData* data = game_context->data;
    FURI_LOG_I(TAG, "Sending beacon");

    // The message for game 42 should look like...  "RPS:BA042:YourFlip\r\n"
    furi_string_printf(
        data->buffer,
        "%s%c%c%03u:%s\r\n",
        RPS_GAME_NAME,
        GameRfPurposeBeacon,
        MAJOR_VERSION,
        data->gameNumber,
        furi_hal_version_get_name_ptr());
    rps_broadcast(game_context, data->buffer);
}

// Temporary - the KeyLeft button handler invokes this method.
// We broadcast - "RPS:" + join"J" + version"A" + game"###" + "NYourNameHere" + " :" + "YourFlip" + "\r\n"
// @param game_context pointer to a GameContext.
// @param gameNumber the game to join (from previous beacon).
static void rps_broadcast_join(GameContext* game_context, unsigned int gameNumber) {
    GameData* data = game_context->data;
    FURI_LOG_I(TAG, "Joining game %d.", gameNumber);

    // The message for game 42 should look like...  "RPS:JA042NYourNameHere :YourFlip\r\n"
    furi_string_printf(
        data->buffer,
        "%s%c%c%03u%s :%s\r\n",
        RPS_GAME_NAME,
        GameRfPurposeJoin,
        MAJOR_VERSION,
        data->gameNumber,
        CONTACT_INFO,
        furi_hal_version_get_name_ptr());
    rps_broadcast(game_context, data->buffer);
}

// Calculates the elapsed duration (in ticks) since a previous tick.
// @param tick previous tick obtained from furi_get_tick().
static uint32_t duration(uint32_t tick) {
    uint32_t current = furi_get_tick();
    // Every 55 days the tick could wrap.
    if(current < tick) {
        FURI_LOG_T(TAG, "tick count wrapped!  current:%ld prev:%ld", current, tick);
        return current + (UINT32_MAX - tick);
    }

    return current - tick;
}

// Checks if game state is result (win/loss/tie).
// @param state GameState to check.
// @returns true if game state is a win, loss or tie.
static bool isResult(GameState state) {
    return isWin(state) || isLoss(state) || isTie(state);
}

// Checks if game state is final move (rock/paper/scissors).
// @param state GameState to check.
// @returns true if game state is a rock, paper, scissors.
static bool isFinalMove(GameState state) {
    return (StateRock == state) || (StatePaper == state) || (StateScissors == state);
}

static bool isError(GameState state) {
    return (StateError == state) || (StateErrorLocalFast == state) ||
           (StateErrorRemoteFast == state) || (StateErrorRemoteTimeout == state);
}

// Temporary timings, since I don't have second Flipper & send commands via laptop.
#define DURATION_NO_MOVE_DETECTED_ERROR 60000
#define DURATION_SHOW_ERROR 3000
#define DURATION_SHOW_MOVES 500
#define DURATION_WIN_LOSS_TIE 10000

// Updates the state machine, if needed.
// @param game_context pointer to a GameContext.
static void rps_state_machine_update(GameContext* game_context) {
    GameData* d = game_context->data;
    FURI_LOG_I(TAG, "Validating game state. local:%c Remote:%c", d->localPlayer, d->remotePlayer);

    // Did player leave after joining?
    if((StateReady == d->remotePlayer) &&
       (duration(d->remoteMoveTick) > DURATION_NO_MOVE_DETECTED_ERROR)) {
        d->remotePlayer = StateLookingForPlayer;
        d->remoteMoveTick = furi_get_tick();
        d->localPlayer = StateErrorRemoteTimeout;
        d->localMoveTick = furi_get_tick();
        // Should we tell other player we timed out?
        FURI_LOG_I(TAG, "Timed out after joining.");
        return;
    }

    // TEMP - After Error, we reset back to Looking for player.
    if(isError(d->localPlayer) && (duration(d->localMoveTick) > DURATION_SHOW_ERROR)) {
        d->remotePlayer = StateLookingForPlayer;
        d->remoteMoveTick = furi_get_tick();
        d->localPlayer = StateLookingForPlayer;
        d->localMoveTick = furi_get_tick();
        FURI_LOG_I(TAG, "Reset from Error to Looking for player.");
        return;
    }

    // TEMP - After Win, Loss, Tie -  we reset back to Ready.
    if(isResult(d->localPlayer) && (duration(d->localMoveTick) > DURATION_WIN_LOSS_TIE)) {
        d->remotePlayer = StateReady;
        d->remoteMoveTick = furi_get_tick();
        d->localPlayer = StateReady;
        d->localMoveTick = furi_get_tick();
        // Should we tell other player we are Ready?
        FURI_LOG_I(TAG, "Ready for next game.");
        return;
    }

    // Check for winner.
    if(isFinalMove(d->localPlayer) && isFinalMove(d->remotePlayer) &&
       (duration(d->localMoveTick) > DURATION_SHOW_MOVES)) {
        d->localMoveTick = furi_get_tick();
        d->remoteMoveTick = furi_get_tick();
        if((d->localPlayer == StateRock) && (d->remotePlayer == StateScissors)) {
            d->localPlayer = StateWonRock;
            d->remotePlayer = StateLostScissors;
            FURI_LOG_I(TAG, "Local won w/Rock.");
        } else if((d->localPlayer == StateScissors) && (d->remotePlayer == StatePaper)) {
            d->localPlayer = StateWonScissors;
            d->remotePlayer = StateLostPaper;
            FURI_LOG_I(TAG, "Local won w/Scissors.");
        } else if((d->localPlayer == StatePaper) && (d->remotePlayer == StateRock)) {
            d->localPlayer = StateWonPaper;
            d->remotePlayer = StateLostRock;
            FURI_LOG_I(TAG, "Local won w/Paper.");
        } else if((d->localPlayer == StateRock) && (d->remotePlayer == StatePaper)) {
            d->localPlayer = StateLostRock;
            d->remotePlayer = StateWonPaper;
            FURI_LOG_I(TAG, "Remote won w/Paper.");
        } else if((d->localPlayer == StateScissors) && (d->remotePlayer == StateRock)) {
            d->localPlayer = StateLostScissors;
            d->remotePlayer = StateWonRock;
            FURI_LOG_I(TAG, "Remote won w/Rock.");
        } else if((d->localPlayer == StatePaper) && (d->remotePlayer == StateScissors)) {
            d->localPlayer = StateLostPaper;
            d->remotePlayer = StateWonScissors;
            FURI_LOG_I(TAG, "Remote won w/Scissors.");
        } else {
            FURI_LOG_I(TAG, "Tie game.");
            if(d->localPlayer == StateRock) {
                d->localPlayer = StateTieRock;
                d->remotePlayer = StateTieRock;
            } else if(d->localPlayer == StatePaper) {
                d->localPlayer = StateTiePaper;
                d->remotePlayer = StateTiePaper;
            } else {
                d->localPlayer = StateTieScissors;
                d->remotePlayer = StateTieScissors;
            }
        }

        GameEvent event = {.type = GameEventPlaySong};
        furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
    }
}

// Update the state machine to reflect that a remote user joined the game.
// @param game_context pointer to a GameContext.
static void rps_state_machine_remote_joined(GameContext* game_context) {
    if(StateLookingForPlayer == game_context->data->localPlayer) {
        FURI_LOG_I(TAG, "Remote player joined our game!");
        game_context->data->remotePlayer = StateReady;
        game_context->data->remoteMoveTick = furi_get_tick();
        game_context->data->localPlayer = StateReady;
        game_context->data->localMoveTick = furi_get_tick();
    } else {
        FURI_LOG_I(
            TAG, "Remote requested join, but we are state %c!", game_context->data->localPlayer);
    }
}

// Update the state machine to reflect the local user's move.
// @param game_context pointer to a GameContext.
// @param move local user move.
static bool rps_state_machine_local_moved(GameContext* game_context, Move move) {
    FURI_LOG_I(TAG, "Local move %c.", move);

    Move localMove = MoveUnknown;
    GameState localState = StateReady;

    if(MoveCount == move && StateReady == game_context->data->localPlayer) {
        localMove = MoveCount1;
        localState = StateCount1;
    } else if(MoveCount == move && StateCount1 == game_context->data->localPlayer) {
        if((StateCount1 == game_context->data->remotePlayer) ||
           (StateCount2 == game_context->data->remotePlayer)) {
            localMove = MoveCount2;
            localState = StateCount2;
        } else {
            localState = StateErrorLocalFast;
            FURI_LOG_I(
                TAG, "Local count sync error. remote is %c.", game_context->data->remotePlayer);
        }
    } else if(StateCount2 == game_context->data->localPlayer) {
        if(MoveRock == move) {
            if((StateCount2 == game_context->data->remotePlayer) ||
               isFinalMove(game_context->data->remotePlayer)) {
                localMove = MoveRock;
                localState = StateRock;
            } else {
                localState = StateErrorLocalFast;
                FURI_LOG_I(
                    TAG, "Local rock sync error. remote is %c.", game_context->data->remotePlayer);
            }
        } else if(MovePaper == move) {
            if((StateCount2 == game_context->data->remotePlayer) ||
               isFinalMove(game_context->data->remotePlayer)) {
                localMove = MovePaper;
                localState = StatePaper;
            } else {
                localState = StateErrorLocalFast;
                FURI_LOG_I(
                    TAG,
                    "Local paper sync error. remote is %c.",
                    game_context->data->remotePlayer);
            }
        } else if(MoveScissors == move) {
            if((StateCount2 == game_context->data->remotePlayer) ||
               isFinalMove(game_context->data->remotePlayer)) {
                localMove = MoveScissors;
                localState = StateScissors;
            } else {
                localState = StateErrorLocalFast;
                FURI_LOG_I(
                    TAG,
                    "Local scissors sync error. remote is %c.",
                    game_context->data->remotePlayer);
            }
        } else {
            FURI_LOG_E(
                TAG,
                "Invalid Local move '%c' error. lState=%c. rState=%c.",
                move,
                game_context->data->localPlayer,
                game_context->data->remotePlayer);
        }
    } else {
        FURI_LOG_E(
            TAG,
            "Invalid Local move '%c' error. lState=%c. rState=%c.",
            move,
            game_context->data->localPlayer,
            game_context->data->remotePlayer);
    }

    if(MoveUnknown != localMove) {
        single_vibro();
        rps_broadcast_move(game_context, localMove);
    }

    if(StateReady != localState) {
        game_context->data->localPlayer = localState;
        game_context->data->localMoveTick = furi_get_tick();
    }

    return StateReady != localState;
}

// Update the state machine to reflect the remote user's move.
// @param game_context pointer to a GameContext.
// @param move remote user move.
static bool rps_state_machine_remote_moved(GameContext* game_context, Move move) {
    GameState remoteState = StateReady;
    FURI_LOG_I(TAG, "Remote move %c.", move);

    if(MoveCount1 == move && StateReady == game_context->data->remotePlayer) {
        remoteState = StateCount1;
    } else if(MoveCount2 == move && StateCount1 == game_context->data->remotePlayer) {
        if((StateCount1 == game_context->data->localPlayer) ||
           (StateCount2 == game_context->data->localPlayer)) {
            remoteState = StateCount2;
        } else {
            remoteState = StateErrorRemoteFast;
            FURI_LOG_I(
                TAG, "Remote count sync error. local is %c.", game_context->data->localPlayer);
        }
    } else if(MoveRock == move && StateCount2 == game_context->data->remotePlayer) {
        if((StateCount2 == game_context->data->localPlayer) ||
           isFinalMove(game_context->data->localPlayer)) {
            remoteState = StateRock;
        } else {
            remoteState = StateErrorRemoteFast;
            FURI_LOG_I(
                TAG, "Remote rock sync error. local is %c.", game_context->data->localPlayer);
        }
    } else if(MovePaper == move && StateCount2 == game_context->data->remotePlayer) {
        if((StateCount2 == game_context->data->localPlayer) ||
           isFinalMove(game_context->data->localPlayer)) {
            remoteState = StatePaper;
        } else {
            remoteState = StateErrorRemoteFast;
            FURI_LOG_I(
                TAG, "Remote paper sync error. local is %c.", game_context->data->localPlayer);
        }
    } else if(MoveScissors == move && StateCount2 == game_context->data->remotePlayer) {
        if((StateCount2 == game_context->data->localPlayer) ||
           isFinalMove(game_context->data->localPlayer)) {
            remoteState = StateScissors;
        } else {
            remoteState = StateErrorRemoteFast;
            FURI_LOG_I(
                TAG, "Remote scissors sync error. local is %c.", game_context->data->localPlayer);
        }
    } else {
        FURI_LOG_E(
            TAG,
            "Remote move '%c' error. lState=%c. rState=%c.",
            move,
            game_context->data->localPlayer,
            game_context->data->remotePlayer);
        remoteState = StateError;
    }

    if(StateReady != remoteState) {
        game_context->data->remotePlayer = remoteState;
        game_context->data->remoteMoveTick = furi_get_tick();
    }

    return StateReady != remoteState;
}

// This is the entry point for our application, which should match the application.fam file.
int32_t rock_paper_scissors_app(void* p) {
    UNUSED(p);

    // For this game we hardcode to 433.92MHz.
    uint32_t frequency = 433920000;

    // TODO: Figure out if frequency is allowed & try multiple frequencies.

    // Configure our initial data.
    GameContext* game_context = malloc(sizeof(GameContext));
    game_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    game_context->data = malloc(sizeof(GameData));
    game_context->data->buffer = furi_string_alloc();
    game_context->data->gameNumber = 42;
    game_context->data->localMoveTick = 0;
    game_context->data->localPlayer = StateLookingForPlayer;
    game_context->data->remoteMoveTick = 0;
    game_context->data->remotePlayer = StateLookingForPlayer;

    // Queue for events
    game_context->queue = furi_message_queue_alloc(8, sizeof(GameEvent));

    // Subghz worker.
    game_context->subghz_txrx = subghz_tx_rx_worker_alloc();

    // Try to start the TX/RX on the frequency and configure our callback
    // whenever new data is received.
    if(subghz_tx_rx_worker_start(game_context->subghz_txrx, frequency)) {
        subghz_tx_rx_worker_set_callback_have_read(
            game_context->subghz_txrx, rps_worker_update_rx_event_callback, game_context);
    } else {
        FURI_LOG_E(TAG, "Failed to start subghz_tx_rx_worker.");

        // For this game we don't show a friendly error about not being
        // allowed to broadcast on this frequency.  Instead the application
        // just exits.
        if(subghz_tx_rx_worker_is_running(game_context->subghz_txrx)) {
            subghz_tx_rx_worker_stop(game_context->subghz_txrx);
        }
        subghz_tx_rx_worker_free(game_context->subghz_txrx);
        furi_message_queue_free(game_context->queue);
        furi_mutex_free(game_context->mutex);
        furi_string_free(game_context->data->buffer);
        free(game_context->data);
        free(game_context);
        return 2;
    }

    // All the subghz CLI apps disable charging; so our game does it too.
    furi_hal_power_suppress_charge_enter();

    // Set ViewPort callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, rps_render_callback, game_context);
    view_port_input_callback_set(view_port, rps_input_callback, game_context->queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Run routine once a second.
    FuriTimer* timer = furi_timer_alloc(rps_timer_callback, FuriTimerTypePeriodic, game_context);
    furi_timer_start(timer, 1000);

    // Main loop
    GameEvent event;
    uint8_t beaconCounter = 0;
    bool processing = true;
    do {
        if(furi_message_queue_get(game_context->queue, &event, FuriWaitForever) == FuriStatusOk) {
            switch(event.type) {
            case GameEventTypeKey:
                if((event.input.type == InputTypeShort) && (event.input.key == InputKeyBack)) {
                    processing = false;
                } else if(event.input.type == InputTypeShort) {
                    unsigned int joinGameNumber;
                    GameEvent newEvent = {
                        .type = GameEventLocalMove, .tick = furi_get_tick(), .move = MoveUnknown};
                    switch(event.input.key) {
                    case InputKeyOk:
                        newEvent.move = MoveCount;
                        break;
                    case InputKeyUp:
                        newEvent.move = MoveRock;
                        break;
                    case InputKeyRight:
                        newEvent.move = MovePaper;
                        break;
                    case InputKeyDown:
                        newEvent.move = MoveScissors;
                        break;

                    case InputKeyLeft:
                        // Temporary: For now, we send "Join" when left button clicked.
                        joinGameNumber = game_context->data->gameNumber;
                        if(furi_mutex_acquire(game_context->mutex, FuriWaitForever) ==
                           FuriStatusOk) {
                            rps_broadcast_join(game_context, joinGameNumber);
                            rps_state_machine_remote_joined(game_context);
                            furi_mutex_release(game_context->mutex);
                        } else {
                            FURI_LOG_E(TAG, "Failed to aquire mutex.");
                        }
                        break;

                    default:
                        FURI_LOG_T(TAG, "No support for key %d", event.input.key);
                        break;
                    }

                    if(newEvent.move != MoveUnknown) {
                        furi_message_queue_put(game_context->queue, &newEvent, FuriWaitForever);
                    }
                }
                break;
            case GameEventPlaySong:
                play_song(game_context->data->localPlayer);
                break;
            case GameEventDataDetected:
                rps_receive_data(game_context, event.tick);
                break;
            case GameEventTypeTimer:
                if(furi_mutex_acquire(game_context->mutex, FuriWaitForever) == FuriStatusOk) {
                    if(StateLookingForPlayer == game_context->data->localPlayer &&
                       ++beaconCounter >= BEACON_DURATION) {
                        rps_broadcast_beacon(game_context);
                        beaconCounter = 0;
                    }
                    rps_state_machine_update(game_context);
                    furi_mutex_release(game_context->mutex);
                } else {
                    FURI_LOG_E(TAG, "Failed to aquire mutex.");
                }
                break;
            case GameEventRemoteBeacon:
                FURI_LOG_I(TAG, "Remote beacon detected. game number %03u", event.gameNumber);
                break;
            case GameEventRemoteJoined:
                if(furi_mutex_acquire(game_context->mutex, FuriWaitForever) == FuriStatusOk) {
                    if(event.gameNumber == game_context->data->gameNumber) {
                        rps_state_machine_remote_joined(game_context);
                    } else {
                        FURI_LOG_T(
                            TAG, "Remote joining another Flipper on game %03u.", event.gameNumber);
                    }
                    furi_mutex_release(game_context->mutex);
                } else {
                    FURI_LOG_E(TAG, "Failed to aquire mutex.");
                }
                break;
            case GameEventLocalMove:
                if(furi_mutex_acquire(game_context->mutex, FuriWaitForever) == FuriStatusOk) {
                    rps_state_machine_local_moved(game_context, event.move);
                    furi_mutex_release(game_context->mutex);
                } else {
                    FURI_LOG_E(TAG, "Failed to aquire mutex for local move.");
                }
                break;
            case GameEventRemoteMove:
                if(furi_mutex_acquire(game_context->mutex, FuriWaitForever) == FuriStatusOk) {
                    rps_state_machine_remote_moved(game_context, event.move);
                    furi_mutex_release(game_context->mutex);
                } else {
                    FURI_LOG_E(TAG, "Failed to aquire mutex for remote move.");
                }
                break;
            default:
                FURI_LOG_E(TAG, "Queue had unknown message type: %u", event.type);
                break;
            }

            // If message contains a sender name furi_string, free it.
            if(event.senderName) {
                furi_string_free(event.senderName);
            }

            // Send signal to update the screen (callback will get invoked at some point later.)
            view_port_update(view_port);
        } else {
            // We had an issue getting message from the queue, so exit application.
            FURI_LOG_E(TAG, "Issue encountered reading from queue.  Exiting application.");
            processing = false;
        }
    } while(processing);

    // Free resources
    furi_timer_free(timer);
    if(subghz_tx_rx_worker_is_running(game_context->subghz_txrx)) {
        subghz_tx_rx_worker_stop(game_context->subghz_txrx);
    }
    subghz_tx_rx_worker_free(game_context->subghz_txrx);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(game_context->queue);
    furi_mutex_free(game_context->mutex);
    furi_string_free(game_context->data->buffer);
    free(game_context->data);
    free(game_context);

    // Reenable charging.
    furi_hal_power_suppress_charge_exit();

    return 0;
}