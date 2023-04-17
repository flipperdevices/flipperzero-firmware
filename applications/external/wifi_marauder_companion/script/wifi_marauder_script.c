#include "../wifi_marauder_app_i.h"
#include "wifi_marauder_script.h"

#define WIFI_MARAUDER_DEFAULT_TIMEOUT_SCAN 15
#define WIFI_MARAUDER_DEFAULT_TIMEOUT_DEAUTH 30
#define WIFI_MARAUDER_DEFAULT_TIMEOUT_PROBE 60
#define WIFI_MARAUDER_DEFAULT_TIMEOUT_SNIFF 60
#define WIFI_MARAUDER_DEFAULT_TIMEOUT_BEACON 60

WifiMarauderScript *wifi_marauder_script_alloc() {
    WifiMarauderScript *script = (WifiMarauderScript *) malloc(sizeof(WifiMarauderScript));
    if (script == NULL) {
        return NULL;
    }
    script->name = NULL;
    script->description = NULL;
    script->first_stage = NULL;
    script->enable_led = WifiMarauderScriptBooleanUndefined;
    script->save_pcap = WifiMarauderScriptBooleanUndefined;
    script->repeat = 1;
    return script;
}

void _wifi_marauder_script_load_meta(WifiMarauderScript *script, cJSON *meta_section) {
    if (meta_section != NULL) {
        // Script description
        cJSON* description = cJSON_GetObjectItem(meta_section, "description");
        if (description != NULL) {
            script->description = strdup(description->valuestring);
        }
        // Enable LED
        cJSON* enable_led_json = cJSON_GetObjectItemCaseSensitive(meta_section, "enableLed");
        if (cJSON_IsBool(enable_led_json)) {
            script->enable_led = enable_led_json->valueint;
        }
        // Save PCAP
        cJSON* save_pcap_json = cJSON_GetObjectItemCaseSensitive(meta_section, "savePcap");
        if (cJSON_IsBool(save_pcap_json)) {
            script->save_pcap = save_pcap_json->valueint;
        }
        // Times the script will be repeated
        cJSON* repeat = cJSON_GetObjectItem(meta_section, "repeat");
        if (repeat != NULL) {
            script->repeat = repeat->valueint;
        }
    }
    if (script->description == NULL) {
        script->description = strdup("My script");
    }
}

WifiMarauderScriptStageScan* _wifi_marauder_script_get_stage_scan(cJSON *stages) {
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
    cJSON* channel = cJSON_GetObjectItem(stage_scan, "channel");
    int scan_channel = channel != NULL ? (int)cJSON_GetNumberValue(channel) : 0;
    cJSON* timeout = cJSON_GetObjectItem(stage_scan, "timeout");
    int scan_timeout = timeout != NULL ? (int)cJSON_GetNumberValue(timeout) : WIFI_MARAUDER_DEFAULT_TIMEOUT_SCAN;

    WifiMarauderScriptStageScan *scan_stage = (WifiMarauderScriptStageScan*) malloc(sizeof(WifiMarauderScriptStageScan));
    scan_stage->type = scan_type;
    scan_stage->channel = scan_channel;
    scan_stage->timeout = scan_timeout;

    return scan_stage;
}

