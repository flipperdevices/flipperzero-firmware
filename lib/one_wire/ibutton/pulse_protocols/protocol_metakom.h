/**
 * @file protocol_metakom.h
 * @author Sergey Gavrilov (who.just.the.doctor@gmail.com)
 * @version 1.0
 * @date 2021-11-18
 * 
 * Metakom pulse format decoder
 */

#pragma once
#include <stdint.h>
#include "../../pulse_protocols/pulse_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ProtocolMetakom ProtocolMetakom;

/**
 * Allocate decoder
 * @return ProtocolMetakom* 
 */
ProtocolMetakom* protocol_metakom_alloc();

/**
 * Free decoder
 * @param metakom 
 */
void protocol_metakom_free(ProtocolMetakom* metakom);

/**
 * Get protocol interface
 * @param metakom 
 * @return PulseProtocol* 
 */
PulseProtocol* protocol_metakom_get_protocol(ProtocolMetakom* metakom);

#ifdef __cplusplus
}
#endif