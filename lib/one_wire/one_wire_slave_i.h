#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OneWireDevice OneWireDevice;
typedef struct OneWireSlave OneWireSlave;

bool onewire_slave_send(OneWireSlave* bus, const uint8_t* address, const uint8_t data_length);
bool onewire_slave_receive(OneWireSlave* bus, uint8_t* data, const uint8_t data_length);

#ifdef __cplusplus
}
#endif
