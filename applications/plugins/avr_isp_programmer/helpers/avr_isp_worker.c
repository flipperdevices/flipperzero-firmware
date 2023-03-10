#include "avr_isp_worker.h"
#include "../lib/driver/avr_isp_prog.h"
#include "../lib/driver/avr_isp_spi_sw.h"
#include "../lib/driver/avr_isp_chip_arr.h"
#include "../lib/driver/avr_isp_vcp_cdc.h"

#include <furi.h>

#define TAG "AvrIspWorker"

typedef enum {
    AvrIspWorkerEvtStop = (1 << 0),
} AvrIspWorkerEvt;

#define AVR_ISP_WORKER_PROG_ALL_EVENTS (AvrIspWorkerEvtStop)

struct AvrIspWorker {
    FuriThread* thread;
    volatile bool worker_running;
    // AvrIspWorkerCallback callback;
    // void* context;
};

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
    avr_isp_vcp_cdc_init();

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

    FURI_LOG_D(TAG, "AvrIspWorker Start");
    while(instance->worker_running) {
        furi_delay_ms(1);

        size_t len = avr_isp_vcp_cdc_receive(AVR_ISP_VCP_CDC_CH, buf, AVR_ISP_VCP_CDC_PKT_LEN);
        while(len > 0) {
            avr_isp_prog_rx(prog, buf, len);

            // for(uint8_t i = 0; i < len; i++) {
            //     FURI_LOG_I(TAG, "--> %X", buf[i]);
            // }

            len = avr_isp_vcp_cdc_receive(AVR_ISP_VCP_CDC_CH, buf, AVR_ISP_VCP_CDC_PKT_LEN);
        }

        len = avr_isp_prog_tx(prog, buf, AVR_ISP_VCP_CDC_PKT_LEN);
        while(len > 0) {
            // for(uint8_t i = 0; i < len; i++) {
            //     FURI_LOG_I(TAG, "<-- %X", buf[i]);
            // }

            avr_isp_vcp_cdc_send(AVR_ISP_VCP_CDC_CH, buf, len);
            furi_delay_ms(1);
            len = avr_isp_prog_tx(prog, buf, AVR_ISP_VCP_CDC_PKT_LEN);
        }
    }
    FURI_LOG_D(TAG, "AvrIspWorker Stop");
    furi_thread_flags_set(furi_thread_get_id(prog_thread), AvrIspWorkerEvtStop);
    avr_isp_prog_exit(prog);
    furi_delay_ms(10);
    furi_thread_join(prog_thread);
    furi_thread_free(prog_thread);

    avr_isp_prog_free(prog);
    avr_isp_vcp_cdc_deinit();
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
