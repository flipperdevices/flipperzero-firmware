#include "cli_i.h"
#include <rpc/rpc.h>

typedef struct {
    Cli* cli;
    bool session_closed;
} CliRpc;

#define CLI_READ_BUFFER_SIZE    100

static void cli_send_bytes_callback(void* context, uint8_t* bytes, size_t bytes_len) {
    furi_assert(context);
    furi_assert(bytes);
    furi_assert(bytes_len);
    CliRpc* cli_rpc = context;

    cli_write(cli_rpc->cli, bytes, bytes_len);
}

static void cli_session_closed_callback(void* context) {
    furi_assert(context);
    CliRpc* cli_rpc = context;

    cli_rpc->session_closed = true;
}

void cli_command_start_rpc_session(Cli* cli, string_t args, void* context) {
    Rpc* rpc = furi_record_open("rpc");
    furi_record_close("rpc");

    RpcSession* rpc_session = rpc_open_session(rpc);
    if (rpc_session == NULL) {
        printf("Another session is in progress\r\n");
        return;
    }

    CliRpc cli_rpc = {.cli = cli, .session_closed = false};
    rpc_set_session_context(rpc_session, &cli_rpc);
    rpc_set_send_bytes_callback(rpc_session, cli_send_bytes_callback);
    rpc_set_session_closed_callback(rpc_session, cli_session_closed_callback);

    uint8_t* buffer = furi_alloc(CLI_READ_BUFFER_SIZE);

    size_t size_received = 0;
    bool exit = false;

    while (!exit) {
        size_received = furi_hal_vcp_rx_with_timeout(buffer, CLI_READ_BUFFER_SIZE, 50);
        if (!furi_hal_vcp_is_connected() || cli_rpc.session_closed) {
            break;
        }

        if (size_received) {
            rpc_feed_bytes(rpc_session, buffer, size_received, 3000);
        }
    }

    rpc_close_session(rpc_session);

    free(buffer);
}


