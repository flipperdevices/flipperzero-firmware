#ifndef GEM_H
#define GEM_H

#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <stdlib.h>
#include <string.h>

// Global constants
#define TAG "Gem"
#define DEBUG false

#define FLIPPER_LCD_WIDTH 128
#define FLIPPER_LCD_HEIGHT 64

#define TITLE "Gem Utility"
#define SUBTITLE "by FiSk"
#define TITLE_OFFSET 3
#define TEXT_OFFSET 10

#define LOGO_HEIGHT 10
#define LOGO_WIDTH 10

#define START_Y 15

// Enumeration for event types
typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

// Struct for plugin events
typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

// Struct for plugin state
typedef struct {
    int x;
    int y;
} PluginState;

// Function prototypes
static void draw_interface(Canvas* const canvas);
static void render_callback(Canvas* const canvas, void* ctx);
static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue);
static void gem_state_init(PluginState* const plugin_state);

#endif // GEM_H