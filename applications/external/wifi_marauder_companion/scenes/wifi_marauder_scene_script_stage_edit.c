#include "../wifi_marauder_app_i.h"

typedef void (*VariableItemSetupCallback)(VariableItem* item);
typedef void (*VariableItemSelectCallback)(WifiMarauderApp* app);

typedef enum WifiMarauderScriptEditItemType {
    WifiMarauderScriptEditItemTypeString,
    WifiMarauderScriptEditItemTypeNumber,
    WifiMarauderScriptEditItemTypeOptionsString,
    WifiMarauderScriptEditItemTypeOptionsNumber,
    WifiMarauderScriptEditItemTypeListString,
    WifiMarauderScriptEditItemTypeListNumber
} WifiMarauderScriptEditItemType;


#define MAX_OPTIONS (12)
typedef struct WifiMarauderScriptEditItem {
    char* name;
    WifiMarauderScriptEditItemType type;
    int num_options;
    char* options[MAX_OPTIONS];
    VariableItemSetupCallback setup_callback;
    VariableItemChangeCallback change_callback;
    VariableItemSelectCallback select_callback;
} WifiMarauderScriptEditItem;

// Menu items =============================================================================================================================
WifiMarauderScriptEditItem* stage_items = NULL;
uint32_t num_stage_items = 0;

void wifi_marauder_scene_script_stage_edit_create_list_strings(WifiMarauderApp* app, char** strings, int string_count) {
    // Deallocates the existing list
    WifiMarauderScriptStageListItem* current_item = app->script_stage_edit_first_item;
    while (current_item != NULL) {
        WifiMarauderScriptStageListItem* next_item = current_item->next_item;
        free(current_item->value);
        free(current_item);
        current_item = next_item;
    }

    // Create a new list with numbers
    WifiMarauderScriptStageListItem* first_item = NULL;
    WifiMarauderScriptStageListItem* previous_item = NULL;
    for (int i = 0; i < string_count; i++) {        
        WifiMarauderScriptStageListItem* item = malloc(sizeof(WifiMarauderScriptStageListItem));
        item->value = strdup(strings[i]);
        item->next_item = NULL;

        if (previous_item == NULL) {
            first_item = item;
        } else {
            previous_item->next_item = item;
        }
        previous_item = item;
    }

    app->script_stage_edit_first_item = first_item;
}

void wifi_marauder_scene_script_stage_edit_create_list_numbers(WifiMarauderApp* app, int* numbers, int number_count) {
    // Deallocates the existing list
    WifiMarauderScriptStageListItem* current_item = app->script_stage_edit_first_item;
    while (current_item != NULL) {
        WifiMarauderScriptStageListItem* next_item = current_item->next_item;
        free(current_item->value);
        free(current_item);
        current_item = next_item;
    }

    // Create a new list with numbers
    WifiMarauderScriptStageListItem* first_item = NULL;
    WifiMarauderScriptStageListItem* previous_item = NULL;
    for (int i = 0; i < number_count; i++) {
        char number_str[32];
        snprintf(number_str, sizeof(number_str), "%d", numbers[i]);
        
        WifiMarauderScriptStageListItem* item = malloc(sizeof(WifiMarauderScriptStageListItem));
        item->value = strdup(number_str);
        item->next_item = NULL;

        if (previous_item == NULL) {
            first_item = item;
        } else {
            previous_item->next_item = item;
        }
        previous_item = item;
    }

    app->script_stage_edit_first_item = first_item;
}

