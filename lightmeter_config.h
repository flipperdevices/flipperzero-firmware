#pragma once

typedef enum {
    // ISO_6,
    // ISO_12,
    // ISO_25,
    // ISO_50,
    ISO_100,
    ISO_200,
    ISO_400,
    ISO_800,
    ISO_1600,
    ISO_3200,
    ISO_6400,
    ISO_12800,
    ISO_25600,
    ISO_51200,
    ISO_102400,

    ISO_NUM,
} LightMeterISONumbers;

typedef enum {
    ND_0,
    ND_2,
    ND_4,
    ND_8,
    ND_16,
    ND_32,
    ND_64,
    ND_128,
    ND_256,
    ND_512,
    ND_1024,
    ND_2048,
    ND_4096,

    ND_NUM,
} LightMeterNDNumbers;

typedef enum {
    AP_1,
    AP_1_4,
    AP_2,
    AP_2_8,
    AP_4,
    AP_5_6,
    AP_8,
    AP_11,
    AP_16,
    AP_22,
    AP_32,
    AP_45,
    AP_64,
    AP_90,
    AP_128,

    AP_NUM,
} LightMeterApertureNumbers;

typedef enum {
    TIME_8000,
    TIME_4000,
    TIME_2000,
    TIME_1000,
    TIME_500,
    TIME_250,
    TIME_125,
    TIME_60,
    TIME_30,
    TIME_15,
    TIME_8,
    TIME_4,
    TIME_2,
    TIME_1S,
    TIME_2S,
    TIME_4S,
    TIME_8S,
    TIME_15S,
    TIME_30S,

    TIME_NUM,
} LightMeterTimeNumbers;
