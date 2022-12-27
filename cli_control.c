#include "cli_control.h"

#include <cli/cli.h>
#include <cli/cli_i.h>
#include <cli/cli_vcp.h>
#include "cligui_main_i.h"

volatile bool gotCallbackSet = false;

FuriStreamBuffer* tx_stream;
FuriStreamBuffer* rx_stream;
static size_t real_rx_handler(uint8_t* buffer, size_t size, uint32_t timeout) {
    size_t rx_cnt = 0;
    while(size > 0) {
        size_t batch_size = size;
        if(batch_size > 128) batch_size = 128;
        size_t len = furi_stream_buffer_receive(rx_stream, buffer, batch_size, timeout);
        if(len == 0) break;
        size -= len;
        buffer += len;
        rx_cnt += len;
    }
    return rx_cnt;
}
static void tx_handler_stdout(const char* buffer, size_t size) {
    furi_stream_buffer_send(tx_stream, buffer, size, FuriWaitForever);
}

static void tx_handler(const uint8_t* buffer, size_t size) {
    furi_thread_set_stdout_callback(tx_handler_stdout);
    furi_stream_buffer_send(tx_stream, buffer, size, FuriWaitForever);
}

static void session_init(void) {}
static void session_deinit(void) {}
static bool session_connected(void) {return true;}
static CliSession session;
void latch_tx_handler() {
    Cli* global_cli = furi_record_open(RECORD_CLI);
    furi_thread_set_stdout_callback(tx_handler_stdout);
    rx_stream = furi_stream_buffer_alloc(128, 1);
    tx_stream = furi_stream_buffer_alloc(128, 1);

    session.tx = &tx_handler;
    session.rx = &real_rx_handler;
    session.tx_stdout  = &tx_handler_stdout;
    session.init = &session_init;
    session.deinit = &session_deinit;
    session.is_connected = &session_connected;
    cli_session_close(global_cli);
    cli_session_open(global_cli, &session);
    // furi_stream_buffer_send(rx_stream, "help\r\n", 6, FuriWaitForever);
    // Unlock loader-lock
    Loader* loader = furi_record_open(RECORD_LOADER);
    Loader_internal* loader_i = (Loader_internal*)loader;
    loader_i->lock_count = 0;
    furi_record_close(RECORD_CLI);
    furi_record_close(RECORD_LOADER);
}
void unlatch_tx_handler() {
    Cli* global_cli = furi_record_open(RECORD_CLI);
    cli_session_close(global_cli);
    cli_session_open(global_cli, &cli_vcp);
    furi_stream_buffer_free(rx_stream);
    furi_stream_buffer_free(tx_stream);
    furi_record_close(RECORD_CLI);
}