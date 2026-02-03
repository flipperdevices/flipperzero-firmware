#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <rpc/rpc_network.h>

#define TAG "ExampleNetwork"

typedef enum {
    ExampleNetworkViewMenu,
    ExampleNetworkViewStatus,
} ExampleNetworkView;

typedef enum {
    ExampleNetworkMenuCheckStatus,
    ExampleNetworkMenuUdpNtp,
    ExampleNetworkMenuTcpEcho,
    ExampleNetworkMenuTcpHttp,
} ExampleNetworkMenuItem;

typedef enum {
    ExampleNetworkStateIdle,
    ExampleNetworkStateChecking,
    ExampleNetworkStateConnecting,
    ExampleNetworkStateConnected,
    ExampleNetworkStateSending,
    ExampleNetworkStateWaiting,
    ExampleNetworkStateSuccess,
    ExampleNetworkStateError,
} ExampleNetworkState;

typedef struct {
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    ExampleNetworkState state;
    int32_t connection_id;
    ExampleNetworkView current_view;
    char status_text[256];
    char response_text[128];
} ExampleNetworkApp;

static void example_network_update_status(ExampleNetworkApp* app) {
    widget_reset(app->widget);

    const char* state_str = "Unknown";
    switch(app->state) {
    case ExampleNetworkStateIdle:
        state_str = "Idle";
        break;
    case ExampleNetworkStateChecking:
        state_str = "Checking...";
        break;
    case ExampleNetworkStateConnecting:
        state_str = "Connecting...";
        break;
    case ExampleNetworkStateConnected:
        state_str = "Connected!";
        break;
    case ExampleNetworkStateSending:
        state_str = "Sending...";
        break;
    case ExampleNetworkStateWaiting:
        state_str = "Waiting response...";
        break;
    case ExampleNetworkStateSuccess:
        state_str = "Success!";
        break;
    case ExampleNetworkStateError:
        state_str = "Error";
        break;
    }

    widget_add_string_element(
        app->widget, 64, 5, AlignCenter, AlignTop, FontPrimary, "Network Example");
    widget_add_string_element(
        app->widget, 64, 20, AlignCenter, AlignTop, FontSecondary, state_str);

    if(app->status_text[0] != '\0') {
        widget_add_string_multiline_element(
            app->widget, 64, 35, AlignCenter, AlignTop, FontSecondary, app->status_text);
    }

    if(app->response_text[0] != '\0') {
        widget_add_string_element(
            app->widget, 64, 50, AlignCenter, AlignTop, FontSecondary, app->response_text);
    }
}

static bool example_network_navigation_callback(void* context) {
    ExampleNetworkApp* app = context;

    if(app->current_view == ExampleNetworkViewStatus) {
        app->current_view = ExampleNetworkViewMenu;
        view_dispatcher_switch_to_view(app->view_dispatcher, ExampleNetworkViewMenu);
        return true;
    }

    view_dispatcher_stop(app->view_dispatcher);
    return true;
}

static void example_network_check_status(ExampleNetworkApp* app) {
    app->state = ExampleNetworkStateChecking;
    app->status_text[0] = '\0';
    app->response_text[0] = '\0';
    example_network_update_status(app);
    app->current_view = ExampleNetworkViewStatus;
    view_dispatcher_switch_to_view(app->view_dispatcher, ExampleNetworkViewStatus);

    bool available = rpc_network_is_available();
    if(available) {
        app->state = ExampleNetworkStateSuccess;
        snprintf(
            app->status_text,
            sizeof(app->status_text),
            "Network available!\nConnect phone with\nFlipper app.");
    } else {
        app->state = ExampleNetworkStateError;
        snprintf(
            app->status_text,
            sizeof(app->status_text),
            "No RPC session.\nConnect phone with\nFlipper app first.");
    }
    example_network_update_status(app);
}

