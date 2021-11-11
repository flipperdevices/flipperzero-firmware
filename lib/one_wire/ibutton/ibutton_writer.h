#pragma once
#include <furi-hal-gpio.h>
#include "ibutton_key.h"
#include "../one_wire_host.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    iButtonWriterOK,
    iButtonWriterSameKey,
    iButtonWriterNoDetect,
    iButtonWriterCannotWrite,
} iButtonWriterResult;

typedef struct iButtonWriter iButtonWriter;

iButtonWriter* ibutton_writer_alloc(OneWireHost* host);
void ibutton_writer_free(iButtonWriter* writer);
iButtonWriterResult ibutton_writer_write(iButtonWriter* writer, iButtonKey* key);
void ibutton_writer_start(iButtonWriter* writer);
void ibutton_writer_stop(iButtonWriter* writer);

#ifdef __cplusplus
}
#endif