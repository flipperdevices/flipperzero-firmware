#include <furi.h>
#include <furi-hal.h>
#include <atomic.h>
#include "ibutton_worker.h"
#include "ibutton_writer.h"
#include "ibutton_key_command.h"
#include "../one_wire_host.h"
#include "../one_wire_slave.h"
#include "../one_wire_device.h"

#include "../pulse_decoder/pulse_decoder.h"
#include "../pulse_decoder/ibutton/protocol_cyfral.h"
#include "../pulse_decoder/ibutton/protocol_metakom.h"

#define IBUTTON_WORKER_DONOTHING_QUANT osWaitForever
#define IBUTTON_WORKER_READ_QUANT 100
#define IBUTTON_WORKER_WRITE_QUANT 1000
#define IBUTTON_WORKER_EMULATE_QUANT 1000

typedef enum {
    iButtonMessageEnd,
    iButtonMessageStop,
    iButtonMessageRead,
    iButtonMessageWrite,
    iButtonMessageEmulate,
} iButtonMessageType;

typedef enum {
    PulseProtocolCyfral,
    PulseProtocolMetakom,
} PulseProtocols;

typedef struct {
    iButtonMessageType type;
    union {
        iButtonKey* key;
    } data;
} iButtonMessage;

typedef struct {
    const uint32_t quant;
    const void (*start)(iButtonWorker* worker);
    const void (*tick)(iButtonWorker* worker);
    const void (*stop)(iButtonWorker* worker);
} iButtonWorkerModeType;

static void ibutton_worker_mode_do_nothing_start(iButtonWorker* worker);
static void ibutton_worker_mode_do_nothing_tick(iButtonWorker* worker);
static void ibutton_worker_mode_do_nothing_stop(iButtonWorker* worker);

static void ibutton_worker_mode_emulate_start(iButtonWorker* worker);
static void ibutton_worker_mode_emulate_tick(iButtonWorker* worker);
static void ibutton_worker_mode_emulate_stop(iButtonWorker* worker);

static void ibutton_worker_mode_read_start(iButtonWorker* worker);
static void ibutton_worker_mode_read_tick(iButtonWorker* worker);
static void ibutton_worker_mode_read_stop(iButtonWorker* worker);

static void ibutton_worker_mode_write_start(iButtonWorker* worker);
static void ibutton_worker_mode_write_tick(iButtonWorker* worker);
static void ibutton_worker_mode_write_stop(iButtonWorker* worker);

typedef enum {
    iButtonWorkerDoNothing = 0,
    iButtonWorkerRead = 1,
    iButtonWorkerWrite = 2,
    iButtonWorkerEmulate = 3,
} iButtonWorkerMode;

const iButtonWorkerModeType ibutton_worker_modes[] = {
    {
        .quant = osWaitForever,
        .start = ibutton_worker_mode_do_nothing_start,
        .tick = ibutton_worker_mode_do_nothing_tick,
        .stop = ibutton_worker_mode_do_nothing_stop,
    },
    {
        .quant = 100,
        .start = ibutton_worker_mode_read_start,
        .tick = ibutton_worker_mode_read_tick,
        .stop = ibutton_worker_mode_read_stop,
    },
    {
        .quant = 1000,
        .start = ibutton_worker_mode_write_start,
        .tick = ibutton_worker_mode_write_tick,
        .stop = ibutton_worker_mode_write_stop,
    },
    {
        .quant = 1000,
        .start = ibutton_worker_mode_emulate_start,
        .tick = ibutton_worker_mode_emulate_tick,
        .stop = ibutton_worker_mode_emulate_stop,
    },
};

struct iButtonWorker {
    iButtonKey* key_p;
    uint8_t* key_data;
    OneWireHost* host;
    OneWireSlave* slave;
    OneWireDevice* device;
    iButtonWriter* writer;
    const GpioPin* gpio;
    iButtonWorkerMode mode_index;
    osMessageQueueId_t messages;
    FuriThread* thread;

    PulseDecoder* pulse_decoder;
    ProtocolCyfral* protocol_cyfral;
    ProtocolMetakom* protocol_metakom;
    uint32_t last_dwt_value;

    iButtonWorkerReadCallback read_cb;
    void* read_cb_ctx;
    iButtonWorkerWriteCallback write_cb;
    void* write_cb_ctx;
    iButtonWorkerEmulateCallback emulate_cb;
    void* emulate_cb_ctx;
};

