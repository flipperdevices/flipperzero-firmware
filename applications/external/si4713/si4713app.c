#include <furi.h>
#include <input/input.h>
#include <gui/gui.h>
#include <stdlib.h>

#include "si4713app.h"
#include "views/transmit_view.h"

#define TAG "SI4713Tuner"
#define SI4713_ADDRESS 0x63
#define SI4713_RESET_PIN &gpio_usart_rx

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void canvas_render_callback(Canvas* const canvas, void* context) {
    furi_assert(context);
    SI4713App* ctx = context;
    furi_mutex_acquire(ctx->mutex, FuriWaitForever);
    FURI_LOG_D(TAG, "Render callback!");
    switch(ctx->state.view) {
    case TransmittingView:
        render_transmit_view(canvas, &ctx->state);
        break;
    case ReceivingView:
    default:
        break;
    }

    furi_mutex_release(ctx->mutex);
}

bool try_setup_device(SI4713App* ctx) {
    furi_assert(ctx);
    if(ctx->state.working == DeviceDisconnected) return false;

    if(furi_hal_i2c_is_device_ready(I2C_BUS, ctx->device->address, 50) &&
       ctx->state.working == DeviceDisconnected) {
        ctx->state.tx_power = 115;
        si4713_set_tx_power(ctx->device, ctx->state.tx_power, 0);
        ctx->state.freq = 10000;
        si4713_tune_fm(ctx->device, ctx->state.freq);
        ctx->state.working = DeviceOn;
        ctx->state.view = TransmittingView;
        return true;
    }
    return false;
}

static void app_free(SI4713App* ctx) {
    furi_assert(ctx);
    FURI_LOG_D(TAG, "freeing mutex");
    furi_mutex_free(ctx->mutex);
    FURI_LOG_D(TAG, "freeing queue");
    furi_message_queue_free(ctx->event_queue);
    FURI_LOG_D(TAG, "freeing device");
    si4713_free(ctx->device, true);

    FURI_LOG_D(TAG, "disabling viewport");
    view_port_enabled_set(ctx->viewport, false);
    FURI_LOG_D(TAG, "removing viewport");
    gui_remove_view_port(ctx->gui, ctx->viewport);
    FURI_LOG_D(TAG, "closing gui");
    furi_record_close(RECORD_GUI);
    FURI_LOG_D(TAG, "freeing viewport");
    view_port_free(ctx->viewport);
    FURI_LOG_D(TAG, "freeing app context");
    free(ctx);
}

int32_t si4713fmtuner_app(void* p) {
    UNUSED(p);

    SI4713App* ctx = malloc(sizeof(SI4713App));

    ctx->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    if(!ctx->event_queue) {
        FURI_LOG_E(TAG, "Could not create FuriMessageQueue");
        free(ctx);
        return 255;
    }

    ctx->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!ctx->mutex) {
        FURI_LOG_E(TAG, "Could not create FuriMutex");
        furi_message_queue_free(ctx->event_queue);
        free(ctx);
        return 255;
    }

    ctx->viewport = view_port_alloc();

    ctx->device = si4713_alloc(SI4713_ADDRESS, SI4713_RESET_PIN, true);

    ctx->state.tx_power = 115;
    si4713_set_tx_power(ctx->device, ctx->state.tx_power, 0);
    ctx->state.freq = 10000;
    si4713_tune_fm(ctx->device, ctx->state.freq);
    ctx->state.working = DeviceOn;
    ctx->state.view = TransmittingView;

    view_port_draw_callback_set(ctx->viewport, canvas_render_callback, ctx);
    view_port_input_callback_set(ctx->viewport, input_callback, ctx->event_queue);

    ctx->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(ctx->gui, ctx->viewport, GuiLayerFullscreen);

    FURI_LOG_D(TAG, "successfully allocated");

    InputEvent event;
    bool running = true;
    while(running) {
        //        if (ctx->state.working == DeviceDisconnected) {
        //            furi_mutex_acquire(ctx->mutex, FuriWaitForever);
        //
        //        }
        if(furi_message_queue_get(ctx->event_queue, &event, 50) != FuriStatusOk) continue;

        furi_mutex_acquire(ctx->mutex, FuriWaitForever);

        switch(event.key) {
        case InputKeyOk:
            if(event.type == InputTypeShort) {
                ctx->state.working = ctx->state.working == DeviceOff ? DeviceOn : DeviceOff;
                si4713_set_tx_power(
                    ctx->device, ctx->state.working == DeviceOn ? ctx->state.tx_power : 0, 0);
                if(ctx->state.working != DeviceOff) si4713_tune_fm(ctx->device, ctx->state.freq);
            }
            break;
        case InputKeyBack:
            if(event.type == InputTypeLong) running = false;
            break;
        default:
            switch(ctx->state.view) {
            case TransmittingView:
                transmit_view_handle_input(&event, ctx);
                break;
            default:
                break;
            }
        }
        furi_mutex_release(ctx->mutex);
        view_port_update(ctx->viewport);
    }

    app_free(ctx);
    return 0;
}
