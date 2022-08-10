#include <furi.h>
#include <furi_hal.h>
#include "lfrfid_worker_i.h"
#include "tools/t5577.h"
#include <stream_buffer.h>
#include <toolbox/pulse_protocols/pulse_glue.h>

#define TAG "LFRFIDWorker"

/**
 * if READ_DEBUG_GPIO is defined:
 *     gpio_ext_pa7 will repeat signal coming from the comparator
 *     gpio_ext_pa6 will show load on the decoder
 */
#define LFRFID_WORKER_READ_DEBUG_GPIO 1

#ifdef LFRFID_WORKER_READ_DEBUG_GPIO
#define LFRFID_WORKER_READ_DEBUG_GPIO_VALUE &gpio_ext_pa7
#define LFRFID_WORKER_READ_DEBUG_GPIO_LOAD &gpio_ext_pa6
#endif

#define LFRFID_WORKER_READ_SWITCH_TIME 3000
#define LFRFID_WORKER_READ_STREAM_SIZE 4096
#define LFRFID_WORKER_EMULATE_BUFFER_SIZE 1024

void lfrfid_worker_mode_emulate_process(LFRFIDWorker* worker);
void lfrfid_worker_mode_read_process(LFRFIDWorker* worker);
void lfrfid_worker_mode_write_process(LFRFIDWorker* worker);
void lfrfid_worker_mode_read_raw_process(LFRFIDWorker* worker);
void lfrfid_worker_mode_emulate_raw_process(LFRFIDWorker* worker);

const LFRFIDWorkerModeType lfrfid_worker_modes[] = {
    [LFRFIDWorkerIdle] = {.process = NULL},
    [LFRFIDWorkerRead] = {.process = lfrfid_worker_mode_read_process},
    [LFRFIDWorkerWrite] = {.process = lfrfid_worker_mode_write_process},
    [LFRFIDWorkerEmulate] = {.process = lfrfid_worker_mode_emulate_process},
    [LFRFIDWorkerReadRaw] = {.process = lfrfid_worker_mode_read_raw_process},
    [LFRFIDWorkerEmulateRaw] = {.process = lfrfid_worker_mode_emulate_raw_process},
};

