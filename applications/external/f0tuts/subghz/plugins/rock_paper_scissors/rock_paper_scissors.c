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

#include "rock_paper_scissors.h"

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
static void play_song(GameContext* game_context) {
    if(furi_hal_speaker_acquire(1000)) {
        GameState state = game_context->data->local_player;
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

        GameEvent event = {.type = GameEventSongEnded, .tick = furi_get_tick()};
        furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
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
    char sender_name[MESSAGE_MAX_LEN] = {0};
    GameRfPurpose purpose;
    uint8_t version;
    unsigned int game_number;
    Move move = MoveUnknown;
    char* sender_contact;

    int index = 0;

    uint8_t message[MESSAGE_MAX_LEN] = {0};
    memset(message, 0x00, MESSAGE_MAX_LEN);
    int len = (int)subghz_tx_rx_worker_read(game_context->subghz_txrx, message, MESSAGE_MAX_LEN);
    // Null terminate buffer at the end of message so we can't overrun the buffer.
    message[MESSAGE_MAX_LEN - 1] = 0;

    // Sender's Flipper Zero name.
    while(index < len && message[index] != ':') {
        sender_name[index] = message[index];
        index++;
    }
    if(index >= len) {
        FURI_LOG_T(TAG, "Message too long, ignoring. >%s<", message);
        return;
    } else if(message[index] != ':') {
        FURI_LOG_T(TAG, "Message missing ':' character, ignoring. >%s<", message);
        return;
    }
    sender_name[index] = 0;

    // subghz chat sends escape message.  Ignore it.
    if(index > 10 && message[0] == 0x1B && message[1] == '[' && message[3] == ';' &&
       message[6] == 'm') {
        index = 7;
        while(index < len && message[index] != ':' && message[index] != 0x1B) {
            sender_name[index] = message[index];
            index++;
        }
        if(index < len && message[index] == 0x1B) {
            // Skip over the "ESC [ 0 m", which is the reset color code.
            // We should be at the ':' character.
            index += 4;
        }
    }

    // Skip the ':' character & check for a space.
    if(++index < len) {
        if(message[index++] != ' ') {
            FURI_LOG_T(TAG, "Message missing ' ' after name, ignoring. >%s<", message);
            return;
        }
    }

    // Check for the game name.
    int game_name_len = (int)strlen(RPS_GAME_NAME);
    for(int i = 0; i < game_name_len; i++) {
        if((index >= len) || (message[index++] != RPS_GAME_NAME[i])) {
            FURI_LOG_T(
                TAG, "Message missing game name '%s', ignoring. >%s<", RPS_GAME_NAME, message);
            return;
        }
    }
    if(index < len) {
        if(message[index++] != ':') {
            FURI_LOG_T(TAG, "Message missing ':' after game name, ignoring. >%s<", message);
            return;
        }
    }

    FURI_LOG_D(TAG, "Got message >%s<", message);

    // The purpose immediately follows the game name.
    if(index >= len) {
        FURI_LOG_W(TAG, "Message missing purpose, ignoring. >%s<", message);
        return;
    }
    purpose = message[index++];

    // The version follows the purpose.
    if(index >= len) {
        FURI_LOG_W(TAG, "Message missing version, ignoring. >%s<", message);
        return;
    }
    version = message[index++];
    FURI_LOG_T(TAG, "Purpose is %c and version is %c", purpose, version);

    // Game number is 3 digits.
    if(sscanf((const char*)message + index, "%03u", &game_number) != 1) {
        FURI_LOG_W(TAG, "Message missing game number, ignoring. >%s<", message);
        return;
    }
    index += 3;

    switch(purpose) {
    case GameRfPurposeMove:
        // We expect this mesage to the game number, move and sender name.
        if(index >= len) {
            FURI_LOG_W(TAG, "Failed to parse move message. >%s<", message);
            return;
        } else {
            move = (Move)message[index];
            // IMPORTANT: The code processing the event needs to furi_string_free the senderName!
            FuriString* name = furi_string_alloc();
            furi_string_set(name, sender_name);

            GameEvent event = {
                .type = GameEventRemoteMove,
                .move = move,
                .tick = tick,
                .sender_name = name,
                .game_number = game_number};
            furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
        }
        break;

    case GameRfPurposeBeacon: {
        // IMPORTANT: The code processing the event needs to furi_string_free the senderName!
        FuriString* name = furi_string_alloc();
        furi_string_set(name, sender_name);

        GameEvent event = {
            .type = GameEventRemoteBeacon, .sender_name = name, .game_number = game_number};
        furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
        break;
    }

    case GameRfPurposeNotBeacon: {
        // IMPORTANT: The code processing the event needs to furi_string_free the senderName!
        FuriString* name = furi_string_alloc();
        furi_string_set(name, sender_name);

        GameEvent event = {
            .type = GameEventRemoteNotBeacon, .sender_name = name, .game_number = game_number};
        furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
        break;
    }

    case GameRfPurposePlayAgain: {
        // IMPORTANT: The code processing the event needs to furi_string_free the senderName!
        FuriString* name = furi_string_alloc();
        furi_string_set(name, sender_name);

        GameEvent event = {
            .type = GameEventRemotePlayAgain, .sender_name = name, .game_number = game_number};
        furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
        break;
    }

    case GameRfPurposeQuit: {
        // IMPORTANT: The code processing the event needs to furi_string_free the senderName!
        FuriString* name = furi_string_alloc();
        furi_string_set(name, sender_name);

        GameEvent event = {
            .type = GameEventRemoteQuit, .sender_name = name, .game_number = game_number};
        furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
        break;
    }

    case GameRfPurposeJoin:
        if(index >= len) {
            FURI_LOG_W(TAG, "Failed to parse join message. >%s<", message);
            return;
        } else {
            sender_contact = (char*)message + index;
            while(index < len) {
                if(message[index] == '\n' || message[index] == '\r') {
                    message[index] = 0;
                    break;
                }
                index++;
            }
            FURI_LOG_T(TAG, "Join had contact of >%s<", sender_contact);

            // IMPORTANT: The code processing the event needs to furi_string_free the senderName!
            FuriString* name = furi_string_alloc();
            furi_string_set(name, sender_name);
            FuriString* contact = furi_string_alloc();
            furi_string_set(contact, sender_contact);

            GameEvent event = {
                .type = GameEventRemoteJoined,
                .sender_name = name,
                .sender_contact = contact,
                .game_number = game_number};
            furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
        }
        break;

    case GameRfPurposeJoinAcknowledge:
        if(index >= len) {
            FURI_LOG_W(TAG, "Failed to parse join acknowledge message. >%s<", message);
            return;
        } else {
            sender_contact = (char*)message + index;
            while(index < len) {
                if(message[index] == '\n' || message[index] == '\r') {
                    message[index] = 0;
                    break;
                }
                index++;
            }

            FURI_LOG_T(TAG, "Join acknowledge for game %d.", game_number);
            FURI_LOG_T(TAG, "Join ack had contact of >%s<", sender_contact);

            FuriString* name = furi_string_alloc();
            furi_string_set(name, sender_name);
            FuriString* contact = furi_string_alloc();
            furi_string_set(contact, sender_contact);

            GameEvent event = {
                .type = GameEventRemoteJoinAcknowledged,
                .sender_name = name,
                .sender_contact = contact,
                .game_number = game_number};
            furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
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

// Render UI when we are hosting the game.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_host_game(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    GameContext* game_context = ctx;

    // Attempt to aquire context, so we can read the data.
    if(furi_mutex_acquire(game_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    GameData* data = game_context->data;
    unsigned int gameNumber = data->game_number;

    canvas_draw_icon(canvas, 0, 0, images[DolphinLocalLooking]);

    canvas_set_font(canvas, FontPrimary);

    if(data->local_player == StateHostingSetFrequency ||
       data->local_player == StateHostingBadFrequency) {
        canvas_draw_box(canvas, 48, 3, 128 - 50, 14);
        canvas_invert_color(canvas);
    }
    uint32_t freq = frequency_list[game_context->data->frequency_index];
    uint16_t freq_mhz = freq / 1000000;
    uint16_t freq_mod = (freq % 1000000) / 10000;
    furi_string_printf(data->buffer, "freq < %03d.%02d >", freq_mhz, freq_mod);
    canvas_draw_str_aligned(
        canvas, 50, 5, AlignLeft, AlignTop, furi_string_get_cstr(data->buffer));
    if(data->local_player == StateHostingSetFrequency ||
       data->local_player == StateHostingBadFrequency) {
        canvas_invert_color(canvas);
    }

    if(data->local_player == StateHostingSetGameNumber) {
        canvas_draw_box(canvas, 48, 18, 128 - 50, 14);
        canvas_invert_color(canvas);
    }
    furi_string_printf(data->buffer, "game < %03d  >", gameNumber);
    canvas_draw_str_aligned(
        canvas, 50, 20, AlignLeft, AlignTop, furi_string_get_cstr(data->buffer));
    if(data->local_player == StateHostingSetGameNumber) {
        canvas_invert_color(canvas);
    }

    if(data->local_player == StateHostingLookingForPlayer) {
        canvas_set_font(canvas, FontSecondary);
        furi_string_printf(data->buffer, "Waiting for player, game %03d.", gameNumber);
        canvas_draw_str_aligned(
            canvas, 0, 53, AlignLeft, AlignTop, furi_string_get_cstr(data->buffer));
    } else if(data->local_player == StateHostingBadFrequency) {
        canvas_draw_str_aligned(canvas, 0, 53, AlignLeft, AlignTop, "Frequency not avail.");
    } else {
        canvas_draw_str_aligned(canvas, 0, 53, AlignLeft, AlignTop, "Press OK to start game.");
    }

    furi_mutex_release(game_context->mutex);
}

// Render UI when we are joining a game.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_join_game(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    GameContext* game_context = ctx;

    // Attempt to aquire context, so we can read the data.
    if(furi_mutex_acquire(game_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    GameData* data = game_context->data;

    canvas_draw_icon(canvas, 0, 0, images[DolphinLocalLooking]);

    canvas_set_font(canvas, FontPrimary);

    if(data->local_player == StateJoiningSetFrequency ||
       data->local_player == StateJoiningBadFrequency) {
        canvas_draw_box(canvas, 48, 3, 128 - 50, 14);
        canvas_invert_color(canvas);
    }
    uint32_t freq = frequency_list[game_context->data->frequency_index];
    uint16_t freq_mhz = freq / 1000000;
    uint16_t freq_mod = (freq % 1000000) / 10000;
    furi_string_printf(data->buffer, "freq < %03d.%02d >", freq_mhz, freq_mod);
    canvas_draw_str_aligned(
        canvas, 50, 5, AlignLeft, AlignTop, furi_string_get_cstr(data->buffer));
    if(data->local_player == StateJoiningSetFrequency ||
       data->local_player == StateJoiningBadFrequency) {
        canvas_invert_color(canvas);
    }

    if(data->local_player == StateJoiningSetGameNumber) {
        canvas_draw_box(canvas, 48, 18, 128 - 50, 14);
        canvas_invert_color(canvas);
    }
    GameInfo* game = remote_games_current(game_context);
    if(game) {
        char prev = remote_games_has_previous(game_context) ? '<' : ' ';
        char next = remote_games_has_next(game_context) ? '>' : ' ';
        furi_string_printf(data->buffer, "game %c %03d  %c", prev, game->game_number, next);
    } else {
        furi_string_printf(data->buffer, "game    none");
    }
    canvas_draw_str_aligned(
        canvas, 50, 20, AlignLeft, AlignTop, furi_string_get_cstr(data->buffer));
    if(data->local_player == StateJoiningSetGameNumber) {
        canvas_invert_color(canvas);
    }

    if(game && game->sender_name) {
        canvas_draw_str_aligned(
            canvas, 50, 35, AlignLeft, AlignTop, furi_string_get_cstr(game->sender_name));
    }

    if(data->local_player == StateJoiningBadFrequency) {
        canvas_draw_str_aligned(canvas, 0, 53, AlignLeft, AlignTop, "Frequency not avail.");
    } else if(game) {
        canvas_draw_str_aligned(canvas, 0, 53, AlignLeft, AlignTop, "Press OK to join game.");
    } else {
        canvas_draw_str_aligned(canvas, 0, 53, AlignLeft, AlignTop, "No games available.");
    }

    furi_mutex_release(game_context->mutex);
}

// Render UI when we are playing the game.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_playing_game(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    GameContext* game_context = ctx;

    // Attempt to aquire context, so we can read the data.
    if(furi_mutex_acquire(game_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    GameData* data = game_context->data;
    GameState local_player = data->local_player;
    GameState remote_player = data->remote_player;

    canvas_set_font(canvas, FontSecondary);

    switch(remote_player) {
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
        if(StateCount2 != local_player) {
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
        if(StateCount2 != local_player) {
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
        if(StateCount2 != local_player) {
            canvas_draw_icon(canvas, 64, 0, images[DolphinRemoteScissors]);
            canvas_draw_str_aligned(canvas, 70, 55, AlignLeft, AlignTop, "Scissors");
        } else {
            canvas_draw_icon(canvas, 64, 0, images[DolphinRemoteCount]);
        }
        break;

    default:
        break;
    }

    switch(local_player) {
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

    default:
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Unexpected. 2");
        break;
    }

    furi_mutex_release(game_context->mutex);
}

// Render UI when we encounter an error in the game.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_error(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    GameContext* game_context = ctx;

    // Attempt to aquire context, so we can read the data.
    if(furi_mutex_acquire(game_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    GameData* data = game_context->data;
    GameState local_player = data->local_player;

    canvas_set_font(canvas, FontPrimary);

    switch(local_player) {
    case StateErrorRemoteTimeout:
        canvas_draw_str_aligned(canvas, 15, 5, AlignLeft, AlignTop, "Remote timeout.");

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 5, 20, AlignLeft, AlignTop, "It appears the remote");
        canvas_draw_str_aligned(canvas, 5, 30, AlignLeft, AlignTop, "user has left the game?");
        break;

    default:
        canvas_draw_str_aligned(canvas, 5, 55, AlignLeft, AlignTop, "Unexpected. 3");
        break;
    }

    furi_mutex_release(game_context->mutex);
}

// Render UI when we are hosting the game.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_main_menu(Canvas* canvas, void* ctx) {
    GameContext* game_context = ctx;

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 2, 0, AlignLeft, AlignTop, "ROCK PAPER SCISSORS");
    canvas_draw_str_aligned(canvas, 30, 15, AlignLeft, AlignTop, "Edit contact info");
    canvas_draw_str_aligned(canvas, 30, 27, AlignLeft, AlignTop, "Host game");
    canvas_draw_str_aligned(canvas, 30, 39, AlignLeft, AlignTop, "Join game");
    canvas_draw_str_aligned(canvas, 30, 51, AlignLeft, AlignTop, "Past games");

    if(game_context->data->local_player == StateMainMenuMessage) {
        canvas_draw_str_aligned(canvas, 20, 15, AlignLeft, AlignTop, ">");
    } else if(game_context->data->local_player == StateMainMenuHost) {
        canvas_draw_str_aligned(canvas, 20, 27, AlignLeft, AlignTop, ">");
    } else if(game_context->data->local_player == StateMainMenuJoin) {
        canvas_draw_str_aligned(canvas, 20, 39, AlignLeft, AlignTop, ">");
    } else if(game_context->data->local_player == StateMainMenuPastGames) {
        canvas_draw_str_aligned(canvas, 20, 51, AlignLeft, AlignTop, ">");
    }
}

// Render UI when we are showing previous games.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_past_games(Canvas* canvas, void* ctx) {
    GameContext* game_context = ctx;

    canvas_set_font(canvas, FontPrimary);

    PlayerStats* stats = game_context->data->viewing_player_stats;
    if(!stats) {
        canvas_draw_str_aligned(canvas, 10, 30, AlignLeft, AlignTop, "NO GAMES PLAYED.");
    } else {
        canvas_draw_str_aligned(
            canvas, 0, 0, AlignLeft, AlignTop, furi_string_get_cstr(stats->last_played));

        furi_string_printf(
            game_context->data->buffer,
            "Win:%d  Lost:%d  Tied:%d",
            stats->win_count,
            stats->loss_count,
            stats->tie_count);
        canvas_draw_str_aligned(
            canvas, 0, 12, AlignLeft, AlignTop, furi_string_get_cstr(game_context->data->buffer));

        canvas_draw_str_aligned(
            canvas, 0, 24, AlignLeft, AlignTop, furi_string_get_cstr(stats->flipper_name));

        canvas_set_font(canvas, FontSecondary);
        char ch = furi_string_get_char(stats->contact, 0);
        for(unsigned int i = 0; i < sizeof(contact_list) / sizeof(contact_list[0]); i++) {
            if(contact_list[i][0] == ch) {
                canvas_draw_str_aligned(canvas, 0, 36, AlignLeft, AlignTop, contact_list[i] + 1);
                ch = 0;
                break;
            }
        }
        if(ch) {
            char id[2] = {ch, 0};
            canvas_draw_str_aligned(canvas, 0, 36, AlignLeft, AlignTop, id);
        }

        canvas_draw_str_aligned(
            canvas, 0, 48, AlignLeft, AlignTop, furi_string_get_cstr(stats->contact) + 1);
    }
}

// Render UI when we are choosing a social identity to share.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_choose_social(Canvas* canvas, void* ctx) {
    GameContext* game_context = ctx;
    UNUSED(game_context);

    int line = game_context->data->social_line;
    int index = line - 2;
    if(index < 0) {
        index = 0;
    }

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 2, 0, AlignLeft, AlignTop, "Share your...");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 15, 15, AlignLeft, AlignTop, contact_list[index++] + 1);
    canvas_draw_str_aligned(canvas, 15, 27, AlignLeft, AlignTop, contact_list[index++] + 1);
    canvas_draw_str_aligned(canvas, 15, 39, AlignLeft, AlignTop, contact_list[index++] + 1);
    if(index < (int)COUNT_OF(contact_list)) {
        canvas_draw_str_aligned(canvas, 15, 51, AlignLeft, AlignTop, contact_list[index++] + 1);
    }

    if(line == 0) {
        canvas_draw_str_aligned(canvas, 5, 15, AlignLeft, AlignTop, ">");
    } else if(line == 1) {
        canvas_draw_str_aligned(canvas, 5, 27, AlignLeft, AlignTop, ">");
    } else {
        canvas_draw_str_aligned(canvas, 5, 39, AlignLeft, AlignTop, ">");
    }
}

// Return the character at the current keyboard cursor position.
// @param game_context pointer to a GameContext.
// @param long_press true if the key was held down for a long time.
static char get_char(GameContext* game_context, bool long_press) {
    int c_r = game_context->data->keyboard_row;
    int c_c = game_context->data->keyboard_col;
    char ch = keyboard[c_r][c_c];

    if(!long_press && ch >= 'A' && ch <= 'Z') {
        ch += 32;
    }

    return ch;
}

// Render an arrow, for enter and backspace.
// @param canvas rendering surface of the Flipper Zero.
// @param x x coordinate of the arrow.
// @param y y coordinate of the arrow.
// @param tail true for enter, false for backspace.
static void draw_arrow(Canvas* canvas, int x, int y, bool tail) {
    canvas_draw_line(canvas, x, y + 2, x + 4, y + 2);
    canvas_draw_line(canvas, x, y + 2, x + 2, y);
    canvas_draw_line(canvas, x, y + 2, x + 2, y + 4);
    if(tail) {
        canvas_draw_line(canvas, x + 4, y + 2, x + 4, y);
    }
}

// Render UI when we are inputting text.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_input_text(Canvas* canvas, void* ctx) {
    GameContext* game_context = ctx;
    UNUSED(game_context);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontKeyboard);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str(canvas, 0, 8, furi_string_get_cstr(game_context->data->keyboard_heading));

    canvas_draw_rframe(canvas, 0, 10, 127, 14, 1);
    int input_offset = furi_string_utf8_length(game_context->data->keyboard_data) - 20;
    if(input_offset < 0) {
        input_offset = 0;
    }
    canvas_draw_str(
        canvas, 2, 20, furi_string_get_cstr(game_context->data->keyboard_data) + input_offset);

    int c_r = game_context->data->keyboard_row;
    int c_c = game_context->data->keyboard_col;

    for(int row = 0; row < 4; row++) {
        for(int col = 0; col < 14; col++) {
            char ch = keyboard[row][col];
            int x = col * 9 + 2;
            int y = row * 10 + 33;

            if(row == c_r && col == c_c) {
                canvas_draw_box(canvas, x - 1, y - 8, 7, 9);
                canvas_set_color(canvas, ColorWhite);
            }

            canvas_draw_glyph(canvas, x, y, ch);

            if(ch == KEYBOARD_BACKSPACE) {
                draw_arrow(canvas, x, y - 5, false);
            } else if(ch == KEYBOARD_ENTER) {
                draw_arrow(canvas, x, y - 5, true);
            }
            canvas_set_color(canvas, ColorBlack);
        }
    }
}

static void rps_render_game_result(Canvas* canvas, void* ctx) {
    GameContext* game_context = ctx;
    UNUSED(game_context);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontKeyboard);
    canvas_set_color(canvas, ColorBlack);

    canvas_set_font(canvas, FontPrimary);

    char* message;
    if(isWin(game_context->data->local_player)) {
        message = "YOU WIN!!!";
    } else if(isLoss(game_context->data->local_player)) {
        message = "YOU LOST.";
    } else {
        message = "DRAW!";
    }
    canvas_draw_str_aligned(canvas, 40, 0, AlignLeft, AlignTop, message);

    canvas_draw_str_aligned(
        canvas, 0, 12, AlignLeft, AlignTop, furi_string_get_cstr(game_context->data->remote_name));

    canvas_set_font(canvas, FontSecondary);
    char ch = furi_string_get_char(game_context->data->remote_contact, 0);
    for(unsigned int i = 0; i < sizeof(contact_list) / sizeof(contact_list[0]); i++) {
        if(contact_list[i][0] == ch) {
            canvas_draw_str_aligned(canvas, 0, 26, AlignLeft, AlignTop, contact_list[i] + 1);
            ch = 0;
            break;
        }
    }
    if(ch) {
        char id[2] = {ch, 0};
        canvas_draw_str_aligned(canvas, 0, 26, AlignLeft, AlignTop, id);
    }

    canvas_draw_str_aligned(
        canvas,
        0,
        38,
        AlignLeft,
        AlignTop,
        furi_string_get_cstr(game_context->data->remote_contact) + 1);

    canvas_draw_str_aligned(canvas, 18, 53, AlignLeft, AlignTop, "Play Again?");

    canvas_draw_box(canvas, 70, 53, 30, 9);
    canvas_set_color(canvas, ColorWhite);
    if(game_context->data->keyboard_col == 0) {
        canvas_draw_str_aligned(canvas, 73, 54, AlignLeft, AlignTop, " Yes >");
    } else {
        canvas_draw_str_aligned(canvas, 73, 54, AlignLeft, AlignTop, "< No");
    }
    canvas_set_color(canvas, ColorBlack);
}

