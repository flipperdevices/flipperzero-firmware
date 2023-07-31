#include "../ublox_i.h"

#define TAG "ublox_scene_data_display_config"

enum UbloxSettingIndex {
    UbloxSettingIndexRefreshRate,
    UbloxSettingIndexBacklightMode,
    UbloxSettingIndexDisplayMode,
    UbloxSettingIndexNotify,
    UbloxSettingIndexPlatformModel,
    UbloxSettingIndexOdometerMode,
};

enum UbloxDataDisplayConfigIndex {
    UbloxDataDisplayConfigIndexDisplayMode,
    UbloxDataDisplayConfigIndexRefreshRate,
    UbloxDataDisplayConfigIndexBacklightMode,
};

#define DISPLAY_VIEW_MODE_COUNT 2
const char* const display_view_mode_text[DISPLAY_VIEW_MODE_COUNT] = {
    "Handheld",
    "Car",
};

const UbloxDataDisplayViewMode display_view_mode_value[DISPLAY_VIEW_MODE_COUNT] = {
    UbloxDataDisplayViewModeHandheld,
    UbloxDataDisplayViewModeCar,
};

#define BACKLIGHT_MODE_COUNT 2
const char* const backlight_mode_text[BACKLIGHT_MODE_COUNT] = {
    "Default",
    "On",
};

const UbloxDataDisplayBacklightMode backlight_mode_value[BACKLIGHT_MODE_COUNT] = {
    UbloxDataDisplayBacklightDefault,
    UbloxDataDisplayBacklightOn,
};

#define REFRESH_RATE_COUNT 8
// double const means that the data is constant and that the pointer
// is constant.
const char* const refresh_rate_text[REFRESH_RATE_COUNT] = {
    "2s",
    "5s",
    "10s",
    "15s",
    "20s",
    "30s",
    "45s",
    "60s",
};

// might need to be ms?
const UbloxDataDisplayRefreshRate refresh_rate_values[REFRESH_RATE_COUNT] = {
    2,
    5,
    10,
    15,
    20,
    30,
    45,
    60,
};

#define NOTIFY_MODE_COUNT 2
const char* const notify_mode_text[NOTIFY_MODE_COUNT] = {
    "On",
    "Off",
};

const UbloxDataDisplayNotifyMode notify_mode_values[NOTIFY_MODE_COUNT] = {
    UbloxDataDisplayNotifyOn,
    UbloxDataDisplayNotifyOff,
};

#define ODOMETER_MODE_COUNT 4
const char* const odometer_mode_text[ODOMETER_MODE_COUNT] = {
    "Run",
    "Cycle",
    "Swim",
    "Car",
};

const UbloxOdometerMode odometer_mode_values[ODOMETER_MODE_COUNT] = {
    UbloxOdometerModeRunning,
    UbloxOdometerModeCycling,
    UbloxOdometerModeSwimming,
    UbloxOdometerModeCar,
};

#define PLATFORM_MODEL_COUNT 6
const char* const platform_model_text[PLATFORM_MODEL_COUNT] = {
    "Portable",
    "Pedest.",
    "Auto.",
    "At Sea",
    "Air. <2g",
    "Wrist",
};

const UbloxPlatformModel platform_model_values[PLATFORM_MODEL_COUNT] = {
    UbloxPlatformModelPortable,
    UbloxPlatformModelPedestrian,
    UbloxPlatformModelAutomotive,
    UbloxPlatformModelAtSea,
    UbloxPlatformModelAirborne2g,
    UbloxPlatformModelWrist,
};

uint8_t ublox_scene_data_display_config_next_refresh_rate(
    const UbloxDataDisplayRefreshRate value,
    void* context) {
    furi_assert(context);

    uint8_t index = 0;
    for(int i = 0; i < REFRESH_RATE_COUNT; i++) {
        if(value == refresh_rate_values[i]) {
            index = i;
            break;
        } else {
            index = 0;
        }
    }
    return index;
}

static void ublox_scene_data_display_config_set_refresh_rate(VariableItem* item) {
    Ublox* ublox = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, refresh_rate_text[index]);
    (ublox->data_display_state).refresh_rate = refresh_rate_values[index];
    FURI_LOG_I(TAG, "set refresh rate to %lds", (ublox->data_display_state).refresh_rate);
}

static uint8_t ublox_scene_data_display_config_next_backlight_mode(
    const UbloxDataDisplayBacklightMode value,
    void* context) {
    furi_assert(context);

    uint8_t index = 0;
    for(int i = 0; i < BACKLIGHT_MODE_COUNT; i++) {
        if(value == backlight_mode_value[i]) {
            index = i;
            break;
        } else {
            index = 0;
        }
    }
    return index;
}

static void ublox_scene_data_display_config_set_backlight_mode(VariableItem* item) {
    Ublox* ublox = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, backlight_mode_text[index]);
    (ublox->data_display_state).backlight_mode = backlight_mode_value[index];

    if((ublox->data_display_state).backlight_mode == UbloxDataDisplayBacklightOn) {
        notification_message_block(ublox->notifications, &sequence_display_backlight_enforce_on);
    } else if((ublox->data_display_state).backlight_mode == UbloxDataDisplayBacklightDefault) {
        notification_message_block(ublox->notifications, &sequence_display_backlight_enforce_auto);
    }
}

