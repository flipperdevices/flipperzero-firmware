#include "airmon_pms.h"

#include <string.h>

#define TAG "AirmonPms"

#define UINT16BE(p) (((p)[0] << 8) | (p)[1])

#define PMS_BAUDRATE 9600

#define PMS_CTRL_FRAME_LEN 4
#define PMS_DATA_FRAME_LEN 28

#define PMS_START_CHAR_1 0x42
#define PMS_START_CHAR_2 0x4d

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

typedef enum {
    RxStateStartChar1,
    RxStateStartChar2,
    RxStateFrameLength,
    RxStateFrameData
} RxState;

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

static void airmon_pms_uart_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    AirmonPmsContext* pms_context = context;

    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(pms_context->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(pms_context->thread), WorkerEvtRxDone);
    }
}

static void airmon_pms_serial_init(AirmonPmsContext* pms_context) {
    furi_hal_console_disable();
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, airmon_pms_uart_on_irq_cb, pms_context);
    furi_hal_uart_set_br(FuriHalUartIdUSART1, PMS_BAUDRATE);
}

static void airmon_pms_serial_deinit(AirmonPmsContext* pms_context) {
    UNUSED(pms_context);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, NULL, NULL);
    furi_hal_console_enable();
}

bool airmon_pms_frame_valid(AirmonPmsContext* pms_context, size_t frame_len) {
    uint16_t checksum =
        PMS_START_CHAR_1 + PMS_START_CHAR_2 + (frame_len >> 8) + (frame_len & 0xff);

    uint8_t* p = pms_context->rx_buf;

    while(p < pms_context->rx_buf + frame_len - 2) {
        checksum += *p++ & 0xff;
    }

    return checksum == UINT16BE(p);
}

void airmon_pms_process_data_frame(AirmonPmsContext* pms_context) {
    furi_mutex_acquire(pms_context->mutex, FuriWaitForever);

    AirmonPmsData* d = &pms_context->pms_data;
    uint8_t* p = pms_context->rx_buf;

    d->pm1_0cf = UINT16BE(p);
    d->pm2_5cf = UINT16BE(p + 2);
    d->pm10cf = UINT16BE(p + 4);
    d->pm1_0at = UINT16BE(p + 6);
    d->pm2_5at = UINT16BE(p + 8);
    d->pm10at = UINT16BE(p + 10);
    d->ct0_3 = UINT16BE(p + 12);
    d->ct0_5 = UINT16BE(p + 14);
    d->ct1_0 = UINT16BE(p + 16);
    d->ct2_5 = UINT16BE(p + 18);
    d->ct5_0 = UINT16BE(p + 20);
    d->ct10 = UINT16BE(p + 22);

    furi_mutex_release(pms_context->mutex);
}

void airmon_pms_process_frame(AirmonPmsContext* pms_context, size_t frame_len) {
    if(!airmon_pms_frame_valid(pms_context, frame_len)) {
        notification_message_block(pms_context->notifications, &sequence_blink_red_10);
        return;
    }

    if(frame_len == PMS_DATA_FRAME_LEN) {
        airmon_pms_process_data_frame(pms_context);
        notification_message_block(pms_context->notifications, &sequence_blink_green_10);
    } else if(frame_len == PMS_CTRL_FRAME_LEN) {
        notification_message_block(pms_context->notifications, &sequence_blink_blue_10);
    } else {
        notification_message_block(pms_context->notifications, &sequence_blink_magenta_10);
    }
}

