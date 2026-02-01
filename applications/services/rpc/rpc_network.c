#include <flipper.pb.h>
#include <network.pb.h>
#include <furi.h>
#include <m-dict.h>

#include "rpc_i.h"

#define TAG "RpcNetwork"

#define RPC_NETWORK_MAX_CONNECTIONS 8

typedef enum {
    RpcNetworkEventResponse = (1 << 0),
    RpcNetworkEventReceiveData = (1 << 1),
    RpcNetworkEventStateChanged = (1 << 2),
    RpcNetworkEventTimeout = (1 << 3),
} RpcNetworkEvent;

#define RPC_NETWORK_ALL_EVENTS \
    (RpcNetworkEventResponse | RpcNetworkEventReceiveData | RpcNetworkEventStateChanged)

typedef struct {
    uint32_t connection_id;
    PB_Network_ConnectionState state;
    PB_Network_Protocol protocol;
    char* host;
    uint32_t port;
    char* resolved_ip;
    PB_Network_ErrorCode last_error;

    // Receive buffer for incoming data
    uint8_t* receive_buffer;
    size_t receive_buffer_size;
    size_t receive_data_len;
    bool receive_data_ready;
} NetworkConnection;

typedef struct {
    RpcSession* session;
    uint32_t next_connection_id;
    uint32_t next_command_id;
    NetworkConnection connections[RPC_NETWORK_MAX_CONNECTIONS];

    // For synchronous request/response
    FuriThreadId waiting_thread;
    uint32_t pending_command_id;
    PB_Main* pending_response;
} RpcNetworkSystem;

static RpcNetworkSystem* rpc_network_system = NULL;

static NetworkConnection* rpc_network_find_connection(RpcNetworkSystem* system, uint32_t connection_id) {
    for(size_t i = 0; i < RPC_NETWORK_MAX_CONNECTIONS; i++) {
        if(system->connections[i].connection_id == connection_id &&
           system->connections[i].state != PB_Network_ConnectionState_DISCONNECTED) {
            return &system->connections[i];
        }
    }
    return NULL;
}

static NetworkConnection* rpc_network_find_free_slot(RpcNetworkSystem* system) {
    for(size_t i = 0; i < RPC_NETWORK_MAX_CONNECTIONS; i++) {
        if(system->connections[i].state == PB_Network_ConnectionState_DISCONNECTED) {
            return &system->connections[i];
        }
    }
    return NULL;
}

static void rpc_network_connection_free(NetworkConnection* conn) {
    if(conn->host) {
        free(conn->host);
        conn->host = NULL;
    }
    if(conn->resolved_ip) {
        free(conn->resolved_ip);
        conn->resolved_ip = NULL;
    }
    if(conn->receive_buffer) {
        free(conn->receive_buffer);
        conn->receive_buffer = NULL;
    }
    conn->connection_id = 0;
    conn->state = PB_Network_ConnectionState_DISCONNECTED;
    conn->protocol = PB_Network_Protocol_TCP;
    conn->port = 0;
    conn->receive_buffer_size = 0;
    conn->receive_data_len = 0;
    conn->receive_data_ready = false;
    conn->last_error = PB_Network_ErrorCode_NONE;
}

// Handle connect_response from Android
static void rpc_network_connect_response_process(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_network_connect_response_tag);
    furi_assert(context);

    RpcNetworkSystem* system = (RpcNetworkSystem*)context;
    const PB_Network_ConnectResponse* response = &request->content.network_connect_response;

    FURI_LOG_D(
        TAG,
        "ConnectResponse: id=%lu, state=%d, error=%d",
        (unsigned long)response->connection_id,
        response->state,
        response->error);

    // Find connection and update state
    NetworkConnection* conn = rpc_network_find_connection(system, response->connection_id);
    if(conn) {
        conn->state = response->state;
        conn->last_error = response->error;
        if(response->resolved_ip) {
            if(conn->resolved_ip) free(conn->resolved_ip);
            conn->resolved_ip = strdup(response->resolved_ip);
        }
    }

    // If this is a response to a pending request, signal the waiting thread
    if(system->pending_command_id == request->command_id && system->waiting_thread) {
        system->pending_response = malloc(sizeof(PB_Main));
        memcpy(system->pending_response, request, sizeof(PB_Main));
        furi_thread_flags_set(system->waiting_thread, RpcNetworkEventResponse);
    }
}

