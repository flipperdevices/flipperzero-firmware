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

// Filters
typedef enum {
    WifiMarauderScriptFilterTypeContains,
    WifiMarauderScriptFilterTypeEqual,
} WifiMarauderScriptFilterType;

typedef struct WifiMarauderScriptSelectFilter {
    WifiMarauderScriptFilterType type;
    char* filter_string;
} WifiMarauderScriptSelectFilter;

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
    WifiMarauderScriptSelectFilter *filters;
    int filter_count;
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
