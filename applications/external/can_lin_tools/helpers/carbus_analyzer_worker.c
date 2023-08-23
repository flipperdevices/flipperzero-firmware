#include "carbus_analyzer_worker.h"

#include <furi.h>

#include "../lib/driver/carbus_analyzer/carbus_analyzer.h"

#define TAG "CarBusAnalyzerWorker"

typedef enum {
    CarBusAnalyzerWorkerEvtStop = (1 << 0),

    CarBusAnalyzerWorkerEvtRx = (1 << 1),
    CarBusAnalyzerWorkerEvtTxCoplete = (1 << 2),
    CarBusAnalyzerWorkerEvtTx = (1 << 3),
    CarBusAnalyzerWorkerEvtState = (1 << 4),

    //CarBusAnalyzerWorkerEvtCfg = (1 << 5),

} CarBusAnalyzerWorkerEvt;

struct CarBusAnalyzerWorker {
    FuriThread* thread;
    volatile bool worker_running;
    uint8_t connect_usb;

    CarBusAnalyzerWorkerCallback callback;
    void* context;
};

#define CARBUS_ANALYZER_WORKER_PROG_ALL_EVENTS (CarBusAnalyzerWorkerEvtStop)
#define CARBUS_ANALYZER_WORKER_ALL_EVENTS                                                       \
    (CarBusAnalyzerWorkerEvtTx | CarBusAnalyzerWorkerEvtTxCoplete | CarBusAnalyzerWorkerEvtRx | \
     CarBusAnalyzerWorkerEvtStop | CarBusAnalyzerWorkerEvtState)

//########################/* VCP CDC */#############################################
#include "usb_cdc.h"
#include <cli/cli_vcp.h>
#include <cli/cli.h>
#include <furi_hal_usb_cdc.h>

#define CARBUS_ANALYZER_VCP_CDC_CH 1
#define CARBUS_ANALYZER_VCP_CDC_PKT_LEN CDC_DATA_SZ
#define CARBUS_ANALYZER_VCP_UART_RX_BUF_SIZE (CARBUS_ANALYZER_VCP_CDC_PKT_LEN * 5)

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
    CarBusAnalyzerWorker* instance = context;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), CarBusAnalyzerWorkerEvtTxCoplete);
}

static void vcp_on_cdc_rx(void* context) {
    furi_assert(context);
    CarBusAnalyzerWorker* instance = context;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), CarBusAnalyzerWorkerEvtRx);
}

static void vcp_state_callback(void* context, uint8_t state) {
    UNUSED(context);

    CarBusAnalyzerWorker* instance = context;
    instance->connect_usb = state;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), CarBusAnalyzerWorkerEvtState);
}

static void vcp_on_cdc_control_line(void* context, uint8_t state) {
    UNUSED(context);
    UNUSED(state);
}

static void vcp_on_line_config(void* context, struct usb_cdc_line_coding* config) {
    UNUSED(context);
    UNUSED(config);
}

static void carbus_analyzer_worker_vcp_cdc_init(void* context) {
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

    furi_hal_cdc_set_callbacks(CARBUS_ANALYZER_VCP_CDC_CH, (CdcCallbacks*)&cdc_cb, context);
}

static void carbus_analyzer_worker_vcp_cdc_deinit(void) {
    //disable callbacks CARBUS_ANALYZER_VCP_CDC_CH
    furi_hal_cdc_set_callbacks(CARBUS_ANALYZER_VCP_CDC_CH, NULL, NULL);

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

static int32_t carbus_analyzer_worker_proc_thread(void* context) {
    CarBusAnalyzer* carbus_analyzer = context;

    FURI_LOG_I(TAG, "CarBusAnalyzerProcessWorker Start");
    while(1) {
        if(furi_thread_flags_get() & CarBusAnalyzerWorkerEvtStop) break;
        carbus_analyzer_process(carbus_analyzer);
    }
    FURI_LOG_I(TAG, "CarBusAnalyzerProcessWorker Stop");
    return 0;
}

static void carbus_analyzer_worker_proc_tx_data(void* context) {
    furi_assert(context);
    CarBusAnalyzerWorker* instance = context;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), CarBusAnalyzerWorkerEvtTx);
}

