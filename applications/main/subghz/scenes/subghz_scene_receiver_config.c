#include "../subghz_i.h"
#include <lib/toolbox/value_index.h>

#define TAG "SubGhzSceneReceiverConfig"

enum SubGhzSettingIndex {
    SubGhzSettingIndexFrequency,
    SubGhzSettingIndexModulation,
    SubGhzSettingIndexHopping,
    SubGhzSettingIndexRAWSound = SubGhzSettingIndexHopping,
    SubGhzSettingIndexBinRAW,
    SubGhzSettingIndexRAWRSSIThreshold = SubGhzSettingIndexBinRAW,
    SubGhzSettingIndexRepeater,
    SubGhzSettingIndexRemoveDuplicates,
    SubGhzSettingIndexDeleteOldSignals,
    SubGhzSettingIndexAutosave,
    SubGhzSettingIndexIgnoreStarline,
    SubGhzSettingIndexIgnoreCars,
    SubGhzSettingIndexIgnoreMagellan,
    SubGhzSettingIndexIgnorePrinceton,
    SubGhzSettingIndexIgnoreNiceFlorS,
    SubGhzSettingIndexIgnoreWeather,
    SubGhzSettingIndexIgnoreTPMS,
    SubGhzSettingIndexSound,
    SubGhzSettingIndexResetToDefault,
    SubGhzSettingIndexLock,
};

#define RAW_THRESHOLD_RSSI_COUNT 11
const char* const raw_threshold_rssi_text[RAW_THRESHOLD_RSSI_COUNT] = {
    "-----",
    "-85.0",
    "-80.0",
    "-75.0",
    "-70.0",
    "-65.0",
    "-60.0",
    "-55.0",
    "-50.0",
    "-45.0",
    "-40.0",

};
const float raw_threshold_rssi_value[RAW_THRESHOLD_RSSI_COUNT] = {
    -90.0f,
    -85.0f,
    -80.0f,
    -75.0f,
    -70.0f,
    -65.0f,
    -60.0f,
    -55.0f,
    -50.0f,
    -45.0f,
    -40.0f,
};

#define COMBO_BOX_COUNT 2

const uint32_t hopping_value[COMBO_BOX_COUNT] = {
    SubGhzHopperStateOFF,
    SubGhzHopperStateRunning,
};

const uint32_t speaker_value[COMBO_BOX_COUNT] = {
    SubGhzSpeakerStateShutdown,
    SubGhzSpeakerStateEnable,
};

const uint32_t bin_raw_value[COMBO_BOX_COUNT] = {
    SubGhzProtocolFlag_Decodable,
    SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_BinRAW,
};

const char* const combobox_text[COMBO_BOX_COUNT] = {
    "OFF",
    "ON",
};

#define REPEATER_COUNT 4
const char* const repeater_text[REPEATER_COUNT] = {
    "OFF",
    "Normal",
    "Long",
    "Short",
};
const uint32_t repeater_value[REPEATER_COUNT] = {
    SubGhzRepeaterStateOff,
    SubGhzRepeaterStateOn,
    SubGhzRepeaterStateOnLong,
    SubGhzRepeaterStateOnShort,
};

static void subghz_scene_receiver_config_set_ignore_filter(
    VariableItem* item,
    SubGhzProtocolFilter filter) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, combobox_text[index]);

    if(index == 0) {
        CLEAR_BIT(subghz->ignore_filter, filter);
    } else {
        SET_BIT(subghz->ignore_filter, filter);
    }

    subghz->last_settings->ignore_filter = subghz->ignore_filter;
}

uint8_t subghz_scene_receiver_config_next_frequency(const uint32_t value, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    SubGhzSetting* setting = subghz_txrx_get_setting(subghz->txrx);

    uint8_t index = 0;
    for(size_t i = 0; i < subghz_setting_get_frequency_count(setting); i++) {
        if(value == subghz_setting_get_frequency(setting, i)) {
            index = i;
            break;
        } else {
            index = subghz_setting_get_frequency_default_index(setting);
        }
    }
    return index;
}

uint8_t subghz_scene_receiver_config_next_preset(const char* preset_name, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    uint8_t index = 0;
    SubGhzSetting* setting = subghz_txrx_get_setting(subghz->txrx);

    for(size_t i = 0; i < subghz_setting_get_preset_count(setting); i++) {
        if(!strcmp(subghz_setting_get_preset_name(setting, i), preset_name)) {
            index = i;
            break;
        } else {
            //  index = subghz_setting_get_frequency_default_index(setting);
        }
    }
    return index;
}

