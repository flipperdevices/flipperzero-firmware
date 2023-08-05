#include "can_hacker2_worker.h"

#include <furi.h>

#include "../lib/driver/mcp251xfd_interconnect/CANEXTFunctions.h"
#include "../lib/driver/mcp251xfd_interconnect/ShowDemoData.h"

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

// static int32_t can_hacker2_worker_prog_thread(void* context) {
//     AvrIspProg* prog = context;
//     FURI_LOG_D(TAG, "AvrIspProgWorker Start");
//     while(1) {
//         if(furi_thread_flags_get() & CanChacker2WorkerEvtStop) break;
//         can_hacker2_prog_avrisp(prog);
//     }
//     FURI_LOG_D(TAG, "AvrIspProgWorker Stop");
//     return 0;
// }

// static void can_hacker2_worker_prog_tx_data(void* context) {
//     furi_assert(context);
//     CanChacker2Worker* instance = context;
//     furi_thread_flags_set(furi_thread_get_id(instance->thread), CanChacker2WorkerEvtTx);
// }

/** Worker thread
 * 
 * @param context 
 * @return exit code 
 */
static int32_t can_hacker2_worker_thread(void* context) {
    CanChacker2Worker* instance = context;
    can_hacker2_worker_vcp_cdc_init(instance);

    //AvrIspProg* prog = can_hacker2_prog_init();
    //can_hacker2_prog_set_tx_callback(prog, can_hacker2_worker_prog_tx_data, instance);

    // uint8_t buf[CAN_HACKER2_VCP_UART_RX_BUF_SIZE];
    // size_t len = 0;

    // FuriThread* prog_thread =
    //     furi_thread_alloc_ex("AvrIspProgWorker", 1024, can_hacker2_worker_prog_thread, prog);
    // furi_thread_start(prog_thread);
    //bool Ext2ModulePresent = false; //! True if a MCP251XFD is present on EXT2 and configured
    uint32_t Ext2SequenceCounter = 0; //! Ext2 sequence counter
    uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};
    UNUSED(data);
    FURI_LOG_D(TAG, "Start");

    uint32_t MessageTimeStamp = 0;
    uint8_t
        RxPayloadData[8]; // In the default demo configuration, all the FIFO have 8 bytes of payload
    MCP251XFD_CANMessage ReceivedMessage;
    ReceivedMessage.PayloadData = &RxPayloadData[0];
    bool receive_event = false;

    //--- Configure the MCP251XFD device ----------

    volatile eERRORRESULT err = ConfigureMCP251XFDDeviceOnEXT2();
    UNUSED(err);

    if(err != ERR_OK) {
        ShowDeviceError(CANEXT2, err); // Show device error
        //Ext1ModulePresent = false;
    } else {
        ShowDeviceDetected(CANEXT2, SYSCLK_Ext2);
        ShowDeviceConfiguration(&MCP2517FD_Ext2_BTStats);
        ShowDeviceFIFOConfiguration(&MCP2517FD_Ext2_FIFOlist[0], MCP2517FD_EXT2_FIFO_COUNT);
        ShowDeviceFilterConfiguration(
            &MCP2517FD_Ext2_FilterList[0], MCP2517FD_EXT2_FILTER_COUNT, false);
        ShowMoreDeviceConfiguration(CANEXT2);
        GetAndShowMCP251XFD_SFRreg(CANEXT2);
        GetAndShowMCP251XFD_CANSFRreg(CANEXT2);
        GetAndShowMCP251XFD_FIFOreg(CANEXT2);
        GetAndShowMCP251XFD_FILTERreg(CANEXT2);
        //Ext2ModulePresent = true;
    }
    furi_delay_ms(100);
    while(instance->worker_running) {
        CheckDeviceINTOnEXT2();
        // furi_delay_ms(100);
        //FURI_LOG_I(TAG, "**** %ld", Ext2SequenceCounter);

        err = TransmitMessageToEXT2(
            0x201,
            &Ext2SequenceCounter,
            MCP251XFD_STANDARD_MESSAGE_ID,
            MCP251XFD_DLC_4BYTE,
            &data[0],
            MCP251XFD_TXQ,
            true);
        err = TransmitMessageToEXT2(
            0x301,
            &Ext2SequenceCounter,
            MCP251XFD_STANDARD_MESSAGE_ID,
            MCP251XFD_DLC_4BYTE,
            &data[0],
            MCP251XFD_TXQ,
            true);
        //TransmitMessageToEXT2(EID_0x10000_to_0x17FFF_MPS_EVENT, &Ext2SequenceCounter, MCP251XFD_EXTENDED_MESSAGE_ID, MCP251XFD_DLC_8BYTE, &data[0], MCP251XFD_FIFO19, true);
        //TransmitMessageToEXT2(SID_EXT1_CLKO_EVENT, &Ext2SequenceCounter, MCP251XFD_STANDARD_MESSAGE_ID, MCP251XFD_DLC_4BYTE, &data[0], MCP251XFD_FIFO26, true);
        //TransmitMessageToEXT2(SID_EXT2_CLKO_EVENT, &Ext2SequenceCounter, MCP251XFD_STANDARD_MESSAGE_ID, MCP251XFD_DLC_4BYTE, &data[0], MCP251XFD_FIFO27, true);

        //--- Get a frame if available in the MCP251XFD on Ext2 ---

        err = ReceiveMessageFromEXT2(
            CANEXT2, &receive_event, &ReceivedMessage, MCP251XFD_PAYLOAD_8BYTE, &MessageTimeStamp);

        while(receive_event) {
            receive_event = false;
            FURI_LOG_I(
                TAG,
                "MTS: %ld, ID:%04lX, SEQ: %ld, DLC: %d",
                MessageTimeStamp,
                ReceivedMessage.MessageID,
                ReceivedMessage.MessageSEQ,
                ReceivedMessage.DLC);
            for(size_t i = 0; i < ReceivedMessage.DLC; i++) {
                FURI_LOG_RAW_I(" %02X", ReceivedMessage.PayloadData[i]);
            }
            FURI_LOG_RAW_I("\r\n");
            FURI_LOG_I(TAG, "**** %ld", Ext2SequenceCounter);

            err = ReceiveMessageFromEXT2(
                CANEXT2,
                &receive_event,
                &ReceivedMessage,
                MCP251XFD_PAYLOAD_8BYTE,
                &MessageTimeStamp);

            // err = TransmitMessageToEXT2(
            // 0x101,
            // &Ext2SequenceCounter,
            // MCP251XFD_STANDARD_MESSAGE_ID,
            // MCP251XFD_DLC_4BYTE,
            // &data[0],
            // MCP251XFD_FIFO5,
            // true);
        }
        err = TransmitMessageToEXT2(
            0x101,
            &Ext2SequenceCounter,
            MCP251XFD_STANDARD_MESSAGE_ID,
            MCP251XFD_DLC_4BYTE,
            &data[0],
            MCP251XFD_FIFO5,
            true);
        //TransmitMessageFromEXT2_No_IRQ(0x101, &Ext2SequenceCounter, MCP251XFD_STANDARD_MESSAGE_ID, MCP251XFD_DLC_4BYTE, &data[0], true);
        //furi_delay_ms(50);
        //--- Check status of the MCP251XFD on Ext2 ---

        err = ERR_OK;

        // uint32_t events =
        //     furi_thread_flags_wait(CAN_HACKER2_WORKER_ALL_EVENTS, FuriFlagWaitAny, FuriWaitForever);

        // if(events & CanChacker2WorkerEvtRx) {
        //     if(can_hacker2_prog_spaces_rx(prog) >= CAN_HACKER2_VCP_CDC_PKT_LEN) {
        //         len = furi_hal_cdc_receive(CAN_HACKER2_VCP_CDC_CH, buf, CAN_HACKER2_VCP_CDC_PKT_LEN);
        //         // for(uint8_t i = 0; i < len; i++) {
        //         //     FURI_LOG_I(TAG, "--> %X", buf[i]);
        //         // }
        //         can_hacker2_prog_rx(prog, buf, len);
        //     } else {
        //         furi_thread_flags_set(
        //             furi_thread_get_id(instance->thread), CanChacker2WorkerEvtRx);
        //     }
        // }

        // if((events & CanChacker2WorkerEvtTxCoplete) || (events & CanChacker2WorkerEvtTx)) {
        //     len = can_hacker2_prog_tx(prog, buf, CAN_HACKER2_VCP_CDC_PKT_LEN);

        //     // for(uint8_t i = 0; i < len; i++) {
        //     //     FURI_LOG_I(TAG, "<-- %X", buf[i]);
        //     // }

        //     if(len > 0) furi_hal_cdc_send(CAN_HACKER2_VCP_CDC_CH, buf, len);
        // }

        // if(events & CanChacker2WorkerEvtStop) {
        //     break;
        // }

        // if(events & CanChacker2WorkerEvtState) {
        //     if(instance->callback)
        //         instance->callback(instance->context, (bool)instance->connect_usb);
        // }
    }

    FURI_LOG_D(TAG, "Stop");

    // furi_thread_flags_set(furi_thread_get_id(prog_thread), CanChacker2WorkerEvtStop);
    // can_hacker2_prog_exit(prog);
    // furi_delay_ms(10);
    // furi_thread_join(prog_thread);
    // furi_thread_free(prog_thread);

    //can_hacker2_prog_free(prog);

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
