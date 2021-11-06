#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <furi-hal-gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OneWireDevice OneWireDevice;
typedef struct OneWireSlave OneWireSlave;
typedef void (*OneWireSlaveResultCallback)(void* context);

OneWireSlave* onewire_slave_alloc(const GpioPin* pin);
void onewire_slave_free(OneWireSlave* bus);

void onewire_slave_start(OneWireSlave* bus);
void onewire_slave_stop(OneWireSlave* bus);

void onewire_slave_attach(OneWireSlave* bus, OneWireDevice* device);
void onewire_slave_detach(OneWireSlave* bus);

bool onewire_slave_send(OneWireSlave* bus, const uint8_t* address, const uint8_t data_length);
bool onewire_slave_receive(OneWireSlave* bus, uint8_t* data, const uint8_t data_length);

void onewire_slave_set_result_callback(
    OneWireSlave* bus,
    OneWireSlaveResultCallback result_cb,
    void* context);

#ifdef __cplusplus
}
#endif
