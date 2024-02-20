#include "airmon_aqi.h"

#include <math.h>

#define CLAMP(x, min, max) ((x > max) ? max : (x < min) ? min : x)
#define LINTP(x, from_l, from_h, to_l, to_h) \
    (((x - (from_l)) / (from_h - (from_l))) * (to_h - (to_l)) + to_l)

#define NUM_BREAKPOINTS 8

static const float pm2_5_breakpoints[NUM_BREAKPOINTS] =
    {0.f, 12.1f, 35.5f, 55.5f, 150.5f, 250.5f, 350.5f, 500.f};

static const float pm10_breakpoints[NUM_BREAKPOINTS] =
    {0.f, 55.f, 155.f, 255.f, 355.f, 425.f, 505.f, 605.f};

static const float aqi_breakpoints[NUM_BREAKPOINTS] =
    {0.f, 51.f, 101.f, 151.f, 201.f, 301.f, 401.f, 500.f};

const char* const aqi_levels[NUM_BREAKPOINTS] = {
    "Good",
    "Moderate",
    "Unhealthy for Sensitive",
    "Unhealthy",
    "Very Unhealthy",
    "Hazardous",
    "Hazardous",
    "Hazardous",
};

int airmon_aqi_breakpoint_idx(const float bps[], float value) {
    int i = 0;
    for(; i < NUM_BREAKPOINTS - 1; i++) {
        if(bps[i] <= value && value < bps[i + 1]) {
            break;
        }
    }
    return i;
}

int airmon_aqi_pm(float pm, const float bps[], float q) {
    float c = floorf(pm / q) * q;
    int i = airmon_aqi_breakpoint_idx(bps, c);
    float aqi = LINTP(c, bps[i], bps[i + 1] - q, aqi_breakpoints[i], aqi_breakpoints[i + 1] - 1);
    return (int)(CLAMP(aqi, aqi_breakpoints[0], aqi_breakpoints[NUM_BREAKPOINTS - 1]));
}

int airmon_aqi(float pm2_5, float pm10) {
    int aqi2_5 = airmon_aqi_pm(pm2_5, pm2_5_breakpoints, 0.1f);
    int aqi10 = airmon_aqi_pm(pm10, pm10_breakpoints, 1.0f);
    if(aqi10 > aqi2_5) {
        return aqi10;
    }
    return aqi2_5;
}

const char* airmon_aqi_level(int aqi) {
    return aqi_levels[airmon_aqi_breakpoint_idx(aqi_breakpoints, aqi)];
}