WifiMarauderScriptStageSelect* _wifi_marauder_script_get_stage_select(cJSON *stages) {
    cJSON *select_stage_json = cJSON_GetObjectItemCaseSensitive(stages, "select");
    if (select_stage_json == NULL) {
        return NULL;
    }

    cJSON *type_json = cJSON_GetObjectItemCaseSensitive(select_stage_json, "type");
    cJSON *filter_json = cJSON_GetObjectItemCaseSensitive(select_stage_json, "filter");
    cJSON *indexes_json = cJSON_GetObjectItemCaseSensitive(select_stage_json, "indexes");
    cJSON *index_json = cJSON_GetObjectItemCaseSensitive(select_stage_json, "index");
    cJSON *allow_repeat_json = cJSON_GetObjectItemCaseSensitive(select_stage_json, "allow_repeat");

    if (!cJSON_IsString(type_json) || !cJSON_IsString(filter_json)) {
        return NULL;
    }

    WifiMarauderScriptSelectType select_type;
    if (strcmp(type_json->valuestring, "ap") == 0) {
        select_type = WifiMarauderScriptSelectTypeAp;
    } else if (strcmp(type_json->valuestring, "station") == 0) {
        select_type = WifiMarauderScriptSelectTypeStation;
    } else if (strcmp(type_json->valuestring, "ssid") == 0) {
        select_type = WifiMarauderScriptSelectTypeSsid;
    } else {
        return NULL;
    }

    char *filter_str = strdup(filter_json->valuestring);

    WifiMarauderScriptStageSelect *stage_select = (WifiMarauderScriptStageSelect*) malloc(sizeof(WifiMarauderScriptStageSelect));
    stage_select->type = select_type;
    stage_select->filter = filter_str;
    stage_select->allow_repeat = cJSON_IsBool(allow_repeat_json) ? allow_repeat_json->valueint : true;

    if (cJSON_IsNumber(index_json)) {
        int* indexes = (int*) malloc(sizeof(int));
        indexes[0] = index_json->valueint;
        stage_select->indexes = indexes;
    } else if (cJSON_IsArray(indexes_json)) {
        int indexes_size = cJSON_GetArraySize(indexes_json);
        int* indexes = (int*) malloc(indexes_size * sizeof(int));
        for (int i = 0; i < indexes_size; i++) {
            cJSON *index_item = cJSON_GetArrayItem(indexes_json, i);
            if (cJSON_IsNumber(index_item)) {
                indexes[i] = index_item->valueint;
            }
        }
        stage_select->indexes = indexes;
    } else {
        stage_select->indexes = NULL;
    }

    return stage_select;
}

WifiMarauderScriptStageDeauth* _wifi_marauder_script_get_stage_deauth(cJSON *stages) {
    cJSON *deauth_stage_json = cJSON_GetObjectItemCaseSensitive(stages, "deauth");
    if (deauth_stage_json == NULL) {
        return NULL;
    }

    cJSON* timeout = cJSON_GetObjectItem(deauth_stage_json, "timeout");
    int deauth_timeout = timeout != NULL ? (int)cJSON_GetNumberValue(timeout) : WIFI_MARAUDER_DEFAULT_TIMEOUT_DEAUTH;

    WifiMarauderScriptStageDeauth *deauth_stage = (WifiMarauderScriptStageDeauth*) malloc(sizeof(WifiMarauderScriptStageDeauth));
    deauth_stage->timeout = deauth_timeout;

    return deauth_stage;
}

WifiMarauderScriptStageProbe* _wifi_marauder_script_get_stage_probe(cJSON *stages) {
    cJSON *probe_stage_json = cJSON_GetObjectItemCaseSensitive(stages, "probe");
    if (probe_stage_json == NULL) {
        return NULL;
    }

    cJSON* timeout = cJSON_GetObjectItem(probe_stage_json, "timeout");
    int probe_timeout = timeout != NULL ? (int)cJSON_GetNumberValue(timeout) : WIFI_MARAUDER_DEFAULT_TIMEOUT_PROBE;

    WifiMarauderScriptStageProbe *probe_stage = (WifiMarauderScriptStageProbe*) malloc(sizeof(WifiMarauderScriptStageProbe));
    probe_stage->timeout = probe_timeout;

    return probe_stage;
}

WifiMarauderScriptStageSniffRaw* _wifi_marauder_script_get_stage_sniff_raw(cJSON *stages) {
    cJSON* sniffraw_stage_json = cJSON_GetObjectItem(stages, "sniffraw");
    if (sniffraw_stage_json == NULL) {
        return NULL;
    }

    cJSON* timeout_json = cJSON_GetObjectItem(sniffraw_stage_json, "timeout");
    int timeout = timeout_json != NULL ? (int)cJSON_GetNumberValue(timeout_json) : WIFI_MARAUDER_DEFAULT_TIMEOUT_SNIFF;

    WifiMarauderScriptStageSniffRaw *sniff_raw_stage = (WifiMarauderScriptStageSniffRaw*) malloc(sizeof(WifiMarauderScriptStageSniffRaw));
    sniff_raw_stage->timeout = timeout;

    return sniff_raw_stage;
}

