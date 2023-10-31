#include <furi.h>
#include <furi_hal_random.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <stdlib.h>
#include "passgen_icons.h"

#define PASSGEN_MAX_LENGTH 16
#define PASSGEN_CHARACTERS_LENGTH (26 * 4)

#define PASSGEN_DIGITS "0123456789"
#define PASSGEN_LETTERS_LOW "abcdefghijklmnopqrstuvwxyz"
#define PASSGEN_LETTERS_UP "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define PASSGEN_SPECIAL "!#$%^&*.-_"

typedef enum PassGen_Alphabet {
    Digits = 1,
    Lowercase = 2,

    Uppercase = 4,
    Special = 8,

    DigitsLower = Digits | Lowercase,
    DigitsAllLetters = Digits | Lowercase | Uppercase,
    Mixed = DigitsAllLetters | Special
} PassGen_Alphabet;

const char* const PassGen_AlphabetChars[16] = {
    "0", // invalid value
    /*    PASSGEN_SPECIAL    PASSGEN_LETTERS_UP    PASSGEN_LETTERS_LOW */ PASSGEN_DIGITS,
    /*    PASSGEN_SPECIAL    PASSGEN_LETTERS_UP */ PASSGEN_LETTERS_LOW /* PASSGEN_DIGITS */,
    /*    PASSGEN_SPECIAL    PASSGEN_LETTERS_UP */ PASSGEN_LETTERS_LOW PASSGEN_DIGITS,
    /*    PASSGEN_SPECIAL */ PASSGEN_LETTERS_UP /* PASSGEN_LETTERS_LOW    PASSGEN_DIGITS */,
    /*    PASSGEN_SPECIAL */ PASSGEN_LETTERS_UP /* PASSGEN_LETTERS_LOW */ PASSGEN_DIGITS,
    /*    PASSGEN_SPECIAL */ PASSGEN_LETTERS_UP PASSGEN_LETTERS_LOW /* PASSGEN_DIGITS */,
    /*    PASSGEN_SPECIAL */ PASSGEN_LETTERS_UP PASSGEN_LETTERS_LOW PASSGEN_DIGITS,
    PASSGEN_SPECIAL /* PASSGEN_LETTERS_UP    PASSGEN_LETTERS_LOW    PASSGEN_DIGITS */,
    PASSGEN_SPECIAL /* PASSGEN_LETTERS_UP    PASSGEN_LETTERS_LOW */ PASSGEN_DIGITS,
    PASSGEN_SPECIAL /* PASSGEN_LETTERS_UP */ PASSGEN_LETTERS_LOW /* PASSGEN_DIGITS */,
    PASSGEN_SPECIAL /* PASSGEN_LETTERS_UP */ PASSGEN_LETTERS_LOW PASSGEN_DIGITS,
    PASSGEN_SPECIAL PASSGEN_LETTERS_UP /* PASSGEN_LETTERS_LOW    PASSGEN_DIGITS */,
    PASSGEN_SPECIAL PASSGEN_LETTERS_UP /* PASSGEN_LETTERS_LOW */ PASSGEN_DIGITS,
    PASSGEN_SPECIAL PASSGEN_LETTERS_UP PASSGEN_LETTERS_LOW /* PASSGEN_DIGITS */,
    PASSGEN_SPECIAL PASSGEN_LETTERS_UP PASSGEN_LETTERS_LOW PASSGEN_DIGITS,
};

const int AlphabetLevels[] = {Digits, Lowercase, DigitsLower, DigitsAllLetters, Mixed};
const char* AlphabetLevelNames[] = {"1234", "abcd", "ab12", "Ab12", "Ab1#"};
const int AlphabetLevelsCount = sizeof(AlphabetLevels) / sizeof(int);

const NotificationSequence PassGen_Alert_vibro = {
    &message_vibro_on,
    &message_blue_255,
    &message_delay_50,
    &message_vibro_off,
    NULL,
};

typedef struct {
    FuriMessageQueue* input_queue;
    ViewPort* view_port;
    Gui* gui;
    FuriMutex** mutex;
    NotificationApp* notify;
    const char* alphabet;
    char password[PASSGEN_MAX_LENGTH + 1];
    int length; // must be <= PASSGEN_MAX_LENGTH
    int level;
} PassGen;

