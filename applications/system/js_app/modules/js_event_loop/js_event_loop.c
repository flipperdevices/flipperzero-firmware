#include "js_event_loop.h"
#include "../../js_modules.h" // IWYU pragma: keep
#include <expansion/expansion.h>
#include <mlib/m-array.h>

/**
 * @brief Number of arguments that callbacks receive from this module that they can't modify
 */
#define SYSTEM_ARGS 2

/**
 * @brief Context passed to the generic event callback
 */
typedef struct {
    JsEventLoopObjectType object_type;

    struct mjs* mjs;
    mjs_val_t callback;
    // NOTE: not using an mlib array because resizing is not needed.
    mjs_val_t* arguments;
    size_t arity;

    JsEventLoopTransformer transformer;
    void* transformer_context;
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

typedef struct {
    FuriEventLoop* loop;
    struct mjs* mjs;
} JsEventLoopTickContext;

ARRAY_DEF(SubscriptionArray, JsEventLoopSubscription*, M_PTR_OPLIST); //-V575
ARRAY_DEF(ContractArray, JsEventLoopContract*, M_PTR_OPLIST); //-V575

/**
 * @brief Per-module instance control structure
 */
struct JsEventLoop {
    FuriEventLoop* loop;
    SubscriptionArray_t subscriptions;
    ContractArray_t owned_contracts; //<! Contracts that were produced by this module
    JsEventLoopTickContext* tick_context;
};

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

    // save returned args for next call
    if(mjs_array_length(context->mjs, result) != context->arity - SYSTEM_ARGS) return;
    for(size_t i = 0; i < context->arity - SYSTEM_ARGS; i++) {
        mjs_disown(context->mjs, &context->arguments[i + SYSTEM_ARGS]);
        context->arguments[i + SYSTEM_ARGS] = mjs_array_get(context->mjs, result, i);
        mjs_own(context->mjs, &context->arguments[i + SYSTEM_ARGS]);
    }
}

/**
 * @brief Handles non-timer events
 */
static bool js_event_loop_callback(void* object, void* param) {
    JsEventLoopCallbackContext* context = param;

    if(context->transformer) {
        mjs_disown(context->mjs, &context->arguments[1]);
        context->arguments[1] =
            context->transformer(context->mjs, object, context->transformer_context);
        mjs_own(context->mjs, &context->arguments[1]);
    } else {
        // default behavior: take semaphores and mutexes
        switch(context->object_type) {
        case JsEventLoopObjectTypeSemaphore: {
            FuriSemaphore* semaphore = object;
            furi_check(furi_semaphore_acquire(semaphore, 0) == FuriStatusOk);
        } break;
        default:
            // the corresponding check has been performed when we were given the contract
            furi_crash();
        }
    }

    js_event_loop_callback_generic(param);

    return true;
}

/**
 * @brief Cancels an event subscription
 */
