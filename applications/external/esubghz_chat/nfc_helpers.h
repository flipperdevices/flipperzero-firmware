#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_MAX_BYTES 256
#define NFC_CONFIG_PAGES 4

struct ReplayDictNfcEntry {
    uint64_t run_id;
    uint32_t counter;
    uint32_t unused;
} __attribute__((packed));

#ifdef __cplusplus
}
#endif
