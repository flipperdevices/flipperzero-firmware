#pragma once
#include <furi-hal-gpio.h>
#include "ibutton_key.h"

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

iButtonWriter* ibutton_writer_alloc(const GpioPin* gpio);
void ibutton_writer_free(iButtonWriter* writer);
iButtonWriterResult ibutton_writer_write(iButtonWriter* writer, iButtonKey* key);
void start(iButtonWriter* writer);
void stop(iButtonWriter* writer);

#ifdef __cplusplus
}
#endif