static void wifi_marauder_scene_script_stage_edit_list_enter_callback(void* context, uint32_t index) {
    WifiMarauderApp* app = context;
    const WifiMarauderScriptEditItem* menu_item = &stage_items[index];

    // Fixed delete item
    if (index == num_stage_items) {
        uint32_t deleted_stage_index = scene_manager_get_scene_state(app->scene_manager, WifiMarauderSceneScriptEdit);
        if (deleted_stage_index > 0) {
            scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneScriptEdit, deleted_stage_index - 1);
        }
        WifiMarauderScriptStage* previous_stage = NULL;
        WifiMarauderScriptStage* current_stage = app->script->first_stage;
        uint32_t current_stage_index = 0;

        while (current_stage != NULL && current_stage_index < deleted_stage_index) {
            previous_stage = current_stage;
            current_stage = current_stage->next_stage;
            current_stage_index++;
        }

        // Delete the stage
        if (current_stage != NULL) {
            if (previous_stage != NULL) {
                if (current_stage->next_stage != NULL) {
                    previous_stage->next_stage = current_stage->next_stage;
                } else {
                    previous_stage->next_stage = NULL;
                    app->script->last_stage = previous_stage;
                }
            } else {
                if (current_stage->next_stage != NULL) {
                    app->script->first_stage = current_stage->next_stage;
                } else {
                    app->script->first_stage = NULL;
                    app->script->last_stage = NULL;
                }
            }
        }
        app->script_edit_selected_stage = NULL;

        scene_manager_previous_scene(app->scene_manager);
        return;
    }

    if (menu_item->select_callback == NULL) {
        return;
    }
    if (menu_item->type == WifiMarauderScriptEditItemTypeNumber) {
        // Accepts user number input, assigning the value to the reference passed as a parameter
        menu_item->select_callback(app);
        scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneScriptStageEdit, index);
        app->user_input_type = WifiMarauderUserInputTypeNumber;
        scene_manager_next_scene(app->scene_manager, WifiMarauderSceneUserInput);
    } else if (menu_item->type == WifiMarauderScriptEditItemTypeString) {
        // Accepts user string input, assigning the value to the reference passed as a parameter
        menu_item->select_callback(app);
        scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneScriptStageEdit, index);
        app->user_input_type = WifiMarauderUserInputTypeString;
        scene_manager_next_scene(app->scene_manager, WifiMarauderSceneUserInput);
    } else if (menu_item->type == WifiMarauderScriptEditItemTypeListString) {
        // Accepts the strings that compose the list
        menu_item->select_callback(app);
        wifi_marauder_scene_script_stage_edit_create_list_strings(app, *app->script_stage_edit_strings_reference, *app->script_stage_edit_string_count_reference);
        scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneScriptStageEdit, index);
        scene_manager_next_scene(app->scene_manager, WifiMarauderSceneScriptStageEditList);
    } else if (menu_item->type == WifiMarauderScriptEditItemTypeListNumber) {
        // Accepts the numbers that compose the list
        menu_item->select_callback(app);
        wifi_marauder_scene_script_stage_edit_create_list_numbers(app, *app->script_stage_edit_numbers_reference, *app->script_stage_edit_number_count_reference);
        scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneScriptStageEdit, index);
        scene_manager_next_scene(app->scene_manager, WifiMarauderSceneScriptStageEditList);
    }
}

// Scan stage =============================================================================================================================
static void wifi_marauder_scan_stage_type_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageScan* stage = app->script_edit_selected_stage->stage;
    variable_item_set_current_value_index(item, stage->type);
}

static void wifi_marauder_scan_stage_type_change_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);

    // Get menu item
    uint8_t current_stage_index = variable_item_list_get_selected_item_index(app->script_stage_edit_list);
    const WifiMarauderScriptEditItem* menu_item = &stage_items[current_stage_index];

    // Defines the text of the selected option
    uint8_t option_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, menu_item->options[option_index]);

    // Updates the attribute value of the current stage
    WifiMarauderScriptStageScan* stage = app->script_edit_selected_stage->stage;
    stage->type = option_index;
}

static void wifi_marauder_scan_stage_channel_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageScan* stage = app->script_edit_selected_stage->stage;
    if (stage->channel >= 0 && stage->channel < 12) {
        variable_item_set_current_value_index(item, stage->channel);
    } else {
        variable_item_set_current_value_index(item, 0);
    }
}

static void wifi_marauder_scan_stage_channel_change_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);

    // Get menu item
    uint8_t current_stage_index = variable_item_list_get_selected_item_index(app->script_stage_edit_list);
    const WifiMarauderScriptEditItem* menu_item = &stage_items[current_stage_index];

    // Defines the text of the selected option
    uint8_t option_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, menu_item->options[option_index]);

    // Updates the attribute value of the current stage
    WifiMarauderScriptStageScan* stage = app->script_edit_selected_stage->stage;
    stage->channel = option_index;
}

