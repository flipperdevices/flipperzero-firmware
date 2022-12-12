#include "pocsag_tx_rx_worker.h"
#include "pocsag_hal.h"
#include <furi.h>

#define TAG "POCSAGTxRxWorker"

#define POCSAG_TXRX_WORKER_BUF_SIZE 2048
//you can not set more than 62 because it will not fit into the FIFO CC1101
#define POCSAG_TXRX_WORKER_MAX_TXRX_SIZE 60

#define POCSAG_TXRX_WORKER_TIMEOUT_READ_WRITE_BUF 40

struct PocsagTxRxWorker {
    FuriThread* thread;
    FuriStreamBuffer* stream_tx;
    FuriStreamBuffer* stream_rx;

    volatile bool worker_running;
    volatile bool worker_stoping;

    PocsagTxRxWorkerStatus status;

    uint32_t frequency;

    PocsagTxRxWorkerCallbackHaveRead callback_have_read;
    void* context_have_read;
};

bool pocsag_tx_rx_worker_write(PocsagTxRxWorker* instance, uint8_t* data, size_t size) {
    furi_assert(instance);
    bool ret = false;
    size_t stream_tx_free_byte = furi_stream_buffer_spaces_available(instance->stream_tx);
    if(size && (stream_tx_free_byte >= size)) {
        if(furi_stream_buffer_send(
                instance->stream_tx, data, size, POCSAG_TXRX_WORKER_TIMEOUT_READ_WRITE_BUF) ==
           size) {
            ret = true;
        }
    }
    return ret;
}

size_t pocsag_tx_rx_worker_available(PocsagTxRxWorker* instance) {
    furi_assert(instance);
    return furi_stream_buffer_bytes_available(instance->stream_rx);
}

size_t pocsag_tx_rx_worker_read(PocsagTxRxWorker* instance, uint8_t* data, size_t size) {
    furi_assert(instance);
    return furi_stream_buffer_receive(instance->stream_rx, data, size, 0);
}

void pocsag_tx_rx_worker_set_callback_have_read(
        PocsagTxRxWorker* instance,
        PocsagTxRxWorkerCallbackHaveRead callback,
        void* context) {
    furi_assert(instance);
    furi_assert(callback);
    furi_assert(context);
    instance->callback_have_read = callback;
    instance->context_have_read = context;
}

bool pocsag_tx_rx_worker_rx(PocsagTxRxWorker* instance, uint8_t* data, uint8_t* size) {
    uint8_t timeout = 100;
    bool ret = false;
    if(instance->status != PocsagTxRxWorkerStatusRx) {
        furi_hal_subghz_rx();
        instance->status = PocsagTxRxWorkerStatusRx;
        furi_delay_tick(1);
    }
    //waiting for reception to complete
    while(furi_hal_gpio_read(&gpio_cc1101_g0)) {
        furi_delay_tick(1);
        if(!--timeout) {
            FURI_LOG_W(TAG, "RX cc1101_g0 timeout");
            furi_hal_subghz_flush_rx();
            furi_hal_subghz_rx();
            break;
        }
    }

    if(furi_hal_subghz_rx_pipe_not_empty()) {
        FURI_LOG_I(
                TAG,
                "RSSI: %03.1fdbm LQI: %d",
                (double)furi_hal_subghz_get_rssi(),
                furi_hal_subghz_get_lqi());
        if(furi_hal_subghz_is_rx_data_crc_valid()) {
            furi_hal_subghz_read_packet(data, size);
            ret = true;
        }
        furi_hal_subghz_flush_rx();
        furi_hal_subghz_rx();
    }
    return ret;
}

void pocsag_tx_rx_worker_tx(PocsagTxRxWorker* instance, uint8_t* data, size_t size) {
    uint8_t timeout = 200;
    if(instance->status != PocsagTxRxWorkerStatusIDLE) {
        furi_hal_subghz_idle();
    }
    furi_hal_subghz_write_packet(data, size);
    furi_hal_subghz_tx(); //start send
    instance->status = PocsagTxRxWorkerStatusTx;
    while(!furi_hal_gpio_read(&gpio_cc1101_g0)) { // Wait for GDO0 to be set -> sync transmitted
        furi_delay_tick(1);
        if(!--timeout) {
            FURI_LOG_W(TAG, "TX !cc1101_g0 timeout");
            break;
        }
    }
    while(furi_hal_gpio_read(&gpio_cc1101_g0)) { // Wait for GDO0 to be cleared -> end of packet
        furi_delay_tick(1);
        if(!--timeout) {
            FURI_LOG_W(TAG, "TX cc1101_g0 timeout");
            break;
        }
    }
    furi_hal_subghz_idle();
    instance->status = PocsagTxRxWorkerStatusIDLE;
}
/** Worker thread
 * 
 * @param context 
 * @return exit code 
 */
