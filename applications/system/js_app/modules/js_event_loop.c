#include "js_event_loop.h"
#include "../js_modules.h" // IWYU pragma: keep
#include <expansion/expansion.h>
#include <furi/core/event_loop_i.h>

typedef struct {
    FuriEventLoop* loop;
} JsEventLoop;

typedef struct {
    struct mjs* mjs;
    mjs_val_t callback;
    size_t arity;
    mjs_val_t* arguments;
} JsEventLoopCallbackContext;

static void js_event_loop_callback(void* param) {
    JsEventLoopCallbackContext* context = (JsEventLoopCallbackContext*)param;
    mjs_val_t result;
    mjs_apply(
        context->mjs,
        &result,
        context->callback,
        MJS_UNDEFINED,
        context->arity,
        context->arguments);

    // save returned value until next call
    if(mjs_array_length(context->mjs, result) != context->arity - 1) return;
    for(size_t i = 0; i < context->arity - 1; i++) {
        mjs_disown(context->mjs, &context->arguments[i + 1]);
        context->arguments[i + 1] = mjs_array_get(context->mjs, result, i);
        mjs_own(context->mjs, &context->arguments[i + 1]);
    }
}

static void js_event_loop_subscribe(struct mjs* mjs) {
    // get arguments
    if(mjs_nargs(mjs) < 2)
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "requires at least 2 arguments");
    mjs_val_t contract_arg = mjs_arg(mjs, 0);
    mjs_val_t callback = mjs_arg(mjs, 1);
    if(!mjs_is_foreign(contract_arg))
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "invalid contract");
    if(!mjs_is_function(callback))
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "callback must be a function");
    JsEventLoopContract* contract = (JsEventLoopContract*)mjs_get_ptr(mjs, contract_arg);

    // create callback context
    JsEventLoopCallbackContext* context = malloc(sizeof(JsEventLoopCallbackContext));
    context->arity = mjs_nargs(mjs) - 1;
    context->arguments = calloc(context->arity, sizeof(mjs_val_t));
    context->arguments[0] = MJS_UNDEFINED;
    for(size_t i = 1; i < context->arity; i++) {
        mjs_val_t arg = mjs_arg(mjs, i + 1);
        mjs_own(mjs, &arg);
        context->arguments[i] = arg;
    }
    mjs_own(mjs, &callback);
    context->callback = callback;
    context->mjs = mjs;

    // subscribe
    JsEventLoop* module = mjs_get_ptr(mjs, mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0));
    switch(contract->object_type) {
    case JsEventLoopObjectTypeTimer: {
        FuriEventLoopTimer* timer = furi_event_loop_timer_alloc(
            module->loop, js_event_loop_callback, contract->timer_type, context);
        furi_event_loop_timer_start(timer, contract->interval_ticks);
    } break;
    default:
        break;
    }
}

static void js_event_loop_run(struct mjs* mjs) {
    JsEventLoop* module = mjs_get_ptr(mjs, mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0));
    furi_event_loop_run(module->loop);
}

static void js_event_loop_stop(struct mjs* mjs) {
    JsEventLoop* module = mjs_get_ptr(mjs, mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0));
    furi_event_loop_stop(module->loop);
}

static void js_event_loop_timer(struct mjs* mjs) {
    // get arguments
    if(mjs_nargs(mjs) != 2) JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "requires 2 arguments");
    mjs_val_t mode_arg = mjs_arg(mjs, 0);
    mjs_val_t interval_arg = mjs_arg(mjs, 1);
    const char* mode_str = mjs_get_string(mjs, &mode_arg, NULL);
    if(!mode_str) JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "mode must be a string");
    if(!mjs_is_number(interval_arg))
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "interval must be a number");
    FuriEventLoopTimerType mode;
    if(strcasecmp(mode_str, "periodic") == 0) {
        mode = FuriEventLoopTimerTypePeriodic;
    } else if(strcasecmp(mode_str, "oneshot") == 0) {
        mode = FuriEventLoopTimerTypeOnce;
    } else {
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "unknown mode");
    }
    int32_t interval = mjs_get_int32(mjs, interval_arg);

    // make timer contract
    JsEventLoopContract* contract = malloc(sizeof(JsEventLoopContract));
    contract->object_type = JsEventLoopObjectTypeTimer;
    contract->object = NULL;
    contract->interval_ticks = furi_ms_to_ticks((uint32_t)interval);
    contract->timer_type = mode;
    mjs_return(mjs, mjs_mk_foreign(mjs, contract));
}

static void* js_event_loop_create(struct mjs* mjs, mjs_val_t* object) {
    mjs_val_t event_loop_obj = mjs_mk_object(mjs);
    JsEventLoop* module = malloc(sizeof(JsEventLoop));
    module->loop = furi_event_loop_alloc();

    // event loop control
    mjs_set(mjs, event_loop_obj, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, module));
    mjs_set(mjs, event_loop_obj, "subscribe", ~0, MJS_MK_FN(js_event_loop_subscribe));
    mjs_set(mjs, event_loop_obj, "run", ~0, MJS_MK_FN(js_event_loop_run));
    mjs_set(mjs, event_loop_obj, "stop", ~0, MJS_MK_FN(js_event_loop_stop));

    // built-in contracts
    mjs_set(mjs, event_loop_obj, "timer", ~0, MJS_MK_FN(js_event_loop_timer));

    *object = event_loop_obj;
    return (void*)module;
}

static void js_event_loop_destroy(void* inst) {
    if(inst) {
        JsEventLoop* module = (JsEventLoop*)inst;
        free(module);
    }

    expansion_enable(furi_record_open(RECORD_EXPANSION));
    furi_record_close(RECORD_EXPANSION);
}

static const JsModuleDescriptor js_event_loop_desc = {
    "event_loop",
    js_event_loop_create,
    js_event_loop_destroy,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_event_loop_desc,
};

const FlipperAppPluginDescriptor* js_event_loop_ep(void) {
    return &plugin_descriptor;
}
