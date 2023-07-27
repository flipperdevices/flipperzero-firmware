#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>

#define U2F_DATA_FOLDER EXT_PATH("u2f/")
#define U2F_CERT_FILE U2F_DATA_FOLDER "assets/cert.der"
#define U2F_CERT_KEY_FILE U2F_DATA_FOLDER "assets/cert_key.u2f"
#define U2F_KEY_OLD_FILE U2F_DATA_FOLDER "key.u2f"
#define U2F_CNT_OLD_FILE INT_PATH(".cnt.u2f")
#define U2F_KEY_FILE INT_PATH(".key.u2f")
#define U2F_CNT_FILE U2F_DATA_FOLDER "cnt.u2f"

bool u2f_data_check(bool cert_only);

bool u2f_data_cert_check();

uint32_t u2f_data_cert_load(uint8_t* cert);

bool u2f_data_cert_key_load(uint8_t* cert_key);

bool u2f_data_key_load(uint8_t* device_key);

bool u2f_data_key_generate(uint8_t* device_key);

bool u2f_data_cnt_read(uint32_t* cnt);

bool u2f_data_cnt_write(uint32_t cnt);

#ifdef __cplusplus
}
#endif
