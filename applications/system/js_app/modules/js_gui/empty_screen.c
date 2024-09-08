#include "../../js_modules.h" // IWYU pragma: keep
#include <gui/modules/empty_screen.h>

static void js_gui_empty_screen_destructor(struct mjs* mjs, mjs_val_t obj) {
    EmptyScreen* empty_screen = JS_GET_INST(mjs, obj);
    empty_screen_free(empty_screen);
}

static void js_gui_empty_screen_make(struct mjs* mjs) {
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY); // 0 args

    EmptyScreen* empty_screen = empty_screen_alloc();

    mjs_val_t js_empty_screen = mjs_mk_object(mjs);
    mjs_set(mjs, js_empty_screen, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, empty_screen));
    mjs_set(
        mjs,
        js_empty_screen,
        MJS_DESTRUCTOR_PROP_NAME,
        ~0,
        MJS_MK_FN(js_gui_empty_screen_destructor));
    mjs_set(
        mjs,
        js_empty_screen,
        "_view",
        ~0,
        mjs_mk_foreign(mjs, empty_screen_get_view(empty_screen)));
    mjs_return(mjs, js_empty_screen);
}

static void* js_gui_empty_screen_create(struct mjs* mjs, mjs_val_t* object, JsModules* modules) {
    if(M_UNLIKELY(!js_module_get(modules, "gui"))) return NULL;

    mjs_val_t ctor = mjs_mk_object(mjs);
    mjs_set(mjs, ctor, "make", ~0, MJS_MK_FN(js_gui_empty_screen_make));

    *object = ctor;
    return NULL;
}

static const JsModuleDescriptor js_gui_empty_screen_desc = {
    "gui__empty_screen",
    js_gui_empty_screen_create,
    NULL,
    NULL,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_gui_empty_screen_desc,
};

const FlipperAppPluginDescriptor* js_gui_empty_screen_ep(void) {
    return &plugin_descriptor;
}