SubGhzHopperState subghz_scene_receiver_config_hopper_value_index(void* context) {
    furi_assert(context);
    SubGhz* subghz = context;

    if(subghz_txrx_hopper_get_state(subghz->txrx) == SubGhzHopperStateOFF) {
        return SubGhzHopperStateOFF;
    } else {
        variable_item_set_current_value_text(
            variable_item_list_get(subghz->variable_item_list, SubGhzSettingIndexFrequency),
            " -----");
        return SubGhzHopperStateRunning;
    }
}

static void subghz_scene_receiver_config_set_frequency(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    SubGhzSetting* setting = subghz_txrx_get_setting(subghz->txrx);

    if(subghz_txrx_hopper_get_state(subghz->txrx) == SubGhzHopperStateOFF) {
        char text_buf[10] = {0};
        uint32_t frequency = subghz_setting_get_frequency(setting, index);
        SubGhzRadioPreset preset = subghz_txrx_get_preset(subghz->txrx);

        snprintf(
            text_buf,
            sizeof(text_buf),
            "%lu.%02lu",
            frequency / 1000000,
            (frequency % 1000000) / 10000);
        variable_item_set_current_value_text(item, text_buf);
        subghz_txrx_set_preset(
            subghz->txrx,
            furi_string_get_cstr(preset.name),
            frequency,
            0,
            0,
            preset.data,
            preset.data_size);

        preset = subghz_txrx_get_preset(subghz->txrx);

        subghz->last_settings->frequency = preset.frequency;
        subghz_setting_set_default_frequency(setting, preset.frequency);
    } else {
        variable_item_set_current_value_index(
            item, subghz_setting_get_frequency_default_index(setting));
    }
}

static void subghz_scene_receiver_config_set_preset(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    SubGhzSetting* setting = subghz_txrx_get_setting(subghz->txrx);

    const char* preset_name = subghz_setting_get_preset_name(setting, index);
    variable_item_set_current_value_text(item, preset_name);
    //subghz->last_settings->preset = index;
    SubGhzRadioPreset preset = subghz_txrx_get_preset(subghz->txrx);
    subghz_txrx_set_preset(
        subghz->txrx,
        preset_name,
        preset.frequency,
        0,
        0,
        subghz_setting_get_preset_data(setting, index),
        subghz_setting_get_preset_data_size(setting, index));
    subghz->last_settings->preset_index = index;
}

static void subghz_scene_receiver_config_set_hopping_running(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    SubGhzHopperState index = variable_item_get_current_value_index(item);
    SubGhzSetting* setting = subghz_txrx_get_setting(subghz->txrx);
    VariableItem* frequency_item =
        variable_item_list_get(subghz->variable_item_list, SubGhzSettingIndexFrequency);

    variable_item_set_current_value_text(item, combobox_text[(uint8_t)index]);

    if(index == SubGhzHopperStateOFF) {
        char text_buf[10] = {0};
        uint32_t frequency = subghz_setting_get_default_frequency(setting);
        SubGhzRadioPreset preset = subghz_txrx_get_preset(subghz->txrx);

        snprintf(
            text_buf,
            sizeof(text_buf),
            "%lu.%02lu",
            frequency / 1000000,
            (frequency % 1000000) / 10000);
        variable_item_set_current_value_text(frequency_item, text_buf);

        // Maybe better add one more function with only with the frequency argument?
        subghz_txrx_set_preset(
            subghz->txrx,
            furi_string_get_cstr(preset.name),
            frequency,
            0,
            0,
            preset.data,
            preset.data_size);
        variable_item_set_current_value_index(
            frequency_item, subghz_setting_get_frequency_default_index(setting));
    } else {
        variable_item_set_current_value_text(frequency_item, " -----");
        variable_item_set_current_value_index(
            frequency_item, subghz_setting_get_frequency_default_index(setting));
    }
    subghz->last_settings->enable_hopping = index != SubGhzHopperStateOFF;
    subghz_txrx_hopper_set_state(subghz->txrx, index);
}

static void subghz_scene_receiver_config_set_speaker(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, combobox_text[index]);
    subghz_txrx_speaker_set_state(subghz->txrx, speaker_value[index]);
    subghz->last_settings->enable_sound = (speaker_value[index] == SubGhzSpeakerStateEnable);
}

