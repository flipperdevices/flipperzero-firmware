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
    if(!js_loop) {
        // `event_loop` module must be imported before `gui`
        *object = MJS_UNDEFINED;
        return NULL;
    }
    FuriEventLoop* loop = js_event_loop_get_loop(js_loop);

    // create C object
    JsGui* gui = malloc(sizeof(JsGui));
    gui->dispatcher = view_dispatcher_alloc_ex(loop);

    // create viewDispatcher object
    mjs_val_t view_dispatcher = mjs_mk_object(mjs);
    mjs_set(mjs, view_dispatcher, "add", ~0, MJS_MK_FN(js_gui_vd_add));

    mjs_val_t module = mjs_mk_object(mjs);
    mjs_set(mjs, module, "viewDispatcher", ~0, view_dispatcher);

    *object = module;
    return gui;
}

static void js_gui_destroy(void* inst) {
    if(!inst) return;
    // TODO:
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