// Handle send_response from Android
static void rpc_network_send_response_process(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_network_send_response_tag);
    furi_assert(context);

    RpcNetworkSystem* system = (RpcNetworkSystem*)context;
    const PB_Network_SendResponse* response = &request->content.network_send_response;

    FURI_LOG_D(
        TAG,
        "SendResponse: conn=%lu, bytes=%lu, error=%d",
        (unsigned long)response->connection_id,
        (unsigned long)response->bytes_sent,
        response->error);

    // If this is a response to a pending request, signal the waiting thread
    if(system->pending_command_id == request->command_id && system->waiting_thread) {
        system->pending_response = malloc(sizeof(PB_Main));
        memcpy(system->pending_response, request, sizeof(PB_Main));
        furi_thread_flags_set(system->waiting_thread, RpcNetworkEventResponse);
    }
}

// Handle receive_data from Android (unsolicited data push)
static void rpc_network_receive_data_process(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_network_receive_data_tag);
    furi_assert(context);

    RpcNetworkSystem* system = (RpcNetworkSystem*)context;
    const PB_Network_ReceiveData* data = &request->content.network_receive_data;

    FURI_LOG_D(
        TAG,
        "ReceiveData: conn=%lu, size=%zu",
        (unsigned long)data->connection_id,
        data->data ? data->data->size : 0);

    NetworkConnection* conn = rpc_network_find_connection(system, data->connection_id);
    if(conn && data->data && data->data->size > 0) {
        // Append to receive buffer
        size_t new_size = conn->receive_data_len + data->data->size;
        if(new_size > conn->receive_buffer_size) {
            conn->receive_buffer = realloc(conn->receive_buffer, new_size);
            conn->receive_buffer_size = new_size;
        }
        memcpy(
            conn->receive_buffer + conn->receive_data_len, data->data->bytes, data->data->size);
        conn->receive_data_len = new_size;
        conn->receive_data_ready = true;

        // Signal waiting thread if any
        if(system->waiting_thread) {
            furi_thread_flags_set(system->waiting_thread, RpcNetworkEventReceiveData);
        }
    }
}

// Handle close_response from Android
static void rpc_network_close_response_process(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_network_close_response_tag);
    furi_assert(context);

    RpcNetworkSystem* system = (RpcNetworkSystem*)context;
    const PB_Network_CloseResponse* response = &request->content.network_close_response;

    FURI_LOG_D(
        TAG,
        "CloseResponse: conn=%lu, error=%d",
        (unsigned long)response->connection_id,
        response->error);

    // Clean up the connection
    NetworkConnection* conn = rpc_network_find_connection(system, response->connection_id);
    if(conn) {
        rpc_network_connection_free(conn);
    }

    // If this is a response to a pending request, signal the waiting thread
    if(system->pending_command_id == request->command_id && system->waiting_thread) {
        system->pending_response = malloc(sizeof(PB_Main));
        memcpy(system->pending_response, request, sizeof(PB_Main));
        furi_thread_flags_set(system->waiting_thread, RpcNetworkEventResponse);
    }
}

// Handle state_changed from Android (connection state notifications)
static void rpc_network_state_changed_process(const PB_Main* request, void* context) {
    furi_assert(request);
    furi_assert(request->which_content == PB_Main_network_state_changed_tag);
    furi_assert(context);

    RpcNetworkSystem* system = (RpcNetworkSystem*)context;
    const PB_Network_StateChanged* state = &request->content.network_state_changed;

    FURI_LOG_I(
        TAG,
        "StateChanged: conn=%lu, state=%d, error=%d",
        (unsigned long)state->connection_id,
        state->state,
        state->error);

    NetworkConnection* conn = rpc_network_find_connection(system, state->connection_id);
    if(conn) {
        conn->state = state->state;
        conn->last_error = state->error;

        if(state->state == PB_Network_ConnectionState_DISCONNECTED) {
            rpc_network_connection_free(conn);
        }
    }

    // Signal waiting thread if any
    if(system->waiting_thread) {
        furi_thread_flags_set(system->waiting_thread, RpcNetworkEventStateChanged);
    }
}