static void example_network_udp_ntp(ExampleNetworkApp* app) {
    app->state = ExampleNetworkStateConnecting;
    app->status_text[0] = '\0';
    app->response_text[0] = '\0';
    snprintf(app->status_text, sizeof(app->status_text), "pool.ntp.org:123 (UDP)");
    example_network_update_status(app);
    app->current_view = ExampleNetworkViewStatus;
    view_dispatcher_switch_to_view(app->view_dispatcher, ExampleNetworkViewStatus);

    if(!rpc_network_is_available()) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "No RPC session\nConnect phone first");
        example_network_update_status(app);
        return;
    }

    // Connect via UDP to NTP server
    char resolved_ip[64] = {0};
    int32_t conn_id = rpc_network_connect("pool.ntp.org", 123, true, 10000, resolved_ip, sizeof(resolved_ip));

    if(conn_id < 0) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "UDP connect failed!");
        example_network_update_status(app);
        return;
    }

    app->connection_id = conn_id;
    app->state = ExampleNetworkStateSending;
    snprintf(
        app->status_text,
        sizeof(app->status_text),
        "UDP ready! ID=%ld\nIP: %s",
        (long)conn_id,
        resolved_ip[0] ? resolved_ip : "unknown");
    example_network_update_status(app);

    // Build NTP request packet (48 bytes)
    // First byte: LI=0, VN=3, Mode=3 (client) = 0x1b
    uint8_t ntp_request[48];
    memset(ntp_request, 0, sizeof(ntp_request));
    ntp_request[0] = 0x1b;

    int32_t sent = rpc_network_send(conn_id, ntp_request, sizeof(ntp_request));

    if(sent < 0) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Send failed!");
        example_network_update_status(app);
        rpc_network_close(conn_id);
        return;
    }

    app->state = ExampleNetworkStateWaiting;
    snprintf(app->status_text, sizeof(app->status_text), "Sent NTP request\nWaiting...");
    example_network_update_status(app);

    // Receive NTP response
    uint8_t ntp_response[48];
    int32_t received = rpc_network_receive(conn_id, ntp_response, sizeof(ntp_response), 10000);

    if(received >= 48) {
        // Extract transmit timestamp (bytes 40-43, big-endian)
        // This is the 10th 32-bit word (0-indexed)
        uint32_t ntp_time = ((uint32_t)ntp_response[40] << 24) |
                           ((uint32_t)ntp_response[41] << 16) |
                           ((uint32_t)ntp_response[42] << 8) |
                           ((uint32_t)ntp_response[43]);

        // Convert NTP timestamp (since 1900) to Unix timestamp (since 1970)
        // NTP epoch offset: 2208988800 seconds
        uint32_t unix_time = ntp_time - 2208988800UL;

        app->state = ExampleNetworkStateSuccess;
        snprintf(app->status_text, sizeof(app->status_text), "NTP time received!");

        // Display Unix timestamp (user can convert to readable time)
        snprintf(app->response_text, sizeof(app->response_text), "Unix: %lu", (unsigned long)unix_time);

        FURI_LOG_I(TAG, "NTP time: %lu (unix: %lu)", (unsigned long)ntp_time, (unsigned long)unix_time);
    } else if(received == 0) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Timeout - no response");
    } else if(received > 0) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Short response: %ld bytes", (long)received);
    } else {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Receive error");
    }

    example_network_update_status(app);
    rpc_network_close(conn_id);
}

static void example_network_tcp_echo(ExampleNetworkApp* app) {
    app->state = ExampleNetworkStateConnecting;
    app->status_text[0] = '\0';
    app->response_text[0] = '\0';
    snprintf(app->status_text, sizeof(app->status_text), "tcpbin.com:4242 (TCP)");
    example_network_update_status(app);
    app->current_view = ExampleNetworkViewStatus;
    view_dispatcher_switch_to_view(app->view_dispatcher, ExampleNetworkViewStatus);

    if(!rpc_network_is_available()) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "No RPC session\nConnect phone first");
        example_network_update_status(app);
        return;
    }

    char resolved_ip[64] = {0};
    int32_t conn_id = rpc_network_connect("tcpbin.com", 4242, false, 15000, resolved_ip, sizeof(resolved_ip));

    if(conn_id < 0) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Connect failed!");
        example_network_update_status(app);
        return;
    }

    app->connection_id = conn_id;
    app->state = ExampleNetworkStateSending;
    snprintf(
        app->status_text,
        sizeof(app->status_text),
        "Connected! ID=%ld\nIP: %s",
        (long)conn_id,
        resolved_ip[0] ? resolved_ip : "unknown");
    example_network_update_status(app);

    const char* test_msg = "Hello from Flipper!\n";
    int32_t sent = rpc_network_send(conn_id, (const uint8_t*)test_msg, strlen(test_msg));

    if(sent < 0) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Send failed!");
        example_network_update_status(app);
        rpc_network_close(conn_id);
        return;
    }

    app->state = ExampleNetworkStateWaiting;
    snprintf(
        app->status_text,
        sizeof(app->status_text),
        "Sent %ld bytes\nWaiting for echo...",
        (long)sent);
    example_network_update_status(app);

    uint8_t recv_buf[128];
    int32_t received = rpc_network_receive(conn_id, recv_buf, sizeof(recv_buf) - 1, 10000);

    if(received > 0) {
        recv_buf[received] = '\0';
        app->state = ExampleNetworkStateSuccess;
        snprintf(app->status_text, sizeof(app->status_text), "Received %ld bytes:", (long)received);
        strncpy(app->response_text, (char*)recv_buf, 30);
        app->response_text[30] = '\0';
        FURI_LOG_I(TAG, "Echo response: %s", recv_buf);
    } else if(received == 0) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Timeout - no response");
    } else {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Receive error");
    }

    example_network_update_status(app);
    rpc_network_close(conn_id);
}