static void js_event_loop_subscription_cancel(struct mjs* mjs) {
    JsEventLoopSubscription* subscription = JS_GET_CONTEXT(mjs);

    if(subscription->object_type == JsEventLoopObjectTypeTimer) {
        furi_event_loop_timer_stop(subscription->object);
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

    mjs_return(mjs, MJS_UNDEFINED);
}

/**
 * @brief Subscribes a JavaScript function to an event
 */
static void js_event_loop_subscribe(struct mjs* mjs) {
    JsEventLoop* module = JS_GET_CONTEXT(mjs);

    // get arguments
    JsEventLoopContract* contract;
    mjs_val_t callback;
    JS_FETCH_ARGS_OR_RETURN(
        mjs, JS_AT_LEAST, JS_ARG_STRUCT(JsEventLoopContract, &contract), JS_ARG_FN(&callback));

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
    context->arity = mjs_nargs(mjs) - SYSTEM_ARGS + 2;
    context->arguments = calloc(context->arity, sizeof(mjs_val_t));
    context->arguments[0] = subscription_obj;
    context->arguments[1] = MJS_UNDEFINED;
    for(size_t i = SYSTEM_ARGS; i < context->arity; i++) {
        mjs_val_t arg = mjs_arg(mjs, i - SYSTEM_ARGS + 2);
        context->arguments[i] = arg;
        mjs_own(mjs, &context->arguments[i]);
    }
    context->mjs = mjs;
    context->callback = callback;
    mjs_own(mjs, &context->callback);
    mjs_own(mjs, &context->arguments[0]);
    mjs_own(mjs, &context->arguments[1]);

    // queue and stream contracts must have a transform callback, others are allowed to delegate
    // the obvious default behavior to this module
    if(contract->object_type == JsEventLoopObjectTypeQueue ||
       contract->object_type == JsEventLoopObjectTypeStream) {
        furi_check(contract->non_timer.transformer);
    }
    context->transformer = contract->non_timer.transformer;
    context->transformer_context = contract->non_timer.transformer_context;

    // subscribe
    switch(contract->object_type) {
    case JsEventLoopObjectTypeTimer: {
        FuriEventLoopTimer* timer = furi_event_loop_timer_alloc(
            module->loop, js_event_loop_callback_generic, contract->timer.type, context);
        furi_event_loop_timer_start(timer, contract->timer.interval_ticks);
        contract->object = timer;
    } break;
    case JsEventLoopObjectTypeSemaphore:
        furi_event_loop_subscribe_semaphore(
            module->loop,
            contract->object,
            contract->non_timer.event,
            js_event_loop_callback,
            context);
        break;
    case JsEventLoopObjectTypeQueue:
        furi_event_loop_subscribe_message_queue(
            module->loop,
            contract->object,
            contract->non_timer.event,
            js_event_loop_callback,
            context);
        break;
    default:
        furi_crash("unimplemented");
    }

    subscription->object = contract->object;
    SubscriptionArray_push_back(module->subscriptions, subscription);
    mjs_return(mjs, subscription_obj);
}

/**
 * @brief Runs the event loop until it is stopped
 */
static void js_event_loop_run(struct mjs* mjs) {
    JsEventLoop* module = JS_GET_CONTEXT(mjs);
    furi_event_loop_run(module->loop);
}

/**
 * @brief Stops a running event loop
 */
static void js_event_loop_stop(struct mjs* mjs) {
    JsEventLoop* module = JS_GET_CONTEXT(mjs);
    furi_event_loop_stop(module->loop);
}

/**
 * @brief Creates a timer event that can be subscribed to just like any other
 * event
 */
static void js_event_loop_timer(struct mjs* mjs) {
    // get arguments
    const char* mode_str;
    int32_t interval;
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_STR(&mode_str), JS_ARG_INT32(&interval));
    JsEventLoop* module = JS_GET_CONTEXT(mjs);

    FuriEventLoopTimerType mode;
    if(strcasecmp(mode_str, "periodic") == 0) {
        mode = FuriEventLoopTimerTypePeriodic;
    } else if(strcasecmp(mode_str, "oneshot") == 0) {
        mode = FuriEventLoopTimerTypeOnce;
    } else {
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "argument 0: unknown mode");
    }

    // make timer contract
    JsEventLoopContract* contract = malloc(sizeof(JsEventLoopContract));
    *contract = (JsEventLoopContract){
        .magic = JsForeignMagic_JsEventLoopContract,
        .object_type = JsEventLoopObjectTypeTimer,
        .object = NULL,
        .timer =
            {
                .interval_ticks = furi_ms_to_ticks((uint32_t)interval),
                .type = mode,
            },
    };
    ContractArray_push_back(module->owned_contracts, contract);
    mjs_return(mjs, mjs_mk_foreign(mjs, contract));
}

/**
 * @brief Queue transformer. Takes `mjs_val_t` pointers out of a queue and
 * returns their dereferenced value
 */
static mjs_val_t
    js_event_loop_queue_transformer(struct mjs* mjs, FuriEventLoopObject* object, void* context) {
    UNUSED(context);
    mjs_val_t* message_ptr;
    furi_check(furi_message_queue_get(object, &message_ptr, 0) == FuriStatusOk);
    mjs_val_t message = *message_ptr;
    mjs_disown(mjs, message_ptr);
    free(message_ptr);
    return message;
}

/**
 * @brief Sends a message to a queue
 */
static void js_event_loop_queue_send(struct mjs* mjs) {
    // get arguments
    mjs_val_t message;
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_ANY(&message));
    JsEventLoopContract* contract = JS_GET_CONTEXT(mjs);

    // send message
    mjs_val_t* message_ptr = malloc(sizeof(mjs_val_t));
    *message_ptr = message;
    mjs_own(mjs, message_ptr);
    furi_message_queue_put(contract->object, &message_ptr, 0);

    mjs_return(mjs, MJS_UNDEFINED);
}

/**
 * @brief Creates a queue
 */
static void js_event_loop_queue(struct mjs* mjs) {
    // get arguments
    int32_t length;
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_INT32(&length));
    JsEventLoop* module = JS_GET_CONTEXT(mjs);

    // make queue contract
    JsEventLoopContract* contract = malloc(sizeof(JsEventLoopContract));
    *contract = (JsEventLoopContract){
        .magic = JsForeignMagic_JsEventLoopContract,
        .object_type = JsEventLoopObjectTypeQueue,
        // we could store `mjs_val_t`s in the queue directly if not for mJS' requirement to have consistent pointers to owned values
        .object = furi_message_queue_alloc((size_t)length, sizeof(mjs_val_t*)),
        .non_timer =
            {
                .event = FuriEventLoopEventIn,
                .transformer = js_event_loop_queue_transformer,
            },
    };
    ContractArray_push_back(module->owned_contracts, contract);

    // return object with control methods
    mjs_val_t queue = mjs_mk_object(mjs);
    mjs_set(mjs, queue, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, contract));
    mjs_set(mjs, queue, "input", ~0, mjs_mk_foreign(mjs, contract));
    mjs_set(mjs, queue, "send", ~0, MJS_MK_FN(js_event_loop_queue_send));
    mjs_return(mjs, queue);
}

