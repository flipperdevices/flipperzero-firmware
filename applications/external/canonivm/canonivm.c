#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>

#define TEXTSIZE 20
#define MAXENTRY 1
const GpioPin* const pin_shoot = &gpio_ext_pb2;

typedef enum {
    OffsetSelect = 0,
    IntervalSelect = 1,
} Selected;

typedef enum {
    Config = 0,
    Shooting = 1,
} Prgmode;

typedef struct {
    unsigned int offset;
    unsigned int interval;
    Prgmode mode;
    Selected entry;
} TheContext;

static void app_draw_callback(Canvas* canvas, void* ctx) {
    char text[TEXTSIZE];
    TheContext* context = ctx;

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 1, 15, "Canon intervalometer");
    canvas_draw_line(canvas, 0, 16, 128, 16);
    canvas_draw_str(canvas, 1, 48, "Press OK to start");
    snprintf(text, TEXTSIZE, "Start offset:%d", context->offset);
    canvas_draw_str(canvas, 1, 28, text);
    if(context->entry == OffsetSelect) {
        canvas_draw_disc(canvas, 120, 24, 4);
    }
    snprintf(text, TEXTSIZE, "Interval:%d", context->interval);
    canvas_draw_str(canvas, 1, 38, text);
    if(context->entry == IntervalSelect) {
        canvas_draw_disc(canvas, 120, 34, 4);
    }
}

static void app_input_callback(InputEvent* event, void* ctx) {
    furi_message_queue_put(ctx, event, FuriWaitForever);
}

bool configsetting(TheContext* context, InputEvent event) {
    if(event.key == InputKeyDown) {
        context->entry++;
        if(context->entry > MAXENTRY) context->entry = OffsetSelect;
    }
    if(event.key == InputKeyUp) {
        if(context->entry == 0) {
            context->entry = IntervalSelect;
        } else {
            context->entry--;
        }
    }
    if(event.key == InputKeyLeft) {
        if(context->entry == IntervalSelect) {
            if(context->interval > 0) context->interval--;
        } else {
            if(context->offset > 0) context->offset--;
        }
    }

    if(event.key == InputKeyRight) {
        if(context->entry == IntervalSelect) {
            context->interval++;
        } else {
            context->offset++;
        }
    }

    if(event.key == InputKeyBack) {
        return false;
    }
    if(event.key == InputKeyOk) {
        context->mode = Shooting;
    }
    return true;
}

void shooting(void*) {
    furi_hal_gpio_write(pin_shoot, true);
    furi_delay_ms(500);
    furi_hal_gpio_write(pin_shoot, false);
}

int32_t canonivm_main(void* p) {
    ViewPort* wp = NULL;
    FuriMessageQueue* event_queue;
    FuriTimer* timer;
    TheContext* context;
    Gui* gui;

    // Initialization
    UNUSED(p);
    context = malloc(sizeof(TheContext));
    context->offset = 0;
    context->interval = 10;
    context->mode = Config;
    event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    wp = view_port_alloc();
    gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, wp, GuiLayerFullscreen);
    view_port_draw_callback_set(wp, app_draw_callback, context);
    view_port_input_callback_set(wp, app_input_callback, event_queue);

    timer = furi_timer_alloc(shooting, FuriTimerTypePeriodic, context);

    furi_hal_gpio_init_simple(pin_shoot, GpioModeOutputPushPull);

    // Main loop
    for(bool is_running = true; is_running;) {
        InputEvent event;
        const FuriStatus status = furi_message_queue_get(event_queue, &event, FuriWaitForever);

        if(status != FuriStatusOk || event.type != InputTypePress) {
            continue;
        }

        if(context->mode == Config) {
            is_running = configsetting(context, event);
            if(context->mode == Shooting) {
                furi_timer_start(timer, furi_ms_to_ticks(context->interval * 1000));
            }
        } else {
            if(event.key == InputKeyBack) {
                context->mode = Config;
                furi_timer_stop(timer);
            }
        }
    }

    // Release resources
    furi_hal_gpio_init_simple(pin_shoot, GpioModeAnalog);
    view_port_enabled_set(wp, false);
    gui_remove_view_port(gui, wp);
    furi_timer_free(timer);
    furi_message_queue_free(event_queue);
    view_port_free(wp);
    furi_record_close(RECORD_GUI);
    free(context);
    return 0;
}
