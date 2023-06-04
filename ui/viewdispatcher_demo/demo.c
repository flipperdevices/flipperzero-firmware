/*

This is an example of using a ViewDispatcher.  It is a simple Hello World program that
allows you to move a cursor around the screen with the arrow keys.  Pressing the OK button
will switch between two Views (the views share the same callback functions), just have 
different screen orientations.  Pressing the the back button will exit the program.

The code is from the Message Queue wiki page 
(https://github.com/jamisonderek/flipper-zero-tutorials/wiki/Message-Queue) and
also the ViewDispatcher section of the User Interface wiki page
(https://github.com/jamisonderek/flipper-zero-tutorials/wiki/User-Interface#viewdispatcher).

*/

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/view.h>

typedef enum {
    MyEventTypeKey,
    MyEventTypeDone,
} MyEventType;

typedef struct {
    MyEventType type; // The reason for this event.
    InputEvent input; // This data is specific to keypress data.
} MyEvent;

typedef enum {
    MyViewId,
    MyOtherViewId,
} ViewId;

FuriMessageQueue* queue;
int x = 32;
int y = 48;
ViewId current_view;

static void my_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 30, "Hello world");
    canvas_draw_str_aligned(canvas, x, y, AlignLeft, AlignTop, "^");
}

static bool my_input_callback(InputEvent* input_event, void* context) {
    furi_assert(context);
    bool handled = false;
    // we set our callback context to be the view_dispatcher.
    ViewDispatcher* view_dispatcher = context;

    if(input_event->type == InputTypeShort) {
        if(input_event->key == InputKeyBack) {
            // Default back handler.
            handled = false;
        } else if(input_event->key == InputKeyLeft) {
            x--;
            handled = true;
        } else if(input_event->key == InputKeyRight) {
            x++;
            handled = true;
        } else if(input_event->key == InputKeyUp) {
            y--;
            handled = true;
        } else if(input_event->key == InputKeyDown) {
            y++;
            handled = true;
        } else if(input_event->key == InputKeyOk) {
            // switch the view!
            view_dispatcher_send_custom_event(view_dispatcher, 42);
            handled = true;
        }
    }

    return handled;
}

bool view_dispatcher_navigation_event_callback(void* context) {
    UNUSED(context);
    // We did not handle the event, so return false.
    return false;
}

bool my_view_dispatcher_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    bool handled = false;
    // we set our callback context to be the view_dispatcher.
    ViewDispatcher* view_dispatcher = context;

    if(event == 42) {
        if(current_view == MyViewId) {
            current_view = MyOtherViewId;
        } else {
            current_view = MyViewId;
        }

        view_dispatcher_switch_to_view(view_dispatcher, current_view);
        handled = true;
    }

    // NOTE: The return value is not currently used by the ViewDispatcher.
    return handled;
}

int32_t viewdispatcher_demo_app() {
    ViewDispatcher* view_dispatcher = view_dispatcher_alloc();

    // For this demo, we just use view_dispatcher as our application context.
    void* my_context = view_dispatcher;

    View* view1 = view_alloc();
    view_set_context(view1, my_context);
    view_set_draw_callback(view1, my_draw_callback);
    view_set_input_callback(view1, my_input_callback);
    view_set_orientation(view1, ViewOrientationHorizontal);

    View* view2 = view_alloc();
    view_set_context(view2, my_context);
    view_set_draw_callback(view2, my_draw_callback);
    view_set_input_callback(view2, my_input_callback);
    view_set_orientation(view2, ViewOrientationVertical);

    // set param 1 of custom event callback (impacts tick and navigation too).
    view_dispatcher_set_event_callback_context(view_dispatcher, my_context);
    view_dispatcher_set_navigation_event_callback(
        view_dispatcher, view_dispatcher_navigation_event_callback);
    view_dispatcher_set_custom_event_callback(
        view_dispatcher, my_view_dispatcher_custom_event_callback);
    view_dispatcher_enable_queue(view_dispatcher);
    view_dispatcher_add_view(view_dispatcher, MyViewId, view1);
    view_dispatcher_add_view(view_dispatcher, MyOtherViewId, view2);

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    current_view = MyViewId;
    view_dispatcher_switch_to_view(view_dispatcher, current_view);
    view_dispatcher_run(view_dispatcher);

    view_dispatcher_remove_view(view_dispatcher, MyViewId);
    view_dispatcher_remove_view(view_dispatcher, MyOtherViewId);
    furi_record_close(RECORD_GUI);
    view_dispatcher_free(view_dispatcher);
    return 0;
}