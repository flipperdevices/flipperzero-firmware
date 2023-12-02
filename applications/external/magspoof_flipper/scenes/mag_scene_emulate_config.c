#include "../mag_i.h"

#define TAG "MagSceneEmulateConfig"

enum MagSettingIndex {
    MagSettingIndexTx,
    MagSettingIndexTrack,
    MagSettingIndexReverse,
    MagSettingIndexClock,
    MagSettingIndexInterpacket,
};

#define TX_COUNT 7
const char* const tx_text[TX_COUNT] = {
    "RFID",
    "GPIO",
    "Piezo",
    "LF + P",
    "NFC",
    "434MHz",
    "868MHz",
};
const uint32_t tx_value[TX_COUNT] = {
    MagTxStateRFID,
    MagTxStateGPIO,
    MagTxStatePiezo,
    MagTxStateLF_P,
    MagTxStateNFC,
    MagTxCC1101_434,
    MagTxCC1101_868,
};

#define TRACK_COUNT 4
const char* const track_text[TRACK_COUNT] = {
    "1 + 2",
    "1",
    "2",
    "3",
};
const uint32_t track_value[TRACK_COUNT] = {
    MagTrackStateOneAndTwo,
    MagTrackStateOne,
    MagTrackStateTwo,
    MagTrackStateThree,
};

#define REVERSE_COUNT 2
const char* const reverse_text[REVERSE_COUNT] = {
    "OFF",
    "ON",
};
const uint32_t reverse_value[REVERSE_COUNT] = {
    MagReverseStateOff,
    MagReverseStateOn,
};

#define CLOCK_COUNT 15
const char* const clock_text[CLOCK_COUNT] = {
    "200us",
    "220us",
    "240us",
    "250us",
    "260us",
    "280us",
    "300us",
    "325us",
    "350us",
    "375us",
    "400us",
    "450us",
    "500us",
    "600us",
    "700us",
};
const uint32_t clock_value[CLOCK_COUNT] = {
    200,
    220,
    240,
    250,
    260,
    280,
    300,
    325,
    350,
    375,
    400,
    450,
    500,
    600,
    700,
};

#define INTERPACKET_COUNT 13
const char* const interpacket_text[INTERPACKET_COUNT] = {
    "0us",
    "2us",
    "4us",
    "6us",
    "8us",
    "10us",
    "12us",
    "14us",
    "16us",
    "18us",
    "20us",
    "25us",
    "30us",
};
const uint32_t interpacket_value[INTERPACKET_COUNT] = {
    0,
    2,
    4,
    6,
    8,
    10,
    12,
    14,
    16,
    18,
    20,
    25,
    30,
};

static void mag_scene_emulate_config_set_tx(VariableItem* item) {
    Mag* mag = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, tx_text[index]);

    mag->setting->tx = tx_value[index];
};

static void mag_scene_emulate_config_set_track(VariableItem* item) {
    Mag* mag = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    if(mag->setting->reverse == MagReverseStateOff) {
        variable_item_set_current_value_text(item, track_text[index]);
        mag->setting->track = track_value[index];
    } else if(mag->setting->reverse == MagReverseStateOn) {
        variable_item_set_current_value_index(
            item, value_index_uint32(MagTrackStateOneAndTwo, track_value, TRACK_COUNT));
    }

    // TODO: Check there is data in selected track?
    //       Only display track options with data?
};

static void mag_scene_emulate_config_set_reverse(VariableItem* item) {
    Mag* mag = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    if(mag->setting->track == MagTrackStateOneAndTwo) {
        // only allow reverse track to be set when playing both 1 and 2
        variable_item_set_current_value_text(item, reverse_text[index]);
        mag->setting->reverse = reverse_value[index];
        //FURI_LOG_D(TAG, "%s", reverse_text[index]);
        //FURI_LOG_D(TAG, "%d", mag->setting->reverse);
    } else {
        variable_item_set_current_value_index(
            item, value_index_uint32(MagReverseStateOff, reverse_value, REVERSE_COUNT));
    }
};

static void mag_scene_emulate_config_set_clock(VariableItem* item) {
    Mag* mag = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, clock_text[index]);

    mag->setting->us_clock = clock_value[index];
};

static void mag_scene_emulate_config_set_interpacket(VariableItem* item) {
    Mag* mag = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, interpacket_text[index]);

    mag->setting->us_interpacket = interpacket_value[index];
};

void mag_scene_emulate_config_on_enter(void* context) {
    // TODO: retrieve current values from struct, rather than setting to default on setup

    Mag* mag = context;
    VariableItem* item;
    uint8_t value_index;

    // TX
    item = variable_item_list_add(
        mag->variable_item_list, "TX via:", TX_COUNT, mag_scene_emulate_config_set_tx, mag);
    value_index = value_index_uint32(mag->setting->tx, tx_value, TX_COUNT);
    scene_manager_set_scene_state(mag->scene_manager, MagSceneEmulateConfig, (uint32_t)item);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, tx_text[value_index]);

    // Track
    item = variable_item_list_add(
        mag->variable_item_list, "Track:", TRACK_COUNT, mag_scene_emulate_config_set_track, mag);
    value_index = value_index_uint32(mag->setting->track, track_value, TRACK_COUNT);
    scene_manager_set_scene_state(mag->scene_manager, MagSceneEmulateConfig, (uint32_t)item);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, track_text[value_index]);

    // Reverse
    //FURI_LOG_D(TAG, "%d", mag->setting->reverse);
    item = variable_item_list_add(
        mag->variable_item_list,
        "Reverse:",
        REVERSE_COUNT,
        mag_scene_emulate_config_set_reverse,
        mag);
    value_index = value_index_uint32(mag->setting->reverse, reverse_value, REVERSE_COUNT);
    scene_manager_set_scene_state(mag->scene_manager, MagSceneEmulateConfig, (uint32_t)item);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, reverse_text[value_index]);

    // Clock
    item = variable_item_list_add(
        mag->variable_item_list, "Clock:", CLOCK_COUNT, mag_scene_emulate_config_set_clock, mag);
    value_index = value_index_uint32(mag->setting->us_clock, clock_value, CLOCK_COUNT);
    scene_manager_set_scene_state(mag->scene_manager, MagSceneEmulateConfig, (uint32_t)item);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, clock_text[value_index]);

    // Interpacket
    /*
    item = variable_item_list_add(
        mag->variable_item_list,
        "Interpacket:",
        INTERPACKET_COUNT,
        mag_scene_emulate_config_set_interpacket,
        mag);
    value_index =
        value_index_uint32(mag->setting->us_interpacket, interpacket_value, INTERPACKET_COUNT);
    scene_manager_set_scene_state(mag->scene_manager, MagSceneEmulateConfig, (uint32_t)item);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, interpacket_text[value_index]);*/
    UNUSED(mag_scene_emulate_config_set_interpacket);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewVariableItemList);
}

bool mag_scene_emulate_config_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    UNUSED(mag);
    UNUSED(scene_manager);
    UNUSED(event);

    return consumed;
}

void mag_scene_emulate_config_on_exit(void* context) {
    Mag* mag = context;
    variable_item_list_set_selected_item(mag->variable_item_list, 0);
    variable_item_list_reset(mag->variable_item_list);
    // mag_last_settings_save?
    // scene_manager_set_scene_state? Using subghz_scene_reciever_config as framework/inspo
}