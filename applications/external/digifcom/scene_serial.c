/*
Initializes a text box window
Turns on USB serial
Wires up data streams between USB serial and dmcomm
Prints truncated output to text box

On exit, disconnects and shuts down USB serial

TODO: dmcomm outputs a lot of white space, we should
probably, at some point, filter the USB/Dmcomm stuff to 
'interesting' data, so the screen isn't mostly blank
*/
#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "dmcomm_lib/fcom.h"
#include "scene_serial.h"
#include <furi_hal_cortex.h>

typedef struct {
    UsbUartConfig cfg;
    UsbUartState state;
} SceneUsbUartBridge;

static SceneUsbUartBridge* scene_usb_uart;

void usb_uart_tx(void* context, uint8_t* data, size_t len) {
    // USB -> dmcomm
    App* app = context;

    // Copy data into the flipper UI
    char out[65];
    memset(out, 0, 65);
    memcpy(out, data, len);
    furi_check(furi_mutex_acquire(app->text_box_mutex, FuriWaitForever) == FuriStatusOk);
    furi_string_cat_str(app->text_box_store, out);
    size_t l = furi_string_size(app->text_box_store);
    // Trim if necessary
    if(l > 128) furi_string_right(app->text_box_store, l - 128);
    furi_check(furi_mutex_release(app->text_box_mutex) == FuriStatusOk);
    view_dispatcher_send_custom_event(app->view_dispatcher, SerialCustomEventTextUpdate);

    // Send to dmcomm
    dmcomm_senddata(app, data, len);
}

void dmcomm_tx(void* context) {
    // dmcomm -> USB
    furi_assert(context);
    App* app = context;

    // Read data from the stream
    char out[64];
    size_t recieved = 0;
    memset(out, 0, 64);

    recieved = furi_stream_buffer_receive(app->dmcomm_output_stream, &out, 63, 0);

    // Copy data into the flipper UI
    furi_check(furi_mutex_acquire(app->text_box_mutex, FuriWaitForever) == FuriStatusOk);
    furi_string_cat_str(app->text_box_store, out);
    size_t len = furi_string_size(app->text_box_store);
    // Trim if necessary
    if(len > 128) furi_string_right(app->text_box_store, len - 128);
    furi_check(furi_mutex_release(app->text_box_mutex) == FuriStatusOk);
    view_dispatcher_send_custom_event(app->view_dispatcher, SerialCustomEventTextUpdate);

    // Send to USB
    usb_uart_send(app->usb_uart_bridge, (uint8_t*)out, recieved);
}

void fcom_serial_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_serial_scene_on_enter");
    App* app = context;

    // Initialize the UI
    text_box_reset(app->text_box);
    furi_string_reset(app->text_box_store);
    furi_string_cat_printf(app->text_box_store, "Starting Serial");
    furi_string_push_back(app->text_box_store, '\n');
    furi_string_cat_printf(app->text_box_store, "....");
    furi_string_push_back(app->text_box_store, '\n');

    text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));
    text_box_set_focus(app->text_box, TextBoxFocusEnd);

    // Initialize USB UART
    scene_usb_uart = malloc(sizeof(SceneUsbUartBridge));
    scene_usb_uart->cfg.vcp_ch = 0; // 0 disables logging/CLI, 1 runs alongside
    scene_usb_uart->cfg.baudrate_mode = 4;
    scene_usb_uart->cfg.baudrate = 4; // 9600
    scene_usb_uart->cfg.cb = usb_uart_tx;
    scene_usb_uart->cfg.ctx = context;
    app->usb_uart_bridge = usb_uart_enable(&scene_usb_uart->cfg);

    usb_uart_get_config(app->usb_uart_bridge, &scene_usb_uart->cfg);
    usb_uart_get_state(app->usb_uart_bridge, &scene_usb_uart->state);

    // Link dmcomm output to our callback
    set_serial_callback(dmcomm_tx);

    // Keep screen on while we're in serial mode
    notification_message(app->notification, &sequence_display_backlight_enforce_on);

    // Start the UI
    view_dispatcher_switch_to_view(app->view_dispatcher, FcomSerialView);
}

bool fcom_serial_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_serial_scene_on_event");
    App* app = context;

    bool consumed = false;
    if(event.type == SceneManagerEventTypeTick) {
        // Update serial state, we could reference these data counts too if we wanted
        //uint32_t tx_cnt_last = scene_usb_uart->state.tx_cnt;
        //uint32_t rx_cnt_last = scene_usb_uart->state.rx_cnt;
        usb_uart_get_state(app->usb_uart_bridge, &scene_usb_uart->state);
    }
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SerialCustomEventTextUpdate) {
            // Either serial stream updated the UI text box data
            furi_check(furi_mutex_acquire(app->text_box_mutex, FuriWaitForever) == FuriStatusOk);
            text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));
            furi_check(furi_mutex_release(app->text_box_mutex) == FuriStatusOk);
            consumed = true;
        }
    }

    return consumed; //consumed event
}

void fcom_serial_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_serial_scene_on_exit");
    App* app = context;

    // Unlink dmcomm output
    set_serial_callback(NULL);

    // Shutdown the USB CDC device
    usb_uart_disable(app->usb_uart_bridge);
    free(scene_usb_uart);

    // let screen darken
    notification_message(app->notification, &sequence_display_backlight_enforce_auto);

    // Cancel any command the usb serial user was sending
    dmcomm_sendcommand(app, "0\n");
}
