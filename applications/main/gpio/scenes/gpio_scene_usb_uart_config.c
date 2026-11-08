#include "../usb_uart_bridge.h"
#include "../gpio_app_i.h"
#include <furi_hal.h>

typedef enum {
    UsbUartLineIndexVcp,
    UsbUartLineIndexBaudrate,
    UsbUartLineIndexUart,
    UsbUartLineIndexFlow,
} LineIndex;

static const char* transport_names[] = {"USB", "BLE NUS"};

static const char* vcp_ch[] = {"0 (CLI)", "1"};
static const char* uart_ch[] = {"13,14", "15,16"};
static const char* flow_pins[] = {"None", "2,3", "6,7", "16,15"};
static const char* baudrate_mode[] = {"Host"};
static const uint32_t baudrate_list[] = {
    1200,
    2400,
    4800,
    9600,
    19200,
    28800,
    38400,
    57600,
    115200,
    230400,
    460800,
    921600,
};
static const char* software_de_re[] = {"None", "4"};

static void gpio_scene_usb_uart_cfg_error_dismiss(DialogExResult result, void* context) {
    UNUSED(result);
    GpioApp* app = context;
    app->transport_error = false;
    view_dispatcher_switch_to_view(app->view_dispatcher, GpioAppViewUsbUartCfg);
}

static void gpio_scene_usb_uart_cfg_show_error(GpioApp* app) {
    app->transport_error = true;
    dialog_ex_set_context(app->dialog, app);
    dialog_ex_set_header(app->dialog, "Transport unavailable", 64, 5, AlignCenter, AlignTop);
    dialog_ex_set_text(
        app->dialog,
        "Try again, or disconnect\nUSB RPC before using USB.",
        64,
        23,
        AlignCenter,
        AlignTop);
    dialog_ex_set_left_button_text(app->dialog, "Back");
    dialog_ex_set_right_button_text(app->dialog, NULL);
    dialog_ex_set_result_callback(app->dialog, gpio_scene_usb_uart_cfg_error_dismiss);
    view_dispatcher_switch_to_view(app->view_dispatcher, GpioAppViewExitConfirm);
}

static void gpio_scene_usb_uart_cfg_enter(void* context, uint32_t index) {
    /* Settings apply immediately; OK does not start or stop the bridge. */
    UNUSED(context);
    UNUSED(index);
}

bool gpio_scene_usb_uart_cfg_on_event(void* context, SceneManagerEvent event) {
    GpioApp* app = context;
    furi_assert(app);
    if(event.type == SceneManagerEventTypeBack && app->transport_error) {
        gpio_scene_usb_uart_cfg_error_dismiss(DialogExResultLeft, app);
        return true;
    }
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GpioUsbUartEventConfigSet) {
            UsbUartConfig previous;
            usb_uart_get_config(app->usb_uart_bridge, &previous);
            bool success = usb_uart_set_config(app->usb_uart_bridge, app->usb_uart_cfg);
            usb_uart_get_config(app->usb_uart_bridge, app->usb_uart_cfg);
            if(!success || previous.transport != app->usb_uart_cfg->transport) {
                gpio_scene_usb_uart_cfg_on_exit(app);
                gpio_scene_usb_uart_cfg_on_enter(app);
            }
            if(!success) gpio_scene_usb_uart_cfg_show_error(app);
            return true;
        }
    }
    return false;
}

static void line_transport_cb(VariableItem* item) {
    GpioApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->usb_uart_cfg->transport = index;
    if(index == UsbUartTransportBle) {
        app->usb_uart_cfg->flow_pins = 0;
        if(!app->usb_uart_cfg->baudrate) {
            app->usb_uart_cfg->baudrate = 115200;
            for(size_t baudrate_index = 0; baudrate_index < COUNT_OF(baudrate_list);
                baudrate_index++) {
                if(baudrate_list[baudrate_index] == app->usb_uart_cfg->baudrate) {
                    app->usb_uart_cfg->baudrate_mode = baudrate_index + 1;
                    break;
                }
            }
        }
    }
    variable_item_set_current_value_text(item, transport_names[index]);
    view_dispatcher_send_custom_event(app->view_dispatcher, GpioUsbUartEventConfigSet);
}

static void line_ensure_flow_invariant(GpioApp* app) {
    // GPIO pins PC0, PC1 (16,15) are unavailable for RTS/DTR when LPUART is
    // selected. This function enforces that invariant by resetting flow_pins
    // to None if it is configured to 16,15 when LPUART is selected.

    uint8_t available_flow_pins = app->usb_uart_cfg->transport == UsbUartTransportBle ? 1 :
                                  app->usb_uart_cfg->uart_ch == FuriHalSerialIdLpuart ? 3 :
                                                                                        4;
    VariableItem* item = app->var_item_flow;
    variable_item_set_values_count(item, available_flow_pins);

    if(app->usb_uart_cfg->flow_pins >= available_flow_pins) {
        app->usb_uart_cfg->flow_pins = 0;

        variable_item_set_current_value_index(item, app->usb_uart_cfg->flow_pins);
        variable_item_set_current_value_text(item, flow_pins[app->usb_uart_cfg->flow_pins]);
    }
}

static void line_vcp_cb(VariableItem* item) {
    GpioApp* app = variable_item_get_context(item);
    furi_assert(app);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, vcp_ch[index]);

    app->usb_uart_cfg->vcp_ch = index;
    view_dispatcher_send_custom_event(app->view_dispatcher, GpioUsbUartEventConfigSet);
}

