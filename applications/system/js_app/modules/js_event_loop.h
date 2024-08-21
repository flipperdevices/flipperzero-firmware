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
 * Because JS code can make your module produce a contract that is then never
 * given to `js_event_loop`, your module is responsible for freeing the memory
 * for both the contract itself and the object that it points to.
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
