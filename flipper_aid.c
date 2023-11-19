#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>

/**
 * Enumeration establishes difference between ticks and input events in CPR
*/
typedef enum {
    CPREventTypeTick,
    CPREventTypeInput,
} CPREventType;

/**
 * Structure for CPR events concerning ticks and input events
*/
typedef struct {
    CPREventType type;
    InputEvent input;
} CPREvent;

void flipper_aid_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Flipper Aid CPR timer");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 22, "Press OK to start CPR");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 34, "Release return to exit the app");
}

void flipper_aid_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}


int32_t flipper_aid_app(void* p){
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, flipper_aid_draw_callback, NULL);
    view_port_input_callback_set(view_port, flipper_aid_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    //NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);

    InputEvent event;
    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk){
        if (event.type == InputTypeShort && event.key == InputKeyBack){
            break;
        }else if (event.type == InputTypeShort && event.key == InputKeyOk){
            if (furi_hal_speaker_acquire(1000)){
                furi_hal_speaker_start(220.0f, 1.0f);
                furi_delay_ms(100);
                furi_hal_speaker_stop();
                furi_hal_speaker_release();
            }
        }
    }
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);

    return 0;
}