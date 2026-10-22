#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <nfc/nfc_device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NdefNtagTypeNTAG213 = 0,
    NdefNtagTypeNTAG215 = 1,
    NdefNtagTypeNTAG216 = 2,
} NdefNtagType;

/**
 * Populate `nfc_device` with a freshly-built NTAG21x dump (default UID/version/CC)
 * whose user memory starts with the provided NDEF Message TLV blob.
 *
 * The poller's write flow starts at page 4, so we leave pages 0-3 with sane
 * defaults (UID + CC) — only user pages (4..config_page-1) carry the NDEF data.
 *
 * @return false if the blob doesn't fit in the chosen NTAG's user memory.
 */
bool mf_ultralight_ndef_fill_device(
    NfcDevice* nfc_device,
    NdefNtagType ntag,
    const uint8_t* ndef_blob,
    size_t ndef_size);

/** Human-readable name, e.g. "NTAG215". */
const char* mf_ultralight_ndef_ntag_name(NdefNtagType ntag);

/** User-memory capacity in bytes for the given NTAG (the NDEF TLV blob must fit here). */
size_t mf_ultralight_ndef_user_capacity(NdefNtagType ntag);

#ifdef __cplusplus
}
#endif
