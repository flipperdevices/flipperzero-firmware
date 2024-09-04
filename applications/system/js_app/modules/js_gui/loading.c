#include "../../js_modules.h" // IWYU pragma: keep
#include <gui/modules/loading.h>

static void* js_gui_loading_create(struct mjs* mjs, mjs_val_t* object, JsModules* modules) {
    UNUSED(mjs);
    UNUSED(object);
    UNUSED(modules);
    // TODO:
    return NULL;
}

static void js_gui_loading_destroy(void* inst) {
    if(!inst) return;
}

static const JsModuleDescriptor js_gui_loading_desc = {
    "gui__loading",
    js_gui_loading_create,
    js_gui_loading_destroy,
    NULL,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_gui_loading_desc,
};

const FlipperAppPluginDescriptor* js_gui_loading_ep(void) {
    return &plugin_descriptor;
}
