#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct{
    char* hw_version;
    char* sw_Version;
    char* manufacturer;
}M100ModuleInfo;

typedef enum{
    CHINA_900 = 1,  // Freq_CH-920.125M
    US,             // Freq_CH-902.25M
    EU,             // Freq_CH-865.1M
    CHINA_800,      // Freq_CH-840.125M
    KOREA = 6       // Freq_CH-917.1M
}WorkingArea;

typedef enum{
    CHINA_900 = 1,  // CH_Index(CN,900MHz) = (Freq_CH-920.125M)/0.25M
    US,             // CH_Index(US) = (Freq_CH-902.25M)/0.5M
    EU,             // CH_Index(EU) = (Freq_CH-865.1M)/0.2M
    CHINA_800,      // CH_Index(CN,800MHz) = (Freq_CH-840.125M)/0.25M
    KOREA = 6       // CH_Index(Korea) = (Freq_CH-917.1M)/0.2M
}WorkingChannel;

typedef struct{
    M100ModuleInfo info;
    uint16_t baudrate;

}M100Module;