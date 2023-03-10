#include "avr_isp_worker.h"
#include "avr_isp_prog.h"
#include "avr_isp_spi_sw.h"
#include "avr_isp_chip_arr.h"

#include <furi.h>
#include "usb_cdc.h"
#include <cli/cli_vcp.h>
#include <cli/cli.h>
#include <furi_hal_usb_cdc.h>

#define TAG "AvrIspWorker"
#define AVR_ISP_VCP_CDC_CH 1
#define AVR_ISP_VCP_CDC_PKT_LEN CDC_DATA_SZ
#define AVR_ISP_VCP_UART_RX_BUF_SIZE (AVR_ISP_VCP_CDC_PKT_LEN * 5)

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),

    WorkerEvtTxStop = (1 << 2),
    WorkerEvtCdcRx = (1 << 3),

} WorkerEvtFlags;

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)
#define WORKER_ALL_TX_EVENTS (WorkerEvtTxStop | WorkerEvtCdcRx)

typedef enum {
    AvrIspWorkerEvtStop = (1 << 0),
} AvrIspWorkerEvt;

#define AVR_ISP_WORKER_PROG_ALL_EVENTS (AvrIspWorkerEvtStop)

struct AvrIspWorker {
    //AvrIspProg* prog;
    FuriThread* thread;

    AvrIspWorkerUsbConfig cfg;
    FuriSemaphore* tx_sem;

    volatile bool worker_running;
    // AvrIspWorkerCallback callback;
    // void* context;
};

// static void vcp_on_cdc_tx_complete(void* context);
// static void vcp_on_cdc_rx(void* context);
// static void vcp_state_callback(void* context, uint8_t state);
// static void vcp_on_cdc_control_line(void* context, uint8_t state);
//static void vcp_on_line_config(void* context, struct usb_cdc_line_coding* config);

// static const CdcCallbacks cdc_cb = {
//     vcp_on_cdc_tx_complete,
//     vcp_on_cdc_rx,
//     vcp_state_callback,
//     vcp_on_cdc_control_line,
//     vcp_on_line_config,
// };

// /* VCP callbacks */

// static void vcp_on_cdc_tx_complete(void* context) {
//     AvrIspWorker* instance = context;
//     furi_semaphore_release(instance->tx_sem);
// }

// static void vcp_on_cdc_rx(void* context) {
//     AvrIspWorker* instance = context;
//     UNUSED (instance);
//     //furi_thread_flags_set(furi_thread_get_id(instance->tx_thread), WorkerEvtCdcRx);
// }

// static void vcp_state_callback(void* context, uint8_t state) {
//     UNUSED(context);
//     UNUSED(state);
// }

// static void vcp_on_cdc_control_line(void* context, uint8_t state) {
//     UNUSED(state);
//     AvrIspWorker* instance = context;
//     UNUSED (instance);
//     //furi_thread_flags_set(furi_thread_get_id(instance->thread), WorkerEvtCtrlLineSet);
// }

// static void vcp_on_line_config(void* context, struct usb_cdc_line_coding* config) {
//     UNUSED(config);
//     AvrIspWorker* instance = context;
//     UNUSED (instance);
//     //furi_thread_flags_set(furi_thread_get_id(instance->thread), WorkerEvtLineCfgSet);
// }

//
static void avr_isp_worker_vcp_cdc_init(AvrIspWorker* instance) {
    UNUSED(instance);
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

    //furi_hal_cdc_set_callbacks(AVR_ISP_VCP_CDC_CH, (CdcCallbacks*)&cdc_cb, instance);
}