static void wifi_marauder_scan_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageScan* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_scan_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageScan* stage_scan = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_scan->timeout;
}

WifiMarauderScriptEditItem scan_items[] = {
    {
        "Type", WifiMarauderScriptEditItemTypeOptionsString, 2, {"ap", "station"},
        wifi_marauder_scan_stage_type_setup_callback, wifi_marauder_scan_stage_type_change_callback, NULL
    },
    {
        "Channel", WifiMarauderScriptEditItemTypeOptionsNumber, 12, {"none", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"},
        wifi_marauder_scan_stage_channel_setup_callback, wifi_marauder_scan_stage_channel_change_callback, NULL
    },
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_scan_stage_timeout_setup_callback, NULL, wifi_marauder_scan_stage_timeout_select_callback
    }
};

// Select stage =============================================================================================================================
static void wifi_marauder_select_stage_type_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageSelect* stage = app->script_edit_selected_stage->stage;
    variable_item_set_current_value_index(item, stage->type);
}

static void wifi_marauder_select_stage_type_change_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);

    // Get menu item
    uint8_t current_stage_index = variable_item_list_get_selected_item_index(app->script_stage_edit_list);
    const WifiMarauderScriptEditItem* menu_item = &stage_items[current_stage_index];

    // Defines the text of the selected option
    uint8_t option_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, menu_item->options[option_index]);

    // Updates the attribute value of the current stage
    WifiMarauderScriptStageSelect* stage = app->script_edit_selected_stage->stage;
    stage->type = option_index;
}

static void wifi_marauder_select_stage_filter_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageSelect* stage = app->script_edit_selected_stage->stage;

    if (stage->filter != NULL) {
        variable_item_set_current_value_index(item, 0);
        variable_item_set_current_value_text(item, stage->filter);
    } else {
        variable_item_set_current_value_index(item, 1);
    }
}

static void wifi_marauder_select_stage_filter_change_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageSelect* stage = app->script_edit_selected_stage->stage;

    // Clears the filter if you change the option. Flipper input box does not accept blank text
    if (variable_item_get_current_value_index(item) == 1) {
        stage->filter = NULL;
        variable_item_set_current_value_index(item, 0);
        variable_item_set_values_count(item, 1);
    }

    if (stage->filter != NULL) {
        variable_item_set_current_value_text(item, stage->filter);
    } else {
        variable_item_set_current_value_text(item, "");
    }
}

static void wifi_marauder_select_stage_filter_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageSelect* stage_select = app->script_edit_selected_stage->stage;
    if (stage_select->filter == NULL) {
        stage_select->filter = malloc(128);
    }
    app->user_input_string_reference = &stage_select->filter;
}

static void wifi_marauder_select_stage_indexes_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageSelect* stage_select = app->script_edit_selected_stage->stage;
    app->script_stage_edit_numbers_reference = &stage_select->indexes;
    app->script_stage_edit_number_count_reference = &stage_select->index_count;
}

WifiMarauderScriptEditItem select_items[] = {
    {
        "Type", WifiMarauderScriptEditItemTypeOptionsString, 2, {"ap", "station"},
        wifi_marauder_select_stage_type_setup_callback, wifi_marauder_select_stage_type_change_callback, NULL
    },
    {
        "Filter", WifiMarauderScriptEditItemTypeString, 2, {NULL, NULL},
        wifi_marauder_select_stage_filter_setup_callback, wifi_marauder_select_stage_filter_change_callback, wifi_marauder_select_stage_filter_select_callback
    },
    {
        "Indexes", WifiMarauderScriptEditItemTypeListNumber, 1, {NULL},
        NULL, NULL, wifi_marauder_select_stage_indexes_select_callback
    }
};

// Deauth stage ===========================================================================================================================
static void wifi_marauder_deauth_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageDeauth* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_deauth_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageDeauth* stage_deauth = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_deauth->timeout;
}

WifiMarauderScriptEditItem deauth_items[] = {
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_deauth_stage_timeout_setup_callback, NULL, wifi_marauder_deauth_stage_timeout_select_callback
    }
};

// Probe stage ============================================================================================================================
static void wifi_marauder_probe_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageProbe* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_probe_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageProbe* stage_probe = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_probe->timeout;
}