WifiMarauderScriptStageSniffBeacon* _wifi_marauder_script_get_stage_sniff_beacon(cJSON *stages) {
    cJSON* sniffbeacon_stage_json = cJSON_GetObjectItem(stages, "sniffbeacon");
    if (sniffbeacon_stage_json == NULL) {
        return NULL;
    }

    cJSON* timeout_json = cJSON_GetObjectItem(sniffbeacon_stage_json, "timeout");
    int timeout = timeout_json != NULL ? (int)cJSON_GetNumberValue(timeout_json) : WIFI_MARAUDER_DEFAULT_TIMEOUT_SNIFF;

    WifiMarauderScriptStageSniffBeacon *sniff_beacon_stage = (WifiMarauderScriptStageSniffBeacon*) malloc(sizeof(WifiMarauderScriptStageSniffBeacon));
    sniff_beacon_stage->timeout = timeout;

    return sniff_beacon_stage;
}

WifiMarauderScriptStageSniffDeauth* _wifi_marauder_script_get_stage_sniff_deauth(cJSON *stages) {
    cJSON* sniffdeauth_stage_json = cJSON_GetObjectItem(stages, "sniffdeauth");
    if (sniffdeauth_stage_json == NULL) {
        return NULL;
    }

    cJSON* timeout_json = cJSON_GetObjectItem(sniffdeauth_stage_json, "timeout");
    int timeout = timeout_json != NULL ? (int)cJSON_GetNumberValue(timeout_json) : WIFI_MARAUDER_DEFAULT_TIMEOUT_SNIFF;

    WifiMarauderScriptStageSniffDeauth *sniff_deauth_stage = (WifiMarauderScriptStageSniffDeauth*) malloc(sizeof(WifiMarauderScriptStageSniffDeauth));
    sniff_deauth_stage->timeout = timeout;

    return sniff_deauth_stage;
}

WifiMarauderScriptStageSniffEsp* _wifi_marauder_script_get_stage_sniff_esp(cJSON *stages) {
    cJSON* sniffesp_stage_json = cJSON_GetObjectItem(stages, "sniffesp");
    if (sniffesp_stage_json == NULL) {
        return NULL;
    }

    cJSON* timeout_json = cJSON_GetObjectItem(sniffesp_stage_json, "timeout");
    int timeout = timeout_json != NULL ? (int)cJSON_GetNumberValue(timeout_json) : WIFI_MARAUDER_DEFAULT_TIMEOUT_SNIFF;

    WifiMarauderScriptStageSniffEsp *sniff_esp_stage = (WifiMarauderScriptStageSniffEsp*) malloc(sizeof(WifiMarauderScriptStageSniffEsp));
    sniff_esp_stage->timeout = timeout;

    return sniff_esp_stage;
}

WifiMarauderScriptStageSniffPmkid* _wifi_marauder_script_get_stage_sniff_pmkid(cJSON *stages) {
    cJSON* sniffpmkid_stage_json = cJSON_GetObjectItem(stages, "sniffpmkid");
    if (sniffpmkid_stage_json == NULL) {
        return NULL;
    }

    cJSON* channel_json = cJSON_GetObjectItem(sniffpmkid_stage_json, "channel");
    int channel = channel_json != NULL ? (int)cJSON_GetNumberValue(channel_json) : 0;
    cJSON* timeout_json = cJSON_GetObjectItem(sniffpmkid_stage_json, "timeout");
    int timeout = timeout_json != NULL ? (int)cJSON_GetNumberValue(timeout_json) : WIFI_MARAUDER_DEFAULT_TIMEOUT_SNIFF;
    cJSON* force_deauth_json = cJSON_GetObjectItemCaseSensitive(sniffpmkid_stage_json, "forceDeauth");
    bool force_deauth = cJSON_IsBool(force_deauth_json) ? force_deauth_json->valueint : true;

    WifiMarauderScriptStageSniffPmkid *sniff_pmkid_stage = (WifiMarauderScriptStageSniffPmkid*) malloc(sizeof(WifiMarauderScriptStageSniffPmkid));
    sniff_pmkid_stage->channel = channel;
    sniff_pmkid_stage->timeout = timeout;
    sniff_pmkid_stage->force_deauth = force_deauth;

    return sniff_pmkid_stage;
}

