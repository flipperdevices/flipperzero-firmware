#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/icon.h>
#include <gui/view.h>

#include "action_model.h"
#include "keystroke_selector.h"

#define KEYSTROKE_SELECTOR_DEFAULT_TOP_ROW 2
#define KEYSTROKE_SELECTOR_DISPLAYED_ROWS 5
#define KEYSTROKE_SELECTOR_DISPLAYED_WIDTH 10
#define KEYSTROKE_SELECTOR_DISPLAYED_HEIGHT 13
#define KEYSTROKE_SELECTOR_GLYPH_HEIGHT_OFFSET 10
#define KEYSTROKE_SELECTOR_IMAGE_HEIGHT_OFFSET 1
#define KEYSTROKE_SELECTOR_COLS 12

struct KeystrokeSelector {
    View* view_keystroke_selector;
    ActionModel* action_model;
};

typedef struct KeystrokeSelectorModel KeystrokeSelectorModel;

typedef struct KeystrokeSelectorKeyResult KeystrokeSelectorKeyResult;

static KeystrokeSelectorKeyResult
    keystroke_selector_model_find_key(KeystrokeSelectorModel* model, uint16_t key_code);

static void keystroke_selector_draw_callback(Canvas* canvas, void* context);

static bool keystroke_selector_input_callback(InputEvent* event, void* context);
