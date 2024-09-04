#include "../../js_modules.h" // IWYU pragma: keep
#include "js_gui.h"
#include <mlib/m-array.h>
#include <gui/view_dispatcher.h>
#include "../js_event_loop/js_event_loop.h"
#include <m-array.h>

/**
 * @brief A view instance bound to a view instance freer
 * 
 * In order to reduce boilerplate in glue code for various views, the
 * responsibility of freeing views is placed on the main GUI module. This may
 * violate the single responsibility principle, but I assert that the code is
 * actually more writable and readable this way, as view modules do not have to
 * define, allocate, iterate over and free its own view array.
 */
typedef struct {
    void (*freer)(void*);
    void* instance;
} JsViewGhost;

ARRAY_DEF(JsViewGhosts, JsViewGhost, M_POD_OPLIST);

struct JsGui {
    ViewDispatcher* dispatcher;
    JsViewGhosts_t ghosts;
};

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
    JsViewGhosts_init(module->ghosts);
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

    // execute deferred frees
    JsViewGhosts_it_t it;
    for(JsViewGhosts_it(it, module->ghosts); !JsViewGhosts_end_p(it); JsViewGhosts_next(it)) {
        const JsViewGhost* ghost = JsViewGhosts_cref(it);
        ghost->freer(ghost->instance);
    }

    JsViewGhosts_clear(module->ghosts);
    free(module);
}

void js_gui_defer_free(JsGui* module, void (*freer)(void*), void* instance) {
    furi_check(module);
    furi_check(freer);
    furi_check(instance);
    JsViewGhosts_push_back(module->ghosts, (JsViewGhost){freer, instance});
}

extern const ElfApiInterface js_gui_hashtable_api_interface;

static const JsModuleDescriptor js_gui_desc = {
    "gui",
    js_gui_create,
    js_gui_destroy,
    &js_gui_hashtable_api_interface,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_gui_desc,
};

const FlipperAppPluginDescriptor* js_gui_ep(void) {
    return &plugin_descriptor;
}