static int32_t pocsag_tx_rx_worker_thread(void* context) {
    PocsagTxRxWorker* instance = context;
    FURI_LOG_I(TAG, "Worker start");

//    furi_hal_subghz_reset();
//    furi_hal_subghz_idle();
//    furi_hal_subghz_load_preset(FuriHalSubGhzPresetGFSK9_99KbAsync);
    pocsag_hal_reset();
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);

    furi_hal_subghz_set_frequency_and_path(instance->frequency);
    furi_hal_subghz_flush_rx();

    uint8_t data[POCSAG_TXRX_WORKER_MAX_TXRX_SIZE + 1] = {0};
    size_t size_tx = 0;
    uint8_t size_rx[1] = {0};
    uint8_t timeout_tx = 0;
    bool callback_rx = false;

    while(instance->worker_running) {
        //transmit
        size_tx = furi_stream_buffer_bytes_available(instance->stream_tx);
        if(size_tx > 0 && !timeout_tx) {
            timeout_tx = 10; //20ms
            if(size_tx > POCSAG_TXRX_WORKER_MAX_TXRX_SIZE) {
                furi_stream_buffer_receive(
                        instance->stream_tx,
                        &data,
                        POCSAG_TXRX_WORKER_MAX_TXRX_SIZE,
                        POCSAG_TXRX_WORKER_TIMEOUT_READ_WRITE_BUF);
                pocsag_tx_rx_worker_tx(instance, data, POCSAG_TXRX_WORKER_MAX_TXRX_SIZE);
            } else {
                //todo checking that he managed to write all the data to the TX buffer
                furi_stream_buffer_receive(
                        instance->stream_tx, &data, size_tx, POCSAG_TXRX_WORKER_TIMEOUT_READ_WRITE_BUF);
                pocsag_tx_rx_worker_tx(instance, data, size_tx);
            }
        } else {
            //recive
            if(pocsag_tx_rx_worker_rx(instance, data, size_rx)) {
                if(furi_stream_buffer_spaces_available(instance->stream_rx) >= size_rx[0]) {
                    if(instance->callback_have_read &&
                       furi_stream_buffer_bytes_available(instance->stream_rx) == 0) {
                        callback_rx = true;
                    }
                    //todo checking that he managed to write all the data to the RX buffer
                    furi_stream_buffer_send(
                            instance->stream_rx,
                            &data,
                            size_rx[0],
                            POCSAG_TXRX_WORKER_TIMEOUT_READ_WRITE_BUF);
                    if(callback_rx) {
                        instance->callback_have_read(instance->context_have_read);
                        callback_rx = false;
                    }
                } else {
                    //todo RX buffer overflow
                }
            }
        }

        if(timeout_tx) timeout_tx--;
        furi_delay_tick(1);
    }

    furi_hal_subghz_set_path(FuriHalSubGhzPathIsolate);
    furi_hal_subghz_sleep();

    FURI_LOG_I(TAG, "Worker stop");
    return 0;
}

PocsagTxRxWorker* pocsag_tx_rx_worker_alloc() {
    PocsagTxRxWorker* instance = malloc(sizeof(PocsagTxRxWorker));

    instance->thread =
            furi_thread_alloc_ex("PocsagTxRxWorker", 2048, pocsag_tx_rx_worker_thread, instance);
    instance->stream_tx =
            furi_stream_buffer_alloc(sizeof(uint8_t) * POCSAG_TXRX_WORKER_BUF_SIZE, sizeof(uint8_t));
    instance->stream_rx =
            furi_stream_buffer_alloc(sizeof(uint8_t) * POCSAG_TXRX_WORKER_BUF_SIZE, sizeof(uint8_t));

    instance->status = PocsagTxRxWorkerStatusIDLE;
    instance->worker_stoping = true;

    return instance;
}

void pocsag_tx_rx_worker_free(PocsagTxRxWorker* instance) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);
    furi_stream_buffer_free(instance->stream_tx);
    furi_stream_buffer_free(instance->stream_rx);
    furi_thread_free(instance->thread);

    free(instance);
}

bool pocsag_tx_rx_worker_start(PocsagTxRxWorker* instance, uint32_t frequency) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);
    bool res = false;
    furi_stream_buffer_reset(instance->stream_tx);
    furi_stream_buffer_reset(instance->stream_rx);

    instance->worker_running = true;

    if(furi_hal_region_is_frequency_allowed(frequency)) {
        instance->frequency = frequency;
        res = true;
    }

    furi_thread_start(instance->thread);

    return res;
}

void pocsag_tx_rx_worker_stop(PocsagTxRxWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_running);

    instance->worker_running = false;

    furi_thread_join(instance->thread);
}

bool pocsag_tx_rx_worker_is_running(PocsagTxRxWorker* instance) {
    furi_assert(instance);
    return instance->worker_running;
}
