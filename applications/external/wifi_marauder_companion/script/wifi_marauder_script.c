#include "../wifi_marauder_app_i.h"
#include "wifi_marauder_script.h"

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

WifiMarauderScriptStageBeaconList *_wifi_marauder_script_get_stage_beacon_list(cJSON *stages) {
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
    beaconlist_stage->timeout = timeout != NULL ? (int)cJSON_GetNumberValue(timeout) : 60;
    
    return beaconlist_stage;
}

void _wifi_marauder_script_load_stages(WifiMarauderScript *script, cJSON *stages) {
    // Beacon List stage
    WifiMarauderScriptStageBeaconList *stage_beacon_list = _wifi_marauder_script_get_stage_beacon_list(stages);
    if (stage_beacon_list != NULL) {
        WifiMarauderScriptStage *stage = (WifiMarauderScriptStage*) malloc(sizeof(WifiMarauderScriptStage));
        stage->type = WifiMarauderScriptStageTypeBeaconList;
        stage->stage = stage_beacon_list;
        stage->next_stage = NULL;
        script->first_stage = stage;
    }

    // TODO: load stages
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