// Public API for Flipper apps to use
// These functions are exported in the SDK API

__attribute__((used)) int32_t rpc_network_connect(
    const char* host,
    uint16_t port,
    bool is_udp,
    uint32_t timeout_ms,
    char* resolved_ip_out,
    size_t resolved_ip_size) {
    if(!rpc_network_system || !rpc_network_system->session) {
        FURI_LOG_E(TAG, "Network system not initialized or no RPC session");
        return -1;
    }

    RpcNetworkSystem* system = rpc_network_system;

    // Find free slot
    NetworkConnection* conn = rpc_network_find_free_slot(system);
    if(!conn) {
        FURI_LOG_W(TAG, "Max connections reached");
        return -1;
    }

    // Allocate connection
    uint32_t connection_id = ++system->next_connection_id;
    conn->connection_id = connection_id;
    conn->state = PB_Network_ConnectionState_CONNECTING;
    conn->protocol = is_udp ? PB_Network_Protocol_UDP : PB_Network_Protocol_TCP;
    conn->host = strdup(host);
    conn->port = port;
    conn->receive_buffer = malloc(512);
    conn->receive_buffer_size = 512;
    conn->receive_data_len = 0;
    conn->receive_data_ready = false;

    FURI_LOG_I(
        TAG,
        "Connecting %lu: %s to %s:%u",
        (unsigned long)connection_id,
        is_udp ? "UDP" : "TCP",
        host,
        port);

    // Build and send connect request
    PB_Main request = PB_Main_init_default;
    request.command_id = ++system->next_command_id;
    request.which_content = PB_Main_network_connect_request_tag;
    request.has_next = false;
    request.content.network_connect_request.host = strdup(host);
    request.content.network_connect_request.port = port;
    request.content.network_connect_request.protocol = conn->protocol;
    request.content.network_connect_request.timeout_ms = timeout_ms > 0 ? timeout_ms : 30000;
    request.content.network_connect_request.connection_id = connection_id;

    // Set up to wait for response
    system->pending_command_id = request.command_id;
    system->waiting_thread = furi_thread_get_current_id();
    system->pending_response = NULL;

    // Send request
    rpc_send_and_release(system->session, &request);

    // Wait for response with timeout
    uint32_t flags = furi_thread_flags_wait(
        RpcNetworkEventResponse | RpcNetworkEventStateChanged,
        FuriFlagWaitAny,
        timeout_ms > 0 ? timeout_ms : 30000);

    system->waiting_thread = 0;
    system->pending_command_id = 0;

    if(flags == (uint32_t)FuriFlagErrorTimeout) {
        FURI_LOG_E(TAG, "Connect timeout");
        rpc_network_connection_free(conn);
        return -1;
    }

    // Check response
    if(system->pending_response) {
        PB_Main* response = system->pending_response;
        if(response->which_content == PB_Main_network_connect_response_tag) {
            PB_Network_ConnectResponse* connect_resp = &response->content.network_connect_response;
            if(connect_resp->state == PB_Network_ConnectionState_CONNECTED) {
                conn->state = PB_Network_ConnectionState_CONNECTED;
                if(connect_resp->resolved_ip && resolved_ip_out && resolved_ip_size > 0) {
                    strncpy(resolved_ip_out, connect_resp->resolved_ip, resolved_ip_size - 1);
                    resolved_ip_out[resolved_ip_size - 1] = '\0';
                }
                FURI_LOG_I(TAG, "Connected: id=%lu", (unsigned long)connection_id);
                free(response);
                system->pending_response = NULL;
                return connection_id;
            } else {
                FURI_LOG_E(TAG, "Connect failed: error=%d", connect_resp->error);
            }
        }
        free(response);
        system->pending_response = NULL;
    }

    rpc_network_connection_free(conn);
    return -1;
}

