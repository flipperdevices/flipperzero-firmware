#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OneWireSlave OneWireSlave;
typedef struct OneWireDevice OneWireDevice;

OneWireDevice* onewire_device_alloc(
    uint8_t id_1,
    uint8_t id_2,
    uint8_t id_3,
    uint8_t id_4,
    uint8_t id_5,
    uint8_t id_6,
    uint8_t id_7,
    uint8_t id_8);
void onewire_device_free(OneWireDevice* device);
void onewire_device_send_id(OneWireDevice* device);
void onewire_device_attach(OneWireDevice* device, OneWireSlave* bus);
void onewire_device_detach(OneWireDevice* device);
uint8_t* onewire_device_get_id_p(OneWireDevice* device);

#ifdef __cplusplus
}
#endif