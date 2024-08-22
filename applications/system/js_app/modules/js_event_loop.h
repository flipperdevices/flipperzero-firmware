#include "../js_modules.h" // IWYU pragma: keep
#include <furi/core/event_loop.h>
#include <furi/core/event_loop_timer.h>

typedef enum {
    JsEventLoopObjectTypeTimer,
    JsEventLoopObjectTypeQueue,
    JsEventLoopObjectTypeMutex,
    JsEventLoopObjectTypeSemaphore,
    JsEventLoopObjectTypeStream,
} JsEventLoopObjectType;

/**
 * @brief Adapter for other JS modules that wish to integrate with the event
 * loop
 * 
 * If another module wishes to integrate with `js_event_loop`, it needs to
 * implement a function callable from JS that returns an mJS foreign pointer to
 * an instance of this structure. This value is then read by `event_loop`'s
 * `subscribe` function.
 * 
 * There are two fundamental variants of this structure:
 *   - `object_type` is `JsEventLoopObjectTypeTimer`: the event loop assumes
 *     ownership of the contract. It is responsible for both instantiating and
 *     freeing the timer, as well as freeing the contract. The fields
 *     `timer_type` and `interval_ticks` specify timer parameters.
 *   - `object_type` is something else: the provider is responsible for both
 *     instantiating and freeing the object, as well as freeing the contract.
 *     The field `event` will be passed to `furi_event_loop_subscribe`.
 * 
 * The interface has been designed this way because it is possible for JS code
 * to make your module produce a contract that is never seen by the event loop,
 * resulting in a memory leak if the event loop were to always assume ownership.
 * Timers are special because it does not make sense for a foreign module to
 * produce a timer contract, given that the event loop JS module has a function
 * that produces them. Timers are still handled via contracts in order to reduce
 * code complexity in both the implementation of `js_event_loop` and in JS user
 * code.
 */
typedef struct {
    JsEventLoopObjectType object_type;
    FuriEventLoopObject* object;
    union {
        FuriEventLoopEvent event; //<! Event bitfield. Valid for all object types except timers
        struct {
            FuriEventLoopTimerType
                timer_type; //<! Timer type (periodic or oneshot). Only valid for timers
            uint32_t interval_ticks; //<! Timer interval in ticks. Only valid for timers
        };
    };
} JsEventLoopContract;
