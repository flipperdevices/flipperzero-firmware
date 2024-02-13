#pragma once

int airmon_aqi(float pm2_5, float pm10);
int airmon_aqi_level(int aqi);
const char* airmon_aqi_category(int aqi);