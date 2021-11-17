#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PulseProtocol PulseProtocol;

typedef void (*PulseProtocolPulseCallback)(void* context, bool polarity, uint32_t length);
typedef void (*PulseProtocolResetCallback)(void* context);
typedef void (*PulseProtocolGetDataCallback)(void* context, uint8_t* data, size_t length);
typedef bool (*PulseProtocolDecodedCallback)(void* context);

PulseProtocol* pulse_protocol_alloc();
void pulse_protocol_free(PulseProtocol* protocol);

void pulse_protocol_set_context(PulseProtocol* protocol, void* context);
void pulse_protocol_set_pulse_cb(PulseProtocol* protocol, PulseProtocolPulseCallback callback);
void pulse_protocol_set_reset_cb(PulseProtocol* protocol, PulseProtocolResetCallback callback);
void pulse_protocol_set_get_data_cb(PulseProtocol* protocol, PulseProtocolGetDataCallback callback);
void pulse_protocol_set_decoded_cb(PulseProtocol* protocol, PulseProtocolDecodedCallback callback);

void pulse_protocol_process_pulse(PulseProtocol* protocol, bool polarity, uint32_t length);
bool pulse_protocol_decoded(PulseProtocol* protocol);
void pulse_protocol_get_data(PulseProtocol* protocol, uint8_t* data, size_t length);
void pulse_protocol_reset(PulseProtocol* protocol);

#ifdef __cplusplus
}
#endif