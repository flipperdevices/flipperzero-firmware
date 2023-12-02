#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct Mhz19 Mhz19;

Mhz19* mhz19_alloc();

void mhz19_free(Mhz19* instance);

void mhz19_open(Mhz19* instance);

void mhz19_close(Mhz19* instance);

bool mhz19_read_gas_concentration(Mhz19* instance, uint32_t* value);
