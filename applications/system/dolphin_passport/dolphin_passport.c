#include "assets_icons.h"
#include <core/check.h>
#include <core/record.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <gui/canvas_i.h>
#include <furi_hal_version.h>
#include "dolphin/dolphin.h"
#include "dolphin/helpers/dolphin_state.h"
#include "applications/settings/desktop_settings/desktop_settings_app.h"
#include "math.h"

#define MOODS_TOTAL 1
#define BUTTHURT_MAX 14

typedef enum { EventGameTick, EventKeyPress } EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PassportEvent;

typedef struct {
    Dolphin* dolphin;
    DolphinStats stats;

    DesktopSettings* desktop_settings;

    uint32_t xp_to_levelup;
    uint32_t xp_above_last_levelup;
    uint32_t xp_for_current_level;
    char xp_str[14];
    double xp_progress;
    int xp_fill;

    int max_level;
    uint16_t tmpLvl;
    char level_str[12];

    char my_name[8];

    char mood_str[20];
    uint8_t mood;
    uint8_t moodStrIndex;
    char dumbmode_mood[20];
} Passport;

static const Icon* const portrait[7] = {
    &I_passport_happy1_46x49,
    &I_passport_happy2_46x49,
    &I_passport_happy3_46x49,
    &I_G0ku,
    &I_g0ku_1,
    &I_g0ku_2,
    &I_g0ku_3};
// static const Icon* const portrait_ok[MOODS_TOTAL] = {
// &I_passport_okay1_46x49,
// &I_passport_okay2_46x49,
// &I_passport_okay3_46x49};
// static const Icon* const portrait_bad[MOODS_TOTAL] = {
// &I_passport_bad1_46x49,
// &I_passport_bad2_46x49,
// &I_passport_bad3_46x49};

// static const Icon* const* portraits[MOODS_TOTAL] = {portrait_happy, portrait_ok, portrait_bad};
//static const Icon* const* portraits[MOODS_TOTAL] = {portrait_happy};

static const char* const moods[16] = {
    "Stoned",
    "Baked",
    "Ripped",
    "Joyful",
    "Happy",
    "Satisfied",
    "Relaxed",
    "Nostalgic",
    "Okay",
    "Tired",
    "Bored",
    "Sad",
    "Annoyed",
    "Upset",
    "Angry",
    "Furious"};

void passport_set_variables(Passport* passport) {
    //stats, xp, levels
    passport->dolphin = furi_record_open(RECORD_DOLPHIN);
    passport->stats = dolphin_stats(passport->dolphin);
    furi_record_close(RECORD_DOLPHIN);

    DolphinStats* stats = &passport->stats;

    passport->max_level = dolphin_state_max_level();
    passport->tmpLvl = 0;

    //get XP
    passport->xp_to_levelup = dolphin_state_xp_to_levelup(stats->icounter);
    passport->xp_above_last_levelup = dolphin_state_xp_above_last_levelup(stats->icounter);
    passport->xp_for_current_level = passport->xp_to_levelup + passport->xp_above_last_levelup;

    //get percentage from remaining XP
    if(stats->level == passport->max_level) {
        passport->xp_progress = 100;
    } else {
        passport->xp_progress =
            round(((passport->xp_above_last_levelup * 100) / passport->xp_for_current_level));
    }

    //mood
    passport->desktop_settings = malloc(sizeof(DesktopSettings));
    DESKTOP_SETTINGS_LOAD(passport->desktop_settings);
    DesktopSettings* desktop_settings = passport->desktop_settings;

    passport->mood = 0; // DONT NEED DIFFERENT PICS BASED ON MOOD
    passport->moodStrIndex = stats->butthurt;

    // set mood to "Happy" if dumbmode is enabled
    if(desktop_settings->is_dumbmode) passport->moodStrIndex = passport->moodStrIndex + 4;

    // portrait
    furi_assert((stats->level > 0) && (stats->level <= passport->max_level));
    if(stats->level > 10) passport->tmpLvl = 1;
    if(stats->level > 15) passport->tmpLvl = 2;
    if(stats->level > 18) passport->tmpLvl = 3;
    if(stats->level > 21) passport->tmpLvl = 4;
    if(stats->level > 24) passport->tmpLvl = 5;
    if(stats->level > 27) passport->tmpLvl = 6;

    //string variables set
    //name
    snprintf(passport->my_name, 8, "%s", furi_hal_version_get_name_ptr());
    //mood
    snprintf(passport->mood_str, 20, "Mood: %s", moods[passport->moodStrIndex]);
    //level
    snprintf(passport->level_str, 12, "Level: %hu", stats->level);

    //determine whether to display XP stats or max level
    if(stats->level == passport->max_level) {
        //xp display for main passport view
        snprintf(passport->xp_str, 14, "Max Level");
    } else {
        //xp display for main passport view
        snprintf(
            passport->xp_str,
            14,
            "%lu/%lu",
            passport->xp_above_last_levelup,
            passport->xp_for_current_level);
    }

    free(stats);
}

static void render_callback(Canvas* const canvas, void* mutex) {
    furi_assert(mutex);

    Passport* passport = acquire_mutex((ValueMutex*)mutex, 25);

    //calc bar fill ratio
    //default bar is 65px wide
    passport->xp_fill = (int)round((passport->xp_progress / 100) * 65);

    // draw background
    canvas_draw_icon(canvas, 0, 0, &I_passport_DB);

    // draw portrait
    canvas_draw_icon(canvas, 11, 2, portrait[passport->tmpLvl]);

    //draw flipper info
    canvas_set_font(canvas, FontSecondary);
    //name
    canvas_draw_str(canvas, 58, 10, passport->my_name ? passport->my_name : "Unknown");
    //mood
    canvas_draw_str(canvas, 58, 22, passport->mood_str);
    //level
    canvas_draw_str(canvas, 58, 34, passport->level_str);

    canvas_set_font(canvas, FontBatteryPercent);

    //xp stats
    canvas_draw_str(canvas, 58, 42, passport->xp_str);

    canvas_set_font(canvas, FontSecondary);

    //Round Edge XP Bar

    //blank out background
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 58, 44, 67, 7);

    //draw black xp bar which will be used as outline but also for full bar
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, 58, 45, 67, 6, 1);

    //hollow out xp bar with white "empty" space
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_rbox(canvas, 59, 46, 65, 4, 1);

    //fill bar according to xp percentage
    if(passport->xp_fill >= 2) {
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_rbox(canvas, 59, 46, passport->xp_fill, 4, 1);
    }

    release_mutex((ValueMutex*)mutex, passport);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PassportEvent event = {.type = EventKeyPress, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

int32_t dolphin_passport_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PassportEvent));

    Passport* passport = malloc(sizeof(Passport));

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, passport, sizeof(Passport))) {
        free(passport);
        return 1;
    }

    passport_set_variables(passport);

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PassportEvent event = {0};

    while(event.input.key != InputKeyBack) {
        if(furi_message_queue_get(event_queue, &event, 10) != FuriStatusOk) {
            continue;
        }

        passport = (Passport*)acquire_mutex_block(&state_mutex);

        if(event.type == EventKeyPress) {
            switch(event.input.key) {
            case InputKeyUp:
                break;
            case InputKeyDown:
                break;
            case InputKeyRight:
                break;
            case InputKeyLeft:
                break;
            case InputKeyOk:
                break;
            default:
                break;
            }
        }
        release_mutex(&state_mutex, passport);
        view_port_update(view_port);
    }

    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    gui = NULL;
    furi_message_queue_free(event_queue);

    free(passport);

    return 0;
}