static void subghz_scene_receiver_config_set_bin_raw(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, combobox_text[index]);
    subghz->filter = bin_raw_value[index];
    subghz_txrx_receiver_set_filter(subghz->txrx, subghz->filter);

    // We can set here, but during subghz_last_settings_save filter was changed to ignore BinRAW
    subghz->last_settings->filter = subghz->filter;

    //If the user changed BinRAW menu, dont reset it with the repeater.
    subghz->repeater_bin_raw_was_off = false;
}

static void subghz_scene_receiver_config_set_repeater(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    if(subghz->repeater == SubGhzRepeaterStateOff &&
       subghz_history_get_last_index(subghz->history)) {
        scene_manager_set_scene_state(
            subghz->scene_manager, SubGhzSceneReceiverConfig, SubGhzSettingIndexRepeater);
        view_dispatcher_send_custom_event(
            subghz->view_dispatcher, SubGhzCustomEventSceneSettingRepeater);
        return;
    }

    //Set menu Text.
    variable_item_set_current_value_text(item, repeater_text[index]);

    //Save state and in last settings.
    subghz->repeater = repeater_value[index];
    subghz->last_settings->repeater_state = repeater_value[index];

    //Get the BinRAW menu for state change.
    VariableItem* bin_raw_menu =
        variable_item_list_get(subghz->variable_item_list, SubGhzSettingIndexBinRAW);

    //Change BinRAW to ON or OFF as required, and remember whether I changed it! (Put back for the user.)
    if(repeater_value[index] != SubGhzRepeaterStateOff) {
        if((subghz->filter & SubGhzProtocolFlag_BinRAW) == 0) {
            //Repeater is on, Binraw is Off.
            variable_item_set_current_value_index(
                bin_raw_menu, 1 /*Index of ON in BIN_Raw menu!*/);
            subghz_scene_receiver_config_set_bin_raw(bin_raw_menu);
            subghz->repeater_bin_raw_was_off = true;
        }

        //Lock the BinRAW menu, Flipper doesnt understand everything so BinRAW makes every key send.
        variable_item_set_locked(bin_raw_menu, true, NULL);
    } else {
        //Put BinRAW back how it was, if we changed it.
        if(subghz->repeater_bin_raw_was_off) {
            variable_item_set_current_value_index(
                bin_raw_menu, 0 /*Index of OFF in BIN_Raw menu!*/);
            subghz_scene_receiver_config_set_bin_raw(bin_raw_menu);
        }

        //Lock the BinRAW menu, Flipper doesnt understand everything so BinRAW makes very key send.
        variable_item_set_locked(bin_raw_menu, false, NULL);
    }
}

static void subghz_scene_receiver_config_set_raw_threshold_rssi(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, raw_threshold_rssi_text[index]);
    subghz_threshold_rssi_set(subghz->threshold_rssi, raw_threshold_rssi_value[index]);

    subghz->last_settings->rssi = raw_threshold_rssi_value[index];
}

static void subghz_scene_receiver_config_set_duplicates(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, combobox_text[index]);

    subghz->last_settings->remove_duplicates = subghz->remove_duplicates = index;
    if(index) subghz_history_remove_duplicates(subghz->history);
}

static void subghz_scene_receiver_config_set_delete_old_signals(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, combobox_text[index]);

    subghz->last_settings->delete_old_signals = index == 1;
}

static void subghz_scene_receiver_config_set_autosave(VariableItem* item) {
    SubGhz* subghz = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, combobox_text[index]);

    subghz->last_settings->autosave = index == 1;
}

static inline bool subghz_scene_receiver_config_ignore_filter_get_index(
    SubGhzProtocolFilter filter,
    SubGhzProtocolFilter flag) {
    return READ_BIT(filter, flag) > 0;
}

static void subghz_scene_receiver_config_set_starline(VariableItem* item) {
    subghz_scene_receiver_config_set_ignore_filter(item, SubGhzProtocolFilter_StarLine);
}

static void subghz_scene_receiver_config_set_auto_alarms(VariableItem* item) {
    subghz_scene_receiver_config_set_ignore_filter(item, SubGhzProtocolFilter_AutoAlarms);
}

static void subghz_scene_receiver_config_set_magellan(VariableItem* item) {
    subghz_scene_receiver_config_set_ignore_filter(item, SubGhzProtocolFilter_Magellan);
}