WifiMarauderScriptEditItem probe_items[] = {
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_probe_stage_timeout_setup_callback, NULL, wifi_marauder_probe_stage_timeout_select_callback
    }
};

// Sniff RAW stage ========================================================================================================================
static void wifi_marauder_sniffraw_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageSniffRaw* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_sniffraw_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageSniffRaw* stage_sniffraw = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_sniffraw->timeout;
}

WifiMarauderScriptEditItem sniffraw_items[] = {
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_sniffraw_stage_timeout_setup_callback, NULL, wifi_marauder_sniffraw_stage_timeout_select_callback
    }
};

// Sniff Beacon stage =====================================================================================================================
static void wifi_marauder_sniffbeacon_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageSniffBeacon* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_sniffbeacon_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageSniffBeacon* stage_sniffbeacon = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_sniffbeacon->timeout;
}

WifiMarauderScriptEditItem sniffbeacon_items[] = {
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_sniffbeacon_stage_timeout_setup_callback, NULL, wifi_marauder_sniffbeacon_stage_timeout_select_callback
    }
};

// Sniff Deauth stage =====================================================================================================================
static void wifi_marauder_sniffdeauth_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageSniffDeauth* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_sniffdeauth_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageSniffDeauth* stage_sniffdeauth = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_sniffdeauth->timeout;
}

WifiMarauderScriptEditItem sniffdeauth_items[] = {
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_sniffdeauth_stage_timeout_setup_callback, NULL, wifi_marauder_sniffdeauth_stage_timeout_select_callback
    }
};

// Sniff Esp stage ========================================================================================================================
static void wifi_marauder_sniffesp_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageSniffEsp* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_sniffesp_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageSniffEsp* stage_sniffesp = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_sniffesp->timeout;
}

WifiMarauderScriptEditItem sniffesp_items[] = {
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_sniffesp_stage_timeout_setup_callback, NULL, wifi_marauder_sniffesp_stage_timeout_select_callback
    }
};

// Sniff PMKID stage ======================================================================================================================
static void wifi_marauder_sniffpmkid_stage_force_deauth_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageSniffPmkid* stage = app->script_edit_selected_stage->stage;
    variable_item_set_current_value_index(item, stage->force_deauth);
}

static void wifi_marauder_sniffpmkid_stage_force_deauth_change_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);

    // Get menu item
    uint8_t current_stage_index = variable_item_list_get_selected_item_index(app->script_stage_edit_list);
    const WifiMarauderScriptEditItem* menu_item = &stage_items[current_stage_index];

    // Defines the text of the selected option
    uint8_t option_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, menu_item->options[option_index]);

    // Updates the attribute value of the current stage
    WifiMarauderScriptStageSniffPmkid* stage = app->script_edit_selected_stage->stage;
    stage->force_deauth = option_index;
}

static void wifi_marauder_sniffpmkid_stage_channel_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageSniffPmkid* stage = app->script_edit_selected_stage->stage;
    if (stage->channel >= 0 && stage->channel < 12) {
        variable_item_set_current_value_index(item, stage->channel);
    } else {
        variable_item_set_current_value_index(item, 0);
    }
}

static void wifi_marauder_sniffpmkid_stage_channel_change_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);

    // Get menu item
    uint8_t current_stage_index = variable_item_list_get_selected_item_index(app->script_stage_edit_list);
    const WifiMarauderScriptEditItem* menu_item = &stage_items[current_stage_index];

    // Defines the text of the selected option
    uint8_t option_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, menu_item->options[option_index]);

    // Updates the attribute value of the current stage
    WifiMarauderScriptStageSniffPmkid* stage = app->script_edit_selected_stage->stage;
    stage->channel = option_index;
}

static void wifi_marauder_sniffpmkid_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageSniffPmkid* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_sniffpmkid_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageSniffPmkid* stage_sniffpmkid = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_sniffpmkid->timeout;
}

