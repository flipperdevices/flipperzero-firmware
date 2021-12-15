#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>

bool u2f_cert_check();

uint32_t u2f_cert_load(uint8_t* cert);

bool u2f_cert_key_load(uint8_t* cert_key);

bool u2f_key_load(uint8_t* device_key);

bool u2f_key_generate(uint8_t* device_key);

bool u2f_cnt_read(uint32_t* cnt);

bool u2f_cnt_write(uint32_t cnt);

#ifdef __cplusplus
}
#endif
