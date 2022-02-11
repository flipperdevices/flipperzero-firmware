#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>

#include <notification/notification_messages.h>

#include <totp.h>

typedef enum {
    TotpEventTypeTick,
    TotpEventTypeInput,
} TotpEventType;

typedef struct {
    TotpEventType type;
    InputEvent input;
} TotpEvent;

static void totp_app_draw_callback(Canvas* canvas, void* ctx) {
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "TOTP");
}

static void totp_app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    osMessageQueueId_t event_queue = ctx;

    TotpEvent event = {.type = TotpEventTypeInput, .input = *input_event};
    osMessageQueuePut(event_queue, &event, 0, osWaitForever);
}

static void totp_app_update(void* ctx) {
    furi_assert(ctx);
    osMessageQueueId_t event_queue = ctx;
    TotpEvent event = {.type = TotpEventTypeTick};
    // It's OK to loose this event if system overloaded
    osMessageQueuePut(event_queue, &event, 0, 0);
}

int32_t totp_app(void* p) {
    osMessageQueueId_t event_queue = osMessageQueueNew(8, sizeof(TotpEvent), NULL);
    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, totp_app_draw_callback, NULL);
    view_port_input_callback_set(view_port, totp_app_input_callback, event_queue);
    uint8_t hmacKey[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0xde, 0xad, 0xbe, 0xef}; // Secret key
    TOTP(hmacKey, 10, 30000); // Secret key, Secret key length, Timestep (30s)
    setTimezone(3); // Set timezone +3 Moscow
    uint32_t newCode = getCodeFromTimestamp(1644567912);
    FURI_LOG_I("TOTP", "%ld", newCode);

    // Register view port in GUI
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    TotpEvent event;

    while(1) {
        furi_check(osMessageQueueGet(event_queue, &event, NULL, osWaitForever) == osOK);
        if((event.input.type == InputTypeShort) && (event.input.key == InputKeyBack)) {
            break;
        }
    }

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    osMessageQueueDelete(event_queue);

    furi_record_close("gui");
    return 0;
}
