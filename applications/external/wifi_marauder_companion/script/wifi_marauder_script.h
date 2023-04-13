/*
 * ----------------------------------------------------------------------------------------------------
 * STEPS TO ADD A NEW STAGE:
 * 
 * wifi_marauder_script.h
 * - Complement WifiMarauderScriptStageType enum with new stage
 * - Create struct WifiMarauderScriptStage???? for the new stage
 * 
 * wifi_marauder_script.c
 * - Create function "WifiMarauderScriptStage????* _wifi_marauder_script_get_stage_????(cJSON *stages)"
 * - Change _wifi_marauder_script_load_stages() to load new stage
 * - Add case to free memory in wifi_marauder_script_free()
 * 
 * wifi_marauder_script_executor.c
 * - Create function "void _wifi_marauder_script_execute_????(WifiMarauderScriptStage????* stage)"
 * - Add case in wifi_marauder_script_execute_stage()
 * 
 * ----------------------------------------------------------------------------------------------------
 * IMPLEMENTED STAGES (In order of execution):
 * - Scan
 * - Select
 * - Deauth
 * - Probe
 * - Sniff raw
 * - Sniff beacon
 * - Sniff deauth
 * - Sniff Espressif
 * - Sniff PMKID
 * - Sniff Pwnagotchi
 * - Beacon List
 * ----------------------------------------------------------------------------------------------------
 * SCRIPT SYNTAX:
 * {
 *     "meta": {
 *         "description": "My script",
 *         "repeat": times the script will repeat (default 1),
 *         "enableLed": true (default) | false,
 *         "savePcap": true (default) | false
 *     },
 *     "stages": {
 *         "scan": {
 *             "type": "ap" | "station",
 *             "timeout": seconds,
 *             "channel": 1-11
 *         },
 *         "select": {
 *             "type": "ap" | "station" | "ssid",
 *             "filter": "all" | "contains \"{SSID fragment}\" or equals \"{SSID}\" or ..." (Not implemented yet on Marauder firmware)
 *         },
 *         "deauth": {
 *             "timeout": seconds
 *         },
 *         "probe": {
 *             "timeout": seconds
 *         },
 *         "sniffRaw": {
 *             "timeout": seconds
 *         },
 *         "sniffBeacon": {
 *             "timeout": seconds
 *         },
 *         "sniffDeauth": {
 *             "timeout": seconds
 *         },
 *         "sniffEsp": {
 *             "timeout": seconds
 *         },
 *         "sniffPmkid": {
 *             "forceDeauth": true (default) | false,
 *             "channel": 1-11,
 *             "timeout": seconds
 *         },
 *         "sniffPwn": {
 *             "timeout": seconds
 *         },
 *         "beaconlist": {
 *             "ssids": [
 *                 "SSID 1",
 *                 "SSID 2",
 *                 "SSID 3"
 *             ],
 *             "timeout": seconds
 *         }
 *     }
 * }
 * ----------------------------------------------------------------------------------------------------
 */

#pragma once

#include <storage/storage.h>
#include "cJSON.h"

typedef enum {
    WifiMarauderScriptStageTypeScan,
    WifiMarauderScriptStageTypeSelect,
    WifiMarauderScriptStageTypeDeauth,
    WifiMarauderScriptStageTypeProbe,
    WifiMarauderScriptStageTypeSniffRaw,
    WifiMarauderScriptStageTypeSniffBeacon,
    WifiMarauderScriptStageTypeSniffDeauth,
    WifiMarauderScriptStageTypeSniffEsp,
    WifiMarauderScriptStageTypeSniffPmkid,
    WifiMarauderScriptStageTypeSniffPwn,
    WifiMarauderScriptStageTypeBeaconList,
} WifiMarauderScriptStageType;

typedef enum {
    WifiMarauderScriptScanTypeAp,
    WifiMarauderScriptScanTypeStation
} WifiMarauderScriptScanType;

typedef enum {
    WifiMarauderScriptSelectTypeAp,
    WifiMarauderScriptSelectTypeStation,
    WifiMarauderScriptSelectTypeSsid
} WifiMarauderScriptSelectType;

// Stages
typedef struct WifiMarauderScriptStage {
    WifiMarauderScriptStageType type;
    void* stage;
    struct WifiMarauderScriptStage *next_stage;
} WifiMarauderScriptStage;

typedef struct WifiMarauderScriptStageScan {
    WifiMarauderScriptScanType type;
    int channel;
    int timeout;
} WifiMarauderScriptStageScan;

typedef struct WifiMarauderScriptStageSelect {
    WifiMarauderScriptSelectType type;
    char* filter;
    // TODO: Implement a feature to not select the same items in the next iteration of the script
    bool allow_repeat;
} WifiMarauderScriptStageSelect;

typedef struct WifiMarauderScriptStageDeauth {
    int timeout;
} WifiMarauderScriptStageDeauth;

typedef struct WifiMarauderScriptStageProbe {
    int timeout;
} WifiMarauderScriptStageProbe;

typedef struct WifiMarauderScriptStageSniffRaw {
    int timeout;
} WifiMarauderScriptStageSniffRaw;

typedef struct WifiMarauderScriptStageSniffBeacon {
    int timeout;
} WifiMarauderScriptStageSniffBeacon;

typedef struct WifiMarauderScriptStageSniffDeauth {
    int timeout;
} WifiMarauderScriptStageSniffDeauth;

typedef struct WifiMarauderScriptStageSniffEsp {
    int timeout;
} WifiMarauderScriptStageSniffEsp;

typedef struct WifiMarauderScriptStageSniffPmkid {
    bool force_deauth;
    int channel;
    int timeout;
} WifiMarauderScriptStageSniffPmkid;

typedef struct WifiMarauderScriptStageSniffPwn {
    int timeout;
} WifiMarauderScriptStageSniffPwn;

typedef struct WifiMarauderScriptStageBeaconList {
    char **ssids;
    int ssid_count;
    int timeout;
} WifiMarauderScriptStageBeaconList;

// Script
typedef struct WifiMarauderScript {
    char* name;
    char* description;
    WifiMarauderScriptStage *first_stage;
    // TODO: Think of a way to not change the settings if they are not informed in the JSON
    bool enable_led;
    bool save_pcap;
    int repeat;
} WifiMarauderScript;

WifiMarauderScript* wifi_marauder_script_alloc();
WifiMarauderScript* wifi_marauder_script_parse_raw(const char* script_raw);
WifiMarauderScript* wifi_marauder_script_parse_file(const char* file_path, Storage* storage);
WifiMarauderScriptStage* wifi_marauder_script_get_stage(WifiMarauderScript* script, WifiMarauderScriptStageType stage_type);
void wifi_marauder_script_free(WifiMarauderScript *script);
