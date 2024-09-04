#include "../../js_modules.h" // IWYU pragma: keep
#include <mlib/m-array.h>
#include <gui/view_dispatcher.h>
#include "../js_event_loop/js_event_loop.h"

typedef struct {
    ViewDispatcher* dispatcher;
} JsGui;

/**
 * @brief `viewDispatcher.add`
 */
static void js_gui_vd_add(struct mjs* mjs) {
    mjs_return(mjs, MJS_UNDEFINED);
}

static void* js_gui_create(struct mjs* mjs, mjs_val_t* object, JsModules* modules) {
    // get event loop
    JsEventLoop* js_loop = js_module_get(modules, "event_loop");
    if(M_UNLIKELY(!js_loop)) {
        // `event_loop` must be imported before `gui`
        // likely dead code because our module would fail to link
        return NULL;
    }
    FuriEventLoop* loop = js_event_loop_get_loop(js_loop);

    // create C object
    JsGui* module = malloc(sizeof(JsGui));
    module->dispatcher = view_dispatcher_alloc_ex(loop);

    // create viewDispatcher object
    mjs_val_t view_dispatcher = mjs_mk_object(mjs);
    mjs_set(mjs, view_dispatcher, "add", ~0, MJS_MK_FN(js_gui_vd_add));

    // create API object
    mjs_val_t api = mjs_mk_object(mjs);
    mjs_set(mjs, api, "viewDispatcher", ~0, view_dispatcher);

    *object = api;
    return module;
}

static void js_gui_destroy(void* inst) {
    if(!inst) return;
    JsGui* module = inst;
    view_dispatcher_free(module->dispatcher);
    free(module);
}

static const JsModuleDescriptor js_gui_desc = {
    "gui",
    js_gui_create,
    js_gui_destroy,
    NULL,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_gui_desc,
};

const FlipperAppPluginDescriptor* js_gui_ep(void) {
    return &plugin_descriptor;
}