WifiMarauderScriptEditItem sniffpmkid_items[] = {
    {
        "Force deauth", WifiMarauderScriptEditItemTypeOptionsString, 2, {"no", "yes"},
        wifi_marauder_sniffpmkid_stage_force_deauth_setup_callback, wifi_marauder_sniffpmkid_stage_force_deauth_change_callback, NULL
    },
    {
        "Channel", WifiMarauderScriptEditItemTypeOptionsNumber, 12, {"none", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"},
        wifi_marauder_sniffpmkid_stage_channel_setup_callback, wifi_marauder_sniffpmkid_stage_channel_change_callback, NULL
    },
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_sniffpmkid_stage_timeout_setup_callback, NULL, wifi_marauder_sniffpmkid_stage_timeout_select_callback
    }
};

// Sniff Pwn stage ========================================================================================================================
static void wifi_marauder_sniffpwn_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageSniffPwn* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_sniffpwn_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageSniffPwn* stage_sniffpwn = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_sniffpwn->timeout;
}

WifiMarauderScriptEditItem sniffpwn_items[] = {
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_sniffpwn_stage_timeout_setup_callback, NULL, wifi_marauder_sniffpwn_stage_timeout_select_callback
    }
};

// Beacon List stage ======================================================================================================================
static void wifi_marauder_beaconlist_stage_ssids_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageBeaconList* stage_beaconlist = app->script_edit_selected_stage->stage;
    app->script_stage_edit_strings_reference = &stage_beaconlist->ssids;
    app->script_stage_edit_string_count_reference = &stage_beaconlist->ssid_count;
}

static void wifi_marauder_beaconlist_stage_random_ssids_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageBeaconList* stage = app->script_edit_selected_stage->stage;
    char random_ssids_str[32];
    snprintf(random_ssids_str, sizeof(random_ssids_str), "%d", stage->random_ssids);
    variable_item_set_current_value_text(item, random_ssids_str);
}

static void wifi_marauder_beaconlist_stage_random_ssids_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageBeaconList* stage_beaconlist = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_beaconlist->random_ssids;
}

static void wifi_marauder_beaconlist_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageBeaconList* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_beaconlist_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageBeaconList* stage_beaconlist = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_beaconlist->timeout;
}

WifiMarauderScriptEditItem beaconlist_items[] = {
    {
        "SSIDs", WifiMarauderScriptEditItemTypeListString, 1, {NULL},
        NULL, NULL, wifi_marauder_beaconlist_stage_ssids_select_callback
    },
    {
        "Generate random", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_beaconlist_stage_random_ssids_setup_callback, NULL, wifi_marauder_beaconlist_stage_random_ssids_select_callback
    },
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_beaconlist_stage_timeout_setup_callback, NULL, wifi_marauder_beaconlist_stage_timeout_select_callback
    }
};

// Beacon List stage ======================================================================================================================
static void wifi_marauder_beaconap_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageBeaconAp* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_beaconap_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageBeaconAp* stage_beaconap = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_beaconap->timeout;
}

WifiMarauderScriptEditItem beaconap_items[] = {
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_beaconap_stage_timeout_setup_callback, NULL, wifi_marauder_beaconap_stage_timeout_select_callback
    }
};

// Exec stage =============================================================================================================================
static void wifi_marauder_exec_stage_filter_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageExec* stage = app->script_edit_selected_stage->stage;
    if (stage->command != NULL) {
        variable_item_set_current_value_text(item, stage->command);
    }
}

static void wifi_marauder_exec_stage_filter_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageExec* stage_select = app->script_edit_selected_stage->stage;
    if (stage_select->command == NULL) {
        stage_select->command = malloc(128);
    }
    app->user_input_string_reference = &stage_select->command;
}

WifiMarauderScriptEditItem exec_items[] = {
    {
        "Command", WifiMarauderScriptEditItemTypeString, 1, {NULL},
        wifi_marauder_exec_stage_filter_setup_callback, NULL, wifi_marauder_exec_stage_filter_select_callback
    }
};

// Delay stage ============================================================================================================================
static void wifi_marauder_delay_stage_timeout_setup_callback(VariableItem* item) {
    WifiMarauderApp* app = variable_item_get_context(item);
    WifiMarauderScriptStageDelay* stage = app->script_edit_selected_stage->stage;
    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", stage->timeout);
    variable_item_set_current_value_text(item, timeout_str);
}

static void wifi_marauder_delay_stage_timeout_select_callback(WifiMarauderApp* app) {
    WifiMarauderScriptStageDelay* stage_delay = app->script_edit_selected_stage->stage;
    app->user_input_number_reference = &stage_delay->timeout;
}

