/*
 * Steps to add a new stage:
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
 */

#pragma once

#include <storage/storage.h>
#include "cJSON.h"

typedef enum {
    WifiMarauderScriptStageTypeScan,
    WifiMarauderScriptStageTypeSelect,
    WifiMarauderScriptStageTypeSniffPmkid,
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
    int timeout;
} WifiMarauderScriptStageScan;

typedef struct WifiMarauderScriptStageSelect {
    WifiMarauderScriptSelectType type;
    char* filter;
    // TODO: Implement a feature to not select the same items in the next iteration of the script
    bool allow_repeat;
} WifiMarauderScriptStageSelect;

typedef struct WifiMarauderScriptStageSniffPmkid {
    bool force_deauth;
    int timeout;
} WifiMarauderScriptStageSniffPmkid;

typedef struct WifiMarauderScriptStageBeaconList {
    char **ssids;
    int ssid_count;
    int timeout;
} WifiMarauderScriptStageBeaconList;

// Script
typedef struct WifiMarauderScript {
    char* name;
    WifiMarauderScriptStage *first_stage;
    int repeat;
} WifiMarauderScript;

WifiMarauderScript *wifi_marauder_script_alloc();
WifiMarauderScript *wifi_marauder_script_parse_raw(const char* script_raw);
WifiMarauderScript *wifi_marauder_script_parse_file(const char* file_path, Storage* storage);
//void* wifi_marauder_script_get_stage(WifiMarauderScript *script, WifiMarauderScriptStageType stage);
void wifi_marauder_script_free(WifiMarauderScript *script);