/*********************** READ ***********************/
static void lfrfid_worker_read_capture(bool level, uint32_t duration, void* context) {
#ifdef LFRFID_WORKER_READ_DEBUG_GPIO
    furi_hal_gpio_write(LFRFID_WORKER_READ_DEBUG_GPIO_VALUE, level);
#endif

    StreamBufferHandle_t stream = context;
    LevelDuration level_duration = level_duration_make(level, duration);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xStreamBufferSendFromISR(
        stream, &level_duration, sizeof(LevelDuration), &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void lfrfid_worker_mode_read_process(LFRFIDWorker* worker) {
    furi_hal_rfid_pins_read();

    bool current_type_is_ask;
    if(worker->read_type == LFRFIDWorkerReadTypePSKOnly) {
        furi_hal_rfid_tim_read(62500, 0.25);
        current_type_is_ask = false;
    } else {
        furi_hal_rfid_tim_read(125000, 0.5);
        current_type_is_ask = true;
    }

    furi_hal_rfid_tim_read_start();
    protocol_dict_decoders_start(worker->protocols);

#ifdef LFRFID_WORKER_READ_DEBUG_GPIO
    furi_hal_gpio_init_simple(LFRFID_WORKER_READ_DEBUG_GPIO_VALUE, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(LFRFID_WORKER_READ_DEBUG_GPIO_LOAD, GpioModeOutputPushPull);
#endif

    StreamBufferHandle_t stream = xStreamBufferCreate(
        sizeof(LevelDuration) * LFRFID_WORKER_READ_STREAM_SIZE, sizeof(LevelDuration));
    furi_hal_rfid_tim_read_capture_start(lfrfid_worker_read_capture, stream);

    ProtocolId protocol = PROTOCOL_NO;
    uint32_t tmp_duration = 0;

    ProtocolId last_protocol = PROTOCOL_NO;
    size_t last_size = protocol_dict_get_max_data_size(worker->protocols);
    uint8_t* last_data = malloc(last_size);
    uint8_t* protocol_data = malloc(last_size);
    size_t last_read_count = 0;

    uint32_t switch_os_tick_last = furi_get_tick();

    while(true) {
        if(lfrfid_worker_check_for_stop(worker)) break;

        LevelDuration level_duration;
        size_t size = xStreamBufferReceive(stream, &level_duration, sizeof(LevelDuration), 100);

#ifdef LFRFID_WORKER_READ_DEBUG_GPIO
        furi_hal_gpio_write(LFRFID_WORKER_READ_DEBUG_GPIO_LOAD, true);
#endif

        if(size == 0) {
            continue;
        }

        if(size == sizeof(LevelDuration)) {
            uint32_t duration = level_duration_get_duration(level_duration);

            uint32_t feature = LFRFIDFeaturePSK;
            if(current_type_is_ask) {
                feature = LFRFIDFeatureASK;
            }

            if(level_duration_get_level(level_duration)) {
                tmp_duration = duration;
                protocol = protocol_dict_decoders_feed_by_feature(
                    worker->protocols, true, tmp_duration, feature);
            } else {
                tmp_duration = duration - tmp_duration;
                protocol = protocol_dict_decoders_feed_by_feature(
                    worker->protocols, false, tmp_duration, feature);
                tmp_duration = 0;
            }
        }

        if(protocol != PROTOCOL_NO) {
            // reset switch timer
            switch_os_tick_last = furi_get_tick();

            size_t protocol_data_size = protocol_dict_get_data_size(worker->protocols, protocol);
            protocol_dict_get_data(worker->protocols, protocol, protocol_data, protocol_data_size);

            FURI_LOG_D(
                TAG,
                "%s, %d",
                protocol_dict_get_name(worker->protocols, protocol),
                last_read_count);

            // validate protocol
            if(protocol == last_protocol &&
               memcmp(last_data, protocol_data, protocol_data_size) == 0) {
                last_read_count = last_read_count + 1;

                size_t validation_count =
                    protocol_dict_get_validate_count(worker->protocols, protocol);

                if(last_read_count >= validation_count) {
                    worker->read_cb(LFRFIDWorkerReadDone, protocol, worker->cb_ctx);
                    break;
                }
            } else {
                if(last_protocol == PROTOCOL_NO) {
                    worker->read_cb(LFRFIDWorkerReadSenseCardStart, protocol, worker->cb_ctx);
                }

                last_protocol = protocol;
                memcpy(last_data, protocol_data, protocol_data_size);
                last_read_count = 0;
            }
        } else if(worker->read_type == LFRFIDWorkerReadTypeAuto) {
            // switch mode if no protocol detected
            if((furi_get_tick() - switch_os_tick_last) > LFRFID_WORKER_READ_SWITCH_TIME) {
                if(last_protocol != PROTOCOL_NO) {
                    worker->read_cb(LFRFIDWorkerReadSenseCardEnd, last_protocol, worker->cb_ctx);
                    last_protocol = PROTOCOL_NO;
                    last_read_count = 0;
                }

                switch_os_tick_last = furi_get_tick();
                current_type_is_ask = !current_type_is_ask;

                FURI_LOG_D(TAG, "drop field");
                furi_hal_rfid_change_read_config(1, 0);
                furi_delay_ms(500);

                FURI_LOG_D(TAG, "switch to %s", current_type_is_ask ? "ASK" : "PSK");
                if(current_type_is_ask) {
                    furi_hal_rfid_change_read_config(125000, 0.5);
                } else {
                    furi_hal_rfid_change_read_config(62500, 0.25);
                }

                // try to empty stream buffer
                xStreamBufferReset(stream);
            }
        }

#ifdef LFRFID_WORKER_READ_DEBUG_GPIO
        furi_hal_gpio_write(LFRFID_WORKER_READ_DEBUG_GPIO_LOAD, false);
#endif
    }

    free(last_data);
    free(protocol_data);

    furi_hal_rfid_tim_read_capture_stop();
    furi_hal_rfid_tim_read_stop();

    vStreamBufferDelete(stream);

#ifdef LFRFID_WORKER_READ_DEBUG_GPIO
    furi_hal_gpio_init_simple(LFRFID_WORKER_READ_DEBUG_GPIO_VALUE, GpioModeAnalog);
    furi_hal_gpio_init_simple(LFRFID_WORKER_READ_DEBUG_GPIO_LOAD, GpioModeAnalog);
#endif
}

/*********************** EMULATE ***********************/
typedef struct {
    uint32_t duration[LFRFID_WORKER_EMULATE_BUFFER_SIZE];
    uint32_t pulse[LFRFID_WORKER_EMULATE_BUFFER_SIZE];
} LFRFIDWorkerEmulateBuffer;

typedef enum {
    HalfTransfer,
    TransferComplete,
} LFRFIDWorkerEmulateDMAEvent;

static void lfrfid_worker_emulate_dma_isr(bool half, void* context) {
    StreamBufferHandle_t stream = context;

    if(half) {
        uint32_t flag = HalfTransfer;
        xStreamBufferSendFromISR(stream, &flag, sizeof(uint32_t), pdFALSE);
    } else {
        uint32_t flag = TransferComplete;
        xStreamBufferSendFromISR(stream, &flag, sizeof(uint32_t), pdFALSE);
    }
}

void lfrfid_worker_mode_emulate_process(LFRFIDWorker* worker) {
    LFRFIDWorkerEmulateBuffer* buffer = malloc(sizeof(LFRFIDWorkerEmulateBuffer));
    StreamBufferHandle_t stream = xStreamBufferCreate(sizeof(uint32_t), sizeof(uint32_t));
    LFRFIDProtocol protocol = worker->protocol;
    PulseGlue* pulse_glue = pulse_glue_alloc();

    protocol_dict_encoder_start(worker->protocols, protocol);

    for(size_t i = 0; i < LFRFID_WORKER_EMULATE_BUFFER_SIZE; i++) {
        bool pulse_pop = false;
        while(!pulse_pop) {
            LevelDuration level_duration =
                protocol_dict_encoder_yield(worker->protocols, protocol);
            pulse_pop = pulse_glue_push(
                pulse_glue,
                level_duration_get_level(level_duration),
                level_duration_get_duration(level_duration));
        }
        uint32_t duration, pulse;
        pulse_glue_pop(pulse_glue, &duration, &pulse);
        buffer->duration[i] = duration - 1;
        buffer->pulse[i] = pulse;
    }

#ifdef LFRFID_WORKER_READ_DEBUG_GPIO
    furi_hal_gpio_init_simple(LFRFID_WORKER_READ_DEBUG_GPIO_LOAD, GpioModeOutputPushPull);
#endif

    furi_hal_rfid_tim_emulate_dma_start(
        buffer->duration,
        buffer->pulse,
        LFRFID_WORKER_EMULATE_BUFFER_SIZE,
        lfrfid_worker_emulate_dma_isr,
        stream);

    while(true) {
        uint32_t flag = 0;
        size_t size = xStreamBufferReceive(stream, &flag, sizeof(uint32_t), 100);

#ifdef LFRFID_WORKER_READ_DEBUG_GPIO
        furi_hal_gpio_write(LFRFID_WORKER_READ_DEBUG_GPIO_LOAD, true);
#endif

        if(size == sizeof(uint32_t)) {
            size_t start = 0;

            if(flag == HalfTransfer) {
                start = 0;
            } else if(flag == TransferComplete) {
                start = (LFRFID_WORKER_EMULATE_BUFFER_SIZE / 2);
            }

            for(size_t i = 0; i < (LFRFID_WORKER_EMULATE_BUFFER_SIZE / 2); i++) {
                bool pulse_pop = false;
                while(!pulse_pop) {
                    LevelDuration level_duration =
                        protocol_dict_encoder_yield(worker->protocols, protocol);
                    pulse_pop = pulse_glue_push(
                        pulse_glue,
                        level_duration_get_level(level_duration),
                        level_duration_get_duration(level_duration));
                }
                uint32_t duration, pulse;
                pulse_glue_pop(pulse_glue, &duration, &pulse);
                buffer->duration[start + i] = duration - 1;
                buffer->pulse[start + i] = pulse;
            }
        }

        if(lfrfid_worker_check_for_stop(worker)) {
            break;
        }

#ifdef LFRFID_WORKER_READ_DEBUG_GPIO
        furi_hal_gpio_write(LFRFID_WORKER_READ_DEBUG_GPIO_LOAD, false);
#endif
    }

    furi_hal_rfid_tim_emulate_dma_stop();

#ifdef LFRFID_WORKER_READ_DEBUG_GPIO
    furi_hal_gpio_init_simple(LFRFID_WORKER_READ_DEBUG_GPIO_LOAD, GpioModeAnalog);
#endif

    free(buffer);
    vStreamBufferDelete(stream);
    pulse_glue_free(pulse_glue);
}

/*********************** WRITE ***********************/

void lfrfid_worker_mode_write_process(LFRFIDWorker* worker) {
    LFRFIDProtocol protocol = worker->protocol;
    LFRFIDWriteRequest* request = malloc(sizeof(LFRFIDWriteRequest));
    request->write_type = LFRFIDWriteTypeT5577;

    bool can_be_written = protocol_dict_get_write_data(worker->protocols, protocol, request);

    if(can_be_written) {
        while(!lfrfid_worker_check_for_stop(worker)) {
            t5577_write(&request->t5577);
            furi_delay_ms(1000);
        }
    }

    free(request);
}

/*********************** READ RAW ***********************/

void lfrfid_worker_mode_read_raw_process(LFRFIDWorker* worker) {
    LFRFIDRawWorker* raw_worker = lfrfid_raw_worker_alloc();

    lfrfid_raw_worker_read_set_callback(raw_worker, worker->read_raw_cb, worker->cb_ctx);

    switch(worker->read_type) {
    case LFRFIDWorkerReadTypePSKOnly:
        lfrfid_raw_worker_start_read(raw_worker, worker->raw_filename, 62500, 0.25);
        break;
    case LFRFIDWorkerReadTypeASKOnly:
        lfrfid_raw_worker_start_read(raw_worker, worker->raw_filename, 125000, 0.5);
        break;
    default:
        furi_crash("RAW can be only PSK or ASK");
        break;
    }

    while(!lfrfid_worker_check_for_stop(worker)) {
        furi_delay_ms(100);
    }

    lfrfid_raw_worker_stop(raw_worker);
    lfrfid_raw_worker_read_set_callback(raw_worker, NULL, NULL);

    lfrfid_raw_worker_free(raw_worker);
}

/*********************** EMULATE RAW ***********************/

void lfrfid_worker_mode_emulate_raw_process(LFRFIDWorker* worker) {
    LFRFIDRawWorker* raw_worker = lfrfid_raw_worker_alloc();

    lfrfid_raw_worker_emulate_set_callback(raw_worker, worker->emulate_raw_cb, worker->cb_ctx);
    lfrfid_raw_worker_start_emulate(raw_worker, worker->raw_filename);

    while(!lfrfid_worker_check_for_stop(worker)) {
        furi_delay_ms(100);
    }

    lfrfid_raw_worker_stop(raw_worker);
    lfrfid_raw_worker_emulate_set_callback(raw_worker, NULL, NULL);

    lfrfid_raw_worker_free(raw_worker);
}
