#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>

void u2f_init();

uint16_t u2f_msg_parse(uint8_t* buf, uint16_t len);

uint32_t u2f_get_random();

#ifdef __cplusplus
}
#endif