static void subghz_scene_receiver_config_set_princeton(VariableItem* item) {
    subghz_scene_receiver_config_set_ignore_filter(item, SubGhzProtocolFilter_Princeton);
}

static void subghz_scene_receiver_config_set_niceflors(VariableItem* item) {
    subghz_scene_receiver_config_set_ignore_filter(item, SubGhzProtocolFilter_NiceFlorS);
}

static void subghz_scene_receiver_config_set_weather(VariableItem* item) {
    subghz_scene_receiver_config_set_ignore_filter(item, SubGhzProtocolFilter_Weather);
}
static void subghz_scene_receiver_config_set_tpms(VariableItem* item) {
    subghz_scene_receiver_config_set_ignore_filter(item, SubGhzProtocolFilter_TPMS);
}

static void subghz_scene_receiver_config_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    SubGhz* subghz = context;
    if(index == SubGhzSettingIndexRemoveDuplicates) {
        view_dispatcher_send_custom_event(
            subghz->view_dispatcher, SubGhzCustomEventSceneSettingRemoveDuplicates);
    } else if(index == SubGhzSettingIndexLock) {
        view_dispatcher_send_custom_event(
            subghz->view_dispatcher, SubGhzCustomEventSceneSettingLock);
    } else if(index == SubGhzSettingIndexResetToDefault) {
        // Reset all values to default state!
#if SUBGHZ_LAST_SETTING_SAVE_PRESET
        subghz_txrx_set_preset_internal(
            subghz->txrx,
            SUBGHZ_LAST_SETTING_DEFAULT_FREQUENCY,
            SUBGHZ_LAST_SETTING_DEFAULT_PRESET);
#else
        subghz_txrx_set_default_preset(subghz->txrx, SUBGHZ_LAST_SETTING_DEFAULT_FREQUENCY);
#endif
        SubGhzSetting* setting = subghz_txrx_get_setting(subghz->txrx);
        SubGhzRadioPreset preset = subghz_txrx_get_preset(subghz->txrx);
        const char* preset_name = furi_string_get_cstr(preset.name);
        int preset_index = subghz_setting_get_inx_preset_by_name(setting, preset_name);
        const int default_index = 0;

        subghz->last_settings->frequency = preset.frequency;
        subghz->last_settings->preset_index = preset_index;

        subghz_threshold_rssi_set(subghz->threshold_rssi, raw_threshold_rssi_value[default_index]);
        subghz->filter = bin_raw_value[0];
        subghz->ignore_filter = 0x00;
        subghz->remove_duplicates = false;
        subghz_txrx_receiver_set_filter(subghz->txrx, subghz->filter);
        subghz->last_settings->remove_duplicates = subghz->remove_duplicates;
        subghz->last_settings->ignore_filter = subghz->ignore_filter;
        subghz->last_settings->filter = subghz->filter;
        subghz->last_settings->repeater_state = SubGhzRepeaterStateOff;
        subghz->repeater = SubGhzRepeaterStateOff;
        subghz->last_settings->delete_old_signals = false;
        subghz->last_settings->autosave = false;

        subghz_txrx_speaker_set_state(subghz->txrx, speaker_value[default_index]);
        subghz->last_settings->enable_sound = false;

        subghz_txrx_hopper_set_state(subghz->txrx, hopping_value[default_index]);
        subghz->last_settings->enable_hopping = hopping_value[default_index];

        variable_item_list_set_selected_item(subghz->variable_item_list, default_index);
        variable_item_list_reset(subghz->variable_item_list);
#ifdef FURI_DEBUG
        subghz_last_settings_log(subghz->last_settings);
#endif
        subghz_last_settings_save(subghz->last_settings);

        view_dispatcher_send_custom_event(
            subghz->view_dispatcher, SubGhzCustomEventSceneSettingResetToDefault);
    }
}