__attribute__((used)) int32_t rpc_network_send(int32_t connection_id, const uint8_t* data, size_t size) {
    if(!rpc_network_system || !rpc_network_system->session) {
        return -1;
    }

    RpcNetworkSystem* system = rpc_network_system;
    NetworkConnection* conn = rpc_network_find_connection(system, connection_id);
    if(!conn || conn->state != PB_Network_ConnectionState_CONNECTED) {
        FURI_LOG_E(TAG, "Connection %ld not available", (long)connection_id);
        return -1;
    }

    FURI_LOG_D(TAG, "Sending %zu bytes on connection %ld", size, (long)connection_id);

    // Send in chunks of max 512 bytes
    size_t total_sent = 0;
    while(total_sent < size) {
        size_t chunk_size = size - total_sent;
        if(chunk_size > 512) chunk_size = 512;

        PB_Main request = PB_Main_init_default;
        request.command_id = ++system->next_command_id;
        request.which_content = PB_Main_network_send_request_tag;
        request.has_next = (total_sent + chunk_size < size);
        request.content.network_send_request.connection_id = connection_id;
        request.content.network_send_request.data =
            malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(chunk_size));
        request.content.network_send_request.data->size = chunk_size;
        memcpy(request.content.network_send_request.data->bytes, data + total_sent, chunk_size);

        // Set up to wait for response
        system->pending_command_id = request.command_id;
        system->waiting_thread = furi_thread_get_current_id();
        system->pending_response = NULL;

        rpc_send_and_release(system->session, &request);

        // Wait for response
        uint32_t flags = furi_thread_flags_wait(
            RpcNetworkEventResponse | RpcNetworkEventStateChanged, FuriFlagWaitAny, 10000);

        system->waiting_thread = 0;
        system->pending_command_id = 0;

        if(flags == (uint32_t)FuriFlagErrorTimeout) {
            FURI_LOG_E(TAG, "Send timeout");
            return total_sent > 0 ? (int32_t)total_sent : -1;
        }

        if(system->pending_response) {
            free(system->pending_response);
            system->pending_response = NULL;
        }

        total_sent += chunk_size;
    }

    return (int32_t)total_sent;
}

__attribute__((used)) int32_t rpc_network_receive(int32_t connection_id, uint8_t* buffer, size_t buffer_size, uint32_t timeout_ms) {
    if(!rpc_network_system || !rpc_network_system->session) {
        return -1;
    }

    RpcNetworkSystem* system = rpc_network_system;
    NetworkConnection* conn = rpc_network_find_connection(system, connection_id);
    if(!conn) {
        FURI_LOG_E(TAG, "Connection %ld not found", (long)connection_id);
        return -1;
    }

    // Check if data is already available
    if(conn->receive_data_len > 0) {
        size_t copy_size = conn->receive_data_len;
        if(copy_size > buffer_size) copy_size = buffer_size;
        memcpy(buffer, conn->receive_buffer, copy_size);

        // Remove copied data from buffer
        if(copy_size < conn->receive_data_len) {
            memmove(
                conn->receive_buffer,
                conn->receive_buffer + copy_size,
                conn->receive_data_len - copy_size);
        }
        conn->receive_data_len -= copy_size;
        conn->receive_data_ready = conn->receive_data_len > 0;

        return (int32_t)copy_size;
    }

    // Wait for data
    system->waiting_thread = furi_thread_get_current_id();

    uint32_t flags = furi_thread_flags_wait(
        RpcNetworkEventReceiveData | RpcNetworkEventStateChanged,
        FuriFlagWaitAny,
        timeout_ms > 0 ? timeout_ms : FuriWaitForever);

    system->waiting_thread = 0;

    if(flags == (uint32_t)FuriFlagErrorTimeout) {
        return 0; // Timeout, no data
    }

    // Check for state change (disconnection)
    if(conn->state == PB_Network_ConnectionState_DISCONNECTED) {
        return -1;
    }

    // Copy available data
    if(conn->receive_data_len > 0) {
        size_t copy_size = conn->receive_data_len;
        if(copy_size > buffer_size) copy_size = buffer_size;
        memcpy(buffer, conn->receive_buffer, copy_size);

        if(copy_size < conn->receive_data_len) {
            memmove(
                conn->receive_buffer,
                conn->receive_buffer + copy_size,
                conn->receive_data_len - copy_size);
        }
        conn->receive_data_len -= copy_size;
        conn->receive_data_ready = conn->receive_data_len > 0;

        return (int32_t)copy_size;
    }

    return 0;
}