static void line_port_cb(VariableItem* item) {
    GpioApp* app = variable_item_get_context(item);
    furi_assert(app);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, uart_ch[index]);

    if(index == 0)
        app->usb_uart_cfg->uart_ch = FuriHalSerialIdUsart;
    else if(index == 1)
        app->usb_uart_cfg->uart_ch = FuriHalSerialIdLpuart;

    line_ensure_flow_invariant(app);
    view_dispatcher_send_custom_event(app->view_dispatcher, GpioUsbUartEventConfigSet);
}

static void line_software_de_re_cb(VariableItem* item) {
    GpioApp* app = variable_item_get_context(item);
    furi_assert(app);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, software_de_re[index]);

    app->usb_uart_cfg->software_de_re = index;
    view_dispatcher_send_custom_event(app->view_dispatcher, GpioUsbUartEventConfigSet);
}

static void line_flow_cb(VariableItem* item) {
    GpioApp* app = variable_item_get_context(item);
    furi_assert(app);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, flow_pins[index]);

    app->usb_uart_cfg->flow_pins = index;
    view_dispatcher_send_custom_event(app->view_dispatcher, GpioUsbUartEventConfigSet);
}

static void line_baudrate_cb(VariableItem* item) {
    GpioApp* app = variable_item_get_context(item);
    furi_assert(app);
    uint8_t index = variable_item_get_current_value_index(item);
    if(app->usb_uart_cfg->transport == UsbUartTransportBle) index++;

    char br_text[8];

    if(index > 0) {
        snprintf(br_text, 7, "%lu", baudrate_list[index - 1]);
        variable_item_set_current_value_text(item, br_text);
        app->usb_uart_cfg->baudrate = baudrate_list[index - 1];
    } else {
        variable_item_set_current_value_text(item, baudrate_mode[index]);
        app->usb_uart_cfg->baudrate = 0;
    }
    app->usb_uart_cfg->baudrate_mode = index;
    view_dispatcher_send_custom_event(app->view_dispatcher, GpioUsbUartEventConfigSet);
}

void gpio_scene_usb_uart_cfg_on_enter(void* context) {
    GpioApp* app = context;
    furi_assert(app);
    VariableItemList* var_item_list = app->var_item_list;

    app->usb_uart_cfg = malloc(sizeof(UsbUartConfig));
    usb_uart_get_config(app->usb_uart_bridge, app->usb_uart_cfg);

    VariableItem* item;
    char br_text[8];

    item = variable_item_list_add(var_item_list, "Transport", 2, line_transport_cb, app);
    variable_item_set_current_value_index(item, app->usb_uart_cfg->transport);
    variable_item_set_current_value_text(item, transport_names[app->usb_uart_cfg->transport]);
    if(app->usb_uart_cfg->transport == UsbUartTransportUsb) {
        item = variable_item_list_add(var_item_list, "USB Channel", 2, line_vcp_cb, app);
        variable_item_set_current_value_index(item, app->usb_uart_cfg->vcp_ch);
        variable_item_set_current_value_text(item, vcp_ch[app->usb_uart_cfg->vcp_ch]);
    }

    item = variable_item_list_add(
        var_item_list,
        "Baudrate",
        COUNT_OF(baudrate_list) + (app->usb_uart_cfg->transport == UsbUartTransportUsb ? 1 : 0),
        line_baudrate_cb,
        app);
    variable_item_set_current_value_index(
        item,
        app->usb_uart_cfg->baudrate_mode -
            (app->usb_uart_cfg->transport == UsbUartTransportBle ? 1 : 0));
    if(app->usb_uart_cfg->baudrate_mode > 0) {
        snprintf(br_text, 7, "%lu", baudrate_list[app->usb_uart_cfg->baudrate_mode - 1]);
        variable_item_set_current_value_text(item, br_text);
    } else {
        variable_item_set_current_value_text(
            item, baudrate_mode[app->usb_uart_cfg->baudrate_mode]);
    }

    item = variable_item_list_add(var_item_list, "UART Pins", 2, line_port_cb, app);
    variable_item_set_current_value_index(item, app->usb_uart_cfg->uart_ch);
    variable_item_set_current_value_text(item, uart_ch[app->usb_uart_cfg->uart_ch]);

    item = variable_item_list_add(
        var_item_list, "RTS/DTR Pins", COUNT_OF(flow_pins), line_flow_cb, app);
    variable_item_set_current_value_index(item, app->usb_uart_cfg->flow_pins);
    variable_item_set_current_value_text(item, flow_pins[app->usb_uart_cfg->flow_pins]);
    app->var_item_flow = item;
    line_ensure_flow_invariant(app);

    item = variable_item_list_add(
        var_item_list, "DE/RE Pin", COUNT_OF(software_de_re), line_software_de_re_cb, app);
    variable_item_set_current_value_index(item, app->usb_uart_cfg->software_de_re);
    variable_item_set_current_value_text(item, software_de_re[app->usb_uart_cfg->software_de_re]);

    variable_item_list_set_enter_callback(var_item_list, gpio_scene_usb_uart_cfg_enter, app);
    uint32_t selected = scene_manager_get_scene_state(app->scene_manager, GpioSceneUsbUartCfg);
    uint32_t last = app->usb_uart_cfg->transport == UsbUartTransportUsb ? 5 : 4;
    variable_item_list_set_selected_item(var_item_list, selected > last ? last : selected);

    view_dispatcher_switch_to_view(app->view_dispatcher, GpioAppViewUsbUartCfg);
}

void gpio_scene_usb_uart_cfg_on_exit(void* context) {
    GpioApp* app = context;
    scene_manager_set_scene_state(
        app->scene_manager,
        GpioSceneUsbUartCfg,
        variable_item_list_get_selected_item_index(app->var_item_list));
    app->transport_error = false;
    dialog_ex_reset(app->dialog);
    variable_item_list_reset(app->var_item_list);
    free(app->usb_uart_cfg);
    app->usb_uart_cfg = NULL;
}
