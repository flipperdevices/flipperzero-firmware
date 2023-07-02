#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_usb_cdc.h>
#include <storage/storage.h>
#include "bc_scanner_script.h"
#include "cli/cli_vcp.h"
#include "cli/cli.h"

#define TAG "BarCodeScanner"
#define WORKER_TAG TAG "Worker"
#define FILE_BUFFER_LEN 50

#define SCRIPT_STATE_ERROR (-1)
#define SCRIPT_STATE_END (-2)
#define SCRIPT_STATE_NEXT_LINE (-3)

#define UART_BAUD 19200
#define UART_PORT 0

/**
 * 
 */
typedef enum {
    WorkerEvtToggle = (1 << 0),
    WorkerEvtEnd = (1 << 1),
    WorkerEvtConnect = (1 << 2),
    WorkerEvtDisconnect = (1 << 3),
} WorkerEvtFlags;

/**
 *
 */
struct BarCodeScript {
    BarCodeState st;
    FuriString* file_path;
    FuriThread* thread;
    uint8_t file_buf[FILE_BUFFER_LEN];
    uint8_t buf_len;
    bool is_file_end;
};

/**
 * Speaker scanning sound
 */
static void scan_sound() {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        float frequency = 4000;
        furi_hal_speaker_stop();
        furi_hal_speaker_set_volume(100);
        furi_hal_speaker_start(frequency, 100);
        furi_delay_ms(50);
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}

/**
 *
 */
static void usb_uart_serial_init() {
    furi_hal_usb_unlock();
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_session_close(cli);
    furi_record_close(RECORD_CLI);
    furi_check(furi_hal_usb_set_config(&usb_cdc_single, NULL) == true);
    furi_hal_console_disable();
    furi_hal_uart_set_br(FuriHalUartIdUSART1, UART_BAUD);
}

/**
 *
 */
static void usb_uart_serial_deinit() {
    furi_hal_usb_unlock();
    furi_check(furi_hal_usb_set_config(&usb_cdc_single, NULL) == true);
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_session_open(cli, &cli_vcp);
    furi_record_close(RECORD_CLI);
    furi_hal_console_enable();
}
/**
 *
 * @param chr
 * @return
 */
static bool is_bc_end(const char chr) {
    return ((chr == '\0') || (chr == '\r') || (chr == '\n')); //TODO SPACE NEED???
}

/**
 *
 * @param bc_script
 * @param script_file
 * @return
 */
static uint16_t bc_script_read_file(BarCodeScript* bc_script, File* script_file) {
    UNUSED(is_bc_end);
    bc_script->st.line_nb = 0;
    uint16_t ret = storage_file_read(script_file, bc_script->file_buf, FILE_BUFFER_LEN);
    if(storage_file_eof(script_file)) {
        bc_script->is_file_end = true;
    }
    bc_script->st.line_nb += ret;
    return ret;
}

/**
 * Main worker
 * @param context
 * @return
 */