static void carbus_analyzer_worker_proc_connect(void* context, CarBusAnalyzerStatus status) {
    furi_assert(context);
    CarBusAnalyzerWorker* instance = context;
    switch(status) {
    case CarBusAnalyzerDisconnected:
        if(instance->callback)
            instance->callback(instance->context, CarBusAnalyzerWorkerStatusProcDisconnected);
        break;
    case CarBusAnalyzerConnected:
        if(instance->callback)
            instance->callback(instance->context, CarBusAnalyzerWorkerStatusProcConnected);
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
static int32_t carbus_analyzer_worker_thread(void* context) {
    CarBusAnalyzerWorker* instance = context;
    carbus_analyzer_worker_vcp_cdc_init(instance);

    CarBusAnalyzer* carbus_analyzer = carbus_analyzer_alloc();
    carbus_analyzer_set_callback(
        carbus_analyzer,
        carbus_analyzer_worker_proc_tx_data,
        carbus_analyzer_worker_proc_connect,
        instance);

    uint8_t buf[CARBUS_ANALYZER_VCP_UART_RX_BUF_SIZE];
    size_t len = 0;

    FuriThread* carbus_analyzer_proc = furi_thread_alloc_ex(
        "CarBusAnalyzerProc", 1024, carbus_analyzer_worker_proc_thread, carbus_analyzer);
    furi_thread_start(carbus_analyzer_proc);

    FURI_LOG_I(TAG, "Start");

    //  Check device connected
    while(instance->worker_running) {
        uint32_t events =
            furi_thread_flags_wait(CARBUS_ANALYZER_WORKER_ALL_EVENTS, FuriFlagWaitAny, 250);
        if(events & CarBusAnalyzerWorkerEvtStop) {
            furi_thread_flags_set(
                furi_thread_get_id(instance->thread), CarBusAnalyzerWorkerEvtStop);
            break;
        }
        if(carbus_analyzer_is_device_connected(carbus_analyzer)) {
            if(instance->callback)
                instance->callback(instance->context, CarBusAnalyzerWorkerStatusProcDisconnected);
            break;
        }
    }

    while(instance->worker_running) {
        uint32_t events = furi_thread_flags_wait(
            CARBUS_ANALYZER_WORKER_ALL_EVENTS, FuriFlagWaitAny, FuriWaitForever);

        if(events & CarBusAnalyzerWorkerEvtRx) {
            if(carbus_analyzer_spaces_rx(carbus_analyzer) >= CARBUS_ANALYZER_VCP_CDC_PKT_LEN) {
                len = furi_hal_cdc_receive(
                    CARBUS_ANALYZER_VCP_CDC_CH, buf, CARBUS_ANALYZER_VCP_CDC_PKT_LEN);

                FURI_LOG_RAW_D("--> ");
                for(uint8_t i = 0; i < len; i++) {
                    FURI_LOG_RAW_D("%02X ", buf[i]);
                }
                FURI_LOG_RAW_D("\t\t");

                for(uint8_t i = 0; i < len; i++) {
                    if(buf[i] == 0x0D)
                        FURI_LOG_RAW_D(" [CR]");
                    else if(buf[i] == 0x0A)
                        FURI_LOG_RAW_D(" [LF]");
                    else if(buf[i] == 0x07)
                        FURI_LOG_RAW_D(" [BELL]");
                    else if(buf[i] == 0x00)
                        FURI_LOG_RAW_D(" [00]");
                    else
                        FURI_LOG_RAW_D(" %c", buf[i]);
                }
                FURI_LOG_RAW_D("\r\n");

                carbus_analyzer_rx(carbus_analyzer, buf, len);
            } else {
                furi_thread_flags_set(
                    furi_thread_get_id(instance->thread), CarBusAnalyzerWorkerEvtRx);
            }
        }

        if((events & CarBusAnalyzerWorkerEvtTxCoplete) || (events & CarBusAnalyzerWorkerEvtTx)) {
            len = carbus_analyzer_tx(carbus_analyzer, buf, CARBUS_ANALYZER_VCP_CDC_PKT_LEN);

            if(len > 0) FURI_LOG_RAW_D("<-- ");

            for(uint8_t i = 0; i < len; i++) {
                FURI_LOG_RAW_D("%02X ", buf[i]);
            }
            FURI_LOG_RAW_D("\t\t");

            for(uint8_t i = 0; i < len; i++) {
                if(buf[i] == 0x0D)
                    FURI_LOG_RAW_D(" [CR]");
                else if(buf[i] == 0x0A)
                    FURI_LOG_RAW_D(" [LF]");
                else if(buf[i] == 0x07)
                    FURI_LOG_RAW_D(" [BELL]");
                else if(buf[i] == 0x00)
                    FURI_LOG_RAW_D(" [00]");
                else
                    FURI_LOG_RAW_D(" %c", buf[i]);
            }
            FURI_LOG_RAW_D("\r\n");

            if(len > 0) furi_hal_cdc_send(CARBUS_ANALYZER_VCP_CDC_CH, buf, len);
        }

        if(events & CarBusAnalyzerWorkerEvtStop) {
            break;
        }

        // USB connect event, diconnect CAN
        if(events & CarBusAnalyzerWorkerEvtState) {
            if(!instance->connect_usb) {
                buf[0] = 'C';
                buf[1] = '\r';
                carbus_analyzer_rx(carbus_analyzer, buf, 2);
            }
        }
    }

    FURI_LOG_I(TAG, "Stop");

    furi_thread_flags_set(furi_thread_get_id(carbus_analyzer_proc), CarBusAnalyzerWorkerEvtStop);
    carbus_analyzer_exit(carbus_analyzer);
    furi_delay_ms(10);
    furi_thread_join(carbus_analyzer_proc);
    furi_thread_free(carbus_analyzer_proc);

    carbus_analyzer_free(carbus_analyzer);
    carbus_analyzer_worker_vcp_cdc_deinit();
    return 0;
}

CarBusAnalyzerWorker* carbus_analyzer_worker_alloc(void* context) {
    furi_assert(context);
    UNUSED(context);
    CarBusAnalyzerWorker* instance = malloc(sizeof(CarBusAnalyzerWorker));

    instance->thread = furi_thread_alloc_ex(
        "CarBusAnalyzerWorker", 2048, carbus_analyzer_worker_thread, instance);
    return instance;
}

void carbus_analyzer_worker_free(CarBusAnalyzerWorker* instance) {
    furi_assert(instance);
    furi_check(!instance->worker_running);

    furi_thread_free(instance->thread);

    free(instance);
}

void carbus_analyzer_worker_set_callback(
    CarBusAnalyzerWorker* instance,
    CarBusAnalyzerWorkerCallback callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;
}

void carbus_analyzer_worker_start(CarBusAnalyzerWorker* instance) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);

    instance->worker_running = true;

    furi_thread_start(instance->thread);
}

void carbus_analyzer_worker_stop(CarBusAnalyzerWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_running);

    instance->worker_running = false;
    furi_thread_flags_set(furi_thread_get_id(instance->thread), CarBusAnalyzerWorkerEvtStop);

    furi_thread_join(instance->thread);
}

bool carbus_analyzer_worker_is_running(CarBusAnalyzerWorker* instance) {
    furi_assert(instance);

    return instance->worker_running;
}