static void avr_isp_worker_vcp_cdc_deinit(AvrIspWorker* instance) {
    UNUSED(instance);
    //disable callbacks AVR_ISP_VCP_CDC_CH
    furi_hal_cdc_set_callbacks(AVR_ISP_VCP_CDC_CH, NULL, NULL);

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

static int32_t avr_isp_worker_prog_thread(void* context) {
    AvrIspProg* prog = context;
    uint32_t events = 0;
    FURI_LOG_I(TAG, "AvrIspProgWorker Start");
    while(1) {
        events = furi_thread_flags_get();
        if(events & AvrIspWorkerEvtStop) break;
        avr_isp_prog_avrisp(prog);
    }
    FURI_LOG_I(TAG, "AvrIspProgWorker Stop");
    return 0;
}

/** Worker thread
 * 
 * @param context 
 * @return exit code 
 */
static int32_t avr_isp_worker_thread(void* context) {
    AvrIspWorker* instance = context;
    avr_isp_worker_vcp_cdc_init(instance);

    AvrIspProg* prog = avr_isp_prog_init(AvrIspSpiSwSpeed400Khz);

    UNUSED(instance);
    uint8_t buf[AVR_ISP_VCP_UART_RX_BUF_SIZE];

    // for(size_t i = 0; i < 372; i++) {
    //     if(avr_isp_chip_arr[i].sigs[1] == 0x97) {
    //         if(avr_isp_chip_arr[i].sigs[2] == 0x0B) {
    //             FURI_LOG_I(TAG, "name %s", avr_isp_chip_arr[i].name);
    //             break;
    //         }
    //     }
    // }

    FuriThread* prog_thread =
        furi_thread_alloc_ex("AvrIspProgWorker", 1024, avr_isp_worker_prog_thread, prog);
    furi_thread_start(prog_thread);

    FURI_LOG_I(TAG, "AvrIspWorker Start");
    while(instance->worker_running) {
        furi_delay_ms(1);

        size_t len = furi_hal_cdc_receive(AVR_ISP_VCP_CDC_CH, buf, AVR_ISP_VCP_CDC_PKT_LEN);
        while(len > 0) {
            avr_isp_prog_rx(prog, buf, len);

            for(uint8_t i = 0; i < len; i++) {
                FURI_LOG_I(TAG, "--> %X", buf[i]);
            }

            len = furi_hal_cdc_receive(AVR_ISP_VCP_CDC_CH, buf, AVR_ISP_VCP_CDC_PKT_LEN);
        }

        len = avr_isp_prog_tx(prog, buf, AVR_ISP_VCP_CDC_PKT_LEN);
        while(len > 0) {
            for(uint8_t i = 0; i < len; i++) {
                FURI_LOG_I(TAG, "<-- %X", buf[i]);
            }

            furi_hal_cdc_send(AVR_ISP_VCP_CDC_CH, buf, len);
            furi_delay_ms(1);
            len = avr_isp_prog_tx(prog, buf, AVR_ISP_VCP_CDC_PKT_LEN);
        }
    }
    FURI_LOG_I(TAG, "AvrIspWorker Stop");
    furi_thread_flags_set(furi_thread_get_id(prog_thread), AvrIspWorkerEvtStop);
    avr_isp_prog_exit(prog);
    furi_delay_ms(10);
    furi_thread_join(prog_thread);
    furi_thread_free(prog_thread);

    avr_isp_prog_free(prog);
    avr_isp_worker_vcp_cdc_deinit(instance);
    return 0;
}

AvrIspWorker* avr_isp_worker_alloc(void* context) {
    furi_assert(context);
    UNUSED(context);
    AvrIspWorker* instance = malloc(sizeof(AvrIspWorker));

    instance->thread = furi_thread_alloc_ex("AvrIspWorker", 2048, avr_isp_worker_thread, instance);
    // SubGhz* subghz = context;
    // instance->setting = subghz->setting;
    return instance;
}

void avr_isp_worker_free(AvrIspWorker* instance) {
    furi_assert(instance);

    furi_thread_free(instance->thread);
    free(instance);
}

// void avr_isp_worker_set_callback(
//     AvrIspWorker* instance,
//     AvrIspWorkerCallback callback,
//     void* context) {
//     furi_assert(instance);
//     furi_assert(context);
//     instance->callback = callback;
//     instance->context = context;
// }

void avr_isp_worker_start(AvrIspWorker* instance) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);

    instance->worker_running = true;

    furi_thread_start(instance->thread);
}

void avr_isp_worker_stop(AvrIspWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_running);

    instance->worker_running = false;

    furi_thread_join(instance->thread);
}

bool avr_isp_worker_is_running(AvrIspWorker* instance) {
    furi_assert(instance);
    return instance->worker_running;
}