void ibutton_worker_switch_mode(iButtonWorker* worker, iButtonWorkerMode mode);
void ibutton_worker_set_key(iButtonWorker* worker, iButtonKey* key);
static int32_t ibutton_worker_thread(void* thread_context);

iButtonWorker* ibutton_worker_alloc() {
    iButtonWorker* worker = malloc(sizeof(iButtonWorker));
    worker->gpio = &ibutton_gpio;
    worker->key_p = NULL;
    worker->key_data = malloc(ibutton_key_get_max_size());
    worker->host = onewire_host_alloc(worker->gpio);
    worker->slave = onewire_slave_alloc(worker->gpio);
    worker->writer = ibutton_writer_alloc(worker->host);
    worker->device = onewire_device_alloc(0, 0, 0, 0, 0, 0, 0, 0);
    worker->pulse_decoder = pulse_decoder_alloc();
    worker->protocol_cyfral = protocol_cyfral_alloc();
    worker->protocol_metakom = protocol_metakom_alloc();
    worker->messages = osMessageQueueNew(1, sizeof(iButtonMessage), NULL);
    worker->mode_index = iButtonWorkerDoNothing;
    worker->last_dwt_value = 0;
    worker->read_cb = NULL;
    worker->read_cb_ctx = NULL;
    worker->write_cb = NULL;
    worker->write_cb_ctx = NULL;
    worker->emulate_cb = NULL;
    worker->emulate_cb_ctx = NULL;

    worker->thread = furi_thread_alloc();
    furi_thread_set_name(worker->thread, "ibutton_worker");
    furi_thread_set_callback(worker->thread, ibutton_worker_thread);
    furi_thread_set_context(worker->thread, worker);
    furi_thread_set_stack_size(worker->thread, 2048);

    pulse_decoder_add_protocol(
        worker->pulse_decoder,
        protocol_cyfral_get_protocol(worker->protocol_cyfral),
        PulseProtocolCyfral);
    pulse_decoder_add_protocol(
        worker->pulse_decoder,
        protocol_metakom_get_protocol(worker->protocol_metakom),
        PulseProtocolMetakom);

    return worker;
}

void ibutton_worker_read_set_callback(
    iButtonWorker* worker,
    iButtonWorkerReadCallback callback,
    void* context) {
    furi_check(worker->mode_index == iButtonWorkerDoNothing);
    worker->read_cb = callback;
    worker->read_cb_ctx = context;
}
void ibutton_worker_write_set_callback(
    iButtonWorker* worker,
    iButtonWorkerWriteCallback callback,
    void* context) {
    furi_check(worker->mode_index == iButtonWorkerDoNothing);
    worker->write_cb = callback;
    worker->write_cb_ctx = context;
}

void ibutton_worker_read_start(iButtonWorker* worker, iButtonKey* key) {
    iButtonMessage message = {.type = iButtonMessageRead, .data.key = key};
    furi_check(osMessageQueuePut(worker->messages, &message, 0, osWaitForever) == osOK);
}

void ibutton_worker_read_stop(iButtonWorker* worker) {
    iButtonMessage message = {.type = iButtonMessageStop};
    furi_check(osMessageQueuePut(worker->messages, &message, 0, osWaitForever) == osOK);
}

void ibutton_worker_write_start(iButtonWorker* worker, iButtonKey* key) {
    iButtonMessage message = {.type = iButtonMessageWrite, .data.key = key};
    furi_check(osMessageQueuePut(worker->messages, &message, 0, osWaitForever) == osOK);
}

void ibutton_worker_write_stop(iButtonWorker* worker) {
    iButtonMessage message = {.type = iButtonMessageStop};
    furi_check(osMessageQueuePut(worker->messages, &message, 0, osWaitForever) == osOK);
}

void ibutton_worker_free(iButtonWorker* worker) {
    pulse_decoder_free(worker->pulse_decoder);
    protocol_cyfral_free(worker->protocol_cyfral);

    ibutton_writer_free(worker->writer);

    onewire_slave_stop(worker->slave);
    onewire_slave_free(worker->slave);

    onewire_host_stop(worker->host);
    onewire_host_free(worker->host);
    onewire_device_free(worker->device);

    osMessageQueueDelete(worker->messages);

    furi_thread_free(worker->thread);
    free(worker->key_data);
    free(worker);
}

