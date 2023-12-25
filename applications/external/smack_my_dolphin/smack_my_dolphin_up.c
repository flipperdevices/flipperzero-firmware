#include <furi.h>
#include <gui/gui.h>
#include <power/power_service/power.h>
#include <storage/storage.h>

#define TAG "Smack My Dolphin Up!"
#define DOLPHIN_STATE_FILE_PATH "/int/.dolphin.state"

typedef enum {
    SmackMyDolphinUpEventTypeExit,
    SmackMyDolphinUpEventTypeReset
} SmackMyDolphinUpEventType;

typedef struct {
    SmackMyDolphinUpEventType type;
} SmackMyDolphinUpEvent;

SmackMyDolphinUpEvent* event_alloc(SmackMyDolphinUpEventType type) {
    SmackMyDolphinUpEvent* event = malloc(sizeof(SmackMyDolphinUpEvent));
    event->type = type;

    return event;
}

typedef struct {
    FuriMessageQueue* messageQueue;
    ViewPort* viewport;
    Gui* gui;
} SmackMyDolphinUpContext;

static void draw_callback(Canvas* canvas, void* arg) {
    UNUSED(arg);

    const size_t middle_x = canvas_width(canvas) / 2U;

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, middle_x, 12, AlignCenter, AlignBottom, "Smack My Dolphin Up!");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas, middle_x, 30, AlignCenter, AlignBottom, "Press OK to delete dolphin");
    canvas_draw_str_aligned(
        canvas, middle_x, 40, AlignCenter, AlignBottom, "state and restart Flipper");

    canvas_draw_str_aligned(canvas, middle_x, 55, AlignCenter, AlignBottom, "Press BACK to exit");
}

static void input_callback(InputEvent* inputEvent, void* arg) {
    if(inputEvent->type != InputTypeRelease) {
        return;
    }

    SmackMyDolphinUpContext* context = arg;
    SmackMyDolphinUpEvent* event = NULL;

    if(inputEvent->key == InputKeyBack) {
        event = event_alloc(SmackMyDolphinUpEventTypeExit);
    } else if(inputEvent->key == InputKeyOk) {
        event = event_alloc(SmackMyDolphinUpEventTypeReset);
    }

    if(event == NULL) {
        return;
    }

    furi_message_queue_put(context->messageQueue, event, FuriWaitForever);
}

void reset() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    if(storage_common_exists(storage, DOLPHIN_STATE_FILE_PATH)) {
        FS_Error result = storage_common_remove(storage, DOLPHIN_STATE_FILE_PATH);

        if(result != FSE_OK) {
            FURI_LOG_E(
                TAG, "Failed to remove dolphin state file - result is %i, not FSE_OK", result);
        }
    }

    furi_record_close(RECORD_STORAGE);

    power_reboot(PowerBootModeNormal);
}

void run(SmackMyDolphinUpContext* context) {
    for(bool isRunning = true; isRunning;) {
        SmackMyDolphinUpEvent event;
        const FuriStatus status =
            furi_message_queue_get(context->messageQueue, &event, FuriWaitForever);

        if(status != FuriStatusOk) {
            continue;
        }

        switch(event.type) {
        case SmackMyDolphinUpEventTypeExit:
            isRunning = false;
            break;

        case SmackMyDolphinUpEventTypeReset:
            reset();
            break;

        default:
            furi_crash("Programmer error");
        }
    }
}

SmackMyDolphinUpContext* context_alloc() {
    SmackMyDolphinUpContext* context = malloc(sizeof(SmackMyDolphinUpContext));

    context->messageQueue = furi_message_queue_alloc(1, sizeof(SmackMyDolphinUpEvent));

    context->viewport = view_port_alloc();
    view_port_draw_callback_set(context->viewport, draw_callback, NULL);
    view_port_input_callback_set(context->viewport, input_callback, context);

    context->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(context->gui, context->viewport, GuiLayerFullscreen);

    return context;
}

void context_free(SmackMyDolphinUpContext* context) {
    gui_remove_view_port(context->gui, context->viewport);
    furi_record_close(RECORD_GUI);

    view_port_free(context->viewport);
    furi_message_queue_free(context->messageQueue);

    free(context);
}

int32_t smack_my_dolphin_up_app(void* p) {
    UNUSED(p);

    SmackMyDolphinUpContext* context = context_alloc();

    run(context);
    context_free(context);

    return 0;
}
