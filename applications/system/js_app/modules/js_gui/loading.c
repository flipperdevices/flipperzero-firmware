#include "../../js_modules.h" // IWYU pragma: keep
#include "js_gui.h"
#include <gui/modules/loading.h>

static void js_gui_loading_make(struct mjs* mjs) {
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY); // 0 args

    Loading* loading = loading_alloc();
    JsGui* gui = JS_GET_CONTEXT(mjs);
    JS_GUI_DEFER_FREE(gui, loading_free, loading);

    mjs_val_t js_loading = mjs_mk_object(mjs);
    mjs_set(mjs, js_loading, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, loading));
    mjs_set(mjs, js_loading, "_view", ~0, mjs_mk_foreign(mjs, loading_get_view(loading)));
    mjs_return(mjs, js_loading);
}

static void* js_gui_loading_create(struct mjs* mjs, mjs_val_t* object, JsModules* modules) {
    JsGui* js_gui = js_module_get(modules, "gui");
    if(M_UNLIKELY(!js_gui)) return NULL;

    mjs_val_t ctor = mjs_mk_object(mjs);
    mjs_set(mjs, ctor, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, js_gui));
    mjs_set(mjs, ctor, "make", ~0, MJS_MK_FN(js_gui_loading_make));

    *object = ctor;
    return NULL;
}

static const JsModuleDescriptor js_gui_loading_desc = {
    "gui__loading",
    js_gui_loading_create,
    NULL,
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