// We register this callback to get invoked whenever we need to render the screen.
// We render the UI on this callback thread.
// @param canvas rendering surface of the Flipper Zero.
// @param ctx pointer to a GameContext.
static void rps_render_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    GameContext* game_context = ctx;

    if(game_context->data->screen_state == ScreenHostGame) {
        rps_render_host_game(canvas, game_context);
    } else if(game_context->data->screen_state == ScreenError) {
        rps_render_error(canvas, game_context);
    } else if(game_context->data->screen_state == ScreenPlayingGame) {
        rps_render_playing_game(canvas, game_context);
    } else if(game_context->data->screen_state == ScreenJoinGame) {
        rps_render_join_game(canvas, game_context);
    } else if(game_context->data->screen_state == ScreenMainMenu) {
        rps_render_main_menu(canvas, game_context);
    } else if(game_context->data->screen_state == ScreenPastGames) {
        rps_render_past_games(canvas, game_context);
    } else if(game_context->data->screen_state == ScreenEditMessage) {
        rps_render_input_text(canvas, game_context);
    } else if(game_context->data->screen_state == ScreenChooseSocial) {
        rps_render_choose_social(canvas, game_context);
    } else if(game_context->data->screen_state == ScreenFinishedGame) {
        rps_render_game_result(canvas, game_context);
    }
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

    // Try to handle the case that *sometimes* a signal might not get through.
    for(int i = 0; i < 3; i++) {
        if(game_context->data->echo_duration > 0 && game_context->data->echo_duration < 50) {
            furi_delay_ms(game_context->data->echo_duration + i * 5);
            FURI_LOG_I(TAG, "Echoing message");
            while(!subghz_tx_rx_worker_write(game_context->subghz_txrx, message, length)) {
                // Wait a few milliseconds on failure before trying to send again.
                furi_delay_ms(7);
            }
        }
    }
}