static void example_network_tcp_http(ExampleNetworkApp* app) {
    app->state = ExampleNetworkStateConnecting;
    app->status_text[0] = '\0';
    app->response_text[0] = '\0';
    snprintf(app->status_text, sizeof(app->status_text), "example.com:80 (TCP)");
    example_network_update_status(app);
    app->current_view = ExampleNetworkViewStatus;
    view_dispatcher_switch_to_view(app->view_dispatcher, ExampleNetworkViewStatus);

    if(!rpc_network_is_available()) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "No RPC session\nConnect phone first");
        example_network_update_status(app);
        return;
    }

    char resolved_ip[64] = {0};
    int32_t conn_id = rpc_network_connect("example.com", 80, false, 15000, resolved_ip, sizeof(resolved_ip));

    if(conn_id < 0) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Connect failed!");
        example_network_update_status(app);
        return;
    }

    app->connection_id = conn_id;
    app->state = ExampleNetworkStateSending;
    snprintf(
        app->status_text,
        sizeof(app->status_text),
        "Connected! IP: %s",
        resolved_ip[0] ? resolved_ip : "unknown");
    example_network_update_status(app);

    const char* http_req = "GET / HTTP/1.0\r\nHost: example.com\r\n\r\n";
    int32_t sent = rpc_network_send(conn_id, (const uint8_t*)http_req, strlen(http_req));

    if(sent < 0) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Send failed!");
        example_network_update_status(app);
        rpc_network_close(conn_id);
        return;
    }

    app->state = ExampleNetworkStateWaiting;
    snprintf(app->status_text, sizeof(app->status_text), "Sent HTTP request\nWaiting...");
    example_network_update_status(app);

    uint8_t recv_buf[128];
    int32_t received = rpc_network_receive(conn_id, recv_buf, sizeof(recv_buf) - 1, 15000);

    if(received > 0) {
        recv_buf[received] = '\0';
        app->state = ExampleNetworkStateSuccess;

        if(strncmp((char*)recv_buf, "HTTP/", 5) == 0) {
            char* newline = strchr((char*)recv_buf, '\r');
            if(newline) *newline = '\0';
            snprintf(app->status_text, sizeof(app->status_text), "Received %ld bytes", (long)received);
            strncpy(app->response_text, (char*)recv_buf, sizeof(app->response_text) - 1);
        } else {
            snprintf(
                app->status_text, sizeof(app->status_text), "Received %ld bytes\n(not HTTP)", (long)received);
        }
        FURI_LOG_I(TAG, "HTTP response: %.100s", recv_buf);
    } else if(received == 0) {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Timeout - no response");
    } else {
        app->state = ExampleNetworkStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Receive error");
    }

    example_network_update_status(app);
    rpc_network_close(conn_id);
}

static void example_network_submenu_callback(void* context, uint32_t index) {
    ExampleNetworkApp* app = context;

    switch(index) {
    case ExampleNetworkMenuCheckStatus:
        example_network_check_status(app);
        break;
    case ExampleNetworkMenuUdpNtp:
        example_network_udp_ntp(app);
        break;
    case ExampleNetworkMenuTcpEcho:
        example_network_tcp_echo(app);
        break;
    case ExampleNetworkMenuTcpHttp:
        example_network_tcp_http(app);
        break;
    }
}

static ExampleNetworkApp* example_network_app_alloc(void) {
    ExampleNetworkApp* app = malloc(sizeof(ExampleNetworkApp));
    memset(app, 0, sizeof(ExampleNetworkApp));

    Gui* gui = furi_record_open(RECORD_GUI);

    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu, "Check Status", ExampleNetworkMenuCheckStatus, example_network_submenu_callback, app);
    submenu_add_item(
        app->submenu, "UDP Test (NTP time)", ExampleNetworkMenuUdpNtp, example_network_submenu_callback, app);
    submenu_add_item(
        app->submenu, "TCP Echo (tcpbin)", ExampleNetworkMenuTcpEcho, example_network_submenu_callback, app);
    submenu_add_item(
        app->submenu, "TCP HTTP (example.com)", ExampleNetworkMenuTcpHttp, example_network_submenu_callback, app);

    app->widget = widget_alloc();

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(app->view_dispatcher, ExampleNetworkViewMenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, ExampleNetworkViewStatus, widget_get_view(app->widget));
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, example_network_navigation_callback);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    return app;
}

static void example_network_app_free(ExampleNetworkApp* app) {
    view_dispatcher_remove_view(app->view_dispatcher, ExampleNetworkViewMenu);
    view_dispatcher_remove_view(app->view_dispatcher, ExampleNetworkViewStatus);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    furi_record_close(RECORD_GUI);
    free(app);
}

int32_t example_network_app(void* p) {
    UNUSED(p);

    FURI_LOG_I(TAG, "Example Network App starting");

    ExampleNetworkApp* app = example_network_app_alloc();

    view_dispatcher_switch_to_view(app->view_dispatcher, ExampleNetworkViewMenu);
    view_dispatcher_run(app->view_dispatcher);

    example_network_app_free(app);

    FURI_LOG_I(TAG, "Example Network App finished");

    return 0;
}
