#include <furi_hal_delay.h>
#include <furi_hal_rfid.h>
#include <toolbox/stream/file_stream.h>
#include <toolbox/varint.h>
#include <stream_buffer.h>
#include "lfrfid_raw_worker.h"

#define EMULATE_BUFFER_SIZE 1024
#define RFID_DATA_BUFFER_SIZE 2048

#define TAG_EMULATE "RAW EMULATE"

#define RAW_HEADER "RFID RAW"

// emulate mode

typedef struct {
    size_t counter;
    size_t size;
    uint8_t data[RFID_DATA_BUFFER_SIZE];
} RfidFileBuffer;

typedef struct {
    size_t overrun_count;
    StreamBufferHandle_t stream;
} RfidEmulateCtx;

typedef struct {
    Stream* stream;
    RfidFileBuffer buffer;
    uint32_t emulate_buffer_arr[EMULATE_BUFFER_SIZE];
    uint32_t emulate_buffer_ccr[EMULATE_BUFFER_SIZE];
    RfidEmulateCtx ctx;
} LFRFIDRawWorkerEmulateData;

typedef enum {
    HalfTransfer,
    TransferComplete,
} LFRFIDRawEmulateDMAEvent;

// read mode
#define READ_BUFFER_COUNT 4

#define READ_TEMP_DATA_SIZE 10

typedef struct {
    bool occupied;
    size_t counter;
    size_t size;
    uint8_t data[RFID_DATA_BUFFER_SIZE];
} RfidReadBuffer;

typedef struct {
    size_t overrun_count;
    bool overrun;
    StreamBufferHandle_t stream;

    size_t buffers_counter;
    RfidReadBuffer buffers[READ_BUFFER_COUNT];

    size_t tmp_data_length;
    uint8_t tmp_data[READ_TEMP_DATA_SIZE];

} RfidReadCtx;

typedef struct {
    RfidReadCtx ctx;
} LFRFIDRawWorkerReadData;

// main worker

struct LFRFIDRawWorker {
    string_t file_path;
    FuriThread* thread;
    osEventFlagsId_t events;

    LFRFIDWorkerEmulateRawCallback emulate_callback;
    LFRFIDWorkerReadRawCallback read_callback;
    void* context;

    float frequency;
    float duty_cycle;
};

typedef enum {
    LFRFIDRawWorkerEventStop,
} LFRFIDRawWorkerEvent;

static int32_t lfrfid_raw_read_worker_thread(void* thread_context);
static int32_t lfrfid_raw_emulate_worker_thread(void* thread_context);

LFRFIDRawWorker* lfrfid_raw_worker_alloc() {
    LFRFIDRawWorker* worker = malloc(sizeof(LFRFIDRawWorker));

    worker->thread = furi_thread_alloc();
    furi_thread_set_name(worker->thread, "lfrfid_raw_worker");
    furi_thread_set_context(worker->thread, worker);
    furi_thread_set_stack_size(worker->thread, 2048);

    worker->events = osEventFlagsNew(NULL);

    string_init(worker->file_path);
    return worker;
}

void lfrfid_raw_worker_free(LFRFIDRawWorker* worker) {
    furi_thread_free(worker->thread);
    osEventFlagsDelete(worker->events);
    string_clear(worker->file_path);
    free(worker);
}

bool lfrfid_raw_worker_start_read(
    LFRFIDRawWorker* worker,
    const char* file_path,
    float freq,
    float duty_cycle) {
    furi_check(furi_thread_get_state(worker->thread) == FuriThreadStateStopped);

    bool result = true;
    string_set(worker->file_path, file_path);

    worker->frequency = freq;
    worker->duty_cycle = duty_cycle;

    furi_thread_set_callback(worker->thread, lfrfid_raw_read_worker_thread);

    furi_thread_start(worker->thread);
    return result;
}

bool lfrfid_raw_worker_start_emulate(LFRFIDRawWorker* worker, const char* file_path) {
    furi_check(furi_thread_get_state(worker->thread) == FuriThreadStateStopped);

    bool result = true;
    string_set(worker->file_path, file_path);

    furi_thread_set_callback(worker->thread, lfrfid_raw_emulate_worker_thread);

    furi_thread_start(worker->thread);
    return result;
}

bool lfrfid_raw_worker_stop(LFRFIDRawWorker* worker) {
    bool result = true;

    osEventFlagsSet(worker->events, 1 << LFRFIDRawWorkerEventStop);
    furi_thread_join(worker->thread);

    return result;
}

void lfrfid_raw_worker_emulate_set_callback(
    LFRFIDRawWorker* worker,
    LFRFIDWorkerEmulateRawCallback callback,
    void* context) {
    worker->emulate_callback = callback;
    worker->context = context;
}

