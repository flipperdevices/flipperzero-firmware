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

typedef struct {
    JsEventLoopObjectType object_type;
    FuriEventLoopObject* object;
    union {
        FuriEventLoopEvent event;
        struct {
            FuriEventLoopTimerType timer_type;
            uint32_t interval_ticks;
        };
    };
} JsEventLoopContract;
