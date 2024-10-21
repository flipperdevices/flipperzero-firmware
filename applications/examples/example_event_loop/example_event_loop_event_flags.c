/**
 * @file example_event_loop_event_flags.c
 * @brief Example application demonstrating the use of the FuriEventFlag primitive in FuriEventLoop instances.
 *
 * This application receives keystrokes from the input service and sets the appropriate flags,
 * which are subsequently processed in the event loop
 */

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_port.h>

#include <furi_hal_random.h>

#define TAG "ExampleEventLoopEventFlags"

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriEventLoop* event_loop;
    FuriEventFlag* event_flag;
} EventLoopEventFlagsApp;

typedef enum {
    EventLoopEventFlagsOk = (1 << 0),
    EventLoopEventFlagsUp = (1 << 1),
    EventLoopEventFlagsDown = (1 << 2),
    EventLoopEventFlagsLeft = (1 << 3),
    EventLoopEventFlagsRight = (1 << 4),
    EventLoopEventFlagsBack = (1 << 5),
    EventLoopEventFlagsExit = (1 << 6),
} EventLoopEventFlags;

#define EVENT_LOOP_EVENT_FLAGS_MASK                                                 \
    (EventLoopEventFlagsOk | EventLoopEventFlagsUp | EventLoopEventFlagsDown |      \
     EventLoopEventFlagsLeft | EventLoopEventFlagsRight | EventLoopEventFlagsBack | \
     EventLoopEventFlagsExit)

// This function is executed in the GUI context each time an input event occurs (e.g. the user pressed a key)
static void event_loop_event_flags_app_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    EventLoopEventFlagsApp* app = context;
    UNUSED(app);

    if(event->type == InputTypePress) {
        if(event->key == InputKeyOk) {
            furi_event_flag_set(app->event_flag, EventLoopEventFlagsOk);
        } else if(event->key == InputKeyUp) {
            furi_event_flag_set(app->event_flag, EventLoopEventFlagsUp);
        } else if(event->key == InputKeyDown) {
            furi_event_flag_set(app->event_flag, EventLoopEventFlagsDown);
        } else if(event->key == InputKeyLeft) {
            furi_event_flag_set(app->event_flag, EventLoopEventFlagsLeft);
        } else if(event->key == InputKeyRight) {
            furi_event_flag_set(app->event_flag, EventLoopEventFlagsRight);
        } else if(event->key == InputKeyBack) {
            furi_event_flag_set(app->event_flag, EventLoopEventFlagsBack);
        }
    } else if(event->type == InputTypeLong) {
        if(event->key == InputKeyBack) {
            furi_event_flag_set(app->event_flag, EventLoopEventFlagsExit);
        }
    }
}

// This function is executed each time a new event flag is inserted in the input event flag.
static void
    event_loop_event_flags_app_event_flags_callback(FuriEventLoopObject* object, void* context) {
    furi_assert(context);
    EventLoopEventFlagsApp* app = context;

    furi_assert(object == app->event_flag);

    EventLoopEventFlags events =
        furi_event_flag_wait(app->event_flag, EVENT_LOOP_EVENT_FLAGS_MASK, FuriFlagWaitAny, 0);
    furi_check((events) != 0);

    if(events & EventLoopEventFlagsOk) {
        FURI_LOG_I(TAG, "Press \"Ok\"");
    }
    if(events & EventLoopEventFlagsUp) {
        FURI_LOG_I(TAG, "Press \"Up\"");
    }
    if(events & EventLoopEventFlagsDown) {
        FURI_LOG_I(TAG, "Press \"Down\"");
    }
    if(events & EventLoopEventFlagsLeft) {
        FURI_LOG_I(TAG, "Press \"Left\"");
    }
    if(events & EventLoopEventFlagsRight) {
        FURI_LOG_I(TAG, "Press \"Right\"");
    }
    if(events & EventLoopEventFlagsBack) {
        FURI_LOG_I(TAG, "Press \"Back\"");
    }
    if(events & EventLoopEventFlagsExit) {
        FURI_LOG_I(TAG, "Exit App");
        furi_event_loop_stop(app->event_loop);
    }
}

static EventLoopEventFlagsApp* event_loop_event_flags_app_alloc(void) {
    EventLoopEventFlagsApp* app = malloc(sizeof(EventLoopEventFlagsApp));

    // Create event loop instances.
    app->event_loop = furi_event_loop_alloc();
    // Create event flag instances.
    app->event_flag = furi_event_flag_alloc();

    // Create GUI instance.
    app->gui = furi_record_open(RECORD_GUI);
    app->view_port = view_port_alloc();
    // Gain exclusive access to the input events
    view_port_input_callback_set(app->view_port, event_loop_event_flags_app_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    // Notify the event loop about incoming messages in the event flag
    furi_event_loop_subscribe_event_flag(
        app->event_loop,
        app->event_flag,
        FuriEventLoopEventIn | FuriEventLoopEventFlagEdge,
        event_loop_event_flags_app_event_flags_callback,
        app);

    return app;
}

static void event_loop_event_flags_app_free(EventLoopEventFlagsApp* app) {
    gui_remove_view_port(app->gui, app->view_port);

    furi_record_close(RECORD_GUI);
    app->gui = NULL;

    // Delete all instances
    view_port_free(app->view_port);
    app->view_port = NULL;

    // IMPORTANT: The user code MUST unsubscribe from all events before deleting the event loop.
    // Failure to do so will result in a crash.
    furi_event_loop_unsubscribe(app->event_loop, app->event_flag);

    furi_event_flag_free(app->event_flag);
    app->event_flag = NULL;

    furi_event_loop_free(app->event_loop);
    app->event_loop = NULL;

    free(app);
}

static void event_loop_event_flags_app_run(EventLoopEventFlagsApp* app) {
    FURI_LOG_I(TAG, "Press keys to see them printed here.");
    FURI_LOG_I(TAG, "Quickly press different keys to generate events.");
    FURI_LOG_I(TAG, "Long press \"Back\" to exit app.");

    // Run the application event loop. This call will block until the application is about to exit.
    furi_event_loop_run(app->event_loop);
}

/*******************************************************************
 *                     vvv START HERE vvv
 *
 * The application's entry point - referenced in application.fam
 *******************************************************************/
int32_t example_event_loop_event_flags_app(void* arg) {
    UNUSED(arg);

    EventLoopEventFlagsApp* app = event_loop_event_flags_app_alloc();
    event_loop_event_flags_app_run(app);
    event_loop_event_flags_app_free(app);

    return 0;
}
