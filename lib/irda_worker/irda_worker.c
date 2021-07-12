#include "irda_worker.h"

#include "furi/thread.h"
#include "irda.h"
#include "portmacro.h"
#include "projdefs.h"
#include <api-hal-irda.h>
#include <limits.h>
#include <stdint.h>
#include <stream_buffer.h>
#include <furi.h>

struct IrdaWorker {
    FuriThread* thread;
    IrdaDecoderHandler* irda_decoder;
    StreamBufferHandle_t stream;

    TaskHandle_t worker_handle;
    IrdaWorkerReceivedSignal signal;

    IrdaWorkerReceivedSignalCallback received_data_callback;
    void* context;
};

#define IRDA_WORKER_RX_TIMEOUT              500 // ms
#define IRDA_WORKER_RX_RECEIVED             0x01
#define IRDA_WORKER_RX_TIMEOUT_RECEIVED     0x02
#define IRDA_WORKER_OVERRUN                 0x04
#define IRDA_WORKER_EXIT                    0x08

static void irda_worker_rx_timeout_callback(void* context) {
    IrdaWorker* instance = context;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(instance->worker_handle, IRDA_WORKER_RX_TIMEOUT_RECEIVED, eSetBits,  &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/** Rx callback timer
 * 
 * @param level received signal level
 * @param duration received signal duration
 * @param context 
 */
static void irda_worker_rx_callback(void* context, bool level, uint32_t duration) {
    IrdaWorker* instance = context;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    LevelDuration level_duration = level_duration_make(level, duration);

    size_t ret =
        xStreamBufferSendFromISR(instance->stream, &level_duration, sizeof(LevelDuration), &xHigherPriorityTaskWoken);
    uint32_t notify_value = (ret == sizeof(LevelDuration)) ? IRDA_WORKER_RX_RECEIVED : IRDA_WORKER_OVERRUN;
    xTaskNotifyFromISR(instance->worker_handle, notify_value, eSetBits,  &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void irda_worker_process_timeout(IrdaWorker* instance) {
    if (instance->signal.timings_cnt < 2)
        return;

    furi_assert(instance->signal.timings_cnt % 2);

    instance->signal.decoded = false;
    if (instance->received_data_callback)
        instance->received_data_callback(instance->context, &instance->signal);
}

static void irda_worker_process_timings(IrdaWorker* instance, uint32_t duration, bool level) {
    const IrdaMessage* message_decoded = irda_decode(instance->irda_decoder, level, duration);
    if (message_decoded) {
        instance->signal.data.message = *message_decoded;
        instance->signal.timings_cnt = 0;
        instance->signal.decoded = true;
        if (instance->received_data_callback)
            instance->received_data_callback(instance->context, &instance->signal);
    } else {
        /* Skip first timing if it's starts from Space */
        if ((instance->signal.timings_cnt == 0) && !level) {
            return;
        }

        if (instance->signal.timings_cnt < sizeof(instance->signal.data.timings)) {
            instance->signal.data.timings[instance->signal.timings_cnt] = duration;
            ++instance->signal.timings_cnt;
        } else {
            furi_assert(0);
        }
    }
}

/** Worker callback thread
 * 
 * @param context 
 * @return exit code 
 */
static int32_t irda_worker_thread_callback(void* context) {
    IrdaWorker* instance = context;
    uint32_t notify_value = 0;
    LevelDuration level_duration;

    while(1) {
        BaseType_t result;
        result = xTaskNotifyWait(pdFALSE, ULONG_MAX, &notify_value, 1000);
        if (result != pdPASS)
            continue;

        if (notify_value & IRDA_WORKER_RX_RECEIVED) {
            while (sizeof(LevelDuration) == xStreamBufferReceive(instance->stream, &level_duration, sizeof(LevelDuration), 0)) {
                bool level = level_duration_get_level(level_duration);
                uint32_t duration = level_duration_get_duration(level_duration);
                irda_worker_process_timings(instance, duration, level);
            }
        }
        if (notify_value & IRDA_WORKER_OVERRUN) {
            printf("#");
            irda_reset_decoder(instance->irda_decoder);
            instance->signal.timings_cnt = 0;
        }
        if (notify_value & IRDA_WORKER_RX_TIMEOUT_RECEIVED) {
            irda_worker_process_timeout(instance);
            instance->signal.timings_cnt = 0;
        }
        if (notify_value & IRDA_WORKER_EXIT)
            break;
    }

    return 0;
}

void irda_worker_set_received_data_callback(IrdaWorker* instance, IrdaWorkerReceivedSignalCallback callback) {
    furi_assert(instance);
    instance->received_data_callback = callback;
}

IrdaWorker* irda_worker_alloc() {
    IrdaWorker* instance = furi_alloc(sizeof(IrdaWorker));

    instance->thread = furi_thread_alloc();
    furi_thread_set_name(instance->thread, "irda_worker");
    furi_thread_set_stack_size(instance->thread, 2048);
    furi_thread_set_context(instance->thread, instance);
    furi_thread_set_callback(instance->thread, irda_worker_thread_callback);
    
    instance->stream = xStreamBufferCreate(sizeof(LevelDuration) * 512, sizeof(LevelDuration));

    instance->irda_decoder = irda_alloc_decoder();

    return instance;
}

void irda_worker_free(IrdaWorker* instance) {
    furi_assert(instance);

    irda_free_decoder(instance->irda_decoder);
    vStreamBufferDelete(instance->stream);
    furi_thread_free(instance->thread);

    free(instance);
}

void irda_worker_set_context(IrdaWorker* instance, void* context) {
    furi_assert(instance);
    instance->context = context;
}

void irda_worker_start(IrdaWorker* instance) {
    furi_assert(instance);
    furi_assert(!instance->worker_handle);

    furi_thread_start(instance->thread);

    instance->worker_handle = furi_thread_get_thread_id(instance->thread);
    api_hal_irda_rx_irq_init(IRDA_WORKER_RX_TIMEOUT);
    api_hal_irda_rx_irq_set_callback(irda_worker_rx_callback, instance);
    api_hal_irda_timeout_irq_set_callback(irda_worker_rx_timeout_callback, instance);
}

void irda_worker_stop(IrdaWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_handle);

    api_hal_irda_timeout_irq_set_callback(NULL, NULL);
    api_hal_irda_rx_irq_set_callback(NULL, NULL);
    api_hal_irda_rx_irq_deinit();

    xTaskNotify(instance->worker_handle, IRDA_WORKER_EXIT, eSetBits);

    instance->worker_handle = NULL;

    furi_thread_join(instance->thread);
}
