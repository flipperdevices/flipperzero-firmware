/**
 * @file nfc_read_memory.h
 * @brief Free heap required to read a card without running out of memory.
 */
#pragma once

#include <furi.h>

#include <nfc/protocols/nfc_protocol.h>
#include <nfc/protocols/felica/felica.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>
#include <nfc/protocols/st25tb/st25tb.h>

// Measured ISO14443-4A read peak (~8.4 KiB, incl. parser plugins) plus margin
#define NFC_READ_MEMORY_BASE_FREE_HEAP (12 * 1024)
// Parser plugins are loaded into a single block
#define NFC_READ_MEMORY_MIN_HEAP_BLOCK (8 * 1024)

/**
 * @brief Get the free heap required to read a protocol.
 *
 * @param protocol protocol to be read, or NfcProtocolInvalid if not detected yet.
 * @return required free heap in bytes.
 */
static inline size_t nfc_read_memory_required(NfcProtocol protocol) {
    size_t data_size = 0;

    switch(protocol) {
    case NfcProtocolMfClassic:
        data_size = sizeof(MfClassicData);
        break;
    case NfcProtocolMfUltralight:
        data_size = sizeof(MfUltralightData);
        break;
    case NfcProtocolFelica:
        data_size = sizeof(FelicaData);
        break;
    case NfcProtocolSt25tb:
        data_size = sizeof(St25tbData);
        break;
    default:
        break;
    }

    // The poller and NfcDevice each hold a copy of the data
    return NFC_READ_MEMORY_BASE_FREE_HEAP + 2 * data_size;
}

/**
 * @brief Check whether a heap state allows reading a protocol.
 *
 * @param protocol protocol to be read, or NfcProtocolInvalid if not detected yet.
 * @param free_heap total free heap in bytes.
 * @param max_free_block largest free block in bytes.
 * @return true if the read can be started.
 */
static inline bool
    nfc_read_memory_is_enough(NfcProtocol protocol, size_t free_heap, size_t max_free_block) {
    return free_heap >= nfc_read_memory_required(protocol) &&
           max_free_block >= NFC_READ_MEMORY_MIN_HEAP_BLOCK;
}

/**
 * @brief Check whether the current heap allows reading a protocol.
 *
 * @param protocol protocol to be read, or NfcProtocolInvalid if not detected yet.
 * @return true if the read can be started.
 */
static inline bool nfc_read_memory_is_available(NfcProtocol protocol) {
    return nfc_read_memory_is_enough(
        protocol, memmgr_get_free_heap(), memmgr_heap_get_max_free_block());
}
