#include "../wifi_marauder_app_i.h"
#include "wifi_marauder_script.h"

#define WIFI_MARAUDER_DEFAULT_TIMEOUT_SCAN 15
#define WIFI_MARAUDER_DEFAULT_TIMEOUT_BEACON 60

WifiMarauderScript *wifi_marauder_script_alloc() {
    WifiMarauderScript *script = (WifiMarauderScript *) malloc(sizeof(WifiMarauderScript));
    if (script == NULL) {
        return NULL;
    }
    script->name = NULL;
    script->first_stage = NULL;
    script->repeat = 1;
    return script;
}

void _wifi_marauder_script_load_meta(WifiMarauderScript *script, cJSON *meta_section) {
    if (meta_section != NULL) {
        // Script name
        cJSON* name = cJSON_GetObjectItem(meta_section, "name");
        if (name != NULL) {
            script->name = strdup(name->valuestring);
        }
        // Times the script will be repeated
        cJSON* repeat = cJSON_GetObjectItem(meta_section, "repeat");
        if (repeat != NULL) {
            script->repeat = repeat->valueint;
        }
    }
    if (script->name == NULL) {
        script->name = "New script";
    }
}

WifiMarauderScriptStageScan *_wifi_marauder_script_get_stage_scan(cJSON *stages) {
    cJSON* stage_scan = cJSON_GetObjectItem(stages, "scan");
    if (stage_scan == NULL) {
        return NULL;
    }
    cJSON* type = cJSON_GetObjectItem(stage_scan, "type");
    if (type == NULL) {
        return NULL;
    }
    WifiMarauderScriptScanType scan_type;
    if (strcmp(type->valuestring, "ap") == 0) {
        scan_type = WifiMarauderScriptScanTypeAp;
    } else if (strcmp(type->valuestring, "station") == 0) {
        scan_type = WifiMarauderScriptScanTypeStation;
    } else {
        return NULL;
    }
    cJSON* timeout = cJSON_GetObjectItem(stage_scan, "timeout");
    int scan_timeout = timeout != NULL ? (int)cJSON_GetNumberValue(timeout) : WIFI_MARAUDER_DEFAULT_TIMEOUT_SCAN;

    WifiMarauderScriptStageScan *scan_stage = (WifiMarauderScriptStageScan*) malloc(sizeof(WifiMarauderScriptStageScan));
    scan_stage->type = scan_type;
    scan_stage->timeout = scan_timeout;

    return scan_stage;
}


WifiMarauderScriptStageBeaconList* _wifi_marauder_script_get_stage_beacon_list(cJSON *stages) {
    cJSON* stage_beaconlist = cJSON_GetObjectItem(stages, "beaconlist");
    if (stage_beaconlist == NULL) {
        return NULL;
    }
    WifiMarauderScriptStageBeaconList *beaconlist_stage = (WifiMarauderScriptStageBeaconList*) malloc(sizeof(WifiMarauderScriptStageBeaconList));
    if (beaconlist_stage == NULL) {
        return NULL;
    }
    cJSON* ssids = cJSON_GetObjectItem(stage_beaconlist, "ssids");
    if (ssids == NULL) {
        return NULL;
    }
    // SSID count
    int ssid_count = cJSON_GetArraySize(ssids);
    if (ssid_count == 0) {
        return NULL;
    }
    beaconlist_stage->ssid_count = ssid_count;
    // SSIDs
    beaconlist_stage->ssids = (char**) malloc(sizeof(char*) * ssid_count);
    if (beaconlist_stage->ssids == NULL) {
        return NULL;
    }
    for (int i = 0; i < ssid_count; i++) {
        cJSON* ssid = cJSON_GetArrayItem(ssids, i);
        if (ssid == NULL) {
            continue;
        }
        char* ssid_string = cJSON_GetStringValue(ssid);
        if (ssid_string == NULL) {
            continue;
        }
        beaconlist_stage->ssids[i] = (char*) malloc(sizeof(char) * (strlen(ssid_string) + 1));
        strcpy(beaconlist_stage->ssids[i], ssid_string);
    }
    // Timeout
    cJSON* timeout = cJSON_GetObjectItem(stage_beaconlist, "timeout");
    beaconlist_stage->timeout = timeout != NULL ? (int)cJSON_GetNumberValue(timeout) : WIFI_MARAUDER_DEFAULT_TIMEOUT_BEACON;
    
    return beaconlist_stage;
}

