#include "can_hacker2_worker.h"

#include <furi.h>

#include "../lib/driver/can_hacker2/can_hacker2.h"

#define TAG "CanChacker2Worker"

typedef enum {
    CanChacker2WorkerEvtStop = (1 << 0),

    CanChacker2WorkerEvtRx = (1 << 1),
    CanChacker2WorkerEvtTxCoplete = (1 << 2),
    CanChacker2WorkerEvtTx = (1 << 3),
    CanChacker2WorkerEvtState = (1 << 4),

    //CanChacker2WorkerEvtCfg = (1 << 5),

} CanChacker2WorkerEvt;

struct CanChacker2Worker {
    FuriThread* thread;
    volatile bool worker_running;
    uint8_t connect_usb;

    CanChacker2WorkerCallback callback;
    void* context;
};

#define CAN_HACKER2_WORKER_PROG_ALL_EVENTS (CanChacker2WorkerEvtStop)
#define CAN_HACKER2_WORKER_ALL_EVENTS                                                  \
    (CanChacker2WorkerEvtTx | CanChacker2WorkerEvtTxCoplete | CanChacker2WorkerEvtRx | \
     CanChacker2WorkerEvtStop | CanChacker2WorkerEvtState)

//########################/* VCP CDC */#############################################
#include "usb_cdc.h"
#include <cli/cli_vcp.h>
#include <cli/cli.h>
#include <furi_hal_usb_cdc.h>

#define CAN_HACKER2_VCP_CDC_CH 1
#define CAN_HACKER2_VCP_CDC_PKT_LEN CDC_DATA_SZ
#define CAN_HACKER2_VCP_UART_RX_BUF_SIZE (CAN_HACKER2_VCP_CDC_PKT_LEN * 5)

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
    CanChacker2Worker* instance = context;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), CanChacker2WorkerEvtTxCoplete);
}

static void vcp_on_cdc_rx(void* context) {
    furi_assert(context);
    CanChacker2Worker* instance = context;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), CanChacker2WorkerEvtRx);
}

static void vcp_state_callback(void* context, uint8_t state) {
    UNUSED(context);

    CanChacker2Worker* instance = context;
    instance->connect_usb = state;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), CanChacker2WorkerEvtState);
}

static void vcp_on_cdc_control_line(void* context, uint8_t state) {
    UNUSED(context);
    UNUSED(state);
}

static void vcp_on_line_config(void* context, struct usb_cdc_line_coding* config) {
    UNUSED(context);
    UNUSED(config);
}

static void can_hacker2_worker_vcp_cdc_init(void* context) {
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

    furi_hal_cdc_set_callbacks(CAN_HACKER2_VCP_CDC_CH, (CdcCallbacks*)&cdc_cb, context);
}

static void can_hacker2_worker_vcp_cdc_deinit(void) {
    //disable callbacks CAN_HACKER2_VCP_CDC_CH
    furi_hal_cdc_set_callbacks(CAN_HACKER2_VCP_CDC_CH, NULL, NULL);

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

static int32_t can_hacker2_worker_proc_thread(void* context) {
    CanHacker2* can_hacker2 = context;

    FURI_LOG_I(TAG, "CanHacker2ProcessWorker Start");
    while(1) {
        if(furi_thread_flags_get() & CanChacker2WorkerEvtStop) break;
        can_hacker2_process(can_hacker2);
    }
    FURI_LOG_I(TAG, "CanHacker2ProcessWorker Stop");
    return 0;
}

static void can_hacker2_worker_proc_tx_data(void* context) {
    furi_assert(context);
    CanChacker2Worker* instance = context;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), CanChacker2WorkerEvtTx);
}

static void can_hacker2_worker_proc_connect(void* context, CanHacker2Status status) {
    furi_assert(context);
    CanChacker2Worker* instance = context;
    switch(status) {
    case CanHacker2Disconnected:
        if(instance->callback)
            instance->callback(instance->context, CanChacker2WorkerStatusProcDisconnected);
        break;
    case CanHacker2Connected:
        if(instance->callback)
            instance->callback(instance->context, CanChacker2WorkerStatusProcConnected);
        break;

    default:
        furi_crash("Unknown status");
        break;
    }
}

/** Worker thread
 * 
 * @param context 
 * @return exit code 
 */
