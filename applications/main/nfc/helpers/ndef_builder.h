#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum NDEF blob size we will produce — fits comfortably in NTAG216 (888 user bytes). */
#define NDEF_BUILDER_MAX_SIZE 800

/**
 * Build a complete NDEF Message TLV (0x03 .. 0xFE terminator) for a URI/URL record.
 *
 * Common prefixes (http://, https://, tel:, mailto: …) are encoded as a one-byte
 * URI identifier code per the NFC Forum URI RTD, so they don't take payload space.
 *
 * @param uri               null-terminated input string
 * @param out               destination buffer
 * @param out_capacity      destination capacity in bytes
 * @param out_size          on success, set to the number of bytes written
 * @return                  true if the blob fit; false otherwise
 */
bool ndef_builder_build_uri(const char* uri, uint8_t* out, size_t out_capacity, size_t* out_size);

/**
 * Build a complete NDEF Message TLV for a plain Text record (UTF-8, language code "en").
 */
bool ndef_builder_build_text(const char* text, uint8_t* out, size_t out_capacity, size_t* out_size);

/**
 * Build a complete NDEF Message TLV for a Wi-Fi Simple Configuration credential
 * (WPS Credential, application/vnd.wfa.wsc) using WPA2-PSK / AES.
 *
 * @param ssid       null-terminated SSID
 * @param password   null-terminated passphrase (8..63 chars typical)
 */
bool ndef_builder_build_wifi_wpa2(
    const char* ssid,
    const char* password,
    uint8_t* out,
    size_t out_capacity,
    size_t* out_size);

#ifdef __cplusplus
}
#endif
