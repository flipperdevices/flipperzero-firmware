#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <lib/digital_signal/digital_signal.h>

#define NFCA_MAX_UID_LEN (10)

typedef struct {
    uint8_t uid[NFCA_MAX_UID_LEN];
    uint8_t uid_len;
    uint8_t atqa[2];
    uint8_t sak;
    bool iso14443_4_compliant;
} NfcaData;

typedef struct {
    DigitalSignal* one;
    DigitalSignal* zero;
    DigitalSignal* tx_signal;
} NfcaSignal;

uint16_t nfca_get_crc16(uint8_t* buff, uint16_t len);

void nfca_append_crc16(uint8_t* buff, uint16_t len);

bool nfca_emulation_handler(
    uint8_t* buff_rx,
    uint16_t buff_rx_len,
    uint8_t* buff_tx,
    uint16_t* buff_tx_len);

NfcaSignal* nfca_signal_alloc();

void nfca_signal_free(NfcaSignal* nfca_signal);

void nfca_signal_encode(NfcaSignal* nfca_signal, uint8_t* data, uint16_t bits, uint8_t* parity);

bool nfca_poller_check_presence();

bool nfca_poller_activate(NfcaData* nfca_data);

void nfca_poller_sleep();
