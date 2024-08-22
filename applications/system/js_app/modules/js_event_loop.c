#include "js_event_loop.h"
#include "../js_modules.h" // IWYU pragma: keep
#include <expansion/expansion.h>
#include <furi/core/event_loop_i.h>
#include <mlib/m-array.h>

/**
 * @brief Context passed to the generic event callback
 */
typedef struct {
    JsEventLoopObjectType object_type;
    struct mjs* mjs;
    mjs_val_t callback;
    // NOTE: not using an mlib array because resizing is not needed.
    size_t arity;
    mjs_val_t* arguments;
} JsEventLoopCallbackContext;

/**
 * @brief Contains data needed to cancel a subscription
 */
typedef struct {
    FuriEventLoop* loop;
    JsEventLoopObjectType object_type;
    FuriEventLoopObject* object;
    JsEventLoopCallbackContext* context;
    JsEventLoopContract* contract;
    void* subscriptions; // SubscriptionArray_t, which we can't reference in this definition
} JsEventLoopSubscription;

ARRAY_DEF(SubscriptionArray, JsEventLoopSubscription*, M_PTR_OPLIST); //-V575

/**
 * @brief Per-module instance control structure
 */
typedef struct {
    FuriEventLoop* loop;
    SubscriptionArray_t subscriptions;
} JsEventLoop;

/**
 * @brief Generic event callback, handles all events by calling the JS callbacks
 */
static void js_event_loop_callback_generic(void* param) {
    JsEventLoopCallbackContext* context = param;
    mjs_val_t result;
    mjs_apply(
        context->mjs,
        &result,
        context->callback,
        MJS_UNDEFINED,
        context->arity,
        context->arguments);

    // save returned value as args for next call
    // argument 0 never changes and contains the subscription object
    if(mjs_array_length(context->mjs, result) != context->arity - 1) return;
    for(size_t i = 0; i < context->arity - 1; i++) {
        mjs_disown(context->mjs, &context->arguments[i + 1]);
        context->arguments[i + 1] = mjs_array_get(context->mjs, result, i);
        mjs_own(context->mjs, &context->arguments[i + 1]);
    }
}

/**
 * @brief Handles non-timer events
 */
static bool js_event_loop_callback(void* object, void* param) {
    JsEventLoopCallbackContext* context = param;
    switch(context->object_type) {
    case JsEventLoopObjectTypeSemaphore: {
        FuriSemaphore* semaphore = object;
        furi_check(furi_semaphore_acquire(semaphore, 0) == FuriStatusOk);
    } break;
    default:
        furi_crash("unimplemented");
    }
    js_event_loop_callback_generic(param);
    return true;
}

/**
 * @brief Cancels an event subscription
 */
static void js_event_loop_do_unsubscribe(JsEventLoopSubscription* subscription) {
    if(subscription->object_type == JsEventLoopObjectTypeTimer) {
        // timer objects and contracts are freed by us, read the docs for `JsEventLoopContract`
        furi_event_loop_timer_stop(subscription->object);
        furi_event_loop_timer_free(subscription->object);
        free(subscription->contract);
    } else {
        furi_event_loop_unsubscribe(subscription->loop, subscription->object);
    }
    free(subscription->context->arguments);
    free(subscription->context);

    // find and remove ourselves from the array
    SubscriptionArray_it_t iterator;
    for(SubscriptionArray_it(iterator, subscription->subscriptions);
        !SubscriptionArray_end_p(iterator);
        SubscriptionArray_next(iterator)) {
        JsEventLoopSubscription* item = *SubscriptionArray_cref(iterator);
        if(item == subscription) break;
    }
    SubscriptionArray_remove(subscription->subscriptions, iterator);
    free(subscription);
}

/**
 * @brief Cancels an event subscription, callable from JS
 */
static void js_event_loop_subscription_cancel(struct mjs* mjs) {
    JsEventLoopSubscription* subscription =
        mjs_get_ptr(mjs, mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0));
    js_event_loop_do_unsubscribe(subscription);
    mjs_return(mjs, MJS_UNDEFINED);
}

/**
 * @brief Subscribes a JavaScript function to an event
 */
