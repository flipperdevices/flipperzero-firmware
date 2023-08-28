#include "lin_hacker_worker.h"

#include <furi.h>

#include "../lib/driver/lin/lin_bus.h"

#define TAG "LinChackerWorker"

typedef enum {
    LinChackerWorkerEvtStop = (1 << 0),

    LinChackerWorkerEvtRx = (1 << 1),
    LinChackerWorkerEvtTxCoplete = (1 << 2),
    LinChackerWorkerEvtTx = (1 << 3),
    LinChackerWorkerEvtState = (1 << 4),

    //LinChackerWorkerEvtCfg = (1 << 5),

} LinChackerWorkerEvt;

struct LinChackerWorker {
    FuriThread* thread;
    volatile bool worker_running;
    uint8_t connect_usb;

    LinChackerWorkerCallback callback;
    void* context;
};

#define LIN_HACKER_WORKER_PROG_ALL_EVENTS (LinChackerWorkerEvtStop)
#define LIN_HACKER_WORKER_ALL_EVENTS                                                \
    (LinChackerWorkerEvtTx | LinChackerWorkerEvtTxCoplete | LinChackerWorkerEvtRx | \
     LinChackerWorkerEvtStop | LinChackerWorkerEvtState)

//########################/* VCP CDC */#############################################
#include "usb_cdc.h"
#include <cli/cli_vcp.h>
#include <cli/cli.h>
#include <furi_hal_usb_cdc.h>

#define LIN_HACKER_VCP_CDC_CH 1
#define LIN_HACKER_VCP_CDC_PKT_LEN CDC_DATA_SZ
#define LIN_HACKER_VCP_UART_RX_BUF_SIZE (LIN_HACKER_VCP_CDC_PKT_LEN * 5)

static void vcp_on_cdc_tx_complete(void* context);
static void vcp_on_cdc_rx(void* context);
static void vcp_state_callback(void* context, uint8_t state);
static void vcp_on_cdc_control_line(void* context, uint8_t state);
static void vcp_on_line_config(void* context, struct usb_cdc_line_coding* config);

static const CdcCallbacks cdc_cb = {
    vcp_on_cdc_tx_complete,
    vcp_on_cdc_rx,
    vcp_state_callback,
    vcp_on_cdc_control_line,
    vcp_on_line_config,
};

/* VCP callbacks */

static void vcp_on_cdc_tx_complete(void* context) {
    furi_assert(context);
    LinChackerWorker* instance = context;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), LinChackerWorkerEvtTxCoplete);
}

static void vcp_on_cdc_rx(void* context) {
    furi_assert(context);
    LinChackerWorker* instance = context;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), LinChackerWorkerEvtRx);
}

static void vcp_state_callback(void* context, uint8_t state) {
    UNUSED(context);

    LinChackerWorker* instance = context;
    instance->connect_usb = state;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), LinChackerWorkerEvtState);
}

static void vcp_on_cdc_control_line(void* context, uint8_t state) {
    UNUSED(context);
    UNUSED(state);
}

static void vcp_on_line_config(void* context, struct usb_cdc_line_coding* config) {
    UNUSED(context);
    UNUSED(config);
}

static void lin_hacker_worker_vcp_cdc_init(void* context) {
    furi_hal_usb_unlock();
    Cli* cli = furi_record_open(RECORD_CLI);
    //close cli
    cli_session_close(cli);
    //disable callbacks VCP_CDC=0
    furi_hal_cdc_set_callbacks(0, NULL, NULL);
    //set 2 cdc
    furi_check(furi_hal_usb_set_config(&usb_cdc_dual, NULL) == true);
    //open cli VCP_CDC=0
    cli_session_open(cli, &cli_vcp);
    furi_record_close(RECORD_CLI);

    furi_hal_cdc_set_callbacks(LIN_HACKER_VCP_CDC_CH, (CdcCallbacks*)&cdc_cb, context);
}

static void lin_hacker_worker_vcp_cdc_deinit(void) {
    //disable callbacks LIN_HACKER_VCP_CDC_CH
    furi_hal_cdc_set_callbacks(LIN_HACKER_VCP_CDC_CH, NULL, NULL);

    Cli* cli = furi_record_open(RECORD_CLI);
    //close cli
    cli_session_close(cli);
    furi_hal_usb_unlock();
    //set 1 cdc
    furi_check(furi_hal_usb_set_config(&usb_cdc_single, NULL) == true);
    //open cli VCP_CDC=0
    cli_session_open(cli, &cli_vcp);
    furi_record_close(RECORD_CLI);
}

//#################################################################################

// static int32_t lin_hacker_worker_proc_thread(void* context) {
//     UNUSED(context);
//     // CanHacker2* can_hacker2 = context;

//     // FURI_LOG_I(TAG, "CanHacker2ProcessWorker Start");
//     // while(1) {
//     //     if(furi_thread_flags_get() & LinChackerWorkerEvtStop) break;
//     //     lin_hacker_process(can_hacker2);
//     // }
//     // FURI_LOG_I(TAG, "CanHacker2ProcessWorker Stop");
//     return 0;
// }

