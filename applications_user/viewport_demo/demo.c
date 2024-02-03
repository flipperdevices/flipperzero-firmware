/*

This is an example of using a ViewPort.  It is a simple Hello World program that
allows you to move a cursor around the screen with the arrow keys.  Pressing
the back button will exit the program.

Uncomment the different view_port_set_orientation() calls to see how the
orientation of the screen and keypad change.

The code is from the Message Queue wiki page 
(https://github.com/jamisonderek/flipper-zero-tutorials/wiki/Message-Queue) and
also the ViewPort section of the User Interface wiki page
(https://github.com/jamisonderek/flipper-zero-tutorials/wiki/User-Interface#viewport).

*/

#include <furi.h>
#include <gui/gui.h>

typedef enum {
    MyEventTypeKey,
    MyEventTypeDone,
} MyEventType;

typedef struct {
    MyEventType type; // The reason for this event.
    InputEvent input; // This data is specific to keypress data.
} MyEvent;

FuriMessageQueue* queue;
int x = 32;
int y = 48;

static void my_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 30, "Hello world");
    canvas_draw_str(canvas, x, y, "^");
}

static void my_input_callback(InputEvent* input_event, void* context) {
    UNUSED(context);
    if(input_event->type == InputTypeShort) {
        if(input_event->key == InputKeyLeft) x--;
        if(input_event->key == InputKeyRight) x++;
        if(input_event->key == InputKeyUp) y--;
        if(input_event->key == InputKeyDown) y++;
        if(input_event->key == InputKeyBack) {
            MyEvent event;
            event.type = MyEventTypeDone;
            furi_message_queue_put(queue, &event, FuriWaitForever);
        }
    }
}

int32_t viewport_demo_app() {
    void* my_context = NULL;
    queue = furi_message_queue_alloc(8, sizeof(MyEvent));

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, my_draw_callback, my_context);
    view_port_input_callback_set(view_port, my_input_callback, my_context);
    //view_port_set_orientation(view_port, ViewPortOrientationHorizontal);
    //view_port_set_orientation(view_port, ViewPortOrientationHorizontalFlip); // upside down
    view_port_set_orientation(view_port, ViewPortOrientationVertical); // USB/keypad bottom
    //view_port_set_orientation(view_port, ViewPortOrientationVerticalFlip); // USB/keypad top

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    MyEvent event;
    bool keep_processing = true;
    while(keep_processing) {
        if(furi_message_queue_get(queue, &event, FuriWaitForever) == FuriStatusOk) {
            if(event.type == MyEventTypeDone) {
                keep_processing = false;
            }
        } else {
            keep_processing = false;
        }
    }

    furi_message_queue_free(queue);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    return 0;
}