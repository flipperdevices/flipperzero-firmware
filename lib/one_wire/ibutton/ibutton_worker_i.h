#pragma once
#include "ibutton_worker.h"
#include "ibutton_writer.h"
#include "../one_wire_host.h"
#include "../one_wire_slave.h"
#include "../one_wire_device.h"
#include "../pulse_decoder/pulse_decoder.h"
#include "../pulse_decoder/ibutton/protocol_cyfral.h"
#include "../pulse_decoder/ibutton/protocol_metakom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PulseProtocolCyfral,
    PulseProtocolMetakom,
} PulseProtocols;

typedef struct {
    const uint32_t quant;
    const void (*start)(iButtonWorker* worker);
    const void (*tick)(iButtonWorker* worker);
    const void (*stop)(iButtonWorker* worker);
} iButtonWorkerModeType;

typedef enum {
    iButtonWorkerIdle = 0,
    iButtonWorkerRead = 1,
    iButtonWorkerWrite = 2,
    iButtonWorkerEmulate = 3,
} iButtonWorkerMode;

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
    iButtonWorkerWriteCallback write_cb;
    iButtonWorkerEmulateCallback emulate_cb;
    void* cb_ctx;
};

extern const iButtonWorkerModeType ibutton_worker_modes[];

void ibutton_worker_switch_mode(iButtonWorker* worker, iButtonWorkerMode mode);

#ifdef __cplusplus
}
#endif