#include "../../js_modules.h" // IWYU pragma: keep
#include "../js_event_loop/js_event_loop.h"
#include <gui/modules/submenu.h>

#define CHOOSE_QUEUE_SIZE 16

typedef struct {
    Submenu* submenu;
    FuriEventLoop* loop;
    FuriMessageQueue* choose_queue;
    JsEventLoopContract* contract;
} JsGuiSubmenu;

static mjs_val_t js_gui_submenu_transformer(FuriEventLoopObject* object, void* context) {
    struct mjs* mjs = context;
    FuriMessageQueue* queue = object;
    uint32_t index;
    furi_message_queue_get(queue, &index, FuriWaitForever);
    return mjs_mk_number(mjs, (double)index);
}

static void js_gui_submenu_callback(void* context, uint32_t index) {
    FuriMessageQueue* queue = context;
    furi_message_queue_put(queue, &index, FuriWaitForever);
}

static void js_gui_submenu_set_items(struct mjs* mjs) {
    mjs_val_t items;
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_ARR(&items));
    JsGuiSubmenu* object = JS_GET_CONTEXT(mjs);

    submenu_reset(object->submenu);
    for(uint32_t i = 0; i < mjs_array_length(mjs, items); i++) {
        mjs_val_t item = mjs_array_get(mjs, items, i);
        const char* label = mjs_get_string(mjs, &item, NULL);
        if(!label)
            JS_ERROR_AND_RETURN(
                mjs, MJS_BAD_ARGS_ERROR, "argument 0: item %lu: expected string", i);
        submenu_add_item(object->submenu, label, i, js_gui_submenu_callback, object->choose_queue);
    }
}

static void js_gui_submenu_set_header(struct mjs* mjs) {
    const char* header;
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_STR(&header));
    JsGuiSubmenu* object = JS_GET_CONTEXT(mjs);
    submenu_set_header(object->submenu, header);
}

static void js_gui_submenu_destructor(struct mjs* mjs, mjs_val_t obj) {
    JsGuiSubmenu* object = JS_GET_INST(mjs, obj);
    submenu_free(object->submenu);
    furi_event_loop_maybe_unsubscribe(object->loop, object->choose_queue);
    furi_message_queue_free(object->choose_queue);
    free(object->contract);
    free(object);
}

static void js_gui_submenu_make(struct mjs* mjs) {
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY); // 0 args

    JsGuiSubmenu* object = malloc(sizeof(JsGuiSubmenu));
    object->submenu = submenu_alloc();
    object->choose_queue = furi_message_queue_alloc(CHOOSE_QUEUE_SIZE, sizeof(uint32_t));
    object->loop = JS_GET_CONTEXT(mjs);

    JsEventLoopContract* contract = malloc(sizeof(JsEventLoopContract));
    contract->object = object->choose_queue;
    contract->object_type = JsEventLoopObjectTypeQueue;
    contract->event = FuriEventLoopEventIn;
    contract->transformer = js_gui_submenu_transformer;
    contract->transformer_context = mjs;
    object->contract = contract;

    mjs_val_t js_submenu = mjs_mk_object(mjs);
    mjs_set(mjs, js_submenu, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, object));
    mjs_set(mjs, js_submenu, MJS_DESTRUCTOR_PROP_NAME, ~0, MJS_MK_FN(js_gui_submenu_destructor));
    mjs_set(mjs, js_submenu, "setItems", ~0, MJS_MK_FN(js_gui_submenu_set_items));
    mjs_set(mjs, js_submenu, "setHeader", ~0, MJS_MK_FN(js_gui_submenu_set_header));
    mjs_set(mjs, js_submenu, "chosen", ~0, mjs_mk_foreign(mjs, object->contract));
    mjs_set(mjs, js_submenu, "_view", ~0, mjs_mk_foreign(mjs, submenu_get_view(object->submenu)));
    mjs_return(mjs, js_submenu);
}

static void* js_gui_submenu_create(struct mjs* mjs, mjs_val_t* object, JsModules* modules) {
    JsEventLoop* js_loop = js_module_get(modules, "event_loop");
    if(M_UNLIKELY(!js_loop)) return NULL;
    if(M_UNLIKELY(!js_module_get(modules, "gui"))) return NULL;

    mjs_val_t ctor = mjs_mk_object(mjs);
    mjs_set(mjs, ctor, INST_PROP_NAME, ~0, MJS_MK_FN(js_event_loop_get_loop(js_loop)));
    mjs_set(mjs, ctor, "make", ~0, MJS_MK_FN(js_gui_submenu_make));

    *object = ctor;
    return NULL;
}

static const JsModuleDescriptor js_gui_submenu_desc = {
    "gui__submenu",
    js_gui_submenu_create,
    NULL,
    NULL,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_gui_submenu_desc,
};

const FlipperAppPluginDescriptor* js_gui_submenu_ep(void) {
    return &plugin_descriptor;
}
