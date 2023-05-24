#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include "GPIO_reader_item.h"

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    int pin;
    int pullMode;
    FuriMutex* mutex;
} PluginState;

static void render_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    const PluginState* plugin_state = ctx;
    furi_mutex_acquire(plugin_state->mutex, FuriWaitForever);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        canvas_width(canvas) / 2,
        canvas_height(canvas) / 10,
        AlignCenter,
        AlignCenter,
        "GPIO reader");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas,
        canvas_width(canvas) / 2,
        canvas_height(canvas) / 10 * 3,
        AlignCenter,
        AlignCenter,
        gpio_item_get_pin_name(plugin_state->pin));

    canvas_draw_str_aligned(
        canvas,
        canvas_width(canvas) / 2,
        canvas_height(canvas) / 10 * 5,
        AlignCenter,
        AlignCenter,
        gpio_item_get_pull_mode(plugin_state->pullMode));

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        canvas_width(canvas) / 2,
        canvas_height(canvas) / 10 * 8,
        AlignCenter,
        AlignCenter,
        gpio_item_get_pin_level(plugin_state->pin));

    furi_mutex_release(plugin_state->mutex);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void GPIO_reader_state_init(PluginState* const plugin_state) {
    plugin_state->pin = 0;
    plugin_state->pullMode = 0;
    gpio_item_configure_pin(plugin_state->pin, plugin_state->pullMode);
}

int32_t GPIO_reader_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    PluginState* plugin_state = malloc(sizeof(PluginState));
    GPIO_reader_state_init(plugin_state);
    plugin_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!plugin_state->mutex) {
        FURI_LOG_E("GPIO_reader", "cannot create mutex\r\n");
        free(plugin_state);
        return 255;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, plugin_state);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        furi_mutex_acquire(plugin_state->mutex, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress || event.input.type == InputTypeRepeat) {
                    switch(event.input.key) {
                    case InputKeyRight:
                        plugin_state->pin = (plugin_state->pin + 1) % GPIO_ITEM_COUNT;
                        gpio_item_configure_pin(plugin_state->pin, plugin_state->pullMode);
                        break;
                    case InputKeyLeft:
                        plugin_state->pin =
                            (plugin_state->pin - 1 + GPIO_ITEM_COUNT) % GPIO_ITEM_COUNT;
                        gpio_item_configure_pin(plugin_state->pin, plugin_state->pullMode);
                        break;
                    case InputKeyUp:
                        plugin_state->pullMode = (plugin_state->pullMode + 1) % GPIO_PULL_COUNT;
                        gpio_item_configure_pin(plugin_state->pin, plugin_state->pullMode);
                        break;
                    case InputKeyDown:
                        plugin_state->pullMode =
                            (plugin_state->pullMode - 1 + GPIO_PULL_COUNT) % GPIO_PULL_COUNT;
                        gpio_item_configure_pin(plugin_state->pin, plugin_state->pullMode);
                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        view_port_update(view_port);
        furi_mutex_release(plugin_state->mutex);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    furi_mutex_free(plugin_state->mutex);
    furi_message_queue_free(event_queue);
    free(plugin_state);

    return 0;
}