WifiMarauderScriptStage* _wifi_marauder_script_create_stage(WifiMarauderScriptStageType type, void* stage_data) {
    WifiMarauderScriptStage* stage = (WifiMarauderScriptStage*) malloc(sizeof(WifiMarauderScriptStage));
    stage->type = type;
    stage->stage = stage_data;
    stage->next_stage = NULL;
    return stage;
}

void _wifi_marauder_script_add_stage(WifiMarauderScript *script, WifiMarauderScriptStage *stage, WifiMarauderScriptStage **prev_stage) {
    if (*prev_stage != NULL) {
        (*prev_stage)->next_stage = stage;
    } else {
        script->first_stage = stage;
    }
    *prev_stage = stage;
}

void _wifi_marauder_script_load_stages(WifiMarauderScript *script, cJSON *stages) {
    WifiMarauderScriptStage *prev_stage = NULL;

    // Scan stage
    WifiMarauderScriptStageScan *stage_scan = _wifi_marauder_script_get_stage_scan(stages);
    if (stage_scan != NULL) {
        WifiMarauderScriptStage *stage = _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeScan, stage_scan);
        _wifi_marauder_script_add_stage(script, stage, &prev_stage);
    }

    // Beacon List stage
    WifiMarauderScriptStageBeaconList *stage_beacon_list = _wifi_marauder_script_get_stage_beacon_list(stages);
    if (stage_beacon_list != NULL) {
        WifiMarauderScriptStage *stage = _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeBeaconList, stage_beacon_list);
        _wifi_marauder_script_add_stage(script, stage, &prev_stage);
    }
}

WifiMarauderScript *wifi_marauder_script_parse_raw(const char* json_raw) {
    WifiMarauderScript *script = wifi_marauder_script_alloc();
    if (script == NULL) {
        return NULL;
    }
    cJSON* json = cJSON_Parse(json_raw);
    if (json == NULL) {
        return NULL;
    }
    cJSON* meta = cJSON_GetObjectItem(json, "meta");
    _wifi_marauder_script_load_meta(script, meta);
    cJSON* stages = cJSON_GetObjectItem(json, "stages");
    _wifi_marauder_script_load_stages(script, stages);

    return script;
}

WifiMarauderScript *wifi_marauder_script_parse_file(const char* file_path, Storage* storage) {
    WifiMarauderScript *script = NULL;
    File* script_file = storage_file_alloc(storage);

    if (storage_file_open(script_file, file_path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint32_t file_size = storage_file_size(script_file);
        char* json_buffer = (char*)malloc(file_size + 1);
        uint16_t bytes_read = storage_file_read(script_file, json_buffer, file_size);
        json_buffer[bytes_read] = '\0';
        
        script = wifi_marauder_script_parse_raw(json_buffer);
        storage_file_close(script_file);
    }

    storage_file_free(script_file);
    return script;
}

void wifi_marauder_script_free(WifiMarauderScript *script) {
    if (script == NULL) {
        return;
    }
    WifiMarauderScriptStage *current_stage = script->first_stage;
    while (current_stage != NULL) {
        WifiMarauderScriptStage *next_stage = current_stage->next_stage;
        switch (current_stage->type) {
            case WifiMarauderScriptStageTypeScan:
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeSelect:
                for (int i = 0; i < ((WifiMarauderScriptStageSelect *) current_stage->stage)->filter_count; i++) {
                    free(((WifiMarauderScriptStageSelect *) current_stage->stage)->filters[i].filter_string);
                }
                free(((WifiMarauderScriptStageSelect *) current_stage->stage)->filters);
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeSniffPmkid:
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeBeaconList:
                for (int i = 0; i < ((WifiMarauderScriptStageBeaconList *) current_stage->stage)->ssid_count; i++) {
                    free(((WifiMarauderScriptStageBeaconList *) current_stage->stage)->ssids[i]);
                }
                free(((WifiMarauderScriptStageBeaconList *) current_stage->stage)->ssids);
                free(current_stage->stage);
                break;
        }
        free(current_stage);
        current_stage = next_stage;
    }
    free(script->name);
    free(script);
}