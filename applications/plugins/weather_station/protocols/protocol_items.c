#include "protocol_items.h"

const SubGhzProtocol* weather_station_protocol_registry_items[] = {
    &ws_protocol_infactory,
};

const SubGhzProtocolRegistry weather_station_protocol_registry = {
    .items = weather_station_protocol_registry_items,
    .size = COUNT_OF(weather_station_protocol_registry_items)};