WifiMarauderScriptEditItem delay_items[] = {
    {
        "Timeout", WifiMarauderScriptEditItemTypeNumber, 1, {NULL},
        wifi_marauder_delay_stage_timeout_setup_callback, NULL, wifi_marauder_delay_stage_timeout_select_callback
    }
};

// ========================================================================================================================================
void wifi_marauder_scene_script_stage_edit_setup(WifiMarauderApp* app) {
    switch (app->script_edit_selected_stage->type) {
        case WifiMarauderScriptStageTypeScan:
            stage_items = scan_items;
            num_stage_items = 3;
            break;
        case WifiMarauderScriptStageTypeSelect:
            stage_items = select_items;
            num_stage_items = 3;
            break;
        case WifiMarauderScriptStageTypeDeauth:
            stage_items = deauth_items;
            num_stage_items = 1;
            break;
        case WifiMarauderScriptStageTypeProbe:
            stage_items = probe_items;
            num_stage_items = 1;
            break;
        case WifiMarauderScriptStageTypeSniffRaw:
            stage_items = sniffraw_items;
            num_stage_items = 1;
            break;
        case WifiMarauderScriptStageTypeSniffBeacon:
            stage_items = sniffbeacon_items;
            num_stage_items = 1;
            break;
        case WifiMarauderScriptStageTypeSniffDeauth:
            stage_items = sniffdeauth_items;
            num_stage_items = 1;
            break;
        case WifiMarauderScriptStageTypeSniffEsp:
            stage_items = sniffesp_items;
            num_stage_items = 1;
            break;
        case WifiMarauderScriptStageTypeSniffPmkid:
            stage_items = sniffpmkid_items;
            num_stage_items = 3;
            break;
        case WifiMarauderScriptStageTypeSniffPwn:
            stage_items = sniffpwn_items;
            num_stage_items = 1;
            break;
        case WifiMarauderScriptStageTypeBeaconList:
            stage_items = beaconlist_items;
            num_stage_items = 3;
            break;
        case WifiMarauderScriptStageTypeBeaconAp:
            stage_items = beaconap_items;
            num_stage_items = 1;
            break;
        case WifiMarauderScriptStageTypeExec:
            stage_items = exec_items;
            num_stage_items = 1;
            break;
        case WifiMarauderScriptStageTypeDelay:
            stage_items = delay_items;
            num_stage_items = 1;
            break;
    }
}

void wifi_marauder_scene_script_stage_edit_on_enter(void* context) {
    WifiMarauderApp* app = context;
    VariableItemList* script_stage_edit_list = app->script_stage_edit_list;

    variable_item_list_set_enter_callback(app->script_stage_edit_list, wifi_marauder_scene_script_stage_edit_list_enter_callback, app);
    wifi_marauder_scene_script_stage_edit_setup(app);

    if (stage_items != NULL) {
        for (uint32_t i = 0; i < num_stage_items; i++) {
            WifiMarauderScriptEditItem* stage_item = &stage_items[i];

            // Changes the list item to handle it in callbacks
            VariableItem* list_item = variable_item_list_add(app->script_stage_edit_list, stage_item->name, stage_item->num_options, stage_item->change_callback, app);

            variable_item_list_set_selected_item(app->script_stage_edit_list, i);
            if (stage_item->setup_callback != NULL) {
                stage_item->setup_callback(list_item);
            }
            if (stage_item->change_callback != NULL) {
                stage_item->change_callback(list_item);
            }
        }
    }

    variable_item_list_add(app->script_stage_edit_list, "[-] DELETE STAGE", 0, NULL, app);

    variable_item_list_set_selected_item(script_stage_edit_list, scene_manager_get_scene_state(app->scene_manager, WifiMarauderSceneScriptStageEdit));
    view_dispatcher_switch_to_view(app->view_dispatcher, WifiMarauderAppViewScriptStageEdit);
}

bool wifi_marauder_scene_script_stage_edit_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void wifi_marauder_scene_script_stage_edit_on_exit(void* context) {
    WifiMarauderApp* app = context;
    variable_item_list_reset(app->script_stage_edit_list);
}
