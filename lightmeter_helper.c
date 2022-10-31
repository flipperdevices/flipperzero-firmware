#include "lightmeter_helper.h"
#include "lightmeter_config.h"

static const float aperture_numbers[] = {
    [AP_1] = 1.0,
    [AP_1_4] = 1.4,
    [AP_2] = 2.0,
    [AP_2_8] = 2.8,
    [AP_4] = 4.0,
    [AP_5_6] = 5.6,
    [AP_8] = 8,
    [AP_11] = 11,
    [AP_16] = 16,
    [AP_22] = 22,
    [AP_32] = 32,
    [AP_45] = 45,
    [AP_64] = 64,
    [AP_90] = 90,
    [AP_128] = 128,
};

static const float time_numbers[] = {
    [TIME_8000] = 1.0 / 8000, [TIME_4000] = 1.0 / 4000, [TIME_2000] = 1.0 / 2000,
    [TIME_1000] = 1.0 / 1000, [TIME_500] = 1.0 / 500,   [TIME_250] = 1.0 / 250,
    [TIME_125] = 1.0 / 125,   [TIME_60] = 1.0 / 60,     [TIME_30] = 1.0 / 30,
    [TIME_15] = 1.0 / 15,     [TIME_8] = 1.0 / 8,       [TIME_4] = 1.0 / 4,
    [TIME_2] = 1.0 / 2,       [TIME_1S] = 1.0,          [TIME_2S] = 2.0,
    [TIME_4S] = 4.0,          [TIME_8S] = 8.0,          [TIME_15S] = 15.0,
    [TIME_30S] = 30.0,
};

float lux2ev(float lux) {
    return log2(lux / 2.5);
}

float getMinDistance(float x, float v1, float v2) {
    if(x - v1 > v2 - x) {
        return v2;
    }

    return v1;
}

// Convert calculated aperture value to photography style aperture value.
float normalizeAperture(float a) {
    for(int i = 0; i < AP_NUM; i++) {
        float a1 = aperture_numbers[i];
        float a2 = aperture_numbers[i + 1];

        if(a1 < a && a2 >= a) {
            return getMinDistance(a, a1, a2);
        }
    }

    return 0;
}

float normalizeTime(float a) {
    for(int i = 0; i < TIME_NUM; i++) {
        float a1 = time_numbers[i];
        float a2 = time_numbers[i + 1];

        if(a1 < a && a2 >= a) {
            return getMinDistance(a, a1, a2);
        }
    }

    return 0;
}
