#include "../../js_modules.h" // IWYU pragma: keep
#include <gui/modules/loading.h>

static void js_gui_loading_destructor(struct mjs* mjs, mjs_val_t obj) {
    Loading* loading = JS_GET_INST(mjs, obj);
    loading_free(loading);
}

static void js_gui_loading_make(struct mjs* mjs) {
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY); // 0 args

    Loading* loading = loading_alloc();

    mjs_val_t js_loading = mjs_mk_object(mjs);
    mjs_set(mjs, js_loading, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, loading));
    mjs_set(mjs, js_loading, MJS_DESTRUCTOR_PROP_NAME, ~0, MJS_MK_FN(js_gui_loading_destructor));
    mjs_set(mjs, js_loading, "_view", ~0, mjs_mk_foreign(mjs, loading_get_view(loading)));
    mjs_return(mjs, js_loading);
}

static void* js_gui_loading_create(struct mjs* mjs, mjs_val_t* object, JsModules* modules) {
    if(M_UNLIKELY(!js_module_get(modules, "gui"))) return NULL;

    mjs_val_t ctor = mjs_mk_object(mjs);
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
