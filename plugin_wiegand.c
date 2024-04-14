
#include "plugin_interface.h"

#include <flipper_application/flipper_application.h>

static int wiegand_format_count(uint8_t bit_length, uint64_t bits) {
    UNUSED(bit_length);
    UNUSED(bits);
    return 1337;
}

static int wiegand_format_description(uint8_t bit_length, uint64_t bits, size_t index) {
    UNUSED(bit_length);
    UNUSED(bits);
    UNUSED(index);

    return 0;
}

/* Actual implementation of app<>plugin interface */
static const PluginWiegand plugin_wiegand = {
    .name = "Plugin Wiegand",
    .count = &wiegand_format_count,
    .description = &wiegand_format_description,
};

/* Plugin descriptor to comply with basic plugin specification */
static const FlipperAppPluginDescriptor plugin_wiegand_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin_wiegand,
};

/* Plugin entry point - must return a pointer to const descriptor */
const FlipperAppPluginDescriptor* plugin_wiegand_ep(void) {
    return &plugin_wiegand_descriptor;
}