// static void lin_hacker_worker_proc_tx_data(void* context) {
//     furi_assert(context);
//     LinChackerWorker* instance = context;
//     furi_thread_flags_set(furi_thread_get_id(instance->thread), LinChackerWorkerEvtTx);
// }

/** Worker thread
 * 
 * @param context 
 * @return exit code 
 */
static int32_t lin_hacker_worker_thread(void* context) {
    LinChackerWorker* instance = context;
    lin_hacker_worker_vcp_cdc_init(instance);

    // CanHacker2* can_hacker2 = lin_hacker_alloc();
    // lin_hacker_set_callback(can_hacker2, lin_hacker_worker_proc_tx_data, instance);

    // uint8_t buf[LIN_HACKER_VCP_UART_RX_BUF_SIZE];
    // size_t len = 0;

    // FuriThread* lin_hacker_proc =
    //     furi_thread_alloc_ex("CanHacker2Proc", 1024, lin_hacker_worker_proc_thread, can_hacker2);
    // furi_thread_start(lin_hacker_proc);

    FURI_LOG_I(TAG, "Start");

    //furi_hal_console_disable();
    furi_hal_console_deinit();

    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, NULL, NULL);
    furi_hal_uart_deinit(FuriHalUartIdUSART1);

    furi_hal_console_init(FuriHalUartIdLPUART1, CONSOLE_BAUDRATE);
    furi_hal_console_enable();

    //furi_hal_uart_set_br(FuriHalUartIdUSART1, 9600);
    //furi_hal_uart_set_irq_cb(uart_ch, usb_uart_on_irq_cb, usb_uart);

    //LinBus* lin_bus = lin_bus_init(LinBusModeMaster, 9600);
    LinBus* lin_bus = lin_bus_init(LinBusModeSlave, 9600);

    furi_delay_ms(100);
    //uint8_t temp = 0;

    // LinBusFrame frame = {
    //     .id = 0x03,
    //     .data = {0x01, 0x02, 0x03, 0x04, 0x05},
    //     .length = 5,
    //     .crc_type = LinBusChecksumTypeClassic,
    //     //.response_length = 2,
    //     .frame_type = LinBusMasterResponse,
    // };

    // LinBusFrame frame1 = {
    //     .id = 0x01,
    //     .data = {0x01, 0x02, 0x03, 0x04, 0x05},
    //     .length = 5,
    //     .crc_type = LinBusChecksumTypeClassic,
    //     .response_length = 2,
    //     .frame_type = LinBusMasterRequest,
    // };
    size_t count_frame = 0;
    LinBusFrame frame_resp = {
        .id = 0x23,
        .data = {0x01, 0x02, 0x03, 0x04, 0x05},
        .length = 5,
        .crc_type = LinBusChecksumTypeClassic,
        .response_length = 0,
        .frame_type = LinBusSlaveResponse,
    };
    LinBusFrame frame_resp1 = {
        .id = 0x24,
        .data = {0xFF, 0xEE, 0xDD, 0x11, 0x05, 0x06, 0x07, 0x08},
        .length = 8,
        .crc_type = LinBusChecksumTypeClassic,
        .response_length = 0,
        .frame_type = LinBusSlaveResponse,
    };
    LinBusFrame frame_resp2 = {
        .id = 0x25,
        .data = {0xAA, 0xBB, 0x03},
        .length = 3,
        .crc_type = LinBusChecksumTypeClassic,
        .response_length = 0,
        .frame_type = LinBusSlaveResponse,
    };
    if(lin_bus_slave_mode_add_or_update_response_id(lin_bus, &frame_resp)) {
        //frame_resp.data[0]++;
    }
    if(lin_bus_slave_mode_add_or_update_response_id(lin_bus, &frame_resp1)) {
        //frame_resp.data[0]++;
    }
    if(lin_bus_slave_mode_add_or_update_response_id(lin_bus, &frame_resp2)) {
        //frame_resp.data[0]++;
    }
    while(instance->worker_running) {
        size_t len = lin_bus_get_rx_frame_available(lin_bus);

        if(len > 0) {
            LinBusFrame frame = lin_bus_get_rx_frame_read(lin_bus);
            count_frame++;
            FURI_LOG_I(TAG, "count_frame: %d", count_frame);
            FURI_LOG_I(TAG, "len: %d", len);
            FURI_LOG_I(TAG, "id: %02X", frame.id);
            FURI_LOG_I(TAG, "length: %d", frame.length);
            FURI_LOG_RAW_I("--> ");
            for(uint8_t i = 0; i < frame.length; i++) {
                FURI_LOG_RAW_I("%02X", frame.data[i]);
            }
            FURI_LOG_RAW_I("\r\n");
            FURI_LOG_I(TAG, "crc: %02X", frame.crc);
        }
        //furi_delay_ms(250);

        // if(lin_bus_tx_async(lin_bus, &frame)) {
        //     //frame.data[0]++;
        //     furi_delay_ms(100);
        //     lin_bus_tx_async(lin_bus, &frame1);
        //     furi_delay_ms(100);
        // }

        // uint32_t events = furi_thread_flags_wait(
        //     LIN_HACKER_WORKER_ALL_EVENTS, FuriFlagWaitAny, FuriWaitForever);

        // if(events & LinChackerWorkerEvtRx) {
        //    // if(lin_hacker_spaces_rx(can_hacker2) >= LIN_HACKER_VCP_CDC_PKT_LEN)
        //     {
        //         len =
        //             furi_hal_cdc_receive(LIN_HACKER_VCP_CDC_CH, buf, LIN_HACKER_VCP_CDC_PKT_LEN);

        //         FURI_LOG_RAW_I("--> ");
        //         for(uint8_t i = 0; i < len; i++) {
        //             if(buf[i] == 0x0D)
        //                 FURI_LOG_RAW_I(" [CR]");
        //             else if(buf[i] == 0x0A)
        //                 FURI_LOG_RAW_I(" [LF]");
        //             else
        //                 FURI_LOG_RAW_I(" %c (%02X)", buf[i], buf[i]);
        //         }
        //         FURI_LOG_RAW_I("\r\n");

        //        // lin_hacker_rx(can_hacker2, buf, len);
        //     }
        //     // else {
        //     //     furi_thread_flags_set(
        //     //         furi_thread_get_id(instance->thread), LinChackerWorkerEvtRx);
        //     // }
        // }

        // if((events & LinChackerWorkerEvtTxCoplete) || (events & LinChackerWorkerEvtTx)) {
        //    // len = lin_hacker_tx(can_hacker2, buf, LIN_HACKER_VCP_CDC_PKT_LEN);

        //     if(len > 0) FURI_LOG_RAW_I("<-- ");
        //     for(uint8_t i = 0; i < len; i++) {
        //         if(buf[i] == 0x0D)
        //             FURI_LOG_RAW_I(" [CR]");
        //         else if(buf[i] == 0x0A)
        //             FURI_LOG_RAW_I(" [LF]");
        //         else if(buf[i] == 0x07)
        //             FURI_LOG_RAW_I(" [BELL]");
        //         else
        //             FURI_LOG_RAW_I(" %c (%02X)", buf[i], buf[i]);
        //     }
        //     FURI_LOG_RAW_I("\r\n");

        //     if(len > 0) furi_hal_cdc_send(LIN_HACKER_VCP_CDC_CH, buf, len);
        // }

        // if(events & LinChackerWorkerEvtStop) {
        //     break;
        // }

        // if(events & LinChackerWorkerEvtState) {
        //     if(instance->callback)
        //         instance->callback(instance->context, (bool)instance->connect_usb);
        // }
    }

    lin_bus_deinit(lin_bus);

    //furi_hal_uart_set_irq_cb(uart_ch, NULL, NULL);
    //furi_hal_console_enable();

    //furi_thread_flags_set(furi_thread_get_id(lin_hacker_proc), LinChackerWorkerEvtStop);
    //lin_hacker_exit(can_hacker2);
    furi_delay_ms(10);
    // furi_thread_join(lin_hacker_proc);
    // furi_thread_free(lin_hacker_proc);

    //lin_hacker_free(can_hacker2);

    //furi_hal_uart_init(FuriHalUartIdUSART1, 9600);
    furi_hal_console_deinit();
    furi_hal_console_init(FuriHalUartIdUSART1, CONSOLE_BAUDRATE);
    //furi_hal_console_enable();
    lin_hacker_worker_vcp_cdc_deinit();

    FURI_LOG_I(TAG, "Stop");
    return 0;
}

LinChackerWorker* lin_hacker_worker_alloc(void* context) {
    furi_assert(context);
    UNUSED(context);
    LinChackerWorker* instance = malloc(sizeof(LinChackerWorker));

    instance->thread =
        furi_thread_alloc_ex("LinChackerWorker", 2048, lin_hacker_worker_thread, instance);
    return instance;
}

void lin_hacker_worker_free(LinChackerWorker* instance) {
    furi_assert(instance);
    furi_check(!instance->worker_running);

    furi_thread_free(instance->thread);

    free(instance);
}

void lin_hacker_worker_set_callback(
    LinChackerWorker* instance,
    LinChackerWorkerCallback callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;
}

void lin_hacker_worker_start(LinChackerWorker* instance) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);

    instance->worker_running = true;

    furi_thread_start(instance->thread);
}

void lin_hacker_worker_stop(LinChackerWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_running);

    instance->worker_running = false;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), LinChackerWorkerEvtStop);

    furi_thread_join(instance->thread);
}

bool lin_hacker_worker_is_running(LinChackerWorker* instance) {
    furi_assert(instance);

    return instance->worker_running;
}