void subghz_scene_receiver_config_on_enter(void* context) {
    SubGhz* subghz = context;
    VariableItem* item;
    uint8_t value_index;
    SubGhzSetting* setting = subghz_txrx_get_setting(subghz->txrx);
    SubGhzRadioPreset preset = subghz_txrx_get_preset(subghz->txrx);

    item = variable_item_list_add(
        subghz->variable_item_list,
        "Frequency",
        subghz_setting_get_frequency_count(setting),
        subghz_scene_receiver_config_set_frequency,
        subghz);
    value_index = subghz_scene_receiver_config_next_frequency(preset.frequency, subghz);
    variable_item_set_current_value_index(item, value_index);
    char text_buf[10] = {0};
    uint32_t frequency = subghz_setting_get_frequency(setting, value_index);
    snprintf(
        text_buf,
        sizeof(text_buf),
        "%lu.%02lu",
        frequency / 1000000,
        (frequency % 1000000) / 10000);
    variable_item_set_current_value_text(item, text_buf);

    item = variable_item_list_add(
        subghz->variable_item_list,
        "Modulation",
        subghz_setting_get_preset_count(setting),
        subghz_scene_receiver_config_set_preset,
        subghz);
    value_index =
        subghz_scene_receiver_config_next_preset(furi_string_get_cstr(preset.name), subghz);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(
        item, subghz_setting_get_preset_name(setting, value_index));

    if(scene_manager_get_scene_state(subghz->scene_manager, SubGhzSceneReadRAW) !=
       SubGhzCustomEventManagerSet) {
        // Hopping
        item = variable_item_list_add(
            subghz->variable_item_list,
            "Hopping",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_hopping_running,
            subghz);
        value_index = subghz_scene_receiver_config_hopper_value_index(subghz);

        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);
    }

    if(scene_manager_get_scene_state(subghz->scene_manager, SubGhzSceneReadRAW) !=
       SubGhzCustomEventManagerSet) {
        item = variable_item_list_add(
            subghz->variable_item_list,
            "Bin RAW",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_bin_raw,
            subghz);

        value_index = value_index_uint32(subghz->filter, bin_raw_value, COMBO_BOX_COUNT);
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);
        variable_item_set_locked(
            item, subghz->repeater != SubGhzRepeaterStateOff, "Turn off\nRepeater\nto do that!");

        item = variable_item_list_add(
            subghz->variable_item_list,
            "Repeater",
            REPEATER_COUNT,
            subghz_scene_receiver_config_set_repeater,
            subghz);

        value_index = value_index_uint32(subghz->repeater, repeater_value, REPEATER_COUNT);
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, repeater_text[value_index]);

        item = variable_item_list_add(
            subghz->variable_item_list,
            "Remove Duplicates",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_duplicates,
            subghz);

        value_index = subghz->remove_duplicates;
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);

        item = variable_item_list_add(
            subghz->variable_item_list,
            "Delete Old Signals on Full Memory",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_delete_old_signals,
            subghz);

        value_index = subghz->last_settings->delete_old_signals;
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);

        item = variable_item_list_add(
            subghz->variable_item_list,
            "Autosave",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_autosave,
            subghz);

        value_index = subghz->last_settings->autosave;
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);

        item = variable_item_list_add(
            subghz->variable_item_list,
            "Ignore Starline",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_starline,
            subghz);

        value_index = subghz_scene_receiver_config_ignore_filter_get_index(
            subghz->ignore_filter, SubGhzProtocolFilter_StarLine);
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);

        item = variable_item_list_add(
            subghz->variable_item_list,
            "Ignore Cars",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_auto_alarms,
            subghz);

        value_index = subghz_scene_receiver_config_ignore_filter_get_index(
            subghz->ignore_filter, SubGhzProtocolFilter_AutoAlarms);
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);

        item = variable_item_list_add(
            subghz->variable_item_list,
            "Ignore Magellan",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_magellan,
            subghz);

        value_index = subghz_scene_receiver_config_ignore_filter_get_index(
            subghz->ignore_filter, SubGhzProtocolFilter_Magellan);
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);

        item = variable_item_list_add(
            subghz->variable_item_list,
            "Ignore Princeton",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_princeton,
            subghz);

        value_index = subghz_scene_receiver_config_ignore_filter_get_index(
            subghz->ignore_filter, SubGhzProtocolFilter_Princeton);
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);

        item = variable_item_list_add(
            subghz->variable_item_list,
            "Ignore Nice Flor-S / Nice One",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_niceflors,
            subghz);

        value_index = subghz_scene_receiver_config_ignore_filter_get_index(
            subghz->ignore_filter, SubGhzProtocolFilter_NiceFlorS);
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);

        item = variable_item_list_add(
            subghz->variable_item_list,
            "Ignore Weather",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_weather,
            subghz);

        value_index = subghz_scene_receiver_config_ignore_filter_get_index(
            subghz->ignore_filter, SubGhzProtocolFilter_Weather);
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);

        item = variable_item_list_add(
            subghz->variable_item_list,
            "Ignore TPMS",
            COMBO_BOX_COUNT,
            subghz_scene_receiver_config_set_tpms,
            subghz);

        value_index = subghz_scene_receiver_config_ignore_filter_get_index(
            subghz->ignore_filter, SubGhzProtocolFilter_TPMS);
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, combobox_text[value_index]);
    }

    // Enable speaker, will send all incoming noises and signals to speaker so you can listen how your remote sounds like :)
    item = variable_item_list_add(
        subghz->variable_item_list,
        "Sound",
        COMBO_BOX_COUNT,
        subghz_scene_receiver_config_set_speaker,
        subghz);
    value_index = value_index_uint32(
        subghz_txrx_speaker_get_state(subghz->txrx), speaker_value, COMBO_BOX_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, combobox_text[value_index]);

    if(scene_manager_get_scene_state(subghz->scene_manager, SubGhzSceneReadRAW) !=
       SubGhzCustomEventManagerSet) {
        // Reset to default
        variable_item_list_add(subghz->variable_item_list, "Reset to Default", 1, NULL, NULL);

        variable_item_list_set_enter_callback(
            subghz->variable_item_list,
            subghz_scene_receiver_config_var_list_enter_callback,
            subghz);
    }
    if(scene_manager_get_scene_state(subghz->scene_manager, SubGhzSceneReadRAW) !=
       SubGhzCustomEventManagerSet) {
        // Lock keyboard
        variable_item_list_add(subghz->variable_item_list, "Lock Keyboard", 1, NULL, NULL);
        variable_item_list_set_enter_callback(
            subghz->variable_item_list,
            subghz_scene_receiver_config_var_list_enter_callback,
            subghz);
    }

    if(scene_manager_get_scene_state(subghz->scene_manager, SubGhzSceneReadRAW) ==
       SubGhzCustomEventManagerSet) {
        item = variable_item_list_add(
            subghz->variable_item_list,
            "RSSI Threshold:",
            RAW_THRESHOLD_RSSI_COUNT,
            subghz_scene_receiver_config_set_raw_threshold_rssi,
            subghz);
        value_index = value_index_float(
            subghz_threshold_rssi_get(subghz->threshold_rssi),
            raw_threshold_rssi_value,
            RAW_THRESHOLD_RSSI_COUNT);
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, raw_threshold_rssi_text[value_index]);
    }

    variable_item_list_set_selected_item(
        subghz->variable_item_list,
        scene_manager_get_scene_state(subghz->scene_manager, SubGhzSceneReceiverConfig));
    scene_manager_set_scene_state(subghz->scene_manager, SubGhzSceneReceiverConfig, 0);

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewIdVariableItemList);
}