WifiMarauderScriptStageSniffPwn* _wifi_marauder_script_get_stage_sniff_pwn(cJSON *stages) {
    cJSON* sniffpwn_stage_json = cJSON_GetObjectItem(stages, "sniffpwn");
    if (sniffpwn_stage_json == NULL) {
        return NULL;
    }

    cJSON* timeout_json = cJSON_GetObjectItem(sniffpwn_stage_json, "timeout");
    int timeout = timeout_json != NULL ? (int)cJSON_GetNumberValue(timeout_json) : WIFI_MARAUDER_DEFAULT_TIMEOUT_SNIFF;

    WifiMarauderScriptStageSniffPwn *sniff_pwn_stage = (WifiMarauderScriptStageSniffPwn*) malloc(sizeof(WifiMarauderScriptStageSniffPwn));
    sniff_pwn_stage->timeout = timeout;

    return sniff_pwn_stage;
}

WifiMarauderScriptStageBeaconList* _wifi_marauder_script_get_stage_beacon_list(cJSON *stages) {
    cJSON* stage_beaconlist = cJSON_GetObjectItem(stages, "beaconList");
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
    // Random SSIDs
    cJSON* random_ssids = cJSON_GetObjectItem(stage_beaconlist, "generate");
    beaconlist_stage->random_ssids = random_ssids != NULL ? (int)cJSON_GetNumberValue(random_ssids) : 0;
    
    return beaconlist_stage;
}

