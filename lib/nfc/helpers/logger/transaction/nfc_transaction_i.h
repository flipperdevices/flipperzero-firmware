#pragma once

#include "nfc_transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t nfc_transaction_get_size(uint8_t history_size_bytes);
NfcTransaction* nfc_transaction_alloc(
    uint32_t id,
    FuriHalNfcEvent event,
    uint32_t time,
    uint8_t history_size_bytes,
    uint8_t max_chain_count);

void nfc_transaction_refresh(
    NfcTransaction* instance,
    uint32_t id,
    FuriHalNfcEvent event,
    uint32_t time);

void nfc_transaction_complete(NfcTransaction* instance, uint32_t time);

NfcTransactionType nfc_transaction_get_type(const NfcTransaction* instance);
uint32_t nfc_transaction_get_id(const NfcTransaction* instance);

void nfc_transaction_append(
    NfcTransaction* transaction,
    const uint32_t time,
    const uint8_t* data,
    const size_t data_size,
    bool response);
void nfc_transaction_append_history(NfcTransaction* transaction, NfcHistoryItem* item);
bool nfc_transaction_save(Stream* stream, const NfcTransaction* transaction);

#ifdef __cplusplus
}
#endif