bool subghz_scene_receiver_config_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubGhzCustomEventSceneSettingRepeater) {
            subghz_rx_key_state_set(subghz, SubGhzRxKeyStateTX);
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneNeedSaving);
            consumed = true;
        } else if(event.event == SubGhzCustomEventSceneSettingRemoveDuplicates) {
            subghz_history_remove_duplicates(subghz->history);
            scene_manager_previous_scene(subghz->scene_manager);
            consumed = true;
        } else if(event.event == SubGhzCustomEventSceneSettingLock) {
            subghz_lock(subghz);
            scene_manager_previous_scene(subghz->scene_manager);
            consumed = true;
        } else if(event.event == SubGhzCustomEventSceneSettingResetToDefault) {
            scene_manager_previous_scene(subghz->scene_manager);
            consumed = true;
        }
    }
    return consumed;
}

void subghz_scene_receiver_config_on_exit(void* context) {
    SubGhz* subghz = context;

    if(subghz->repeater_bin_raw_was_off) {
        subghz->last_settings->filter = bin_raw_value[0 /*BinRAW Off*/];
    }

    variable_item_list_set_selected_item(subghz->variable_item_list, 0);
    variable_item_list_reset(subghz->variable_item_list);
#ifdef FURI_DEBUG
    subghz_last_settings_log(subghz->last_settings);
#endif
    subghz_last_settings_save(subghz->last_settings);
    scene_manager_set_scene_state(
        subghz->scene_manager, SubGhzSceneReadRAW, SubGhzCustomEventManagerNoSet);
}