// Our GameEventSendCounter handler invokes this method.
// We broadcast - "YourFlip: " + "RPS:" + move"M" + version"A" + game"###" + move"R" + "\r\n"
// @param game_context pointer to a GameContext.
// @param moveToSend the move to send to the remote player.
static void rps_broadcast_move(GameContext* game_context, Move moveToSend) {
    GameData* data = game_context->data;
    FURI_LOG_I(TAG, "Sending move %c", moveToSend);

    // The message for game 42 with a move with value Rock should look like...  "YourFlip: RPS:MA042R\r\n"
    furi_string_printf(
        data->buffer,
        "%s: %s:%c%c%03u%c\r\n",
        furi_hal_version_get_name_ptr(),
        RPS_GAME_NAME,
        GameRfPurposeMove,
        MAJOR_VERSION,
        data->game_number,
        moveToSend);
    rps_broadcast(game_context, data->buffer);
}

// Our GameEventTypeTimer handler invokes this method.
// We broadcast - "YourFlip: " + "RPS:" + beacon"B" + version"A" + game"###" + "\r\n"
// @param game_context pointer to a GameContext.
static void rps_broadcast_beacon(GameContext* game_context) {
    GameData* data = game_context->data;
    FURI_LOG_I(TAG, "Sending beacon");

    // The message for game 42 should look like...  "YourFlip: RPS:BA042\r\n"
    furi_string_printf(
        data->buffer,
        "%s: %s:%c%c%03u\r\n",
        furi_hal_version_get_name_ptr(),
        RPS_GAME_NAME,
        GameRfPurposeBeacon,
        MAJOR_VERSION,
        data->game_number);
    rps_broadcast(game_context, data->buffer);
}

// Our GameEventTypeTimer handler invokes this method.
// We broadcast - "YourFlip: " + "RPS:" + notbeacon"N" + version"A" + game"###" + "\r\n"
// @param game_context pointer to a GameContext.
static void rps_broadcast_not_beacon(GameContext* game_context) {
    GameData* data = game_context->data;
    FURI_LOG_I(TAG, "Sending not beacon");

    // The message for game 42 should look like...  "YourFlip: RPS:NA042\r\n"
    furi_string_printf(
        data->buffer,
        "%s: %s:%c%c%03u\r\n",
        furi_hal_version_get_name_ptr(),
        RPS_GAME_NAME,
        GameRfPurposeNotBeacon,
        MAJOR_VERSION,
        data->game_number);
    rps_broadcast(game_context, data->buffer);
}

static void rps_broadcast_play_again(GameContext* game_context) {
    GameData* data = game_context->data;
    FURI_LOG_I(TAG, "Sending play again");

    // The message for game 42 should look like...  "YourFlip: RPS:PA042\r\n"
    furi_string_printf(
        data->buffer,
        "%s: %s:%c%c%03u\r\n",
        furi_hal_version_get_name_ptr(),
        RPS_GAME_NAME,
        GameRfPurposePlayAgain,
        MAJOR_VERSION,
        data->game_number);
    rps_broadcast(game_context, data->buffer);
}

static void rps_broadcast_quit(GameContext* game_context) {
    GameData* data = game_context->data;
    FURI_LOG_I(TAG, "Sending quit");

    // The message for game 42 should look like...  "YourFlip: RPS:QA042\r\n"
    furi_string_printf(
        data->buffer,
        "%s: %s:%c%c%03u\r\n",
        furi_hal_version_get_name_ptr(),
        RPS_GAME_NAME,
        GameRfPurposeQuit,
        MAJOR_VERSION,
        data->game_number);
    rps_broadcast(game_context, data->buffer);
}

// Send message that indicates Flipper is joining a specific game.
// We broadcast - "YourFlip: " + "RPS:" + join"J" + version"A" + game"###" + "NYourNameHere" + "\r\n"
// @param game_context pointer to a GameContext.
static void rps_broadcast_join(GameContext* game_context) {
    GameData* data = game_context->data;
    data->echo_duration = 42;
    unsigned int gameNumber = data->game_number;
    FURI_LOG_I(TAG, "Joining game %d.", gameNumber);

    // The message for game 42 should look like...  "YourFlip: RPS:JA042NYourNameHere\r\n"
    furi_string_printf(
        data->buffer,
        "%s: %s:%c%c%03u%s\r\n",
        furi_hal_version_get_name_ptr(),
        RPS_GAME_NAME,
        GameRfPurposeJoin,
        MAJOR_VERSION,
        data->game_number,
        furi_string_get_cstr(data->local_contact));
    rps_broadcast(game_context, data->buffer);
}

