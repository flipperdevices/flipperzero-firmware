#include "stdint.h"
#include "furi_hal_i2c.h"
#include <stddef.h>
#include <stdint.h>

void write_dump(uint8_t* buffer, size_t size);
void dump(uint8_t* out);
void write_credit(float value);
float read_credit();
void calc_credit(float value, uint8_t* result);