static int32_t can_hacker2_worker_thread(void* context) {
    CanChacker2Worker* instance = context;
    can_hacker2_worker_vcp_cdc_init(instance);

    CanHacker2* can_hacker2 = can_hacker2_alloc();
    can_hacker2_set_callback(
        can_hacker2, can_hacker2_worker_proc_tx_data, can_hacker2_worker_proc_connect, instance);

    uint8_t buf[CAN_HACKER2_VCP_UART_RX_BUF_SIZE];
    size_t len = 0;

    FuriThread* can_hacker2_proc =
        furi_thread_alloc_ex("CanHacker2Proc", 1024, can_hacker2_worker_proc_thread, can_hacker2);
    furi_thread_start(can_hacker2_proc);

    FURI_LOG_I(TAG, "Start");

    //  Check device connected
    while(instance->worker_running) {
        uint32_t events =
            furi_thread_flags_wait(CAN_HACKER2_WORKER_ALL_EVENTS, FuriFlagWaitAny, 250);
        if(events & CanChacker2WorkerEvtStop) {
            furi_thread_flags_set(furi_thread_get_id(instance->thread), CanChacker2WorkerEvtStop);
            break;
        }
        if(can_hacker2_is_device_connected(can_hacker2)) {
            if(instance->callback)
                instance->callback(instance->context, CanChacker2WorkerStatusProcDisconnected);
            break;
        }
    }

    while(instance->worker_running) {
        uint32_t events = furi_thread_flags_wait(
            CAN_HACKER2_WORKER_ALL_EVENTS, FuriFlagWaitAny, FuriWaitForever);

        if(events & CanChacker2WorkerEvtRx) {
            if(can_hacker2_spaces_rx(can_hacker2) >= CAN_HACKER2_VCP_CDC_PKT_LEN) {
                len =
                    furi_hal_cdc_receive(CAN_HACKER2_VCP_CDC_CH, buf, CAN_HACKER2_VCP_CDC_PKT_LEN);

                FURI_LOG_RAW_I("--> ");
                for(uint8_t i = 0; i < len; i++) {
                    if(buf[i] == 0x0D)
                        FURI_LOG_RAW_I(" [CR]");
                    else if(buf[i] == 0x0A)
                        FURI_LOG_RAW_I(" [LF]");
                    else
                        FURI_LOG_RAW_I(" %c (%02X)", buf[i], buf[i]);
                }
                FURI_LOG_RAW_I("\r\n");

                can_hacker2_rx(can_hacker2, buf, len);
            } else {
                furi_thread_flags_set(
                    furi_thread_get_id(instance->thread), CanChacker2WorkerEvtRx);
            }
        }

        if((events & CanChacker2WorkerEvtTxCoplete) || (events & CanChacker2WorkerEvtTx)) {
            len = can_hacker2_tx(can_hacker2, buf, CAN_HACKER2_VCP_CDC_PKT_LEN);

            if(len > 0) FURI_LOG_RAW_I("<-- ");
            for(uint8_t i = 0; i < len; i++) {
                if(buf[i] == 0x0D)
                    FURI_LOG_RAW_I(" [CR]");
                else if(buf[i] == 0x0A)
                    FURI_LOG_RAW_I(" [LF]");
                else if(buf[i] == 0x07)
                    FURI_LOG_RAW_I(" [BELL]");
                else
                    FURI_LOG_RAW_I(" %c (%02X)", buf[i], buf[i]);
            }
            FURI_LOG_RAW_I("\r\n");

            if(len > 0) furi_hal_cdc_send(CAN_HACKER2_VCP_CDC_CH, buf, len);
        }

        if(events & CanChacker2WorkerEvtStop) {
            break;
        }

        // USB connect event, diconnect CAN
        if(events & CanChacker2WorkerEvtState) {
            if(!instance->connect_usb) {
                buf[0] = 'C';
                buf[1] = '\r';
                can_hacker2_rx(can_hacker2, buf, 2);
            }
        }
    }

    FURI_LOG_I(TAG, "Stop");

    furi_thread_flags_set(furi_thread_get_id(can_hacker2_proc), CanChacker2WorkerEvtStop);
    can_hacker2_exit(can_hacker2);
    furi_delay_ms(10);
    furi_thread_join(can_hacker2_proc);
    furi_thread_free(can_hacker2_proc);

    can_hacker2_free(can_hacker2);
    can_hacker2_worker_vcp_cdc_deinit();
    return 0;
}

CanChacker2Worker* can_hacker2_worker_alloc(void* context) {
    furi_assert(context);
    UNUSED(context);
    CanChacker2Worker* instance = malloc(sizeof(CanChacker2Worker));

    instance->thread =
        furi_thread_alloc_ex("CanChacker2Worker", 2048, can_hacker2_worker_thread, instance);
    return instance;
}

void can_hacker2_worker_free(CanChacker2Worker* instance) {
    furi_assert(instance);
    furi_check(!instance->worker_running);

    furi_thread_free(instance->thread);

    free(instance);
}

void can_hacker2_worker_set_callback(
    CanChacker2Worker* instance,
    CanChacker2WorkerCallback callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;
}

void can_hacker2_worker_start(CanChacker2Worker* instance) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);

    instance->worker_running = true;

    furi_thread_start(instance->thread);
}

void can_hacker2_worker_stop(CanChacker2Worker* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_running);

    instance->worker_running = false;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), CanChacker2WorkerEvtStop);

    furi_thread_join(instance->thread);
}

bool can_hacker2_worker_is_running(CanChacker2Worker* instance) {
    furi_assert(instance);

    return instance->worker_running;
}
