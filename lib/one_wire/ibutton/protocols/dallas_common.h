#pragma once

#include <one_wire/one_wire_host.h>

#define DALLAS_COMMON_ROM_SIZE 8U

bool dallas_common_read_rom(OneWireHost* host, uint8_t* data);
