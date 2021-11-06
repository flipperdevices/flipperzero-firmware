#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <furi-hal-gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CONDITIONAL_SEARCH = 0,
    NORMAL_SEARCH = 1,
} OneWireHostSearchMode;

typedef struct OneWireHost OneWireHost;

OneWireHost* onewire_host_alloc(const GpioPin* gpio);
void onewire_host_free(OneWireHost* host);
bool onewire_host_reset(OneWireHost* host);
bool onewire_host_read_bit(OneWireHost* host);
uint8_t onewire_host_read(OneWireHost* host);
void onewire_host_read_bytes(OneWireHost* host, uint8_t* buffer, uint16_t count);
void onewire_host_write_bit(OneWireHost* host, bool value);
void onewire_host_write(OneWireHost* host, uint8_t value);
void onewire_host_skip(OneWireHost* host);
void onewire_host_start(OneWireHost* host);
void onewire_host_stop(OneWireHost* host);
void onewire_host_reset_search(OneWireHost* host);
void onewire_host_target_search(OneWireHost* host, uint8_t family_code);
uint8_t onewire_host_search(OneWireHost* host, uint8_t* newAddr, OneWireHostSearchMode mode);

#ifdef __cplusplus
}
#endif