static uint8_t ublox_scene_data_display_config_next_display_view_mode(
    const UbloxDataDisplayViewMode value,
    void* context) {
    furi_assert(context);

    uint8_t index = 0;
    for(int i = 0; i < DISPLAY_VIEW_MODE_COUNT; i++) {
        if(value == display_view_mode_value[i]) {
            index = i;
            break;
        } else {
            index = 0;
        }
    }
    return index;
}

static void ublox_scene_data_display_config_set_display_view_mode(VariableItem* item) {
    Ublox* ublox = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, display_view_mode_text[index]);
    (ublox->data_display_state).view_mode = display_view_mode_value[index];
}

static uint8_t ublox_scene_data_display_config_next_notify_mode(
    const UbloxDataDisplayNotifyMode value,
    void* context) {
    furi_assert(context);

    uint8_t index = 0;
    for(int i = 0; i < NOTIFY_MODE_COUNT; i++) {
        if(value == notify_mode_values[i]) {
            index = i;
            break;
        } else {
            index = 0;
        }
    }
    return index;
}

static void ublox_scene_data_display_config_set_notify_mode(VariableItem* item) {
    Ublox* ublox = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, notify_mode_text[index]);
    (ublox->data_display_state).notify_mode = notify_mode_values[index];
}

static uint8_t ublox_scene_data_display_config_next_odometer_mode(
    const UbloxOdometerMode value,
    void* context) {
    furi_assert(context);

    uint8_t index = 0;
    for(int i = 0; i < ODOMETER_MODE_COUNT; i++) {
        if(value == odometer_mode_values[i]) {
            index = i;
            break;
        } else {
            index = 0;
        }
    }
    return index;
}

static void ublox_scene_data_display_config_set_odometer_mode(VariableItem* item) {
    Ublox* ublox = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, odometer_mode_text[index]);
    (ublox->device_state).odometer_mode = odometer_mode_values[index];
    // device has to be re-configured at next read
    ublox->gps_initted = false;
}

static uint8_t ublox_scene_data_display_config_next_platform_model(
    const UbloxPlatformModel value,
    void* context) {
    furi_assert(context);

    uint8_t index = 0;
    for(int i = 0; i < PLATFORM_MODEL_COUNT; i++) {
        if(value == platform_model_values[i]) {
            index = i;
            break;
        } else {
            index = 0;
        }
    }
    return index;
}

static void ublox_scene_data_display_config_set_platform_model(VariableItem* item) {
    Ublox* ublox = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, platform_model_text[index]);
    (ublox->device_state).platform_model = platform_model_values[index];
    // device has to be re-configured at next read
    ublox->gps_initted = false;
}

void ublox_scene_data_display_config_on_enter(void* context) {
    Ublox* ublox = context;
    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(
        ublox->variable_item_list,
        "Refresh Rate:",
        REFRESH_RATE_COUNT,
        ublox_scene_data_display_config_set_refresh_rate,
        ublox);

    value_index = ublox_scene_data_display_config_next_refresh_rate(
        (ublox->data_display_state).refresh_rate, ublox);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, refresh_rate_text[value_index]);

    item = variable_item_list_add(
        ublox->variable_item_list,
        "Backlight:",
        BACKLIGHT_MODE_COUNT,
        ublox_scene_data_display_config_set_backlight_mode,
        ublox);
    value_index = ublox_scene_data_display_config_next_backlight_mode(
        (ublox->data_display_state).backlight_mode, ublox);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, backlight_mode_text[value_index]);

    item = variable_item_list_add(
        ublox->variable_item_list,
        "Display Mode:",
        DISPLAY_VIEW_MODE_COUNT,
        ublox_scene_data_display_config_set_display_view_mode,
        ublox);
    value_index = ublox_scene_data_display_config_next_display_view_mode(
        (ublox->data_display_state).view_mode, ublox);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, display_view_mode_text[value_index]);

    item = variable_item_list_add(
        ublox->variable_item_list,
        "Notify:",
        NOTIFY_MODE_COUNT,
        ublox_scene_data_display_config_set_notify_mode,
        ublox);
    value_index = ublox_scene_data_display_config_next_notify_mode(
        (ublox->data_display_state).notify_mode, ublox);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, notify_mode_text[value_index]);

    item = variable_item_list_add(
        ublox->variable_item_list,
        "Platform Model:",
        PLATFORM_MODEL_COUNT,
        ublox_scene_data_display_config_set_platform_model,
        ublox);
    value_index = ublox_scene_data_display_config_next_platform_model(
        (ublox->device_state).platform_model, ublox);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, platform_model_text[value_index]);

    item = variable_item_list_add(
        ublox->variable_item_list,
        "Odo Mode:",
        ODOMETER_MODE_COUNT,
        ublox_scene_data_display_config_set_odometer_mode,
        ublox);
    value_index = ublox_scene_data_display_config_next_odometer_mode(
        (ublox->device_state).odometer_mode, ublox);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, odometer_mode_text[value_index]);

    view_dispatcher_switch_to_view(ublox->view_dispatcher, UbloxViewVariableItemList);
}

bool ublox_scene_data_display_config_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void ublox_scene_data_display_config_on_exit(void* context) {
    Ublox* ublox = context;
    variable_item_list_set_selected_item(ublox->variable_item_list, 0);
    variable_item_list_reset(ublox->variable_item_list);
}
