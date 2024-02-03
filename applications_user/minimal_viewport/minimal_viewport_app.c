#include <furi.h>
#include <gui/gui.h>
typedef struct {
    int type;
    InputEvent input;
} AppEvent;
static void input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* queue = ctx;
    AppEvent event = {
        .type = input_event->type,
        .input = *input_event,
    };
    furi_message_queue_put(queue, &event, FuriWaitForever);
}
static void render_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 15, 30, AlignLeft, AlignTop, "HELLO WORLD!");
}
//static ViewPort* view_port = view_port_alloc();
int32_t minimal_viewport_demo_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* queue = furi_message_queue_alloc(8, sizeof(AppEvent));
    ViewPort* view_port = view_port_alloc();
    view_port_input_callback_set(view_port, input_callback, queue);
    view_port_draw_callback_set(view_port, render_callback, NULL);
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    bool processing = true;
    AppEvent event;
    do {
        if(furi_message_queue_get(queue, &event, FuriWaitForever) == FuriStatusOk) {
            if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                processing = false;
            }
        }
    } while(processing);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}