#include <core/common_defines.h>
#include "js_modules.h"

void plugin_test(void) {
    FURI_LOG_I("", "Test");
    mjs_val_t flipper_obj = mjs_mk_object(NULL);
    UNUSED(flipper_obj);
}

static const FlipperAppPluginDescriptor advanced_plugin1_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin_test,
};

const FlipperAppPluginDescriptor* js_module_test_ep() {
    return &advanced_plugin1_descriptor;
}
