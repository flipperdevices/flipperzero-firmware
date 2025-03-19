#pragma once

#include <furi.h>
#include <nfc/nfc.h>
#include <nfc/nfc_poller.h>

typedef struct {
    bool select;
    bool keep_field;
    bool append_crc;
    NfcProtocol protocol;
    BitBuffer* tx_buffer;
} NfcCliRawRequest;

typedef struct {
    BitBuffer* rx_buffer;
    FuriString* activation_string;
} NfcCliRawResponse;
