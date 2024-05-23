#include <gui/modules/variable_item_list.h>

#include "settings_scene.h"
#include "../app_context.h"
#include "../tone_gen.h"
#include "../utils/linked_list.h"

// Not actively used in this instance.
void menu_callback_settings_scene(void* context, uint32_t index) {
    UNUSED(context);
    UNUSED(index);
}

static uint8_t wave_option_values[] = {SINE, SQUARE};
static char* wave_option_names[] = {"Sine", "Square"};
static void wave_type_option_change(VariableItem* item) {
    struct AppContext_t* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, wave_option_names[index]);
    ((struct ToneData_t*)app->additionalData)->waveType = index;
}

// Since the max number of options for variable item lists is
// the size of an 8-bit integer, we need to limit the max
// number of steps. In this case, we limit it to 241 total
// steps available, incrementing in steps of 10.
#define MIN_FREQ 100
#define MAX_FREQ 2500
#define FREQ_STEPS 10
#define INDEX_TO_FREQ(index) (uint16_t)((index * FREQ_STEPS) + MIN_FREQ)
#define FREQ_TO_INDEX(freq) (uint8_t)((freq - MIN_FREQ) / FREQ_STEPS)
char* frequencyStr;
static void frequency_option_change(VariableItem* item) {
    struct AppContext_t* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    ((struct ToneData_t*)app->additionalData)->frequency = INDEX_TO_FREQ(index);
    snprintf(frequencyStr, 8, "%dhz", ((struct ToneData_t*)app->additionalData)->frequency);
    variable_item_set_current_value_text(item, frequencyStr);
}

char* volumeStr;
static void volume_option_change(VariableItem* item) {
    struct AppContext_t* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    ((struct ToneData_t*)app->additionalData)->volume = ((float)(index)) / 10.0f;
    snprintf(volumeStr, 5, "%d", (index * 10));
    variable_item_set_current_value_text(item, volumeStr);
}

/** resets the menu, gives it content, callbacks and selection enums */
void scene_on_enter_settings_scene(void* context) {
    FURI_LOG_I(TAG, "scene_on_enter_settings_scene");
    struct AppContext_t* app = (struct AppContext_t*)context;

    // Setup our menu
    FURI_LOG_D(TAG, "Adding view menu");
    struct View_t* variableItemListView = app->activeViews[ToneGenAppView_VariableItemList];

    // Set the currently active view
    variable_item_list_reset(variableItemListView->viewData);

    FURI_LOG_D(TAG, "Adding options for settings");
    // Wave type setting
    VariableItem* item = variable_item_list_add(
        variableItemListView->viewData,
        "Wave Display",
        COUNT_OF(wave_option_values),
        wave_type_option_change,
        app);
    variable_item_set_current_value_index(
        item, ((struct ToneData_t*)app->additionalData)->waveType);
    variable_item_set_current_value_text(
        item, wave_option_names[((struct ToneData_t*)app->additionalData)->waveType]);

    // Frequency setting
    item = variable_item_list_add(
        variableItemListView->viewData,
        "Frequency",
        FREQ_TO_INDEX(MAX_FREQ) + 1,
        frequency_option_change,
        app);
    variable_item_set_current_value_index(
        item, FREQ_TO_INDEX(((struct ToneData_t*)app->additionalData)->frequency));

    frequencyStr = calloc(8, sizeof(char));
    snprintf(frequencyStr, 8, "%dhz", ((struct ToneData_t*)app->additionalData)->frequency);
    variable_item_set_current_value_text(item, frequencyStr);

    // Volume setting
    item = variable_item_list_add(
        variableItemListView->viewData, "Volume", 11, volume_option_change, app);
    variable_item_set_current_value_index(
        item, (uint8_t)(((struct ToneData_t*)app->additionalData)->volume * 10.0f));

    volumeStr = calloc(5, sizeof(char));
    snprintf(
        volumeStr,
        5,
        "%d",
        ((uint8_t)(((struct ToneData_t*)app->additionalData)->volume * 100.0f)));
    variable_item_set_current_value_text(item, volumeStr);

    view_dispatcher_switch_to_view(app->view_dispatcher, ToneGenAppView_VariableItemList);
}

// Not actively used in this instance.
bool scene_on_event_settings_scene(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "scene_on_event_settings_scene");
    UNUSED(context);
    UNUSED(event);
    return false;
}

void scene_on_exit_settings_scene(void* context) {
    FURI_LOG_I(TAG, "scene_on_exit_settings_scene");
    UNUSED(context);
    free(frequencyStr);
    free(volumeStr);
}