__attribute__((used)) void rpc_network_close(int32_t connection_id) {
    if(!rpc_network_system || !rpc_network_system->session) {
        return;
    }

    RpcNetworkSystem* system = rpc_network_system;
    NetworkConnection* conn = rpc_network_find_connection(system, connection_id);
    if(!conn) {
        return;
    }

    FURI_LOG_I(TAG, "Closing connection %ld", (long)connection_id);

    PB_Main request = PB_Main_init_default;
    request.command_id = ++system->next_command_id;
    request.which_content = PB_Main_network_close_request_tag;
    request.has_next = false;
    request.content.network_close_request.connection_id = connection_id;

    rpc_send_and_release(system->session, &request);

    // Don't wait for response, just clean up locally
    rpc_network_connection_free(conn);
}

__attribute__((used)) bool rpc_network_is_available(void) {
    return rpc_network_system != NULL && rpc_network_system->session != NULL;
}

void* rpc_system_network_alloc(RpcSession* session) {
    furi_assert(session);

    RpcNetworkSystem* system = malloc(sizeof(RpcNetworkSystem));
    memset(system, 0, sizeof(RpcNetworkSystem));
    system->session = session;
    system->next_connection_id = 0;
    system->next_command_id = 1000; // Start at 1000 to avoid conflicts

    // Initialize all connections as disconnected
    for(size_t i = 0; i < RPC_NETWORK_MAX_CONNECTIONS; i++) {
        system->connections[i].connection_id = 0;
        system->connections[i].state = PB_Network_ConnectionState_DISCONNECTED;
        system->connections[i].protocol = PB_Network_Protocol_TCP;
        system->connections[i].host = NULL;
        system->connections[i].port = 0;
        system->connections[i].receive_buffer = NULL;
        system->connections[i].receive_buffer_size = 0;
    }

    // Register handlers for RESPONSES from Android
    RpcHandler rpc_handler = {
        .message_handler = NULL,
        .decode_submessage = NULL,
        .context = system,
    };

    rpc_handler.message_handler = rpc_network_connect_response_process;
    rpc_add_handler(session, PB_Main_network_connect_response_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_network_send_response_process;
    rpc_add_handler(session, PB_Main_network_send_response_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_network_receive_data_process;
    rpc_add_handler(session, PB_Main_network_receive_data_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_network_close_response_process;
    rpc_add_handler(session, PB_Main_network_close_response_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_network_state_changed_process;
    rpc_add_handler(session, PB_Main_network_state_changed_tag, &rpc_handler);

    // Store global reference for the public API
    rpc_network_system = system;

    FURI_LOG_I(TAG, "Network RPC system allocated");

    return system;
}

void rpc_system_network_free(void* context) {
    furi_assert(context);

    RpcNetworkSystem* system = (RpcNetworkSystem*)context;

    // Clear global reference
    if(rpc_network_system == system) {
        rpc_network_system = NULL;
    }

    // Free all connections
    for(size_t i = 0; i < RPC_NETWORK_MAX_CONNECTIONS; i++) {
        rpc_network_connection_free(&system->connections[i]);
    }

    if(system->pending_response) {
        free(system->pending_response);
    }

    free(system);

    FURI_LOG_I(TAG, "Network RPC system freed");
}
