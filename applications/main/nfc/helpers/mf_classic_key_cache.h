#pragma once

#include <nfc/protocols/mf_classic/mf_classic.h>

#ifdef __cplusplus
extern "C" {
#endif

bool mf_classic_key_cache_save(const MfClassicData* data);

bool mf_classic_key_cache_load(const uint8_t* uid, size_t uid_len, MfClassicDeviceKeys* keys);

#ifdef __cplusplus
}
#endif