// Send message that acknowledges Flipper joining a specific game.
// We broadcast -  "YourFlip: " + "RPS:" + joinAck"A" + version"A" + game"###" + "NYourNameHere" + "\r\n"
// @param game_context pointer to a GameContext.
static void rps_broadcast_join_acknowledge(GameContext* game_context) {
    GameData* data = game_context->data;
    data->echo_duration = 12;
    unsigned int gameNumber = data->game_number;
    FURI_LOG_I(TAG, "Acknowledge joining game %d.", gameNumber);

    // The message for game 42 should look like...  "YourFlip: RPS:AA042NYourNameHere\r\n"
    furi_string_printf(
        data->buffer,
        "%s: %s:%c%c%03u%s\r\n",
        furi_hal_version_get_name_ptr(),
        RPS_GAME_NAME,
        GameRfPurposeJoinAcknowledge,
        MAJOR_VERSION,
        data->game_number,
        furi_string_get_cstr(data->local_contact));
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

// Updates the state machine, if needed.
// @param game_context pointer to a GameContext.
static void rps_state_machine_update(GameContext* game_context) {
    GameData* d = game_context->data;
    if((d->screen_state != ScreenPlayingGame) && (d->screen_state != ScreenError)) {
        FURI_LOG_T(TAG, "Not in playing game state.  screenState:%d", d->screen_state);
        return;
    }

    FURI_LOG_I(
        TAG, "Validating game state. local:%c Remote:%c", d->local_player, d->remote_player);

    // Did player leave after joining?
    if((StateReady == d->remote_player) &&
       (duration(d->remote_move_tick) > DURATION_NO_MOVE_DETECTED_ERROR)) {
        d->remote_player = StateUnknown;
        d->remote_move_tick = furi_get_tick();
        d->local_player = StateErrorRemoteTimeout;
        d->local_move_tick = furi_get_tick();
        d->screen_state = ScreenError;

        // Should we tell other player we timed out?
        FURI_LOG_I(TAG, "Timed out after joining.");
        return;
    }

    // Check for winner.
    if(isFinalMove(d->local_player) && isFinalMove(d->remote_player) &&
       (duration(d->local_move_tick) > DURATION_SHOW_MOVES)) {
        d->local_move_tick = furi_get_tick();
        d->remote_move_tick = furi_get_tick();
        if((d->local_player == StateRock) && (d->remote_player == StateScissors)) {
            d->local_player = StateWonRock;
            d->remote_player = StateLostScissors;
            FURI_LOG_I(TAG, "Local won w/Rock.");
        } else if((d->local_player == StateScissors) && (d->remote_player == StatePaper)) {
            d->local_player = StateWonScissors;
            d->remote_player = StateLostPaper;
            FURI_LOG_I(TAG, "Local won w/Scissors.");
        } else if((d->local_player == StatePaper) && (d->remote_player == StateRock)) {
            d->local_player = StateWonPaper;
            d->remote_player = StateLostRock;
            FURI_LOG_I(TAG, "Local won w/Paper.");
        } else if((d->local_player == StateRock) && (d->remote_player == StatePaper)) {
            d->local_player = StateLostRock;
            d->remote_player = StateWonPaper;
            FURI_LOG_I(TAG, "Remote won w/Paper.");
        } else if((d->local_player == StateScissors) && (d->remote_player == StateRock)) {
            d->local_player = StateLostScissors;
            d->remote_player = StateWonRock;
            FURI_LOG_I(TAG, "Remote won w/Rock.");
        } else if((d->local_player == StatePaper) && (d->remote_player == StateScissors)) {
            d->local_player = StateLostPaper;
            d->remote_player = StateWonScissors;
            FURI_LOG_I(TAG, "Remote won w/Scissors.");
        } else {
            FURI_LOG_I(TAG, "Tie game.");
            if(d->local_player == StateRock) {
                d->local_player = StateTieRock;
                d->remote_player = StateTieRock;
            } else if(d->local_player == StatePaper) {
                d->local_player = StateTiePaper;
                d->remote_player = StateTiePaper;
            } else {
                d->local_player = StateTieScissors;
                d->remote_player = StateTieScissors;
            }
        }

        GameEvent event = {.type = GameEventPlaySong};
        furi_message_queue_put(game_context->queue, &event, FuriWaitForever);
    }
}

// Update the state machine to reflect that a remote user joined the game.
// @param game_context pointer to a GameContext.
static bool rps_state_machine_remote_joined(GameContext* game_context) {
    if(StateHostingLookingForPlayer == game_context->data->local_player) {
        FURI_LOG_I(TAG, "Remote player joined our game!");
        game_context->data->remote_player = StateReady;
        game_context->data->remote_move_tick = furi_get_tick();
        game_context->data->local_player = StateReady;
        game_context->data->local_move_tick = furi_get_tick();
        game_context->data->screen_state = ScreenPlayingGame;
        return true;
    } else {
        FURI_LOG_I(
            TAG, "Remote requested join, but we are state %c!", game_context->data->local_player);
        return false;
    }
}

// Update the state machine to reflect the local user's move.
// @param game_context pointer to a GameContext.
// @param move local user move.
static bool rps_state_machine_local_moved(GameContext* game_context, Move move) {
    FURI_LOG_I(TAG, "Local move %c.", move);

    Move localMove = MoveUnknown;
    GameState localState = StateReady;

    if(MoveCount == move && StateReady == game_context->data->local_player) {
        localMove = MoveCount1;
        localState = StateCount1;
    } else if(MoveCount == move && StateCount1 == game_context->data->local_player) {
        localMove = MoveCount2;
        localState = StateCount2;
    } else if(StateCount2 == game_context->data->local_player) {
        if(MoveRock == move) {
            localMove = MoveRock;
            localState = StateRock;
        } else if(MovePaper == move) {
            localMove = MovePaper;
            localState = StatePaper;
        } else if(MoveScissors == move) {
            localMove = MoveScissors;
            localState = StateScissors;
        } else if(MoveCount == move) {
            // Ignore. We are already at count #2.
        } else {
            FURI_LOG_E(
                TAG,
                "Invalid Local move '%c' error. lState=%c. rState=%c.",
                move,
                game_context->data->local_player,
                game_context->data->remote_player);
        }
    } else {
        FURI_LOG_E(
            TAG,
            "Invalid Local move '%c' error. lState=%c. rState=%c.",
            move,
            game_context->data->local_player,
            game_context->data->remote_player);
    }

    if(MoveUnknown != localMove) {
        single_vibro();
        rps_broadcast_move(game_context, localMove);
    }

    if(StateReady != localState) {
        game_context->data->local_player = localState;
        game_context->data->local_move_tick = furi_get_tick();
    }

    return StateReady != localState;
}

// Update the state machine to reflect the remote user's move.
// @param game_context pointer to a GameContext.
// @param move remote user move.
static bool rps_state_machine_remote_moved(GameContext* game_context, Move move) {
    GameState remoteState = StateReady;
    FURI_LOG_I(TAG, "Remote move %c.", move);

    if(MoveCount1 == move && StateReady == game_context->data->remote_player) {
        remoteState = StateCount1;
    } else if(MoveCount2 == move && StateCount1 == game_context->data->remote_player) {
        remoteState = StateCount2;
    } else if(MoveRock == move && StateCount2 == game_context->data->remote_player) {
        remoteState = StateRock;
    } else if(MovePaper == move && StateCount2 == game_context->data->remote_player) {
        remoteState = StatePaper;
    } else if(MoveScissors == move && StateCount2 == game_context->data->remote_player) {
        remoteState = StateScissors;
    } else {
        FURI_LOG_E(
            TAG,
            "Remote move '%c' error. lState=%c. rState=%c.",
            move,
            game_context->data->local_player,
            game_context->data->remote_player);
    }

    if(StateReady != remoteState) {
        game_context->data->remote_player = remoteState;
        game_context->data->remote_move_tick = furi_get_tick();
    }

    return StateReady != remoteState;
}

static bool update_frequency(GameContext* game_context) {
    uint32_t frequency = frequency_list[game_context->data->frequency_index];

    // Stop the TX/RX worker if it is running.
    if(subghz_tx_rx_worker_is_running(game_context->subghz_txrx)) {
        FURI_LOG_T(TAG, "Stopped existing tx_rx_worker.");
        subghz_tx_rx_worker_stop(game_context->subghz_txrx);
        subghz_tx_rx_worker_free(game_context->subghz_txrx);
        game_context->subghz_txrx = subghz_tx_rx_worker_alloc();
    }

    if(!furi_hal_region_is_frequency_allowed(frequency)) {
        FURI_LOG_I(TAG, "Frequency not allowed %ld.", frequency);
        return false;
    }

    // Try to start the TX/RX on the frequency and configure our callback
    // whenever new data is received.

    // TODO: This is a hack. We should be able to get the device without
    // hardcoding the name.
    const SubGhzDevice* device = subghz_devices_get_by_name("cc1101_int");
    furi_assert(device);
    if(subghz_tx_rx_worker_start(game_context->subghz_txrx, device, frequency)) {
        subghz_tx_rx_worker_set_callback_have_read(
            game_context->subghz_txrx, rps_worker_update_rx_event_callback, game_context);
    } else {
        FURI_LOG_E(TAG, "Failed to start subghz_tx_rx_worker.");

        if(subghz_tx_rx_worker_is_running(game_context->subghz_txrx)) {
            subghz_tx_rx_worker_stop(game_context->subghz_txrx);
        }

        return false;
    }

    FURI_LOG_I(TAG, "Listening on frequency %ld.", frequency);
    return true;
}

static void remote_games_clear(GameContext* game_context) {
    game_context->data->remote_selected_game = NULL;
    while(game_context->data->remote_games) {
        GameInfo* game = game_context->data->remote_games;
        game_context->data->remote_games = game->next_game;
        if(game->sender_name) {
            furi_string_free(game->sender_name);
        }
        free(game);
    }
}

static GameInfo* remote_games_current(GameContext* game_context) {
    return game_context->data->remote_selected_game;
}

static bool remote_games_has_next(GameContext* game_context) {
    return game_context->data->remote_selected_game &&
           game_context->data->remote_selected_game->next_game;
}

static void remote_games_next(GameContext* game_context) {
    if(remote_games_has_next(game_context)) {
        game_context->data->remote_selected_game =
            game_context->data->remote_selected_game->next_game;
    }
}

static bool remote_games_has_previous(GameContext* game_context) {
    return game_context->data->remote_selected_game &&
           game_context->data->remote_games != game_context->data->remote_selected_game;
}

static void remote_games_previous(GameContext* game_context) {
    if(game_context->data->remote_selected_game) {
        uint16_t game_number = game_context->data->remote_selected_game->game_number;
        if(game_number > 0) {
            GameInfo* prevGame = remote_games_find(game_context, game_number);
            if(prevGame) {
                game_context->data->remote_selected_game = prevGame;
            }
        }
    }
}

static GameInfo* remote_games_find(GameContext* game_context, uint16_t game_number) {
    GameInfo* prevGame = NULL;
    GameInfo* game = game_context->data->remote_games;
    while(game) {
        if(game->game_number >= game_number) {
            return prevGame;
        }
        prevGame = game;
        game = game->next_game;
    }
    return prevGame;
}

static void remote_games_add(GameContext* game_context, GameEvent* game_event) {
    furi_assert(game_context);
    furi_assert(game_event);

    if(furi_mutex_acquire(game_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    // Add the game to the list, if not already in the list.
    GameInfo* game = remote_games_find(game_context, game_event->game_number);
    if(game == NULL &&
       (game_context->data->remote_games == NULL ||
        game_context->data->remote_games->game_number != game_event->game_number)) {
        game = malloc(sizeof(GameInfo));
        game->game_number = game_event->game_number;
        game->sender_name = game_event->sender_name;
        game_event->sender_name = NULL; // We are now responsible for freeing this string.
        game->next_game = game_context->data->remote_games;

        game_context->data->remote_games = game;
        FURI_LOG_I(TAG, "Game number %d added to front of list.", game_event->game_number);
        if(!game_context->data->remote_selected_game) {
            game_context->data->remote_selected_game = game;
            FURI_LOG_I(TAG, "Game number %d selected.", game->game_number);
        }
    } else if(
        game != NULL &&
        (game->next_game == NULL || (game->next_game->game_number != game_event->game_number))) {
        // We have a new game, so add it to the list.
        GameInfo* newGame = malloc(sizeof(GameInfo));
        newGame->game_number = game_event->game_number;
        newGame->sender_name = game_event->sender_name;
        game_event->sender_name = NULL; // We are now responsible for freeing this string.
        newGame->next_game = game->next_game;
        game->next_game = newGame;
        FURI_LOG_I(
            TAG, "Game number %d added after %d.", game_event->game_number, game->game_number);
    } else {
        FURI_LOG_T(TAG, "Game number %d already in list.", game_event->game_number);
    }

    furi_mutex_release(game_context->mutex);
}

static void remote_games_remove(GameContext* game_context, GameEvent* game_event) {
    furi_assert(game_context);
    furi_assert(game_event);

    FURI_LOG_I(TAG, "Removing game number %d.", game_event->game_number);

    GameInfo* game = remote_games_find(game_context, game_event->game_number + 1);
    if(game && game->game_number == game_event->game_number) {
        // We found the game, so remove it from the list.
        FURI_LOG_I(TAG, "Found game to remove.");

        // Check to see if the currently selected game is this game.
        if(game_context->data->remote_selected_game == game) {
            if(game_context->data->remote_selected_game->next_game) {
                // Move to next available game.
                game_context->data->remote_selected_game =
                    game_context->data->remote_selected_game->next_game;
            } else {
                // Move to first game.
                game_context->data->remote_selected_game = game_context->data->remote_games;
            }

            if(game_context->data->remote_selected_game == game) {
                game_context->data->remote_selected_game = NULL;
                FURI_LOG_I(TAG, "Clearing remote_selected_game.");
            }
        }

        // Remove the game from the list.
        GameInfo* game_previous = remote_games_find(game_context, game->game_number);
        if(!game_previous) {
            game_context->data->remote_games = game->next_game;
            if(game->sender_name) {
                furi_string_free(game->sender_name);
            }
            free(game);
        } else {
            game_previous->next_game = game->next_game;
            if(game->sender_name) {
                furi_string_free(game->sender_name);
            }
            free(game);
        }

        FURI_LOG_I(TAG, "Done removing game.");
    }
}

// This is a helper method that creates the game directory if it does not exist.
// @param storage pointer to a Storage.
static void ensure_dir_exists(Storage* storage) {
    // If apps_data directory doesn't exist, create it.
    if(!storage_dir_exists(storage, RPS_APPS_DATA_FOLDER)) {
        FURI_LOG_I(TAG, "Creating directory: %s", RPS_APPS_DATA_FOLDER);
        storage_simply_mkdir(storage, RPS_APPS_DATA_FOLDER);
    } else {
        FURI_LOG_I(TAG, "Directory exists: %s", RPS_APPS_DATA_FOLDER);
    }

    // If rock_paper_scissors directory doesn't exist, create it.
    if(!storage_dir_exists(storage, RPS_GAME_FOLDER)) {
        FURI_LOG_I(TAG, "Creating directory: %s", RPS_GAME_FOLDER);
        storage_simply_mkdir(storage, RPS_GAME_FOLDER);
    } else {
        FURI_LOG_I(TAG, "Directory exists: %s", RPS_GAME_FOLDER);
    }
}

// Saves a local contact to the file system.
// @param game_context pointer to a GameContext.
static void save_local_contact(GameContext* game_context) {
    if(furi_mutex_acquire(game_context->mutex, FuriWaitForever) != FuriStatusOk) {
        return;
    }

    Storage* storage = furi_record_open(RECORD_STORAGE);
    ensure_dir_exists(storage);
    FURI_LOG_I(TAG, "Saving social: %s", furi_string_get_cstr(game_context->data->local_contact));

    File* social_file = storage_file_alloc(storage);
    if(storage_file_open(social_file, RPS_SOCIAL_PATH, FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
        FURI_LOG_T(TAG, "Opened file: %s", RPS_SOCIAL_PATH);
        int offset =
            (KEYBOARD_MAX_LEN + 2) * furi_string_get_char(game_context->data->local_contact, 0);
        if(offset < 0) {
            offset = 0;
        }

        int file_size = storage_file_size(social_file);
        if(file_size < offset) {
            storage_file_seek(social_file, file_size, true);
            for(int i = file_size; i < offset; i++) {
                storage_file_write(social_file, "\0", 1);
            }
        }

        storage_file_seek(social_file, offset, true);
        if(!storage_file_write(
               social_file,
               furi_string_get_cstr(game_context->data->local_contact),
               furi_string_size(game_context->data->local_contact))) {
            FURI_LOG_E(TAG, "Failed to write to file.");
        }
        storage_file_write(social_file, "\n", 1);

        storage_file_seek(social_file, 0, true);
        storage_file_write(
            social_file,
            furi_string_get_cstr(game_context->data->local_contact),
            furi_string_size(game_context->data->local_contact));
        storage_file_write(social_file, "\n", 1);
    } else {
        FURI_LOG_E(TAG, "Failed to open file: %s", RPS_SOCIAL_PATH);
    }

    storage_file_close(social_file);
    storage_file_free(social_file);
    furi_record_close(RECORD_STORAGE);

    furi_mutex_release(game_context->mutex);
}

// Loads a local contact from the file system.
// @param index index of the contact to load.
// @param skip_first_char true if the first character should be skipped.
// @param buffer pointer to a FuriString.
static void load_social_data(int index, bool skip_first_char, FuriString* buffer) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    File* social_file = storage_file_alloc(storage);
    if(storage_file_open(social_file, RPS_SOCIAL_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        char ch;
        FURI_LOG_T(TAG, "Opened file: %s", RPS_SOCIAL_PATH);
        int offset = (KEYBOARD_MAX_LEN + 2) * index;

        if(skip_first_char) {
            offset++; // Skip first character when filling keyboard buffer.
        }

        int file_size = storage_file_size(social_file);
        if(file_size > offset) {
            storage_file_seek(social_file, offset, true);
            furi_string_reset(buffer);
            while(storage_file_read(social_file, &ch, 1) && !storage_file_eof(social_file)) {
                if(ch == '\n' || ch == '\0') {
                    break;
                }
                furi_string_push_back(buffer, ch);
            }
        }
    } else {
        FURI_LOG_E(TAG, "Failed to open file: %s", RPS_SOCIAL_PATH);
    }

    storage_file_close(social_file);
    storage_file_free(social_file);
    furi_record_close(RECORD_STORAGE);
}

// Fills the keyboard data array with the previously saved social information.
// @param game_context pointer to a GameContext.
static void load_keyboard_data(GameContext* game_context) {
    load_social_data(
        contact_list[game_context->data->social_line][0], true, game_context->data->keyboard_data);
}

// Loads the initial local contact from the file system.
// @param game_context pointer to a GameContext.
static void load_local_contact(GameContext* game_context) {
    load_social_data(0, false, game_context->data->local_contact);
    if(furi_string_size(game_context->data->local_contact) > 0) {
        char ch = furi_string_get_char(game_context->data->local_contact, 0);
        for(int i = 0; i < (int)COUNT_OF(contact_list); i++) {
            if(contact_list[i][0] == ch) {
                game_context->data->social_line = i;
                break;
            }
        }
    }
}

// Saves a game result to the file system.
// @param game_context pointer to a GameContext.
static void save_result(GameContext* game_context) {
    if(furi_mutex_acquire(game_context->mutex, FuriWaitForever) != FuriStatusOk) {
        return;
    }

    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);

    furi_string_printf(
        game_context->data->buffer,
        "%c%c\t%04d-%02d-%02dT%02d:%02d:%02d\t%s\t%s",
        game_context->data->local_player,
        game_context->data->remote_player,
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute,
        datetime.second,
        (game_context->data->remote_name) ? furi_string_get_cstr(game_context->data->remote_name) :
                                            "Unknown",
        (game_context->data->remote_contact) ?
            furi_string_get_cstr(game_context->data->remote_contact) :
            CONTACT_INFO_NONE);

    FURI_LOG_I(TAG, "Saving result: %s", furi_string_get_cstr(game_context->data->buffer));

    Storage* storage = furi_record_open(RECORD_STORAGE);
    ensure_dir_exists(storage);

    File* games_file = storage_file_alloc(storage);

    // Append contents to ending of games.txt (create if doesn't exist)
    if(storage_file_open(games_file, RPS_GAME_PATH, FSAM_WRITE, FSOM_OPEN_APPEND)) {
        FURI_LOG_T(TAG, "Opened file: %s", RPS_GAME_PATH);
        if(!storage_file_write(
               games_file,
               furi_string_get_cstr(game_context->data->buffer),
               furi_string_size(game_context->data->buffer))) {
            FURI_LOG_E(TAG, "Failed to write to file.");
        }
        storage_file_write(games_file, "\n", 1);
    } else {
        FURI_LOG_E(TAG, "Failed to open file: %s", RPS_GAME_PATH);
    }

    furi_string_printf(
        game_context->data->buffer,
        "%04d-%02d-%02dT%02d:%02d:%02d",
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute,
        datetime.second);

    update_player_stats(
        game_context,
        game_context->data->remote_player,
        furi_string_get_cstr(game_context->data->remote_name),
        furi_string_get_cstr(game_context->data->remote_contact),
        furi_string_get_cstr(game_context->data->buffer));

    storage_file_close(games_file);
    storage_file_free(games_file);
    furi_record_close(RECORD_STORAGE);

    furi_mutex_release(game_context->mutex);
}

static void update_player_stats(
    GameContext* game_context,
    GameState remote_player,
    const char* remote_name,
    const char* remote_contact,
    const char* datetime) {
    PlayerStats* stat = game_context->data->player_stats;

    FURI_LOG_I(TAG, "Searching for player: %s", remote_name);

    while(stat) {
        if(furi_string_cmp_str(stat->flipper_name, remote_name) == 0) {
            break;
        }
        stat = stat->next;
    }

    if(!stat) {
        FURI_LOG_I(TAG, "Not found player: %s", remote_name);
        stat = malloc(sizeof(PlayerStats));
        stat->loss_count += isLoss((GameState)remote_player) ? 1 : 0;
        stat->win_count += isWin((GameState)remote_player) ? 1 : 0;
        stat->tie_count += isTie((GameState)remote_player) ? 1 : 0;
        stat->next = NULL;
        stat->prev = NULL;
        stat->flipper_name = furi_string_alloc();
        furi_string_set_str(stat->flipper_name, remote_name);
        stat->contact = furi_string_alloc();
        furi_string_set_str(stat->contact, remote_contact);
        stat->last_played = furi_string_alloc();
        furi_string_set_str(stat->last_played, datetime);
        furi_string_set_char(stat->last_played, 10, ' ');
    } else {
        FURI_LOG_I(TAG, "Found player: %s", remote_name);
        stat->loss_count += isLoss((GameState)remote_player) ? 1 : 0;
        stat->win_count += isWin((GameState)remote_player) ? 1 : 0;
        stat->tie_count += isTie((GameState)remote_player) ? 1 : 0;
        furi_string_set_str(stat->contact, remote_contact);
        furi_string_set_str(stat->last_played, datetime);
        furi_string_set_char(stat->last_played, 10, ' ');
    }

    // Remove the stat from the list, if it is connected.
    if(game_context->data->player_stats && game_context->data->player_stats != stat) {
        if(stat->prev) {
            FURI_LOG_I(TAG, "Setting stat->prev->next.");
            stat->prev->next = stat->next;
        }
        if(stat->next) {
            FURI_LOG_I(TAG, "Setting stat->next->next.");
            stat->next->prev = stat->prev;
        }

        FURI_LOG_I(TAG, "Setting player_stats->prev.");
        stat->prev = NULL;
        game_context->data->player_stats->prev = stat;
        stat->next = game_context->data->player_stats;
        game_context->data->player_stats = stat;
    } else if(game_context->data->player_stats && game_context->data->player_stats == stat) {
        // We are already at the start of the list.
    } else {
        // This is the first stat.
        game_context->data->player_stats = stat;
    }

    FURI_LOG_I(
        TAG,
        "Added %s w:%d l:%d t:%d",
        furi_string_get_cstr(stat->flipper_name),
        stat->win_count,
        stat->loss_count,
        stat->tie_count);
}

static void load_player_stats(GameContext* game_context) {
    game_context->data->player_stats = NULL;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* games_file = storage_file_alloc(storage);

    if(storage_file_open(games_file, RPS_GAME_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Opened file: %s", RPS_GAME_PATH);

        while(!storage_file_eof(games_file)) {
            char ch;
            furi_string_reset(game_context->data->buffer);
            while(storage_file_read(games_file, &ch, 1) && !storage_file_eof(games_file)) {
                furi_string_push_back(game_context->data->buffer, ch);
                if(ch == '\n') {
                    break;
                }
            }

            char local_player;
            char remote_player;
            char datetime[20];
            char remote_name[32];
            char remote_contact[64];
            int parsed = sscanf(
                furi_string_get_cstr(game_context->data->buffer),
                "%c%c\t%s\t%s\t%s",
                &local_player,
                &remote_player,
                datetime,
                remote_name,
                remote_contact);

            if(parsed != 5) {
                FURI_LOG_I(
                    TAG,
                    "Failed to parse entry: %s count was %d",
                    furi_string_get_cstr(game_context->data->buffer),
                    parsed);
            } else {
                FURI_LOG_I(
                    TAG,
                    "Parsed entry: %c %c\t%s\t%s\t%s",
                    local_player,
                    remote_player,
                    datetime,
                    remote_name,
                    remote_contact);

                update_player_stats(
                    game_context, remote_player, remote_name, remote_contact, datetime);
            }
        }

        FURI_LOG_I(TAG, "Finished parsing file.");
    } else {
        FURI_LOG_E(TAG, "Failed to open file: %s", RPS_GAME_PATH);
    }

    storage_file_close(games_file);
    storage_file_free(games_file);
    furi_record_close(RECORD_STORAGE);
}

// This is the entry point for our application, which should match the application.fam file.
int32_t rock_paper_scissors_app(void* p) {
    UNUSED(p);

    // Configure our initial data.
    GameContext* game_context = malloc(sizeof(GameContext));
    game_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    game_context->data = malloc(sizeof(GameData));
    game_context->data->buffer = furi_string_alloc();
    game_context->data->game_number = 42;
    game_context->data->frequency_index = 10;
    game_context->data->local_move_tick = 0;
    game_context->data->remote_move_tick = 0;
    game_context->data->local_player = StateMainMenuMessage;
    game_context->data->remote_player = StateUnknown;
    game_context->data->screen_state = ScreenMainMenu;
    game_context->data->remote_games = NULL;
    game_context->data->local_contact = furi_string_alloc();
    furi_string_set(game_context->data->local_contact, CONTACT_INFO);
    game_context->data->keyboard_heading = furi_string_alloc();
    game_context->data->keyboard_data = furi_string_alloc();

    load_player_stats(game_context);
    load_local_contact(game_context);

    // Queue for events
    game_context->queue = furi_message_queue_alloc(8, sizeof(GameEvent));

    // Subghz worker.
    game_context->subghz_txrx = subghz_tx_rx_worker_alloc();
    subghz_devices_init();

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
                if((event.input.type == InputTypeLong) && (event.input.key == InputKeyBack)) {
                    // Long press back to exit.
                    processing = false;
                } else if(
                    (event.input.type == InputTypeShort) && (event.input.key == InputKeyBack) &&
                    game_context->data->screen_state != ScreenFinishedGame) {
                    if(game_context->data->local_player == StateHostingLookingForPlayer) {
                        rps_broadcast_not_beacon(game_context);
                    }

                    // Short press back to go back to main menu.
                    game_context->data->local_player = StateMainMenuHost;
                    game_context->data->remote_player = StateUnknown;
                    game_context->data->screen_state = ScreenMainMenu;
                } else if(
                    game_context->data->screen_state == ScreenHostGame &&
                    event.input.type == InputTypeShort) {
                    GameEvent newEvent = {
                        .type = GameEventLocalMove, .tick = furi_get_tick(), .move = MoveUnknown};
                    switch(event.input.key) {
                    case InputKeyUp:
                        if(game_context->data->local_player == StateHostingLookingForPlayer) {
                            rps_broadcast_not_beacon(game_context);
                        }
                        game_context->data->local_player = StateHostingSetFrequency;
                        break;
                    case InputKeyDown:
                        if(furi_hal_region_is_frequency_allowed(
                               frequency_list[game_context->data->frequency_index])) {
                            if(game_context->data->local_player == StateHostingLookingForPlayer) {
                                rps_broadcast_not_beacon(game_context);
                            }
                            game_context->data->local_player = StateHostingSetGameNumber;
                        }
                        break;
                    case InputKeyOk:
                        if(update_frequency(game_context)) {
                            game_context->data->local_player = StateHostingLookingForPlayer;
                        } else {
                            game_context->data->local_player = StateHostingBadFrequency;
                        }
                        break;
                    case InputKeyRight:
                        if(game_context->data->local_player == StateHostingSetGameNumber) {
                            if(game_context->data->game_number < 999) {
                                game_context->data->game_number++;
                            }
                        } else if(
                            game_context->data->local_player == StateHostingSetFrequency ||
                            game_context->data->local_player == StateHostingBadFrequency) {
                            if((uint8_t)(game_context->data->frequency_index + 1) <
                               sizeof(frequency_list) / sizeof(frequency_list[0])) {
                                game_context->data->frequency_index++;
                                if(furi_hal_region_is_frequency_allowed(
                                       frequency_list[game_context->data->frequency_index])) {
                                    game_context->data->local_player = StateHostingSetFrequency;
                                } else {
                                    game_context->data->local_player = StateHostingBadFrequency;
                                }
                            }
                        }
                        break;
                    case InputKeyLeft:
                        if(game_context->data->local_player == StateHostingSetGameNumber) {
                            if(game_context->data->game_number > 0) {
                                game_context->data->game_number--;
                            }
                        } else if(
                            game_context->data->local_player == StateHostingSetFrequency ||
                            game_context->data->local_player == StateHostingBadFrequency) {
                            if(game_context->data->frequency_index > 0) {
                                game_context->data->frequency_index--;
                                if(furi_hal_region_is_frequency_allowed(
                                       frequency_list[game_context->data->frequency_index])) {
                                    game_context->data->local_player = StateHostingSetFrequency;
                                } else {
                                    game_context->data->local_player = StateHostingBadFrequency;
                                }
                            }
                        }
                        break;

                    default:
                        FURI_LOG_T(TAG, "No support for key %d", event.input.key);
                        break;
                    }

                    if(newEvent.move != MoveUnknown) {
                        furi_message_queue_put(game_context->queue, &newEvent, FuriWaitForever);
                    }
                } else if(
                    game_context->data->screen_state == ScreenJoinGame &&
                    event.input.type == InputTypeShort) {
                    GameInfo* game;
                    GameEvent newEvent = {
                        .type = GameEventLocalMove, .tick = furi_get_tick(), .move = MoveUnknown};
                    switch(event.input.key) {
                    case InputKeyUp:
                        if(furi_hal_region_is_frequency_allowed(
                               frequency_list[game_context->data->frequency_index])) {
                            game_context->data->local_player = StateJoiningSetFrequency;
                        } else {
                            game_context->data->local_player = StateJoiningBadFrequency;
                        }
                        break;
                    case InputKeyDown:
                        if(furi_hal_region_is_frequency_allowed(
                               frequency_list[game_context->data->frequency_index])) {
                            game_context->data->local_player = StateJoiningSetGameNumber;
                        }
                        break;
                    case InputKeyOk:
                        game = remote_games_current(game_context);
                        if(game && update_frequency(game_context)) {
                            // We send "Join" when OK button clicked.

                            game_context->data->game_number = game->game_number;
                            if(furi_mutex_acquire(game_context->mutex, FuriWaitForever) ==
                               FuriStatusOk) {
                                game_context->data->local_player = StateHostingLookingForPlayer;
                                rps_broadcast_join(game_context);
                                furi_mutex_release(game_context->mutex);
                            } else {
                                FURI_LOG_E(TAG, "Failed to aquire mutex.");
                            }
                        } else if(game) {
                            game_context->data->local_player = StateJoiningBadFrequency;
                        }
                        break;
                    case InputKeyRight:
                        if(game_context->data->local_player == StateJoiningSetGameNumber) {
                            remote_games_next(game_context);
                        } else if(
                            game_context->data->local_player == StateJoiningSetFrequency ||
                            game_context->data->local_player == StateJoiningBadFrequency) {
                            if((uint8_t)(game_context->data->frequency_index + 1) <
                               sizeof(frequency_list) / sizeof(frequency_list[0])) {
                                game_context->data->frequency_index++;
                                if(furi_hal_region_is_frequency_allowed(
                                       frequency_list[game_context->data->frequency_index])) {
                                    game_context->data->local_player = StateJoiningSetFrequency;
                                } else {
                                    game_context->data->local_player = StateJoiningBadFrequency;
                                }
                                remote_games_clear(game_context);
                                update_frequency(game_context);
                            }
                        }
                        break;
                    case InputKeyLeft:
                        if(game_context->data->local_player == StateJoiningSetGameNumber) {
                            remote_games_previous(game_context);
                        } else if(
                            game_context->data->local_player == StateJoiningSetFrequency ||
                            game_context->data->local_player == StateJoiningBadFrequency) {
                            if(game_context->data->frequency_index > 0) {
                                game_context->data->frequency_index--;
                                if(furi_hal_region_is_frequency_allowed(
                                       frequency_list[game_context->data->frequency_index])) {
                                    game_context->data->local_player = StateJoiningSetFrequency;
                                } else {
                                    game_context->data->local_player = StateJoiningBadFrequency;
                                }
                                remote_games_clear(game_context);
                                update_frequency(game_context);
                            }
                        }
                        break;

                    default:
                        FURI_LOG_T(TAG, "No support for key %d", event.input.key);
                        break;
                    }

                    if(newEvent.move != MoveUnknown) {
                        furi_message_queue_put(game_context->queue, &newEvent, FuriWaitForever);
                    }
                } else if(
                    game_context->data->screen_state == ScreenPlayingGame &&
                    event.input.type == InputTypeShort) {
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
                    default:
                        FURI_LOG_T(TAG, "No support for key %d", event.input.key);
                        break;
                    }

                    if(newEvent.move != MoveUnknown) {
                        furi_message_queue_put(game_context->queue, &newEvent, FuriWaitForever);
                    }
                } else if(
                    game_context->data->screen_state == ScreenError &&
                    event.input.type == InputTypeShort) {
                    switch(event.input.key) {
                    case InputKeyOk:
                        game_context->data->local_player = StateMainMenuHost;
                        game_context->data->remote_player = StateUnknown;
                        game_context->data->screen_state = ScreenMainMenu;
                        break;
                    default:
                        FURI_LOG_T(TAG, "No support for key %d", event.input.key);
                        break;
                    }
                } else if(
                    game_context->data->screen_state == ScreenEditMessage &&
                    event.input.type == InputTypeShort) {
                    char ch;
                    switch(event.input.key) {
                    case InputKeyLeft:
                        if(game_context->data->keyboard_col) {
                            game_context->data->keyboard_col--;
                        } else {
                            game_context->data->keyboard_col = 13;
                        }
                        break;

                    case InputKeyRight:
                        if(game_context->data->keyboard_col < 13) {
                            game_context->data->keyboard_col++;
                        } else {
                            game_context->data->keyboard_col = 0;
                        }
                        break;

                    case InputKeyUp:
                        if(game_context->data->keyboard_row) {
                            game_context->data->keyboard_row--;
                        } else {
                            game_context->data->keyboard_row = 3;
                        }
                        break;

                    case InputKeyDown:
                        if(game_context->data->keyboard_row < 3) {
                            game_context->data->keyboard_row++;
                        } else {
                            game_context->data->keyboard_row = 0;
                        }
                        break;

                    case InputKeyOk:
                        ch = get_char(game_context, true);
                        if(ch >= ' ' && ch <= '~') {
                            int len = furi_string_utf8_length(game_context->data->keyboard_data);
                            if(len < KEYBOARD_MAX_LEN) {
                                furi_string_push_back(game_context->data->keyboard_data, ch);
                            } else {
                                single_vibro();
                            }
                        } else if(ch == KEYBOARD_BACKSPACE) {
                            int len = furi_string_utf8_length(game_context->data->keyboard_data);
                            if(len > 0) {
                                furi_string_left(game_context->data->keyboard_data, len - 1);
                            }
                        } else if(ch == KEYBOARD_ENTER) {
                            furi_string_printf(
                                game_context->data->local_contact,
                                "%c%s",
                                contact_list[game_context->data->social_line][0],
                                furi_string_get_cstr(game_context->data->keyboard_data));
                            save_local_contact(game_context);
                            game_context->data->screen_state = ScreenMainMenu;
                        }
                        break;

                    default:
                        break;
                    }

                } else if(
                    game_context->data->screen_state == ScreenEditMessage &&
                    event.input.type == InputTypeLong) {
                    char ch;
                    switch(event.input.key) {
                    case InputKeyOk:
                        ch = get_char(game_context, false);
                        if(ch >= ' ' && ch <= '~') {
                            int len = furi_string_utf8_length(game_context->data->keyboard_data);
                            if(len < KEYBOARD_MAX_LEN) {
                                furi_string_push_back(game_context->data->keyboard_data, ch);
                            } else {
                                single_vibro();
                            }
                        } else if(ch == KEYBOARD_BACKSPACE) {
                            furi_string_left(game_context->data->keyboard_data, 0);
                        }
                        break;

                    default:
                        break;
                    }
                } else if(
                    game_context->data->screen_state == ScreenChooseSocial &&
                    event.input.type == InputTypeShort) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        if(game_context->data->social_line) {
                            game_context->data->social_line--;
                        } else {
                            single_vibro();
                        }
                        break;

                    case InputKeyDown:
                        if(++game_context->data->social_line >= (int)COUNT_OF(contact_list)) {
                            game_context->data->social_line--;
                            single_vibro();
                        }
                        break;

                    case InputKeyOk:
                        if(game_context->data->social_line == 0) {
                            furi_string_set(game_context->data->local_contact, CONTACT_INFO_NONE);
                            save_local_contact(game_context);
                            game_context->data->screen_state = ScreenMainMenu;
                        } else {
                            furi_string_set(
                                game_context->data->keyboard_heading,
                                contact_list[game_context->data->social_line] + 1);
                            game_context->data->keyboard_row = 3;
                            game_context->data->keyboard_col = 13;
                            load_keyboard_data(game_context);
                            game_context->data->screen_state = ScreenEditMessage;
                        }
                        break;

                    default:
                        break;
                    }
                } else if(
                    game_context->data->screen_state == ScreenMainMenu &&
                    event.input.type == InputTypeShort) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        if(game_context->data->local_player == StateMainMenuJoin) {
                            game_context->data->local_player = StateMainMenuHost;
                        } else if(game_context->data->local_player == StateMainMenuPastGames) {
                            game_context->data->local_player = StateMainMenuJoin;
                        } else if(game_context->data->local_player == StateMainMenuHost) {
                            game_context->data->local_player = StateMainMenuMessage;
                        }
                        break;
                    case InputKeyDown:
                        if(game_context->data->local_player == StateMainMenuHost) {
                            game_context->data->local_player = StateMainMenuJoin;
                        } else if(game_context->data->local_player == StateMainMenuJoin) {
                            game_context->data->local_player = StateMainMenuPastGames;
                        } else if(game_context->data->local_player == StateMainMenuMessage) {
                            game_context->data->local_player = StateMainMenuHost;
                        }
                        break;
                    case InputKeyOk:
                        if(game_context->data->local_player == StateMainMenuHost) {
                            if(furi_hal_region_is_frequency_allowed(
                                   frequency_list[game_context->data->frequency_index])) {
                                game_context->data->local_player = StateHostingSetFrequency;
                            } else {
                                game_context->data->local_player = StateHostingBadFrequency;
                            }
                            game_context->data->remote_player = StateUnknown;
                            game_context->data->local_move_tick = furi_get_tick();
                            game_context->data->remote_move_tick = furi_get_tick();
                            game_context->data->screen_state = ScreenHostGame;
                        } else if(game_context->data->local_player == StateMainMenuJoin) {
                            if(furi_hal_region_is_frequency_allowed(
                                   frequency_list[game_context->data->frequency_index])) {
                                game_context->data->local_player = StateJoiningSetFrequency;
                                update_frequency(game_context);
                            } else {
                                game_context->data->local_player = StateJoiningBadFrequency;
                            }
                            remote_games_clear(game_context);
                            game_context->data->remote_player = StateUnknown;
                            game_context->data->local_move_tick = furi_get_tick();
                            game_context->data->remote_move_tick = furi_get_tick();
                            game_context->data->screen_state = ScreenJoinGame;
                        } else if(game_context->data->local_player == StateMainMenuPastGames) {
                            game_context->data->viewing_player_stats =
                                game_context->data->player_stats;
                            game_context->data->screen_state = ScreenPastGames;
                        } else if(game_context->data->local_player == StateMainMenuMessage) {
                            game_context->data->screen_state = ScreenChooseSocial;
                        }
                        break;
                    default:
                        FURI_LOG_T(TAG, "No support for key %d", event.input.key);
                        break;
                    }
                } else if(
                    game_context->data->screen_state == ScreenFinishedGame &&
                    event.input.type == InputTypeShort) {
                    switch(event.input.key) {
                    case InputKeyLeft:
                        game_context->data->keyboard_col = 0; // YES - Play again
                        break;
                    case InputKeyRight:
                        game_context->data->keyboard_col = 1; // NO - Back to main menu
                        break;
                    case InputKeyBack:
                        game_context->data->keyboard_col = 1; // NO - Back to main menu
                        rps_broadcast_quit(game_context);
                        game_context->data->local_player = StateMainMenuHost;
                        game_context->data->remote_player = StateUnknown;
                        game_context->data->screen_state = ScreenMainMenu;
                        break;
                    case InputKeyOk:
                        if(game_context->data->keyboard_col == 1) {
                            rps_broadcast_quit(game_context);
                            game_context->data->local_player = StateMainMenuHost;
                            game_context->data->remote_player = StateUnknown;
                            game_context->data->screen_state = ScreenMainMenu;
                        } else {
                            rps_broadcast_play_again(game_context);
                            game_context->data->remote_player = StateReady;
                            game_context->data->remote_move_tick = furi_get_tick();
                            game_context->data->local_player = StateReady;
                            game_context->data->local_move_tick = furi_get_tick();
                            game_context->data->screen_state = ScreenPlayingGame;
                        }
                        break;
                    default:
                        break;
                    }
                } else if(
                    game_context->data->screen_state == ScreenPastGames &&
                    event.input.type == InputTypeShort) {
                    switch(event.input.key) {
                    case InputKeyLeft:
                        if(game_context->data->viewing_player_stats) {
                            if(game_context->data->viewing_player_stats->prev) {
                                game_context->data->viewing_player_stats =
                                    game_context->data->viewing_player_stats->prev;
                                FURI_LOG_I(
                                    TAG,
                                    "Moved to item %s.",
                                    furi_string_get_cstr(
                                        game_context->data->viewing_player_stats->flipper_name));
                            } else {
                                FURI_LOG_I(TAG, "Viewing first item in list.");
                            }
                        }
                        break;
                    case InputKeyRight:
                        if(game_context->data->viewing_player_stats) {
                            if(game_context->data->viewing_player_stats->next) {
                                game_context->data->viewing_player_stats =
                                    game_context->data->viewing_player_stats->next;
                                FURI_LOG_I(
                                    TAG,
                                    "Moved to item %s.",
                                    furi_string_get_cstr(
                                        game_context->data->viewing_player_stats->flipper_name));
                            } else {
                                FURI_LOG_I(TAG, "Viewing last item in list.");
                            }
                        }
                        break;
                    default:
                        FURI_LOG_T(TAG, "No support for key %d", event.input.key);
                        break;
                    }
                }
                break;
            case GameEventPlaySong:
                save_result(game_context);
                play_song(game_context);
                break;
            case GameEventSongEnded:
                if(isResult(game_context->data->local_player)) {
                    game_context->data->keyboard_col = 0; // YES
                    game_context->data->screen_state = ScreenFinishedGame;
                }
                break;
            case GameEventDataDetected:
                rps_receive_data(game_context, event.tick);
                break;
            case GameEventTypeTimer:
                if(furi_mutex_acquire(game_context->mutex, FuriWaitForever) == FuriStatusOk) {
                    if(StateHostingLookingForPlayer == game_context->data->local_player &&
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
                FURI_LOG_I(TAG, "Remote beacon detected. game number %03u", event.game_number);
                remote_games_add(game_context, &event);
                break;
            case GameEventRemoteNotBeacon:
                FURI_LOG_I(TAG, "Remote not beacon detected. game number %03u", event.game_number);
                remote_games_remove(game_context, &event);
                break;
            case GameEventRemotePlayAgain:
                game_context->data->remote_player = StateReady;
                game_context->data->remote_move_tick = furi_get_tick();
                game_context->data->local_player = StateReady;
                game_context->data->local_move_tick = furi_get_tick();
                game_context->data->screen_state = ScreenPlayingGame;
                break;
            case GameEventRemoteQuit:
                game_context->data->local_player = StateMainMenuHost;
                game_context->data->remote_player = StateUnknown;
                game_context->data->screen_state = ScreenMainMenu;
                break;
            case GameEventRemoteJoined:
                if(furi_mutex_acquire(game_context->mutex, FuriWaitForever) == FuriStatusOk) {
                    if(event.game_number == game_context->data->game_number) {
                        if(rps_state_machine_remote_joined(game_context)) {
                            rps_broadcast_join_acknowledge(game_context);
                            if(game_context->data->remote_name) {
                                furi_string_free(game_context->data->remote_name);
                            }
                            game_context->data->remote_name = event.sender_name;
                            if(game_context->data->remote_contact) {
                                furi_string_free(game_context->data->remote_contact);
                            }
                            game_context->data->remote_contact = event.sender_contact;
                            // Take ownership of the name and contact
                            event.sender_name = NULL;
                            event.sender_contact = NULL;
                        } else {
                            remote_games_remove(game_context, &event);
                        }
                    } else {
                        FURI_LOG_T(
                            TAG,
                            "Remote joining another Flipper on game %03u.",
                            event.game_number);
                        remote_games_remove(game_context, &event);
                    }
                    furi_mutex_release(game_context->mutex);
                } else {
                    FURI_LOG_E(TAG, "Failed to aquire mutex.");
                }
                break;
            case GameEventRemoteJoinAcknowledged:
                if(furi_mutex_acquire(game_context->mutex, FuriWaitForever) == FuriStatusOk) {
                    if(event.game_number == game_context->data->game_number) {
                        FURI_LOG_I(TAG, "Remote join acknowledged.");
                        if(game_context->data->remote_name) {
                            furi_string_free(game_context->data->remote_name);
                        }
                        game_context->data->remote_name = event.sender_name;
                        if(game_context->data->remote_contact) {
                            furi_string_free(game_context->data->remote_contact);
                        }
                        game_context->data->remote_contact = event.sender_contact;
                        game_context->data->remote_player = StateReady;
                        game_context->data->remote_move_tick = furi_get_tick();
                        game_context->data->local_player = StateReady;
                        game_context->data->local_move_tick = furi_get_tick();
                        game_context->data->screen_state = ScreenPlayingGame;
                        // Take ownership of the name and contact
                        event.sender_name = NULL;
                        event.sender_contact = NULL;
                    } else {
                        FURI_LOG_T(
                            TAG,
                            "Remote join acknowledge another Flipper on game %03u.",
                            event.game_number);
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
            if(event.sender_name) {
                furi_string_free(event.sender_name);
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
    subghz_devices_deinit();
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(game_context->queue);
    furi_mutex_free(game_context->mutex);
    furi_string_free(game_context->data->buffer);
    furi_string_free(game_context->data->local_contact);
    if(game_context->data->remote_name) {
        furi_string_free(game_context->data->remote_name);
    }
    if(game_context->data->remote_contact) {
        furi_string_free(game_context->data->remote_contact);
    }
    remote_games_clear(game_context);
    free(game_context->data);
    free(game_context);

    // Reenable charging.
    furi_hal_power_suppress_charge_exit();

    return 0;
}