void state_free(PassGen* app) {
    // NOTE: would have preferred if a "safe" memset() was available...
    //       but, since cannot prevent optimization from removing
    //       memset(), fill with random data instead.
    furi_hal_random_fill_buf((void*)(app->password), PASSGEN_MAX_LENGTH);

    gui_remove_view_port(app->gui, app->view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(app->view_port);
    furi_message_queue_free(app->input_queue);
    furi_mutex_free(app->mutex);
    furi_record_close(RECORD_NOTIFICATION);
    free(app);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    PassGen* app = ctx;
    if(input_event->type == InputTypeShort) {
        furi_message_queue_put(app->input_queue, input_event, 0);
    }
}

static void render_callback(Canvas* canvas, void* ctx) {
    char str_length[8];
    PassGen* app = ctx;
    furi_check(furi_mutex_acquire(app->mutex, FuriWaitForever) == FuriStatusOk);

    canvas_clear(canvas);
    canvas_draw_box(canvas, 0, 0, 128, 14);
    canvas_set_color(canvas, ColorWhite);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 11, "Password Generator");

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str_aligned(canvas, 64, 35, AlignCenter, AlignCenter, app->password);

    // Navigation menu:
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 96, 52, &I_Pin_back_arrow_10x8);
    canvas_draw_str(canvas, 108, 60, "Exit");

    canvas_draw_icon(canvas, 54, 52, &I_Vertical_arrow_7x9);
    canvas_draw_str(canvas, 64, 60, AlphabetLevelNames[app->level]);

    snprintf(str_length, sizeof(str_length), "Len: %d", app->length);
    canvas_draw_icon(canvas, 4, 53, &I_Horizontal_arrow_9x7);
    canvas_draw_str(canvas, 15, 60, str_length);

    furi_mutex_release(app->mutex);
}

void build_alphabet(PassGen* app) {
    PassGen_Alphabet mode = AlphabetLevels[app->level];
    if(mode > 0 && mode < 16) {
        app->alphabet = PassGen_AlphabetChars[mode];
    } else {
        app->alphabet =
            PassGen_AlphabetChars[0]; // Invalid mode ... password will be all zero digits
    }
}

PassGen* state_init() {
    PassGen* app = malloc(sizeof(PassGen));
    _Static_assert(8 <= PASSGEN_MAX_LENGTH, "app->length must be set <= PASSGEN_MAX_LENGTH");
    app->length = 8;
    app->level = 2;
    build_alphabet(app);
    app->input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->view_port = view_port_alloc();
    app->gui = furi_record_open(RECORD_GUI);
    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    view_port_input_callback_set(app->view_port, input_callback, app);
    view_port_draw_callback_set(app->view_port, render_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->notify = furi_record_open(RECORD_NOTIFICATION);

    return app;
}

void generate(PassGen* app) {
    memset(app->password, 0, PASSGEN_MAX_LENGTH + 1);

    int char_option_count = strlen(app->alphabet);
    if(char_option_count < 0) {
        return;
    }

    // determine largest character value that avoids bias
    char ceil = CHAR_MAX - (CHAR_MAX % char_option_count) - 1;

    // iteratively fill the password buffer with random values
    // then keep only values that are in-range (no bias)
    void* remaining_buffer = app->password;
    size_t remaining_length = (app->length * sizeof(char));

    while(remaining_length != 0) {
        // fewer calls to hardware TRNG is more efficient
        furi_hal_random_fill_buf(remaining_buffer, remaining_length);

        // keep only values that are in-range (no bias)
        char* target = remaining_buffer;
        char* source = remaining_buffer;
        size_t valid_count = 0;

        for(size_t i = 0; i < remaining_length; i++) {
            int v = *source;
            // if the generated random value is in range, keep it
            if(v < ceil) {
                v %= char_option_count;
                *target = app->alphabet[v];
                // increment target pointer and count of valid items found
                target++;
                valid_count++;
            }
            // always increment the source pointer
            source++;
        }
        remaining_length -= valid_count;
        remaining_buffer = target;
    }
}

void update_password(PassGen* app, bool vibro) {
    generate(app);

    if(vibro)
        notification_message(app->notify, &PassGen_Alert_vibro);
    else
        notification_message(app->notify, &sequence_blink_blue_100);
    view_port_update(app->view_port);
}

int32_t passgenapp(void) {
    PassGen* app = state_init();
    generate(app);

    while(1) {
        InputEvent input;
        while(furi_message_queue_get(app->input_queue, &input, FuriWaitForever) == FuriStatusOk) {
            furi_check(furi_mutex_acquire(app->mutex, FuriWaitForever) == FuriStatusOk);

            if(input.type == InputTypeShort) {
                switch(input.key) {
                case InputKeyBack:
                    furi_mutex_release(app->mutex);
                    state_free(app);
                    return 0;
                case InputKeyDown:
                    if(app->level > 0) {
                        app->level--;
                        build_alphabet(app);
                        update_password(app, false);
                    } else
                        notification_message(app->notify, &sequence_blink_red_100);
                    break;
                case InputKeyUp:
                    if(app->level < AlphabetLevelsCount - 1) {
                        app->level++;
                        build_alphabet(app);
                        update_password(app, false);
                    } else
                        notification_message(app->notify, &sequence_blink_red_100);
                    break;
                case InputKeyLeft:
                    if(app->length > 1) {
                        app->length--;
                        update_password(app, false);
                    } else
                        notification_message(app->notify, &sequence_blink_red_100);
                    break;
                case InputKeyRight:
                    if(app->length < PASSGEN_MAX_LENGTH) {
                        app->length++;
                        update_password(app, false);
                    } else
                        notification_message(app->notify, &sequence_blink_red_100);
                    break;
                case InputKeyOk:
                    update_password(app, true);
                    break;
                default:
                    break;
                }
            }
            furi_mutex_release(app->mutex);
        }
    }
    state_free(app);
    return 0;
}