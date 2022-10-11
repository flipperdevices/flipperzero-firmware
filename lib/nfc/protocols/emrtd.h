#pragma once

#include <furi_hal_nfc.h>

typedef struct {
    uint8_t aid[16];
    uint8_t aid_len;
} EmrtdApplication;

/** Read bank card data
 * @note Try to find LDS1 app
 *
 * @param tx_rx     FuriHalNfcTxRxContext instance
 * @param id_app   IdApplication instance
 * 
 * @return true on success
 */
bool read_id_card(FuriHalNfcTxRxContext* tx_rx, EmrtdApplication* id_app);
