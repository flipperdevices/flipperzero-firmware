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
    uint32_t time_of_last_high_pulse;
    uint32_t time_of_high_to_high;
} StepCounterData;

typedef enum {
    StepCounterEventTypeStep,
    StepCounterEventTypeKey,
} StepCounterEventType;

typedef struct {
    StepCounterEventType type;
    InputEvent input_event;
} StepCounterEvent;

typedef struct {
    FuriMessageQueue* queue;
    StepCounterData* data;
    ViewPort* view_port;
} StepCounterContext;

const GpioPin* const gpio_accelerometer = &gpio_ext_pc0;

void step_callback(void* ctx) {
    StepCounterContext* context = (StepCounterContext*)ctx;
    StepCounterData* stepData = context->data;

    FuriHalCortexTimer timer = furi_hal_cortex_timer_get(0);
    uint32_t now = timer.start;

    if(furi_hal_gpio_read(stepData->pin)) {
        if(stepData->time_of_last_high_pulse != 0) {
            stepData->time_of_high_to_high = now - stepData->time_of_last_high_pulse;
        }
        stepData->time_of_last_high_pulse = now;
    } else {
        uint32_t high_duration = now - stepData->time_of_last_high_pulse;
        bool current_state = high_duration < (stepData->time_of_high_to_high >> 1);

        if(current_state != stepData->prevState) {
            stepData->prevState = current_state;
            stepData->stepCount++;

            StepCounterEvent event = {.type = StepCounterEventTypeStep};
            furi_message_queue_put(context->queue, &event, 0);
        }
    }
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    StepCounterEvent event = {
        .type = StepCounterEventTypeKey,
        .input_event.key = input_event->key,
        .input_event.type = input_event->type,
    };
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

static void render_callback(Canvas* canvas, void* ctx) {
    StepCounterContext* stepContext = (StepCounterContext*)ctx;
    StepCounterData* stepData = stepContext->data;

    char stepText[20];
    snprintf(stepText, sizeof(stepText), "Steps: %ld", stepData->stepCount);
    canvas_draw_str_aligned(canvas, 1, 1, AlignLeft, AlignTop, stepText);

    char buttonText[10];
    snprintf(buttonText, sizeof(buttonText), stepData->counting ? "STOP" : "START");
    canvas_draw_str_aligned(
        canvas,
        canvas_width(canvas) / 2,
        canvas_height(canvas) - 1,
        AlignCenter,
        AlignBottom,
        buttonText);
}

int32_t step_counter_app(void* p) {
    UNUSED(p);

    StepCounterContext* stepContext = malloc(sizeof(StepCounterContext));
    stepContext->data = malloc(sizeof(StepCounterData));
    stepContext->data->pin = gpio_accelerometer;
    stepContext->data->prevState = furi_hal_gpio_read(stepContext->data->pin);
    stepContext->data->stepCount = 0;
    stepContext->data->counting = false;

    stepContext->queue = furi_message_queue_alloc(8, sizeof(StepCounterEvent));

    furi_hal_gpio_init(
        stepContext->data->pin, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_add_int_callback(stepContext->data->pin, step_callback, stepContext);

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, stepContext);
    view_port_input_callback_set(view_port, input_callback, stepContext->queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    StepCounterEvent event;
    bool processing = true;
    do {
        if(furi_message_queue_get(stepContext->queue, &event, FuriWaitForever) == FuriStatusOk) {
            switch(event.type) {
            case StepCounterEventTypeKey:
                if(event.input_event.type == InputTypeShort &&
                   event.input_event.key == InputKeyBack) {
                    processing = false;
                } else if(
                    event.input_event.type == InputTypeShort &&
                    event.input_event.key == InputKeyOk) {
                    stepContext->data->counting = !stepContext->data->counting;
                    view_port_update(view_port);
                }
                break;
            case StepCounterEventTypeStep:
                view_port_update(view_port);
                break;
            default:
                break;
            }
            view_port_update(view_port);
        } else {
            processing = false;
        }
    } while(processing);

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