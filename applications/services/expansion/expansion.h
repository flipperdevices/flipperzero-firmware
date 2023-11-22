#pragma once

#include <furi_hal_serial_types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RECORD_EXPANSION "expansion"

#define EXPANSION_DEFAULT_BAUD_RATE (9600)

typedef struct Expansion Expansion;

void expansion_enable(Expansion* instance, FuriHalSerialId serial_id);

void expansion_disable(Expansion* instance);

#ifdef __cplusplus
}
#endif
