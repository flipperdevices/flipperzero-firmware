#pragma once

#include <gui/view.h>

typedef struct WeatherStationShow WeatherStationShow;

//typedef void (*WeatherStationShowCallback)(SubGhzCustomEvent event, void* context);

// void weather_station_show_set_callback(
//     WeatherStationShow* weather_station_show,
//     WeatherStationShowCallback callback,
//     void* context);

WeatherStationShow* weather_station_show_alloc();

void weather_station_show_free(WeatherStationShow* weather_station_show);

View* weather_station_show_get_view(WeatherStationShow* weather_station_show);

void weather_station_show_add_data_to_show(WeatherStationShow* weather_station_show, const char* key_str);
