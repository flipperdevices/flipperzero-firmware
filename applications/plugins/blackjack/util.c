#include "util.h"

const char *CONFIG_FILE_PATH = EXT_PATH(".blackjack.settings");

static QueueItem *afterDelay;

float lerp(float v0, float v1, float t) {
    if (t > 1) return v1;
    return (1 - t) * v0 + t * v1;
}

Vector lerp_2d(Vector start, Vector end, float t) {
    return (Vector) {
            lerp(start.x, end.x, t),
            lerp(start.y, end.y, t),
    };
}

Vector quadratic_2d(Vector start, Vector control, Vector end, float t) {
    return lerp_2d(
            lerp_2d(start, control, t),
            lerp_2d(control, end, t),
            t
    );
}


void queue(GameState *game_state,
           void (*callback)(GameState *game_state),
           void (*start)(GameState *game_state),
           void (*processing)(const GameState *gameState, Canvas *const canvas,uint32_t duration,uint32_t margin),
           uint32_t duration,uint32_t margin
) {
    if (afterDelay == NULL) {
        game_state->animationStart = game_state->last_tick;
        afterDelay = malloc(sizeof(QueueItem));
        afterDelay->callback = callback;
        afterDelay->processing = processing;
        afterDelay->start = start;
        afterDelay->duration = duration;
        afterDelay->margin = margin;
        afterDelay->next = NULL;
    } else {
        QueueItem *next = afterDelay;
        while (next->next != NULL) { next = (QueueItem *) (next->next); }
        next->next = malloc(sizeof(QueueItem));
        ((QueueItem *) next->next)->callback = callback;
        ((QueueItem *) next->next)->processing = processing;
        ((QueueItem *) next->next)->start = start;
        ((QueueItem *) next->next)->duration = duration;
        ((QueueItem *) next->next)->margin = margin;
        ((QueueItem *) next->next)->next = NULL;
    }
}

void queue_clear() {
    while (afterDelay != NULL) {
        QueueItem *f = afterDelay;
        free(f);
        afterDelay = f->next;
    }
}

void dequeue(GameState *game_state) {
    ((QueueItem *) afterDelay)->callback(game_state);
    QueueItem *f = afterDelay;
    afterDelay = (QueueItem *) afterDelay->next;
    free(f);
    if (afterDelay != NULL && afterDelay->start != NULL)afterDelay->start(game_state);
    game_state->animationStart = game_state->last_tick;
}

void animateQueue(const GameState *game_state, Canvas *const canvas) {
    if (afterDelay != NULL && ((QueueItem *) afterDelay)->processing != NULL) {
        ((QueueItem *) afterDelay)->processing(game_state, canvas, afterDelay->duration, afterDelay->margin);
    }
}

bool run_queue(GameState *game_state) {
    if (afterDelay != NULL) {
        game_state->animating = true;
        if ((game_state->last_tick - game_state->animationStart) > afterDelay->duration) {
            dequeue(game_state);
        }
        return true;
    }
    game_state->animating = false;
    return false;
}

void save_settings(Settings settings) {
    Storage *storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat *file = flipper_format_file_alloc(storage);
    FURI_LOG_D(APP_NAME, "Saving config");
    if (flipper_format_file_open_existing(file, CONFIG_FILE_PATH)) {
        FURI_LOG_D(APP_NAME, "Saving %s: %ld", CONF_ANIMATION_DURATION, settings.animation_duration);
        flipper_format_update_uint32(file, CONF_ANIMATION_DURATION, &(settings.animation_duration), 1);

        FURI_LOG_D(APP_NAME, "Saving %s: %ld", CONF_ANIMATION_MARGIN, settings.animation_margin);
        flipper_format_update_uint32(file, CONF_ANIMATION_MARGIN, &(settings.animation_margin), 1);

        FURI_LOG_D(APP_NAME, "Saving %s: %ld", CONF_MESSAGE_DURATION, settings.message_duration);
        flipper_format_update_uint32(file, CONF_MESSAGE_DURATION, &(settings.message_duration), 1);

        FURI_LOG_D(APP_NAME, "Saving %s: %ld", CONF_STARTING_MONEY, settings.starting_money);
        flipper_format_update_uint32(file, CONF_STARTING_MONEY, &(settings.starting_money), 1);

        FURI_LOG_D(APP_NAME, "Saving %s: %ld", CONF_ROUND_PRICE, settings.round_price);
        flipper_format_update_uint32(file, CONF_ROUND_PRICE, &(settings.round_price), 1);

        FURI_LOG_D(APP_NAME, "Saving %s: %i", CONF_SOUND_EFFECTS, settings.sound_effects?1:0);
        flipper_format_update_bool(file, CONF_SOUND_EFFECTS, &(settings.sound_effects), 1);
        FURI_LOG_D(APP_NAME, "Config saved");
    }else{
        FURI_LOG_E(APP_NAME, "Save error");
    }
    flipper_format_file_close(file);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}