static void js_event_loop_tick(void* param) {
    JsEventLoopTickContext* context = param;
    uint32_t flags = furi_thread_flags_wait(ThreadEventStop, FuriFlagWaitAny | FuriFlagNoClear, 0);
    if(flags & FuriFlagError) {
        return;
    }
    if(flags & ThreadEventStop) {
        furi_event_loop_stop(context->loop);
        mjs_exit(context->mjs);
    }
}

static void* js_event_loop_create(struct mjs* mjs, mjs_val_t* object, JsModules* modules) {
    UNUSED(modules);
    mjs_val_t event_loop_obj = mjs_mk_object(mjs);
    JsEventLoop* module = malloc(sizeof(JsEventLoop));
    JsEventLoopTickContext* tick_ctx = malloc(sizeof(JsEventLoopTickContext));
    module->loop = furi_event_loop_alloc();
    tick_ctx->loop = module->loop;
    tick_ctx->mjs = mjs;
    module->tick_context = tick_ctx;
    furi_event_loop_tick_set(module->loop, 10, js_event_loop_tick, tick_ctx);
    SubscriptionArray_init(module->subscriptions);
    ContractArray_init(module->owned_contracts);

    mjs_set(mjs, event_loop_obj, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, module));
    mjs_set(mjs, event_loop_obj, "subscribe", ~0, MJS_MK_FN(js_event_loop_subscribe));
    mjs_set(mjs, event_loop_obj, "run", ~0, MJS_MK_FN(js_event_loop_run));
    mjs_set(mjs, event_loop_obj, "stop", ~0, MJS_MK_FN(js_event_loop_stop));
    mjs_set(mjs, event_loop_obj, "timer", ~0, MJS_MK_FN(js_event_loop_timer));
    mjs_set(mjs, event_loop_obj, "queue", ~0, MJS_MK_FN(js_event_loop_queue));

    *object = event_loop_obj;
    return module;
}

static void js_event_loop_destroy(void* inst) {
    if(inst) {
        JsEventLoop* module = inst;
        furi_event_loop_stop(module->loop);

        // free subscriptions
        SubscriptionArray_it_t sub_iterator;
        for(SubscriptionArray_it(sub_iterator, module->subscriptions);
            !SubscriptionArray_end_p(sub_iterator);
            SubscriptionArray_next(sub_iterator)) {
            JsEventLoopSubscription* const* sub = SubscriptionArray_cref(sub_iterator);
            free((*sub)->context->arguments);
            free((*sub)->context);
            free(*sub);
        }
        SubscriptionArray_clear(module->subscriptions);

        // free owned contracts
        ContractArray_it_t iterator;
        for(ContractArray_it(iterator, module->owned_contracts); !ContractArray_end_p(iterator);
            ContractArray_next(iterator)) {
            // unsubscribe object
            JsEventLoopContract* contract = *ContractArray_cref(iterator);
            if(contract->object_type == JsEventLoopObjectTypeTimer) {
                furi_event_loop_timer_stop(contract->object);
            } else {
                furi_event_loop_unsubscribe(module->loop, contract->object);
            }

            // free object
            switch(contract->object_type) {
            case JsEventLoopObjectTypeTimer:
                furi_event_loop_timer_free(contract->object);
                break;
            case JsEventLoopObjectTypeSemaphore:
                furi_semaphore_free(contract->object);
                break;
            case JsEventLoopObjectTypeQueue:
                furi_message_queue_free(contract->object);
                break;
            default:
                furi_crash("unimplemented");
            }

            free(contract);
        }
        ContractArray_clear(module->owned_contracts);

        furi_event_loop_free(module->loop);
        free(module->tick_context);
        free(module);
    }
}

extern const ElfApiInterface js_event_loop_hashtable_api_interface;

static const JsModuleDescriptor js_event_loop_desc = {
    "event_loop",
    js_event_loop_create,
    js_event_loop_destroy,
    &js_event_loop_hashtable_api_interface,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_event_loop_desc,
};

const FlipperAppPluginDescriptor* js_event_loop_ep(void) {
    return &plugin_descriptor;
}

FuriEventLoop* js_event_loop_get_loop(JsEventLoop* loop) {
    // porta: not the proudest function that i ever wrote
    furi_check(loop);
    return loop->loop;
}
