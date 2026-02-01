#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <rpc/rpc_network.h>

#define TAG "NetworkTest"

typedef enum {
    NetworkTestViewMenu,
    NetworkTestViewStatus,
} NetworkTestView;

typedef enum {
    NetworkTestMenuCheckStatus,
    NetworkTestMenuConnectEcho,
    NetworkTestMenuConnectHttp,
} NetworkTestMenuItem;

typedef enum {
    NetworkTestStateIdle,
    NetworkTestStateChecking,
    NetworkTestStateConnecting,
    NetworkTestStateConnected,
    NetworkTestStateSending,
    NetworkTestStateWaiting,
    NetworkTestStateSuccess,
    NetworkTestStateError,
} NetworkTestState;

typedef struct {
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    NetworkTestState state;
    int32_t connection_id;
    NetworkTestView current_view;
    char status_text[256];
    char response_text[128];
} NetworkTestApp;

static void network_test_update_status(NetworkTestApp* app) {
    widget_reset(app->widget);

    const char* state_str = "Unknown";
    switch(app->state) {
    case NetworkTestStateIdle:
        state_str = "Idle";
        break;
    case NetworkTestStateChecking:
        state_str = "Checking...";
        break;
    case NetworkTestStateConnecting:
        state_str = "Connecting...";
        break;
    case NetworkTestStateConnected:
        state_str = "Connected!";
        break;
    case NetworkTestStateSending:
        state_str = "Sending...";
        break;
    case NetworkTestStateWaiting:
        state_str = "Waiting response...";
        break;
    case NetworkTestStateSuccess:
        state_str = "Success!";
        break;
    case NetworkTestStateError:
        state_str = "Error";
        break;
    }

    widget_add_string_element(
        app->widget, 64, 5, AlignCenter, AlignTop, FontPrimary, "Network Test");
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

static bool network_test_navigation_callback(void* context) {
    NetworkTestApp* app = context;

    // If we're in the status view, go back to menu
    if(app->current_view == NetworkTestViewStatus) {
        app->current_view = NetworkTestViewMenu;
        view_dispatcher_switch_to_view(app->view_dispatcher, NetworkTestViewMenu);
        return true;
    }

    // Otherwise exit
    view_dispatcher_stop(app->view_dispatcher);
    return true;
}

static void network_test_check_status(NetworkTestApp* app) {
    app->state = NetworkTestStateChecking;
    app->status_text[0] = '\0';
    app->response_text[0] = '\0';
    network_test_update_status(app);
    app->current_view = NetworkTestViewStatus;
    view_dispatcher_switch_to_view(app->view_dispatcher, NetworkTestViewStatus);

    bool available = rpc_network_is_available();
    if(available) {
        app->state = NetworkTestStateSuccess;
        snprintf(
            app->status_text,
            sizeof(app->status_text),
            "Network available!\nConnect phone with\nFlipper app.");
    } else {
        app->state = NetworkTestStateError;
        snprintf(
            app->status_text,
            sizeof(app->status_text),
            "No RPC session.\nConnect phone with\nFlipper app first.");
    }
    network_test_update_status(app);
}

static void network_test_connect_echo(NetworkTestApp* app) {
    app->state = NetworkTestStateConnecting;
    app->status_text[0] = '\0';
    app->response_text[0] = '\0';
    snprintf(app->status_text, sizeof(app->status_text), "tcpbin.com:4242");
    network_test_update_status(app);
    app->current_view = NetworkTestViewStatus;
    view_dispatcher_switch_to_view(app->view_dispatcher, NetworkTestViewStatus);

    // Check if network is available
    if(!rpc_network_is_available()) {
        app->state = NetworkTestStateError;
        snprintf(app->status_text, sizeof(app->status_text), "No RPC session\nConnect phone first");
        network_test_update_status(app);
        return;
    }

    // Connect to echo server
    char resolved_ip[64] = {0};
    int32_t conn_id = rpc_network_connect("tcpbin.com", 4242, false, 15000, resolved_ip, sizeof(resolved_ip));

    if(conn_id < 0) {
        app->state = NetworkTestStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Connect failed!");
        network_test_update_status(app);
        return;
    }

    app->connection_id = conn_id;
    app->state = NetworkTestStateSending;
    snprintf(
        app->status_text,
        sizeof(app->status_text),
        "Connected! ID=%ld\nIP: %s",
        (long)conn_id,
        resolved_ip[0] ? resolved_ip : "unknown");
    network_test_update_status(app);

    // Send test message
    const char* test_msg = "Hello from Flipper!\n";
    int32_t sent = rpc_network_send(conn_id, (const uint8_t*)test_msg, strlen(test_msg));

    if(sent < 0) {
        app->state = NetworkTestStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Send failed!");
        network_test_update_status(app);
        rpc_network_close(conn_id);
        return;
    }

    app->state = NetworkTestStateWaiting;
    snprintf(
        app->status_text,
        sizeof(app->status_text),
        "Sent %ld bytes\nWaiting for echo...",
        (long)sent);
    network_test_update_status(app);

    // Wait for response
    uint8_t recv_buf[128];
    int32_t received = rpc_network_receive(conn_id, recv_buf, sizeof(recv_buf) - 1, 10000);

    if(received > 0) {
        recv_buf[received] = '\0';
        app->state = NetworkTestStateSuccess;
        snprintf(app->status_text, sizeof(app->status_text), "Received %ld bytes:", (long)received);
        // Copy first 30 chars of response
        strncpy(app->response_text, (char*)recv_buf, 30);
        app->response_text[30] = '\0';
        FURI_LOG_I(TAG, "Echo response: %s", recv_buf);
    } else if(received == 0) {
        app->state = NetworkTestStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Timeout - no response");
    } else {
        app->state = NetworkTestStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Receive error");
    }

    network_test_update_status(app);
    rpc_network_close(conn_id);
}

static void network_test_connect_http(NetworkTestApp* app) {
    app->state = NetworkTestStateConnecting;
    app->status_text[0] = '\0';
    app->response_text[0] = '\0';
    snprintf(app->status_text, sizeof(app->status_text), "example.com:80");
    network_test_update_status(app);
    app->current_view = NetworkTestViewStatus;
    view_dispatcher_switch_to_view(app->view_dispatcher, NetworkTestViewStatus);

    if(!rpc_network_is_available()) {
        app->state = NetworkTestStateError;
        snprintf(app->status_text, sizeof(app->status_text), "No RPC session\nConnect phone first");
        network_test_update_status(app);
        return;
    }

    char resolved_ip[64] = {0};
    int32_t conn_id = rpc_network_connect("example.com", 80, false, 15000, resolved_ip, sizeof(resolved_ip));

    if(conn_id < 0) {
        app->state = NetworkTestStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Connect failed!");
        network_test_update_status(app);
        return;
    }

    app->connection_id = conn_id;
    app->state = NetworkTestStateSending;
    snprintf(
        app->status_text,
        sizeof(app->status_text),
        "Connected! IP: %s",
        resolved_ip[0] ? resolved_ip : "unknown");
    network_test_update_status(app);

    // Send HTTP request
    const char* http_req = "GET / HTTP/1.0\r\nHost: example.com\r\n\r\n";
    int32_t sent = rpc_network_send(conn_id, (const uint8_t*)http_req, strlen(http_req));

    if(sent < 0) {
        app->state = NetworkTestStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Send failed!");
        network_test_update_status(app);
        rpc_network_close(conn_id);
        return;
    }

    app->state = NetworkTestStateWaiting;
    snprintf(app->status_text, sizeof(app->status_text), "Sent HTTP request\nWaiting...");
    network_test_update_status(app);

    // Wait for response
    uint8_t recv_buf[256];
    int32_t received = rpc_network_receive(conn_id, recv_buf, sizeof(recv_buf) - 1, 15000);

    if(received > 0) {
        recv_buf[received] = '\0';
        app->state = NetworkTestStateSuccess;

        // Check for HTTP response
        if(strncmp((char*)recv_buf, "HTTP/", 5) == 0) {
            // Extract status line
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
        app->state = NetworkTestStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Timeout - no response");
    } else {
        app->state = NetworkTestStateError;
        snprintf(app->status_text, sizeof(app->status_text), "Receive error");
    }

    network_test_update_status(app);
    rpc_network_close(conn_id);
}

static void network_test_submenu_callback(void* context, uint32_t index) {
    NetworkTestApp* app = context;

    switch(index) {
    case NetworkTestMenuCheckStatus:
        network_test_check_status(app);
        break;
    case NetworkTestMenuConnectEcho:
        network_test_connect_echo(app);
        break;
    case NetworkTestMenuConnectHttp:
        network_test_connect_http(app);
        break;
    }
}

static NetworkTestApp* network_test_app_alloc(void) {
    NetworkTestApp* app = malloc(sizeof(NetworkTestApp));
    memset(app, 0, sizeof(NetworkTestApp));

    Gui* gui = furi_record_open(RECORD_GUI);

    // Create submenu
    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu, "Check Status", NetworkTestMenuCheckStatus, network_test_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Echo Test (tcpbin)", NetworkTestMenuConnectEcho, network_test_submenu_callback, app);
    submenu_add_item(
        app->submenu, "HTTP Test (example.com)", NetworkTestMenuConnectHttp, network_test_submenu_callback, app);

    // Create status widget
    app->widget = widget_alloc();

    // Create view dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(app->view_dispatcher, NetworkTestViewMenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, NetworkTestViewStatus, widget_get_view(app->widget));
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, network_test_navigation_callback);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    return app;
}

static void network_test_app_free(NetworkTestApp* app) {
    view_dispatcher_remove_view(app->view_dispatcher, NetworkTestViewMenu);
    view_dispatcher_remove_view(app->view_dispatcher, NetworkTestViewStatus);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    furi_record_close(RECORD_GUI);
    free(app);
}

int32_t network_test_app(void* p) {
    UNUSED(p);

    FURI_LOG_I(TAG, "Network Test App starting");

    NetworkTestApp* app = network_test_app_alloc();

    view_dispatcher_switch_to_view(app->view_dispatcher, NetworkTestViewMenu);
    view_dispatcher_run(app->view_dispatcher);

    network_test_app_free(app);

    FURI_LOG_I(TAG, "Network Test App finished");

    return 0;
}