static void js_event_loop_subscribe(struct mjs* mjs) {
    JsEventLoop* module = mjs_get_ptr(mjs, mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0));

    // get arguments
    if(mjs_nargs(mjs) < 2)
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "requires at least 2 arguments");
    mjs_val_t contract_arg = mjs_arg(mjs, 0);
    mjs_val_t callback = mjs_arg(mjs, 1);
    if(!mjs_is_foreign(contract_arg))
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "invalid contract");
    if(!mjs_is_function(callback))
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "callback must be a function");
    JsEventLoopContract* contract = mjs_get_ptr(mjs, contract_arg);

    // create subscription object
    JsEventLoopSubscription* subscription = malloc(sizeof(JsEventLoopSubscription));
    JsEventLoopCallbackContext* context = malloc(sizeof(JsEventLoopCallbackContext));
    subscription->loop = module->loop;
    subscription->object_type = contract->object_type;
    subscription->context = context;
    subscription->subscriptions = module->subscriptions;
    if(contract->object_type == JsEventLoopObjectTypeTimer) subscription->contract = contract;
    mjs_val_t subscription_obj = mjs_mk_object(mjs);
    mjs_set(mjs, subscription_obj, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, subscription));
    mjs_set(mjs, subscription_obj, "cancel", ~0, MJS_MK_FN(js_event_loop_subscription_cancel));

    // create callback context
    context->object_type = contract->object_type;
    context->arity = mjs_nargs(mjs) - 1;
    context->arguments = calloc(context->arity, sizeof(mjs_val_t));
    context->arguments[0] = subscription_obj;
    for(size_t i = 1; i < context->arity; i++) {
        mjs_val_t arg = mjs_arg(mjs, i + 1);
        context->arguments[i] = arg;
        mjs_own(mjs, &context->arguments[i]);
    }
    context->mjs = mjs;
    context->callback = callback;
    mjs_own(mjs, &context->callback);
    mjs_own(mjs, &context->arguments[0]);

    // subscribe
    switch(contract->object_type) {
    case JsEventLoopObjectTypeTimer: {
        FuriEventLoopTimer* timer = furi_event_loop_timer_alloc(
            module->loop, js_event_loop_callback_generic, contract->timer_type, context);
        furi_event_loop_timer_start(timer, contract->interval_ticks);
        subscription->object = contract->object = timer;
    } break;
    case JsEventLoopObjectTypeSemaphore: {
        FuriSemaphore* semaphore = contract->object;
        furi_event_loop_subscribe_semaphore(
            module->loop, semaphore, contract->event, js_event_loop_callback, context);
    } break;
    default:
        furi_crash("unimplemented");
    }

    SubscriptionArray_push_back(module->subscriptions, subscription);
    mjs_return(mjs, subscription_obj);
}

/**
 * @brief Runs the event loop until it is stopped
 */
static void js_event_loop_run(struct mjs* mjs) {
    JsEventLoop* module = mjs_get_ptr(mjs, mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0));
    furi_event_loop_run(module->loop);
}

/**
 * @brief Stops a running event loop
 */
static void js_event_loop_stop(struct mjs* mjs) {
    JsEventLoop* module = mjs_get_ptr(mjs, mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0));
    furi_event_loop_stop(module->loop);
}

/**
 * @brief Creates a timer event that can be subscribed to just like and other
 * event
 */
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

// TODO: integrate with other modules

static void* js_event_loop_create(struct mjs* mjs, mjs_val_t* object) {
    mjs_val_t event_loop_obj = mjs_mk_object(mjs);
    JsEventLoop* module = malloc(sizeof(JsEventLoop));
    module->loop = furi_event_loop_alloc();
    SubscriptionArray_init(module->subscriptions);

    mjs_set(mjs, event_loop_obj, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, module));
    mjs_set(mjs, event_loop_obj, "subscribe", ~0, MJS_MK_FN(js_event_loop_subscribe));
    mjs_set(mjs, event_loop_obj, "run", ~0, MJS_MK_FN(js_event_loop_run));
    mjs_set(mjs, event_loop_obj, "stop", ~0, MJS_MK_FN(js_event_loop_stop));
    mjs_set(mjs, event_loop_obj, "timer", ~0, MJS_MK_FN(js_event_loop_timer));

    *object = event_loop_obj;
    return module;
}

static void js_event_loop_destroy(void* inst) {
    if(inst) {
        JsEventLoop* module = inst;
        furi_event_loop_stop(module->loop);

        // free subscriptions
        while(!SubscriptionArray_empty_p(module->subscriptions)) {
            JsEventLoopSubscription** sub = SubscriptionArray_get(module->subscriptions, 0);
            js_event_loop_do_unsubscribe(*sub);
        }

        furi_event_loop_free(module->loop);
        SubscriptionArray_clear(module->subscriptions);
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
