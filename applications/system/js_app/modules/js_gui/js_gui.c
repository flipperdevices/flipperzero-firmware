#include "../../js_modules.h" // IWYU pragma: keep
#include <furi.h>
#include <mlib/m-array.h>
#include <gui/view_dispatcher.h>
#include "../js_event_loop/js_event_loop.h"
#include <m-array.h>

#define EVENT_QUEUE_SIZE 16

typedef struct {
    uint32_t next_view_id;
    FuriEventLoop* loop;
    Gui* gui;
    ViewDispatcher* dispatcher;
    // event stuff
    JsEventLoopContract custom_contract;
    FuriMessageQueue* custom;
    JsEventLoopContract navigation_contract;
    FuriSemaphore* navigation;
} JsGui;

/**
 * @brief `viewDispatcher.add`
 */
static void js_gui_vd_add(struct mjs* mjs) {
    mjs_val_t view_arg;
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_ANY(&view_arg));

    View* view = mjs_get_ptr(mjs, mjs_get(mjs, view_arg, "_view", ~0));
    if(!view) JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "Expected argument 0 to be a View");

    JsGui* module = JS_GET_CONTEXT(mjs);
    view_dispatcher_add_view(module->dispatcher, module->next_view_id, view);
    mjs_return(mjs, mjs_mk_number(mjs, module->next_view_id));
    module->next_view_id++;
}

/**
 * @brief Transformer for custom events
 */
static mjs_val_t js_gui_vd_custom_transformer(FuriEventLoopObject* object, void* context) {
    furi_check(context);
    furi_check(object);
    struct mjs* mjs = context;
    FuriMessageQueue* queue = object;
    uint32_t event;
    furi_check(furi_message_queue_get(queue, &event, 0) == FuriStatusOk);
    return mjs_mk_number(mjs, (double)event);
}

/**
 * @brief ViewDispatcher custom event callback
 */
static bool js_gui_vd_custom_callback(void* context, uint32_t event) {
    furi_check(context);
    JsGui* module = context;
    furi_check(furi_message_queue_put(module->custom, &event, 0) == FuriStatusOk);
    return true;
}

/**
 * @brief ViewDispatcher navigation event callback
 */
static bool js_gui_vd_nav_callback(void* context) {
    furi_check(context);
    JsGui* module = context;
    furi_semaphore_release(module->navigation);
    return true;
}

/**
 * @brief `viewDispatcher.sendCustom`
 */
static void js_gui_vd_send_custom(struct mjs* mjs) {
    int32_t event;
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_INT32(&event));

    JsGui* module = JS_GET_CONTEXT(mjs);
    view_dispatcher_send_custom_event(module->dispatcher, (uint32_t)event);
}

/**
 * @brief `viewDispatcher.switchTo`
 */
static void js_gui_vd_switch_to(struct mjs* mjs) {
    int32_t view_id;
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_INT32(&view_id));

    JsGui* module = JS_GET_CONTEXT(mjs);
    view_dispatcher_switch_to_view(module->dispatcher, (uint32_t)view_id);
}

static void* js_gui_create(struct mjs* mjs, mjs_val_t* object, JsModules* modules) {
    // get event loop
    JsEventLoop* js_loop = js_module_get(modules, "event_loop");
    if(M_UNLIKELY(!js_loop)) return NULL;
    FuriEventLoop* loop = js_event_loop_get_loop(js_loop);

    // create C object
    JsGui* module = malloc(sizeof(JsGui));
    module->loop = loop;
    module->gui = furi_record_open(RECORD_GUI);
    module->dispatcher = view_dispatcher_alloc_ex(loop);
    module->custom = furi_message_queue_alloc(EVENT_QUEUE_SIZE, sizeof(uint32_t));
    module->navigation = furi_semaphore_alloc(EVENT_QUEUE_SIZE, 0);
    view_dispatcher_attach_to_gui(module->dispatcher, module->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_send_to_front(module->dispatcher);

    // subscribe to events and create contracts
    view_dispatcher_set_event_callback_context(module->dispatcher, module);
    view_dispatcher_set_custom_event_callback(module->dispatcher, js_gui_vd_custom_callback);
    view_dispatcher_set_navigation_event_callback(module->dispatcher, js_gui_vd_nav_callback);
    module->custom_contract = (JsEventLoopContract){
        .object = module->custom,
        .object_type = JsEventLoopObjectTypeQueue,
        .event = FuriEventLoopEventIn,
        .transformer = js_gui_vd_custom_transformer,
        .transformer_context = mjs,
    };
    module->navigation_contract = (JsEventLoopContract){
        .object = module->navigation,
        .object_type = JsEventLoopObjectTypeSemaphore,
        .event = FuriEventLoopEventIn,
    };

    // create viewDispatcher object
    mjs_val_t view_dispatcher = mjs_mk_object(mjs);
    mjs_set(mjs, view_dispatcher, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, module));
    mjs_set(mjs, view_dispatcher, "add", ~0, MJS_MK_FN(js_gui_vd_add));
    mjs_set(mjs, view_dispatcher, "sendCustom", ~0, MJS_MK_FN(js_gui_vd_send_custom));
    mjs_set(mjs, view_dispatcher, "switchTo", ~0, MJS_MK_FN(js_gui_vd_switch_to));
    mjs_set(mjs, view_dispatcher, "custom", ~0, mjs_mk_foreign(mjs, &module->custom_contract));
    mjs_set(
        mjs, view_dispatcher, "navigation", ~0, mjs_mk_foreign(mjs, &module->navigation_contract));

    // create API object
    mjs_val_t api = mjs_mk_object(mjs);
    mjs_set(mjs, api, "viewDispatcher", ~0, view_dispatcher);

    *object = api;
    return module;
}

static void js_gui_destroy(void* inst) {
    if(!inst) return;
    JsGui* module = inst;

    // remove views from dispatcher
    for(uint32_t id = 0; id < module->next_view_id; id++) {
        view_dispatcher_remove_view(module->dispatcher, id);
    }
    view_dispatcher_free(module->dispatcher);

    furi_event_loop_maybe_unsubscribe(module->loop, module->custom);
    furi_event_loop_maybe_unsubscribe(module->loop, module->navigation);
    furi_message_queue_free(module->custom);
    furi_semaphore_free(module->navigation);

    furi_record_close(RECORD_GUI);
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
