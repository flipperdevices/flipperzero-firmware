#pragma once

#include <stdint.h>

// UHF module regions
typedef enum {
    WR_CHINA_900 = 1, // Freq_CH-920.125M
    WR_US, // Freq_CH-902.25M
    WR_EU, // Freq_CH-865.1M
    WR_CHINA_800, // Freq_CH-840.125M
    WR_KOREA = 6 // Freq_CH-917.1M
} WorkingRegion;

// UHF module baudrates
static const uint32_t BAUD_RATES[] = {9600, 19200, 115200};
static const uint8_t BAUD_RATES_COUNT = sizeof(BAUD_RATES) / sizeof(BAUD_RATES[0]);
// RF Power Setting
static const uint8_t POWER_DBM[] = {12, 14, 17, 20}; // To be determined ...
static const uint8_t POWER_DBM_COUNT = sizeof(POWER_DBM) / sizeof(POWER_DBM[0]);
// UHF WorkingArea
static const char* WORKING_REGIONS_STR[] = {"CN1", "US", "EU", "CN2", "KR"};
static const uint8_t __working_region_str =
    sizeof(WORKING_REGIONS_STR) / sizeof(WORKING_REGIONS_STR[0]);
static const WorkingRegion WORKING_REGIONS[] = {WR_CHINA_900, WR_US, WR_EU, WR_CHINA_800, WR_KOREA};
static const uint8_t WORKING_REGIONS_COUNT = sizeof(WORKING_REGIONS) / sizeof(WORKING_REGIONS[0]);
// UHF WorkingChannel
// static const string WORKING_CHANNELS_STR[] = {"China 900MHz", "US", "EU", "China 800MHz", "Korea"};
// static const WorkingChannel WORKING_CHANNELS[] = {WC_CHINA_900, WC_US, WC_EU, WC_CHINA_800, WC_KOREA};
// static const uint8_t WORKING_CHANNELS_COUNT = sizeof(WORKING_CHANNELS) / sizeof(WORKING_CHANNELS[0]);