WifiMarauderScriptStageBeaconAp* _wifi_marauder_script_get_stage_beacon_ap(cJSON *stages) {
    cJSON* beaconap_stage_json = cJSON_GetObjectItem(stages, "beaconAp");
    if (beaconap_stage_json == NULL) {
        return NULL;
    }

    cJSON* timeout_json = cJSON_GetObjectItem(beaconap_stage_json, "timeout");
    int timeout = timeout_json != NULL ? (int)cJSON_GetNumberValue(timeout_json) : WIFI_MARAUDER_DEFAULT_TIMEOUT_BEACON;

    WifiMarauderScriptStageBeaconAp *beacon_ap_stage = (WifiMarauderScriptStageBeaconAp*) malloc(sizeof(WifiMarauderScriptStageBeaconAp));
    beacon_ap_stage->timeout = timeout;

    return beacon_ap_stage;
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
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeScan, stage_scan),
            &prev_stage
        );
    }

    // Select stage
    WifiMarauderScriptStageSelect *stage_select = _wifi_marauder_script_get_stage_select(stages);
    if (stage_select != NULL) {
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeSelect, stage_select),
            &prev_stage
        );
    }

    // Deauth stage
    WifiMarauderScriptStageDeauth *stage_deauth = _wifi_marauder_script_get_stage_deauth(stages);
    if (stage_deauth != NULL) {
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeDeauth, stage_deauth),
            &prev_stage
        );
    }

    // Probe stage
    WifiMarauderScriptStageProbe *stage_probe = _wifi_marauder_script_get_stage_probe(stages);
    if (stage_probe != NULL) {
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeProbe, stage_probe),
            &prev_stage
        );
    }

    // Sniff raw stage
    WifiMarauderScriptStageSniffRaw *sniff_raw = _wifi_marauder_script_get_stage_sniff_raw(stages);
    if (sniff_raw != NULL) {
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeSniffRaw, sniff_raw),
            &prev_stage
        );
    }

    // Sniff beacon stage
    WifiMarauderScriptStageSniffBeacon *sniff_beacon = _wifi_marauder_script_get_stage_sniff_beacon(stages);
    if (sniff_beacon != NULL) {
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeSniffBeacon, sniff_beacon),
            &prev_stage
        );
    }

    // Sniff deauth stage
    WifiMarauderScriptStageSniffDeauth *sniff_deauth = _wifi_marauder_script_get_stage_sniff_deauth(stages);
    if (sniff_deauth != NULL) {
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeSniffDeauth, sniff_deauth),
            &prev_stage
        );
    }

    // Sniff esp stage
    WifiMarauderScriptStageSniffEsp *sniff_esp = _wifi_marauder_script_get_stage_sniff_esp(stages);
    if (sniff_esp != NULL) {
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeSniffEsp, sniff_esp),
            &prev_stage
        );
    }

    // Sniff PMKID stage
    WifiMarauderScriptStageSniffPmkid *sniff_pmkid = _wifi_marauder_script_get_stage_sniff_pmkid(stages);
    if (sniff_pmkid != NULL) {
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeSniffPmkid, sniff_pmkid),
            &prev_stage
        );
    }

    // Sniff pwn stage
    WifiMarauderScriptStageSniffPwn *sniff_pwn = _wifi_marauder_script_get_stage_sniff_pwn(stages);
    if (sniff_pwn != NULL) {
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeSniffPwn, sniff_pwn),
            &prev_stage
        );
    }

    // Beacon List stage
    WifiMarauderScriptStageBeaconList *stage_beacon_list = _wifi_marauder_script_get_stage_beacon_list(stages);
    if (stage_beacon_list != NULL) {
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeBeaconList, stage_beacon_list),
            &prev_stage
        );
    }

    // Beacon Ap stage
    WifiMarauderScriptStageBeaconAp *stage_beacon_ap = _wifi_marauder_script_get_stage_beacon_ap(stages);
    if (stage_beacon_ap != NULL) {
        _wifi_marauder_script_add_stage(
            script,
            _wifi_marauder_script_create_stage(WifiMarauderScriptStageTypeBeaconAp, stage_beacon_ap),
            &prev_stage
        );
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
        if (script != NULL) {
            // Set script name
            FuriString* script_name = furi_string_alloc();
            path_extract_filename_no_ext(file_path, script_name);
            script->name = strdup(furi_string_get_cstr(script_name));
            furi_string_free(script_name);
        }
        storage_file_close(script_file);
    }

    storage_file_free(script_file);
    return script;
}

WifiMarauderScriptStage* wifi_marauder_script_get_stage(WifiMarauderScript* script, WifiMarauderScriptStageType stage_type) {
    if (script == NULL) {
        return NULL;
    }
    WifiMarauderScriptStage* current_stage = script->first_stage;
    while (current_stage != NULL) {
        if (current_stage->type == stage_type) {
            return current_stage;
        }
        current_stage = current_stage->next_stage;
    }
    return NULL;
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
                if (((WifiMarauderScriptStageSelect *) current_stage->stage)->filter != NULL) {
                    free(((WifiMarauderScriptStageSelect *) current_stage->stage)->filter);
                }
                if (((WifiMarauderScriptStageSelect *) current_stage->stage)->indexes != NULL) {
                    free(((WifiMarauderScriptStageSelect *) current_stage->stage)->indexes);
                }
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeDeauth:
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeProbe:
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeSniffRaw:
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeSniffBeacon:
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeSniffDeauth:
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeSniffEsp:
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeSniffPmkid:
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeSniffPwn:
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeBeaconList:
                for (int i = 0; i < ((WifiMarauderScriptStageBeaconList *) current_stage->stage)->ssid_count; i++) {
                    free(((WifiMarauderScriptStageBeaconList *) current_stage->stage)->ssids[i]);
                }
                free(((WifiMarauderScriptStageBeaconList *) current_stage->stage)->ssids);
                free(current_stage->stage);
                break;
            case WifiMarauderScriptStageTypeBeaconAp:
                free(current_stage->stage);
                break;
        }
        free(current_stage);
        current_stage = next_stage;
    }
    free(script->name);
    free(script->description);
    free(script);
}