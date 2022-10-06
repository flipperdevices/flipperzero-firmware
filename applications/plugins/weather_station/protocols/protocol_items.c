#include "protocol_items.h"

const SubGhzProtocol* weather_station_protocol_registry_items[] = {
    &subghz_protocol_princeton,
};

const SubGhzProtocolRegistry weather_station_protocol_registry = {
    .items = weather_station_protocol_registry_items,
    .size = COUNT_OF(weather_station_protocol_registry_items)};