static int32_t airmon_pms_worker(void* context) {
    FURI_LOG_D(TAG, "Worker started");

    AirmonPmsContext* pms_context = context;

    pms_context->rx_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);
    size_t rx_offset = 0;

    airmon_pms_serial_init(pms_context);

    FURI_LOG_D(TAG, "Entering event loop");

    RxState rx_state = RxStateStartChar1;
    size_t rx_frame_len = 0;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEvtStop) {
            break;
        }

        if(events & WorkerEvtRxDone) {
            size_t rx_len = 0;

            while(
                (rx_len = furi_stream_buffer_receive(
                     pms_context->rx_stream,
                     pms_context->rx_buf + rx_offset,
                     RX_BUF_SIZE - rx_offset - 1,
                     0))) {
                FURI_LOG_D(TAG, "rx_len: %d", rx_len);

                rx_offset += rx_len;

                bool rx_available = true;
                size_t rx_processed;

                while(rx_available) {
                    rx_processed = 0;

                    switch(rx_state) {
                    case RxStateStartChar1:
                        if(*pms_context->rx_buf == PMS_START_CHAR_1) {
                            rx_state = RxStateStartChar2;
                        }
                        rx_processed = 1;
                        break;

                    case RxStateStartChar2:
                        if(*pms_context->rx_buf == PMS_START_CHAR_2) {
                            rx_state = RxStateFrameLength;
                            rx_processed = 1;
                        } else {
                            rx_state = RxStateStartChar1;
                        }
                        break;

                    case RxStateFrameLength:
                        if(rx_offset > 2) {
                            rx_frame_len = UINT16BE(pms_context->rx_buf);
                            FURI_LOG_D(TAG, "rx_frame_len: %d", rx_frame_len);
                            rx_processed = 2;
                            rx_state = (rx_frame_len >= PMS_CTRL_FRAME_LEN &&
                                        rx_frame_len <= PMS_DATA_FRAME_LEN) ?
                                           RxStateFrameData :
                                           RxStateStartChar1;
                        } else {
                            rx_available = false;
                        }
                        break;

                    case RxStateFrameData:
                        if(rx_offset >= rx_frame_len) {
                            airmon_pms_process_frame(pms_context, rx_frame_len);
                            rx_processed = rx_frame_len;
                            rx_state = RxStateStartChar1;
                        } else {
                            rx_available = false;
                        }
                        break;

                    default:
                        rx_state = RxStateStartChar1;
                        break;
                    }

                    FURI_LOG_D(TAG, "RxState: %d", rx_state);

                    if(rx_processed) {
                        rx_offset -= rx_processed;
                        rx_available = !!rx_offset;
                        if(rx_available) {
                            memmove(
                                pms_context->rx_buf,
                                pms_context->rx_buf + rx_processed,
                                rx_offset);
                        }
                    }
                }
            };
        }
    }

    airmon_pms_serial_deinit(pms_context);
    furi_stream_buffer_free(pms_context->rx_stream);

    FURI_LOG_D(TAG, "Worker stopped");

    return 0;
}

AirmonPmsContext* airmon_pms_context_alloc() {
    AirmonPmsContext* pms_context = malloc(sizeof(AirmonPmsContext));

    pms_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!pms_context->mutex) {
        FURI_LOG_E("AirmonPms", "cannot create mutex\r\n");
        free(pms_context);
        return NULL;
    }

    memset(&pms_context->pms_data, 0, sizeof(AirmonPmsData));

    pms_context->notifications = furi_record_open(RECORD_NOTIFICATION);

    pms_context->thread = furi_thread_alloc();
    furi_thread_set_name(pms_context->thread, "AirmonPmsWorker");
    furi_thread_set_stack_size(pms_context->thread, 1024);
    furi_thread_set_context(pms_context->thread, pms_context);
    furi_thread_set_callback(pms_context->thread, airmon_pms_worker);

    return pms_context;
}

void airmon_pms_context_free(AirmonPmsContext* pms_context) {
    furi_assert(pms_context);
    furi_thread_free(pms_context->thread);
    furi_record_close(RECORD_NOTIFICATION);
    free(pms_context);
}

void airmon_pms_init(AirmonPmsContext* pms_context) {
    furi_assert(pms_context);
    furi_hal_power_enable_otg();
    furi_thread_start(pms_context->thread);
}

void airmon_pms_deinit(AirmonPmsContext* pms_context) {
    furi_assert(pms_context);
    furi_thread_flags_set(furi_thread_get_id(pms_context->thread), WorkerEvtStop);
    furi_thread_join(pms_context->thread);
    furi_hal_power_disable_otg();
}