void save_settings_file(FlipperFormat *file, Settings *settings) {
    flipper_format_write_header_cstr(file, CONFIG_FILE_HEADER, CONFIG_FILE_VERSION);
    flipper_format_write_comment_cstr(file, "Card animation duration in ms");
    flipper_format_write_uint32(file, CONF_ANIMATION_DURATION, &(settings->animation_duration), 1);
    flipper_format_write_comment_cstr(file,
                                      "Card animation margin in ms (how long the card will stay besides the played hand)");
    flipper_format_write_uint32(file, CONF_ANIMATION_MARGIN, &(settings->animation_margin), 1);
    flipper_format_write_comment_cstr(file, "Popup message duration in ms");
    flipper_format_write_uint32(file, CONF_MESSAGE_DURATION, &(settings->message_duration), 1);
    flipper_format_write_comment_cstr(file, "Player's starting money");
    flipper_format_write_uint32(file, CONF_STARTING_MONEY, &(settings->starting_money), 1);
    flipper_format_write_comment_cstr(file, "Round price");
    flipper_format_write_uint32(file, CONF_ROUND_PRICE, &(settings->round_price), 1);
    flipper_format_write_comment_cstr(file, "Enable sound effects");
    flipper_format_write_bool(file, CONF_SOUND_EFFECTS, &(settings->sound_effects), 1);
}

Settings load_settings() {
    Settings settings;

    FURI_LOG_D(APP_NAME, "Loading default settings");
    settings.animation_margin = 200;
    settings.animation_duration = 1500;
    settings.message_duration = 1500;
    settings.starting_money = 200;
    settings.round_price = 10;
    settings.sound_effects = true;

    FURI_LOG_D(APP_NAME, "Opening storage");
    Storage *storage = furi_record_open(RECORD_STORAGE);
    FURI_LOG_D(APP_NAME, "Allocating file");
    FlipperFormat *file = flipper_format_file_alloc(storage);

    FURI_LOG_D(APP_NAME, "Allocating string");
    FuriString *string_value;
    string_value = furi_string_alloc();

    if (storage_common_stat(storage, CONFIG_FILE_PATH, NULL) != FSE_OK) {
        FURI_LOG_D(APP_NAME, "Config file %s not found, creating new one...", CONFIG_FILE_PATH);
        if (!flipper_format_file_open_new(file, CONFIG_FILE_PATH)) {
            FURI_LOG_E(APP_NAME, "Error creating new file %s", CONFIG_FILE_PATH);
            flipper_format_file_close(file);
        } else {
            save_settings_file(file, &settings);
        }
    } else {
        if (!flipper_format_file_open_existing(file, CONFIG_FILE_PATH)) {
            FURI_LOG_E(APP_NAME, "Error opening existing file %s", CONFIG_FILE_PATH);
            flipper_format_file_close(file);
        } else {
            uint32_t value;
            bool valueBool;
            FURI_LOG_D(APP_NAME, "Checking version");
            if (!flipper_format_read_header(file, string_value, &value)) {
                FURI_LOG_E(APP_NAME, "Config file mismatch");
            } else {
                FURI_LOG_D(APP_NAME, "Loading %s", CONF_ANIMATION_DURATION);
                if (flipper_format_read_uint32(file, CONF_ANIMATION_DURATION, &value, 1)) {
                    settings.animation_duration = value;
                    FURI_LOG_D(APP_NAME, "Loaded %s: %ld", CONF_ANIMATION_DURATION, value);
                }
                FURI_LOG_D(APP_NAME, "Loading %s", CONF_ANIMATION_MARGIN);
                if (flipper_format_read_uint32(file, CONF_ANIMATION_MARGIN, &value, 1)) {
                    settings.animation_margin = value;
                    FURI_LOG_D(APP_NAME, "Loaded %s: %ld", CONF_ANIMATION_MARGIN, value);
                }
                FURI_LOG_D(APP_NAME, "Loading %s", CONF_MESSAGE_DURATION);
                if (flipper_format_read_uint32(file, CONF_MESSAGE_DURATION, &value, 1)) {
                    settings.message_duration = value;
                    FURI_LOG_D(APP_NAME, "Loaded %s: %ld", CONF_MESSAGE_DURATION, value);
                }
                FURI_LOG_D(APP_NAME, "Loading %s", CONF_STARTING_MONEY);
                if (flipper_format_read_uint32(file, CONF_STARTING_MONEY, &value, 1)) {
                    settings.starting_money = value;
                    FURI_LOG_D(APP_NAME, "Loaded %s: %ld", CONF_STARTING_MONEY, value);
                }
                FURI_LOG_D(APP_NAME, "Loading %s", CONF_ROUND_PRICE);
                if (flipper_format_read_uint32(file, CONF_ROUND_PRICE, &value, 1)) {
                    settings.round_price = value;
                    FURI_LOG_D(APP_NAME, "Loaded %s: %ld", CONF_ROUND_PRICE, value);
                }
                FURI_LOG_D(APP_NAME, "Loading %s", CONF_SOUND_EFFECTS);
                if (flipper_format_read_bool(file, CONF_SOUND_EFFECTS, &valueBool, 1)) {
                    settings.sound_effects = valueBool;
                    FURI_LOG_D(APP_NAME, "Loaded %s: %i", CONF_ROUND_PRICE, valueBool?1:0);
                }
            }
            flipper_format_file_close(file);
        }
    }

    furi_string_free(string_value);
//        flipper_format_file_close(file);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
    return settings;
}