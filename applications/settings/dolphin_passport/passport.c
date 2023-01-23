#include "assets_icons.h"
#include "dolphin/helpers/dolphin_state.h"
#include <core/check.h>
#include <core/record.h>
#include <furi.h>
#include <gui/gui.h>
#include <furi_hal_version.h>
#include "dolphin/dolphin.h"
#include "../desktop_settings/desktop_settings_app.h"
#include "math.h"

#define MOODS_TOTAL 1
#define BUTTHURT_MAX 14

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    FuriMessageQueue* event_queue;

    Dolphin* dolphin;
    DolphinStats stats;

    DesktopSettings* desktop_settings;

    uint8_t counter;
} Passport;

static const Icon* const portrait_happy[7] = {
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
static const Icon* const* portraits[MOODS_TOTAL] = {portrait_happy};

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

static void input_callback(InputEvent* input, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input};
    furi_message_queue_put(event_queue, &event, 0);
}

static void render_callback(Canvas* canvas, void* ctx) {
    Passport* passport = ctx;
    DolphinStats* stats = &passport->stats;
    DesktopSettings* desktop_settings = passport->desktop_settings;

    char level_str[12];
    char xp_str[14];
    char mood_str[20];
    uint8_t mood = 0;
    uint8_t moodStrIndex = stats->butthurt;
    if(desktop_settings->is_dumbmode) moodStrIndex = moodStrIndex + 4;
    snprintf(mood_str, 20, "Mood: %s", moods[moodStrIndex]);
    mood = 0; // DONT NEED DIFFERENT PICS BASED ON MOOD

    // UNUSED(mood);
    // UNUSED(portraits);

    uint32_t xp_progress = 0;
    uint32_t xp_to_levelup = dolphin_state_xp_to_levelup(stats->icounter);
    uint32_t xp_above_last_levelup = dolphin_state_xp_above_last_levelup(stats->icounter);
    uint32_t xp_for_current_level = xp_to_levelup + xp_above_last_levelup;

    if(stats->level == 30) {
        xp_progress = 0;
    } else {
        xp_progress = xp_to_levelup * 64 / xp_for_current_level;
    }

    // multipass
    canvas_draw_icon(canvas, 0, 0, &I_passport_DB);

    // portrait
    furi_assert((stats->level > 0) && (stats->level <= 30));
    uint16_t tmpLvl = 0;
    if(stats->level > 10) tmpLvl = 1;
    if(stats->level > 15) tmpLvl = 2;
    if(stats->level > 18) tmpLvl = 3;
    if(stats->level > 21) tmpLvl = 4;
    if(stats->level > 24) tmpLvl = 5;
    if(stats->level > 27) tmpLvl = 6;
    canvas_draw_icon(canvas, 11, 2, portraits[mood][tmpLvl]);

    /*//Rabbit
	if((passport->counter >= 1) && (passport->counter < 5))
	{
		canvas_draw_icon(canvas, 11, 2, &I_rabbit_00);
	}
	else if((passport->counter >= 5) && (passport->counter < 10))
	{
		canvas_draw_icon(canvas, 11, 2, &I_rabbit_01);
	}
	else if((passport->counter >= 10) && (passport->counter < 15))
	{
		canvas_draw_icon(canvas, 11, 2, &I_rabbit_02);
	}
	else if((passport->counter >= 15) && (passport->counter < 20))
	{
		canvas_draw_icon(canvas, 11, 2, &I_rabbit_03);
	}
	else if((passport->counter >= 20) && (passport->counter < 25))
	{
		canvas_draw_icon(canvas, 11, 2, &I_rabbit_04);
	}
	else if((passport->counter >= 25) && (passport->counter < 30))
	{
		canvas_draw_icon(canvas, 11, 2, &I_rabbit_05);
	}
	else if((passport->counter >= 30) && (passport->counter < 35))
	{
		canvas_draw_icon(canvas, 11, 2, &I_rabbit_06);
	}
	else if((passport->counter >= 35) && (passport->counter < 40))
	{
		canvas_draw_icon(canvas, 11, 2, &I_rabbit_07);
	}
	
	passport->counter++;
	
	if(passport->counter == 41)
	{
		passport->counter = 1;
	}
	//Sonic
	if((passport->counter >= 1) && (passport->counter < 2))
	{
		canvas_draw_icon(canvas, 11, 2, &I_sonic_00);
	}
	else if((passport->counter >= 2) && (passport->counter < 3))
	{
		canvas_draw_icon(canvas, 11, 2, &I_sonic_01);
	}
	else if((passport->counter >= 3) && (passport->counter < 4))
	{
		canvas_draw_icon(canvas, 11, 2, &I_sonic_02);
	}
	else if((passport->counter >= 4) && (passport->counter < 5))
	{
		canvas_draw_icon(canvas, 11, 2, &I_sonic_03);
	}
	else if((passport->counter >= 5) && (passport->counter < 6))
	{
		canvas_draw_icon(canvas, 11, 2, &I_sonic_04);
	}
	else if((passport->counter >= 6) && (passport->counter < 7))
	{
		canvas_draw_icon(canvas, 11, 2, &I_sonic_05);
	}
	else if((passport->counter >= 7) && (passport->counter < 8))
	{
		canvas_draw_icon(canvas, 11, 2, &I_sonic_06);
	}
	else if((passport->counter >= 8) && (passport->counter < 9))
	{
		canvas_draw_icon(canvas, 11, 2, &I_sonic_07);
	}
	
	passport->counter++;
	
	if(passport->counter == 10)
	{
		passport->counter = 1;
	}

	*/

    const char* my_name = furi_hal_version_get_name_ptr();
    snprintf(level_str, 12, "Level: %hu", stats->level);
    snprintf(xp_str, 14, "%lu/%lu", xp_above_last_levelup, xp_for_current_level);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 58, 10, my_name ? my_name : "Unknown");
    canvas_draw_str(canvas, 58, 22, mood_str);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str(canvas, 58, 34, level_str);
    canvas_set_font(canvas, FontBatteryPercent);
    canvas_draw_str(canvas, 58, 42, xp_str);
    canvas_set_font(canvas, FontSecondary);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 123 - xp_progress, 45, xp_progress + 1, 5);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_line(canvas, 123, 45, 123, 49);
}

Passport* passport_alloc() {
    Passport* app = malloc(sizeof(Passport));

    app->event_queue = furi_message_queue_alloc(10, sizeof(PluginEvent));
    app->counter = 0;

    return app;
}

int32_t passport_app(void* p) {
    UNUSED(p);
    Passport* passport = passport_alloc();

    ViewPort* view_port = view_port_alloc();

    passport->dolphin = furi_record_open(RECORD_DOLPHIN);
    passport->stats = dolphin_stats(passport->dolphin);
    furi_record_close(RECORD_DOLPHIN);

    passport->desktop_settings = malloc(sizeof(DesktopSettings));
    DESKTOP_SETTINGS_LOAD(passport->desktop_settings);

    view_port_draw_callback_set(view_port, render_callback, passport);
    view_port_input_callback_set(view_port, input_callback, passport->event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    view_port_update(view_port);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(passport->event_queue, &event, 10);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {
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
                    case InputKeyBack:
                        processing = false;
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        view_port_update(view_port);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(passport->event_queue);
    furi_record_close(RECORD_GUI);
    //furi_semaphore_free(semaphore);
    free(passport);

    return 0;
}