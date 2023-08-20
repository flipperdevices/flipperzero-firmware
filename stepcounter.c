#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>
#include <gui/gui.h>
#include <input/input.h>
#include <locale/locale.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#define TAG_MEMSIC "memsic_2125_app"
#define TAG_COUNTER "step_counter_app"

typedef struct {
    const GpioPin* pin;
    bool prevState;
    uint32_t stepCount;
    bool counting;
} StepCounterData;

typedef struct {
    FuriMessageQueue* queue;
    StepCounterData* data;
} StepCounterContext;

void step_callback(void* data) {
    StepCounterData* stepData = (StepCounterData*)data;

    if (stepData->counting) {
        bool currentState = furi_hal_gpio_read(stepData->pin);

        if (currentState != stepData->prevState) {
            stepData->prevState = currentState;
            stepData->stepCount++;
            // Aggiorna l'interfaccia utente per mostrare il numero di passi.
            view_port_update(view_port);
        }
    }
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    StepCounterContext* stepContext = furi_message_queue_userdata(queue);

    if (input_event->type == InputTypeShort && input_event->key == InputKeyOk) {
        stepContext->data->counting = !stepContext->data->counting;
        // Aggiorna l'interfaccia utente per riflettere il cambio di stato.
        view_port_update(view_port);
    }
}

static void render_callback(Canvas* canvas, void* ctx) {
    StepCounterContext* stepContext = (StepCounterContext*)ctx;
    StepCounterData* stepData = stepContext->data;

    char stepText[20];
    snprintf(stepText, sizeof(stepText), "Steps: %d", stepData->stepCount);
    canvas_draw_str_aligned(canvas, 1, 1, AlignLeft, AlignTop, stepText);

    char buttonText[10];
    snprintf(buttonText, sizeof(buttonText), stepData->counting ? "STOP" : "START");
    canvas_draw_str_aligned(canvas, canvas->width / 2, canvas->height - 1, AlignCenter, AlignBottom, buttonText);
}

int32_t step_counter_app(void* p) {
    UNUSED(p);

    StepCounterContext* stepContext = malloc(sizeof(StepCounterContext));
    stepContext->data = malloc(sizeof(StepCounterData));
    stepContext->data->pin = &gpio_accelerometer;
    stepContext->data->prevState = furi_hal_gpio_read(stepContext->data->pin);
    stepContext->data->stepCount = 0;
    stepContext->data->counting = false;

    stepContext->queue = furi_message_queue_alloc(8, sizeof(DemoEvent));
    furi_message_queue_set_userdata(stepContext->queue, stepContext);

    furi_hal_gpio_init(stepContext->data->pin, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_add_int_callback(stepContext->data->pin, step_callback, stepContext->data);

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, stepContext);
    view_port_input_callback_set(view_port, input_callback, stepContext->queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    DemoEvent event;
    bool processing = true;
    do {
        if (furi_message_queue_get(stepContext->queue, &event, FuriWaitForever) == FuriStatusOk) {
            switch (event.type) {
            case DemoEventTypeKey:
                if (event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                    processing = false;
                }
                break;
            default:
                break;
            }
            view_port_update(view_port);
        } else {
            processing = false;
        }
    } while (processing);

    furi_hal_gpio_remove_int_callback(stepContext->data->pin);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(stepContext->queue);
    free(stepContext->data);
    free(stepContext);

    return 0;
}