void lfrfid_raw_worker_read_set_callback(
    LFRFIDRawWorker* worker,
    LFRFIDWorkerReadRawCallback callback,
    void* context) {
    worker->read_callback = callback;
    worker->context = context;
}

static inline int8_t get_free_buffer(RfidReadCtx* ctx) {
    int8_t id = -1;
    for(size_t i = 0; i < READ_BUFFER_COUNT; i++) {
        if(ctx->buffers[i].occupied == false) {
            id = i;
            break;
        }
    }

    return id;
}

// pack varint into tmp_data
static inline bool
    write_to_tmp_buffer(RfidReadCtx* ctx, bool level, uint32_t duration, bool* pair_error) {
    bool result = false;

    if(level) {
        if(ctx->tmp_data_length == 0) {
            ctx->tmp_data_length = varint_uint32_pack(duration, ctx->tmp_data);
        } else {
            ctx->tmp_data_length = 0;
            *pair_error = true;
        }
    } else {
        if(ctx->tmp_data_length > 0) {
            ctx->tmp_data_length +=
                varint_uint32_pack(duration, ctx->tmp_data + ctx->tmp_data_length);
            result = true;
        } else {
            ctx->tmp_data_length = 0;
            *pair_error = true;
        }
    }

    return result;
}

static void lfrfid_raw_worker_capture(bool level, uint32_t duration, void* context) {
    RfidReadCtx* ctx = context;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    bool pair_error = false;
    bool need_to_send = write_to_tmp_buffer(ctx, level, duration, &pair_error);

    if(need_to_send) {
        bool overrun = true;
        int8_t id = ctx->buffers_counter;

        if((ctx->buffers[id].size + ctx->tmp_data_length) >= RFID_DATA_BUFFER_SIZE &&
           ctx->buffers[id].occupied == false) {
            // send
            RfidReadBuffer* buffer_p = &ctx->buffers[id];

            buffer_p->occupied = true;

            size_t s = xStreamBufferSendFromISR(
                ctx->stream, &buffer_p, sizeof(RfidReadBuffer*), &xHigherPriorityTaskWoken);

            if(s != sizeof(RfidReadBuffer*)) {
                ctx->overrun_count++;
            }
        }

        if(!ctx->buffers[id].occupied) {
            memcpy(
                ctx->buffers[id].data + ctx->buffers[id].size,
                ctx->tmp_data,
                ctx->tmp_data_length);
            ctx->buffers[id].size += ctx->tmp_data_length;
            ctx->tmp_data_length = 0;

            overrun = false;
        } else {
            id = get_free_buffer(ctx);

            if(id != -1) {
                memcpy(
                    ctx->buffers[id].data + ctx->buffers[id].size,
                    ctx->tmp_data,
                    ctx->tmp_data_length);
                ctx->buffers[id].size += ctx->tmp_data_length;
                ctx->tmp_data_length = 0;
                ctx->buffers_counter = id;

                overrun = false;
            }
        }

        if(overrun) {
            ctx->overrun_count++;
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static int32_t lfrfid_raw_read_worker_thread(void* thread_context) {
    LFRFIDRawWorker* worker = (LFRFIDRawWorker*)thread_context;

    Storage* storage = furi_record_open("storage");
    Stream* stream = file_stream_alloc(storage);
    const char* filename = string_get_cstr(worker->file_path);
    bool file_valid = file_stream_open(stream, filename, FSAM_READ_WRITE, FSOM_CREATE_ALWAYS);

    LFRFIDRawWorkerReadData* data = malloc(sizeof(LFRFIDRawWorkerReadData));

    data->ctx.overrun_count = 0;
    data->ctx.stream =
        xStreamBufferCreate(sizeof(RfidReadBuffer*) * READ_BUFFER_COUNT, sizeof(RfidReadBuffer*));

    if(file_valid) {
        // write header
        size_t size = stream_write_cstring(stream, RAW_HEADER);
        if(size != strlen(RAW_HEADER)) {
            file_valid = false;
        }

        uint32_t max_buffer_size = RFID_DATA_BUFFER_SIZE;
        size = stream_write(stream, (uint8_t*)&max_buffer_size, sizeof(uint32_t));
        if(size != sizeof(uint32_t)) {
            file_valid = false;
        }

        size = stream_write(stream, (uint8_t*)&worker->frequency, sizeof(float));
        if(size != sizeof(float)) {
            file_valid = false;
        }

        size = stream_write(stream, (uint8_t*)&worker->duty_cycle, sizeof(float));
        if(size != sizeof(float)) {
            file_valid = false;
        }
    }

    if(file_valid) {
        // setup carrier
        furi_hal_rfid_pins_read();
        furi_hal_rfid_tim_read(worker->frequency, worker->duty_cycle);
        furi_hal_rfid_tim_read_start();

        // stabilize detector
        furi_hal_delay_ms(1500);

        // start capture
        furi_hal_rfid_tim_read_capture_start(lfrfid_raw_worker_capture, &data->ctx);

        RfidReadBuffer* buffer_p = NULL;
        while(1) {
            size_t size =
                xStreamBufferReceive(data->ctx.stream, &buffer_p, sizeof(RfidReadBuffer*), 100);

            if(size == sizeof(RfidReadBuffer*)) {
                size_t size = 0;

                size = stream_write(stream, (uint8_t*)&buffer_p->size, sizeof(size_t));
                if(size != sizeof(size_t)) {
                    file_valid = false;
                }

                size = stream_write(stream, buffer_p->data, buffer_p->size);
                if(size != buffer_p->size) {
                    file_valid = false;
                }

                buffer_p->size = 0;
                __DMB();
                buffer_p->occupied = false;
                __DMB();
            } else if(size != 0) {
                data->ctx.overrun_count++;
            }

            if(!file_valid) {
                if(worker->read_callback != NULL) {
                    // message file_error to worker
                    worker->read_callback(LFRFIDWorkerReadRawFileError, worker->context);
                }
                break;
            }

            if(data->ctx.overrun_count > 0 && worker->read_callback != NULL) {
                // message overrun to worker
                worker->read_callback(LFRFIDWorkerReadRawOverrun, worker->context);
            }

            uint32_t flags = osEventFlagsGet(worker->events);
            if(FURI_BIT(flags, LFRFIDRawWorkerEventStop)) {
                break;
            };
        }

        furi_hal_rfid_tim_read_capture_stop();
        furi_hal_rfid_tim_read_stop();
    } else {
        if(worker->read_callback != NULL) {
            // message file_error to worker
            worker->read_callback(LFRFIDWorkerReadRawFileError, worker->context);
        }
    }

    if(!file_valid) {
        const uint32_t available_flags = (1 << LFRFIDRawWorkerEventStop);
        while(true) {
            uint32_t flags =
                osEventFlagsWait(worker->events, available_flags, osFlagsWaitAny, osWaitForever);

            if(FURI_BIT(flags, LFRFIDRawWorkerEventStop)) {
                break;
            };
        }
    }

    vStreamBufferDelete(data->ctx.stream);
    stream_free(stream);
    furi_record_close("storage");
    free(data);

    return 0;
}

static void rfid_emulate_dma_isr(bool half, void* context) {
    RfidEmulateCtx* ctx = context;

    if(half) {
        uint32_t flag = HalfTransfer;
        size_t len = xStreamBufferSendFromISR(ctx->stream, &flag, sizeof(uint32_t), pdFALSE);
        if(len != sizeof(uint32_t)) {
            ctx->overrun_count++;
        }
    } else {
        uint32_t flag = TransferComplete;
        size_t len = xStreamBufferSendFromISR(ctx->stream, &flag, sizeof(uint32_t), pdFALSE);
        if(len != sizeof(uint32_t)) {
            ctx->overrun_count++;
        }
    }
}

static bool read_pair(LFRFIDRawWorkerEmulateData* data, uint32_t* duration, uint32_t* pulse) {
    RfidFileBuffer* buffer = &data->buffer;

    size_t length = 0;
    if(buffer->counter >= buffer->size) {
        if(stream_eof(data->stream)) {
            // rewind stream and pass header
            stream_seek(
                data->stream, strlen(RAW_HEADER) + sizeof(uint32_t), StreamOffsetFromStart);
        }

        length = stream_read(data->stream, (uint8_t*)&buffer->size, sizeof(size_t));
        if(length != sizeof(size_t)) {
            FURI_LOG_E(TAG_EMULATE, "read pair: failed to read size");
            return false;
        }

        if(buffer->size > RFID_DATA_BUFFER_SIZE) {
            FURI_LOG_E(TAG_EMULATE, "read pair: buffer size is too big");
            return false;
        }

        length = stream_read(data->stream, buffer->data, buffer->size);
        if(length != buffer->size) {
            FURI_LOG_E(TAG_EMULATE, "read pair: failed to read data");
            return false;
        }

        buffer->counter = 0;
    }

    uint32_t value1 = 0;
    uint32_t value2 = 0;

    buffer->counter += varint_uint32_unpack(
        &value1, &buffer->data[buffer->counter], (size_t)(buffer->size - buffer->counter));

    if(buffer->counter >= buffer->size) {
        FURI_LOG_E(TAG_EMULATE, "read pair: buffer is too small");
        return false;
    }

    buffer->counter += varint_uint32_unpack(
        &value2, &buffer->data[buffer->counter], (size_t)(buffer->size - buffer->counter));

    *duration = value2 / 8;
    *duration -= 1;
    *pulse = value1 / 8;

    return true;
}

static int32_t lfrfid_raw_emulate_worker_thread(void* thread_context) {
    LFRFIDRawWorker* worker = thread_context;

    bool file_valid = true;

    LFRFIDRawWorkerEmulateData* data = malloc(sizeof(LFRFIDRawWorkerEmulateData));

    Storage* storage = furi_record_open("storage");
    data->stream = file_stream_alloc(storage);
    data->ctx.overrun_count = 0;
    data->ctx.stream = xStreamBufferCreate(sizeof(uint32_t), sizeof(uint32_t));
    data->buffer.size = 0;
    data->buffer.counter = 0;

    do {
        const char* filename = string_get_cstr(worker->file_path);

        if(!file_stream_open(data->stream, filename, FSAM_READ, FSOM_OPEN_EXISTING)) {
            file_valid = false;
            break;
        }

        // read header
        char* header_text = malloc(strlen(RAW_HEADER) + 1);

        size_t size = stream_read(data->stream, (uint8_t*)header_text, strlen(RAW_HEADER));
        if(size != strlen(RAW_HEADER) || strcmp(header_text, RAW_HEADER) != 0) {
            file_valid = false;
        }

        free(header_text);

        uint32_t max_buffer_size;
        size = stream_read(data->stream, (uint8_t*)&max_buffer_size, sizeof(uint32_t));
        if(size != sizeof(uint32_t) || max_buffer_size != RFID_DATA_BUFFER_SIZE) {
            file_valid = false;
        }

        size = stream_read(data->stream, (uint8_t*)&worker->frequency, sizeof(float));
        if(size != sizeof(float) || worker->frequency < 0.0f || worker->frequency > 1000000.0f) {
            file_valid = false;
        }

        size = stream_read(data->stream, (uint8_t*)&worker->duty_cycle, sizeof(float));
        if(size != sizeof(float) || worker->duty_cycle < 0.0f || worker->duty_cycle > 1.0f) {
            file_valid = false;
        }

        if(!file_valid) {
            FURI_LOG_E(TAG_EMULATE, "Invalid file header");
            break;
        }

        for(size_t i = 0; i < EMULATE_BUFFER_SIZE; i++) {
            file_valid =
                read_pair(data, &data->emulate_buffer_arr[i], &data->emulate_buffer_ccr[i]);
            if(!file_valid) break;
        }
    } while(false);

    furi_hal_rfid_tim_emulate_dma_start(
        data->emulate_buffer_arr,
        data->emulate_buffer_ccr,
        EMULATE_BUFFER_SIZE,
        rfid_emulate_dma_isr,
        &data->ctx);

    if(!file_valid && worker->emulate_callback != NULL) {
        // message file_error to worker
        worker->emulate_callback(LFRFIDWorkerEmulateRawFileError, worker->context);
    }

    if(file_valid) {
        uint32_t flag = 0;

        while(true) {
            size_t size = xStreamBufferReceive(data->ctx.stream, &flag, sizeof(uint32_t), 100);

            if(size == sizeof(uint32_t)) {
                size_t start = 0;

                if(flag == HalfTransfer) {
                    start = 0;
                } else if(flag == TransferComplete) {
                    start = (EMULATE_BUFFER_SIZE / 2);
                }

                for(size_t i = 0; i < (EMULATE_BUFFER_SIZE / 2); i++) {
                    file_valid = read_pair(
                        data,
                        &data->emulate_buffer_arr[start + i],
                        &data->emulate_buffer_ccr[start + i]);
                    if(!file_valid) break;
                }
            } else if(size != 0) {
                data->ctx.overrun_count++;
            }

            if(!file_valid) {
                if(worker->emulate_callback != NULL) {
                    // message file_error to worker
                    worker->emulate_callback(LFRFIDWorkerEmulateRawFileError, worker->context);
                }
                break;
            }

            if(data->ctx.overrun_count > 0 && worker->emulate_callback != NULL) {
                // message overrun to worker
                worker->emulate_callback(LFRFIDWorkerEmulateRawOverrun, worker->context);
            }

            uint32_t flags = osEventFlagsGet(worker->events);
            if(FURI_BIT(flags, LFRFIDRawWorkerEventStop)) {
                break;
            };
        }
    }

    furi_hal_rfid_tim_emulate_dma_stop();

    if(!file_valid) {
        const uint32_t available_flags = (1 << LFRFIDRawWorkerEventStop);
        while(true) {
            uint32_t flags =
                osEventFlagsWait(worker->events, available_flags, osFlagsWaitAny, osWaitForever);

            if(FURI_BIT(flags, LFRFIDRawWorkerEventStop)) {
                break;
            };
        }
    }

    if(data->ctx.overrun_count) {
        FURI_LOG_E(TAG_EMULATE, "overruns: %lu", data->ctx.overrun_count);
    }

    stream_free(data->stream);
    vStreamBufferDelete(data->ctx.stream);
    furi_record_close("storage");
    free(data);

    return 0;
}