static int32_t bc_scanner_worker(void* context) {
    BarCodeScript* bc_script = context;

    BarCodeWorkerState worker_state = BarCodeStateInit;
    int32_t delay_val = 0;
    UNUSED(delay_val);
    File* script_file = storage_file_alloc(furi_record_open(RECORD_STORAGE));

    FURI_LOG_I(WORKER_TAG, "Init");
    //uint8_t buff[5] = {'p', 'r', 'i', 'v', 'k'};
    //uint8_t state = 99;

    usb_uart_serial_init();

    while(1) {
        //state = furi_hal_cdc_get_ctrl_line_state(FuriHalUartIdUSART1);
        //FURI_LOG_I(WORKER_TAG, "STATTE: %d", state);
        if(worker_state == BarCodeStateInit) {
            if(storage_file_open(
                   script_file,
                   furi_string_get_cstr(bc_script->file_path),
                   FSAM_READ,
                   FSOM_OPEN_EXISTING)) {
                uint64_t size = storage_file_size(script_file);
                bc_script->st.line_nb = size;
                if(size > 0) {
                    if(1) { //TODO Check USB Connect
                        worker_state = BarCodeStateIdle; // Ready to run
                    } else {
                        //worker_state = BadUsbStateNotConnected; // USB not connected
                    }
                } else {
                    FURI_LOG_E(WORKER_TAG, "File empty error");
                    worker_state = BarCodeStateFileError;
                    bc_script->st.error_enum = FileIsEmpty;
                }
            } else {
                FURI_LOG_E(WORKER_TAG, "File open error");
                worker_state = BarCodeStateFileError; // File open error
                bc_script->st.error_enum = FileOpenError;
            }
            bc_script->st.state = worker_state;
        } else if(worker_state == BarCodeStateIdle) { // State: ready to start
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd | WorkerEvtConnect | WorkerEvtToggle,
                FuriFlagWaitAny,
                FuriWaitForever);
            furi_check((flags & FuriFlagError) == 0);
            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtConnect) {
                worker_state = BarCodeStateIdle; // Ready to run
            } else if(flags & WorkerEvtToggle) {
                FURI_LOG_I(WORKER_TAG, "SendUART_MSG");
                bc_script->st.state = BarCodeStateRunning;
                bc_script->st.line_cur = 0;
                furi_delay_ms(450);
                scan_sound();
                while(!bc_script->is_file_end) {
                    bc_script->st.state = BarCodeStateRunning;
                    uint16_t size = bc_script_read_file(bc_script, script_file);
                    bc_script->st.line_cur = size;
                    furi_hal_cdc_send(UART_PORT, bc_script->file_buf, size);
                }
                worker_state = BarCodeStateIdle;
                bc_script->st.state = BarCodeStateDone;
                storage_file_seek(script_file, 0, true);
                bc_script->is_file_end = false;
                continue;
            }
            bc_script->st.state = worker_state;
        } else if(
            (worker_state == BarCodeStateFileError) ||
            (worker_state == BarCodeStateScriptError)) { // State: error
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd, FuriFlagWaitAny, FuriWaitForever); // Waiting for exit command
            furi_check((flags & FuriFlagError) == 0);
            if(flags & WorkerEvtEnd) {
                break;
            }
        }
    }

    usb_uart_serial_deinit();
    storage_file_close(script_file);
    storage_file_free(script_file);
    FURI_LOG_I(WORKER_TAG, "End");

    return 0;
}
/**
 * Open bc_scanner script
 * @param file_path
 * @return
 */
BarCodeScript* bc_scanner_script_open(FuriString* file_path) {
    FURI_LOG_D(WORKER_TAG, "bc_scanner_script_open");
    furi_assert(file_path);

    BarCodeScript* bc_script = malloc(sizeof(BarCodeScript));
    bc_script->file_path = furi_string_alloc();
    furi_string_set(bc_script->file_path, file_path);

    bc_script->st.state = BarCodeStateInit;
    bc_script->st.error[0] = '\0';

    bc_script->thread =
        furi_thread_alloc_ex("BcScannerWorker", 2048, bc_scanner_worker, bc_script);
    furi_thread_start(bc_script->thread);
    return bc_script;
} //-V773

/**
 *
 * @param bc_script
 */
void bc_scanner_script_close(BarCodeScript* bc_script) {
    furi_assert(bc_script);
    furi_thread_flags_set(furi_thread_get_id(bc_script->thread), WorkerEvtEnd);
    furi_thread_join(bc_script->thread);
    furi_thread_free(bc_script->thread);
    furi_string_free(bc_script->file_path);
    free(bc_script);
    FURI_LOG_D(WORKER_TAG, "bc_scanner_script_close");
}

/**
 *
 * @param bc_script
 */
void bc_scanner_script_toggle(BarCodeScript* bc_script) {
    furi_assert(bc_script);
    furi_thread_flags_set(furi_thread_get_id(bc_script->thread), WorkerEvtToggle);
}
/**
 *
 * @param bc_script
 * @return
 */
BarCodeState* bc_scanner_script_get_state(BarCodeScript* bc_script) {
    furi_assert(bc_script);
    return &(bc_script->st);
}
