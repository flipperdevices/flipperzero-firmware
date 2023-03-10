#include "avr_isp_vcp_cdc.h"

#include <furi.h>

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),

    WorkerEvtTxStop = (1 << 2),
    WorkerEvtCdcRx = (1 << 3),

} WorkerEvtFlags;

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)
#define WORKER_ALL_TX_EVENTS (WorkerEvtTxStop | WorkerEvtCdcRx)

// typedef struct {
//     uint8_t vcp_ch;
//     // uint8_t uart_ch;
//     // uint8_t flow_pins;
//     // uint8_t baudrate_mode;
//     uint32_t baudrate;
// } AvrIspWorkerUsbConfig;

// struct AvrIspVcpCdc {
//     AvrIspWorkerUsbConfig cfg;
//     FuriSemaphore* tx_sem;
// };

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

void avr_isp_vcp_cdc_init(void) {
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

void avr_isp_vcp_cdc_deinit(void) {
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

inline int32_t avr_isp_vcp_cdc_receive(uint8_t if_num, uint8_t* buf, uint16_t max_len) {
    return furi_hal_cdc_receive(if_num, buf, max_len);
}

inline void avr_isp_vcp_cdc_send(uint8_t if_num, uint8_t* buf, uint16_t len) {
    furi_hal_cdc_send(if_num, buf, len);
}
