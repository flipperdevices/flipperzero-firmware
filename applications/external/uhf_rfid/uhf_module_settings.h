#pragma once

#include <stdint.h>

// UHF module baudrates
static const uint32_t BAUD_RATES[] = {9600, 19200, 115200};

// UHF module regions
typedef enum {
    WA_CHINA_900 = 1, // Freq_CH-920.125M
    WA_US, // Freq_CH-902.25M
    WA_EU, // Freq_CH-865.1M
    WA_CHINA_800, // Freq_CH-840.125M
    WA_KOREA = 6 // Freq_CH-917.1M
} WorkingArea;

typedef enum {
    WC_CHINA_900 = 1, // CH_Index(CN,900MHz) = (Freq_CH-920.125M)/0.25M
    WC_US, // CH_Index(US) = (Freq_CH-902.25M)/0.5M
    WC_EU, // CH_Index(EU) = (Freq_CH-865.1M)/0.2M
    WC_CHINA_800, // CH_Index(CN,800MHz) = (Freq_CH-840.125M)/0.25M
    WC_KOREA = 6 // CH_Index(Korea) = (Freq_CH-917.1M)/0.2M
} WorkingChannel;

// RF Power Setting
static const uint8_t POWER_DBM[] = {12, 14, 17, 20}; // To be determined ...