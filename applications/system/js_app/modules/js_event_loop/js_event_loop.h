#include "../../js_modules.h" // IWYU pragma: keep
#include <furi/core/event_loop.h>
#include <furi/core/event_loop_timer.h>

/**
 * @file js_event_loop.h
 * 
 * In JS interpreter code, `js_event_loop` always creates and maintains the
 * event loop. There are two ways in which other modules can integrate with this
 * loop:
 *   - Via contracts: The user of your module would have to acquire an opaque
 *     JS value from you and pass it to `js_event_loop`. This is useful for
 *     events that they user may be interested in. For more info, look at
 *     `JsEventLoopContract`. Also look at `js_event_loop_get_loop`, which
 *     you will need to unsubscribe the event loop from your object.
 *   - Directly: When your module is created, you can acquire an instance of
 *     `JsEventLoop` which you can use to acquire an instance of
 *     `FuriEventLoop` that you can manipulate directly, without the JS
 *     programmer having to pass contracts around. This is useful for
 *     "behind-the-scenes" events that the user does not need to know about. For
 *     more info, look at `js_event_loop_get_loop`.
 * 
 * In both cases, your module is responsible for both instantiating,
 * unsubscribing and freeing the object that the event loop subscribes to.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct JsEventLoop JsEventLoop;

typedef enum {
    JsEventLoopObjectTypeTimer,
    JsEventLoopObjectTypeQueue,
    JsEventLoopObjectTypeMutex,
    JsEventLoopObjectTypeSemaphore,
    JsEventLoopObjectTypeStream,
} JsEventLoopObjectType;

typedef mjs_val_t (*JsEventLoopTransformer)(FuriEventLoopObject* object, void* context);

/**
 * @brief Adapter for other JS modules that wish to integrate with the event
 * loop JS module
 * 
 * If another module wishes to integrate with `js_event_loop`, it needs to
 * implement a function callable from JS that returns an mJS foreign pointer to
 * an instance of this structure. This value is then read by `event_loop`'s
 * `subscribe` function.
 * 
 * There are two fundamental variants of this structure:
 *   - `object_type` is `JsEventLoopObjectTypeTimer`: the fields `timer_type`
 *     and `interval_ticks` specify timer parameters.
 *   - `object_type` is something else: the field `event` will be passed to
 *     `furi_event_loop_subscribe`. The specified `callback` will be called to
 *     transform an object into a JS value (called an item) that's passed to the
 *     JS callback. This is useful for example to take an item out of a message
 *     queue and pass it to JS code in a convenient format. If `callback` is
 *     NULL, the event loop will take semaphores and mutexes on its own.
 * 
 * The producer of the contract is responsible for freeing both the contract and
 * the object that it points to when the interpreter is torn down.
 */
typedef struct {
    JsEventLoopObjectType object_type;
    FuriEventLoopObject* object;
    union {
        struct {
            FuriEventLoopEvent event; //<! Valid for all object types except timers
            JsEventLoopTransformer transformer; //!< Valid for queues and streams
            void* transformer_context; //<! Valid for queues and streams
        };
        struct {
            FuriEventLoopTimerType timer_type; //<! Only valid for timers
            uint32_t interval_ticks; //<! Only valid for timers
        };
    };
} JsEventLoopContract;

/**
 * @brief Gets the FuriEventLoop owned by a JsEventLoop
 * 
 * This function is useful in case your JS module wishes to integrate with
 * the event loop without passing contracts through JS code. Your module will be
 * dynamically linked to this one if you use this function, but only if JS code
 * imports `event_loop` _before_ your module. An instance of `JsEventLoop` may
 * be obtained via `js_module_get`.
 */
FuriEventLoop* js_event_loop_get_loop(JsEventLoop* loop);

#ifdef __cplusplus
}
#endif