void ibutton_worker_start_thread(iButtonWorker* worker) {
    furi_thread_start(worker->thread);
}

void ibutton_worker_stop_thread(iButtonWorker* worker) {
    iButtonMessage message = {.type = iButtonMessageEnd};
    furi_check(osMessageQueuePut(worker->messages, &message, 0, osWaitForever) == osOK);
    furi_thread_join(worker->thread);
}

extern COMP_HandleTypeDef hcomp1;

void ibutton_worker_comparator_callback(void* hcomp, void* context) {
    iButtonWorker* worker = context;

    if(hcomp == &hcomp1) {
        uint32_t current_dwt_value = DWT->CYCCNT;

        pulse_decoder_process_pulse(
            worker->pulse_decoder,
            hal_gpio_get_rfid_in_level(),
            current_dwt_value - worker->last_dwt_value);

        worker->last_dwt_value = current_dwt_value;
    }
}

bool ibutton_worker_read_comparator(iButtonWorker* worker) {
    bool result = false;

    pulse_decoder_reset(worker->pulse_decoder);
    hal_gpio_init(&gpio_rfid_pull, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    hal_gpio_write(&gpio_rfid_pull, false);

    hal_gpio_init(&gpio_rfid_carrier_out, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    hal_gpio_write(&gpio_rfid_carrier_out, false);

    api_interrupt_add(ibutton_worker_comparator_callback, InterruptTypeComparatorTrigger, worker);

    worker->last_dwt_value = DWT->CYCCNT;
    HAL_COMP_Start(&hcomp1);

    // TODO: rework with thread events, "pulse_decoder_get_decoded_index_with_timeout"
    delay(100);
    int32_t decoded_index = pulse_decoder_get_decoded_index(worker->pulse_decoder);
    if(decoded_index >= 0) {
        pulse_decoder_get_data(
            worker->pulse_decoder, decoded_index, worker->key_data, ibutton_key_get_max_size());
    }

    switch(decoded_index) {
    case PulseProtocolCyfral:
        furi_check(worker->key_p != NULL);
        ibutton_key_set_type(worker->key_p, iButtonKeyCyfral);
        ibutton_key_set_data(worker->key_p, worker->key_data, ibutton_key_get_max_size());
        result = true;
        break;
    case PulseProtocolMetakom:
        furi_check(worker->key_p != NULL);
        ibutton_key_set_type(worker->key_p, iButtonKeyMetakom);
        ibutton_key_set_data(worker->key_p, worker->key_data, ibutton_key_get_max_size());
        result = true;
        break;
        break;
    default:
        break;
    }

    HAL_COMP_Stop(&hcomp1);
    api_interrupt_remove(ibutton_worker_comparator_callback, InterruptTypeComparatorTrigger);
    furi_hal_rfid_pins_reset();

    return result;
}

bool ibutton_worker_read_dallas(iButtonWorker* worker) {
    bool result = false;
    onewire_host_start(worker->host);
    delay(100);
    __disable_irq();
    if(onewire_host_search(worker->host, worker->key_data, NORMAL_SEARCH)) {
        onewire_host_reset_search(worker->host);

        // key found, verify
        if(onewire_host_reset(worker->host)) {
            onewire_host_write(worker->host, DS1990_CMD_READ_ROM);
            bool key_valid = true;
            for(uint8_t i = 0; i < ibutton_key_get_max_size(); i++) {
                if(onewire_host_read(worker->host) != worker->key_data[i]) {
                    key_valid = false;
                    break;
                }
            }

            if(key_valid) {
                result = true;

                furi_check(worker->key_p != NULL);
                ibutton_key_set_type(worker->key_p, iButtonKeyDS1990);
                ibutton_key_set_data(worker->key_p, worker->key_data, ibutton_key_get_max_size());
            }
        }
    } else {
        onewire_host_reset_search(worker->host);
    }
    onewire_host_stop(worker->host);
    __enable_irq();
    return result;
}

static void ibutton_worker_mode_read_start(iButtonWorker* worker) {
    furi_hal_power_enable_otg();
}
static void ibutton_worker_mode_read_tick(iButtonWorker* worker) {
    bool valid = false;
    if(ibutton_worker_read_dallas(worker)) {
        valid = true;
    } else if(ibutton_worker_read_comparator(worker)) {
        valid = true;
    }

    if(valid) {
        if(worker->read_cb != NULL) {
            worker->read_cb(worker->read_cb_ctx);
        }

        ibutton_worker_switch_mode(worker, iButtonWorkerDoNothing);
    }
}
static void ibutton_worker_mode_read_stop(iButtonWorker* worker) {
    furi_hal_power_disable_otg();
}

static void ibutton_worker_mode_do_nothing_start(iButtonWorker* worker) {
}
static void ibutton_worker_mode_do_nothing_tick(iButtonWorker* worker) {
}
static void ibutton_worker_mode_do_nothing_stop(iButtonWorker* worker) {
}

static void ibutton_worker_mode_emulate_start(iButtonWorker* worker) {
}
static void ibutton_worker_mode_emulate_tick(iButtonWorker* worker) {
}
static void ibutton_worker_mode_emulate_stop(iButtonWorker* worker) {
}

static void ibutton_worker_mode_write_start(iButtonWorker* worker) {
    furi_hal_power_enable_otg();
    onewire_host_start(worker->host);
}

static void ibutton_worker_mode_write_tick(iButtonWorker* worker) {
    furi_check(worker->key_p != NULL);
    iButtonWriterResult writer_result = ibutton_writer_write(worker->writer, worker->key_p);
    iButtonWorkerWriteResult result;
    switch(writer_result) {
    case iButtonWriterOK:
        result = iButtonWorkerWriteOK;
        break;
    case iButtonWriterSameKey:
        result = iButtonWorkerWriteSameKey;
        break;
    case iButtonWriterNoDetect:
        result = iButtonWorkerWriteNoDetect;
        break;
    case iButtonWriterCannotWrite:
        result = iButtonWorkerWriteCannotWrite;
        break;
    default:
        result = iButtonWorkerWriteNoDetect;
        break;
    }

    if(worker->write_cb != NULL) {
        worker->write_cb(worker->write_cb_ctx, result);
    }
}

static void ibutton_worker_mode_write_stop(iButtonWorker* worker) {
    furi_hal_power_disable_otg();
    onewire_host_stop(worker->host);
}

void ibutton_worker_switch_mode(iButtonWorker* worker, iButtonWorkerMode mode) {
    ibutton_worker_modes[worker->mode_index].stop(worker);
    worker->mode_index = mode;
    ibutton_worker_modes[worker->mode_index].start(worker);
}

void ibutton_worker_set_key_p(iButtonWorker* worker, iButtonKey* key) {
    worker->key_p = key;
}

static int32_t ibutton_worker_thread(void* thread_context) {
    iButtonWorker* worker = thread_context;
    bool running = true;
    iButtonMessage message;
    osStatus_t status;

    ibutton_worker_modes[worker->mode_index].start(worker);

    while(running) {
        status = osMessageQueueGet(
            worker->messages, &message, NULL, ibutton_worker_modes[worker->mode_index].quant);
        if(status == osOK) {
            switch(message.type) {
            case iButtonMessageEnd:
                ibutton_worker_set_key_p(worker, NULL);
                ibutton_worker_switch_mode(worker, iButtonWorkerDoNothing);
                running = false;
                break;
            case iButtonMessageStop:
                ibutton_worker_set_key_p(worker, NULL);
                ibutton_worker_switch_mode(worker, iButtonWorkerDoNothing);
                break;
            case iButtonMessageRead:
                ibutton_worker_set_key_p(worker, message.data.key);
                ibutton_worker_switch_mode(worker, iButtonWorkerRead);
                break;
            case iButtonMessageWrite:
                ibutton_worker_set_key_p(worker, message.data.key);
                ibutton_worker_switch_mode(worker, iButtonWorkerWrite);
                break;
            case iButtonMessageEmulate:
                ibutton_worker_set_key_p(worker, message.data.key);
                ibutton_worker_switch_mode(worker, iButtonWorkerEmulate);
                break;
            }
        } else if(status == osErrorTimeout) {
            ibutton_worker_modes[worker->mode_index].tick(worker);
        } else {
            furi_crash("iButton worker error");
        }
    }

    ibutton_worker_modes[worker->mode_index].stop(worker);

    return 0;
}
