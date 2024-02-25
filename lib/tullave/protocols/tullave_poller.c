#include "tullave_poller.h"

#define LOG_TAG "TuLlavePoller"

typedef NfcCommand (*TuLlaveReadHandler)(TuLlavePoller* poller);

void tullave_poller_free(TuLlavePoller* instance) {
    furi_assert(instance);

    tullave_data_free(instance->card_data);
    bit_buffer_free(instance->tx_data);
    bit_buffer_free(instance->rx_data);
    free(instance);
}

TuLlavePoller* tullave_poller_alloc(Iso14443_4aPoller* iso14443_4a_poller) {
    TuLlavePoller* poller = malloc(sizeof(TuLlavePoller));
    poller->iso_poller = iso14443_4a_poller;
    poller->card_data = tullave_data_alloc();
    poller->tx_data = bit_buffer_alloc(TULLAVE_BIT_BUFFER_MAX);
    poller->rx_data = bit_buffer_alloc(TULLAVE_BIT_BUFFER_MAX);
    poller->read_state = TuLlaveStateReadIdle;

    return poller;
}

/**
 * Sends an APDU Command using ISO 14443-4a as transport layer. 
 * It also logs the TX and RX data if debug is enabled on Flipper Zero.
 * 
 * This method is not included in the header, since it is an interal method 
 * 
 * @param poller
 * 
*/
static Iso14443_4aError tullave_iso14443_4a_send_apdu(TuLlavePoller* poller) {
    bit_buffer_reset(poller->rx_data);
    FuriString* tx_log = furi_string_alloc_set_str("TX: ");
    tullave_data_format_bytes(
        tx_log, bit_buffer_get_data(poller->tx_data), bit_buffer_get_size(poller->tx_data));
    FURI_LOG_D(LOG_TAG, furi_string_get_cstr(tx_log));

    Iso14443_4aError error =
        iso14443_4a_poller_send_block(poller->iso_poller, poller->tx_data, poller->rx_data);

    FuriString* rx_log = furi_string_alloc_set_str("RX: ");
    if(error == Iso14443_4aErrorNone) {
        tullave_data_format_bytes(
            rx_log, bit_buffer_get_data(poller->rx_data), bit_buffer_get_size(poller->rx_data));
    } else {
        furi_string_cat_printf(rx_log, "ISO-14443-4a Error: 0x%03x", error);
    }

    FURI_LOG_D(LOG_TAG, furi_string_get_cstr(rx_log));
    return error;
}

NfcCommand tullave_poller_read_initialize(TuLlavePoller* poller) {
    furi_assert(poller);
    bit_buffer_reset(poller->tx_data);
    bit_buffer_reset(poller->rx_data);

    // We need to store the initial ISO 14443-4a data, where resides the UID and other useful information
    // This is for further emulation.
    tullave_data_set_iso_data(poller->card_data, poller->iso_poller->data);
    poller->read_state = TuLlaveStateReadCardNumber;
    return NfcCommandContinue;
}

NfcCommand tullave_poller_read_card_number(TuLlavePoller* poller) {
    furi_assert(poller);
    furi_assert(poller->card_data);

    //Debug command: nfc adpu 00a4040007d4100000030001
    const uint8_t read_card_number_cmd[] = {
        0x00,
        0xa4, // SELECT the app
        0x04,
        0x00, // P1:By name, P2:empty
        0x07, // Lc: Data length
        0xd4,
        0x10,
        0x00,
        0x00,
        0x03,
        0x00,
        0x01 // Application Id
    };

    NfcCommand command = NfcCommandContinue;
    TuLlaveData* data = poller->card_data;

    bit_buffer_copy_bytes(poller->tx_data, read_card_number_cmd, sizeof(read_card_number_cmd));
    Iso14443_4aError error = tullave_iso14443_4a_send_apdu(poller);

    const uint8_t* apdu_resp = bit_buffer_get_data(poller->rx_data);
    if(error == Iso14443_4aErrorNone && apdu_resp[0] == APDU_SW1_GOOD_RESPONSE) {
        uint8_t hexa_card_num[TULLAVE_CARD_NUM_LEN];
        const uint8_t* raw_card_num = &apdu_resp[8];
        uint8_to_hex_chars(raw_card_num, hexa_card_num, TULLAVE_CARD_NUM_LEN);
        data->card_number = furi_string_alloc_printf("%s", hexa_card_num);
        poller->err_code = TuLlaveErrorNone;
        poller->read_state = TuLlaveStateReadBalance;
    } else {
        poller->err_code = TuLlaveErrorCardNotSupported;
        command = NfcCommandStop;
    }
    return command;
}

NfcCommand tullave_poller_read_balance(TuLlavePoller* poller) {
    furi_assert(poller);
    furi_assert(poller->card_data);

    //Debug command: nfc apdu 904c000004
    //Response example: 00000A8C9000
    const uint8_t read_bal_cmd[] = {0x90, 0x4c, 0x00, 0x00, 0x04};
    NfcCommand command = NfcCommandStop;
    TuLlaveData* data = poller->card_data;

    bit_buffer_copy_bytes(poller->tx_data, read_bal_cmd, sizeof(read_bal_cmd));
    Iso14443_4aError error = tullave_iso14443_4a_send_apdu(poller);
    if(error == Iso14443_4aErrorNone) {
        uint8_t c_bal[TULLAVE_BAL_LEN] = {0x0};
        const uint8_t* raw_bal_response = bit_buffer_get_data(poller->rx_data);
        memcpy(c_bal, &raw_bal_response[5], 2);
        memcpy(&c_bal[2], raw_bal_response, 4);
        data->balance = tullave_data_uint8_to_balance(c_bal, TULLAVE_BAL_LEN);
        poller->read_state = TuLlaveStateReadHistory;
        command = NfcCommandContinue;
    }
    return command;
}

NfcCommand tullave_poller_read_history(TuLlavePoller* poller) {
    //Debug command: nfc apdu 00b201242e (index-2 contains number of the transaction)
    //Up to 10 transactions are supported.
    uint8_t read_hist_cmd[] = {0x00, 0xb2, 0x00, 0x24, 0x2e};
    Iso14443_4aError error = Iso14443_4aErrorNone;
    TuLlaveTransaction* trans_record;
    uint8_t c_bal[TULLAVE_BAL_TRAN_LEN] = {0x0};
    simple_array_init(poller->card_data->transaction_history, TULLAVE_HIST_MAX_TRANSACTIONS);
    for(size_t i = 0; i < TULLAVE_HIST_MAX_TRANSACTIONS; i++) {
        read_hist_cmd[2] = i + 1;
        bit_buffer_copy_bytes(poller->tx_data, read_hist_cmd, sizeof(read_hist_cmd));
        error = tullave_iso14443_4a_send_apdu(poller);
        if(error != Iso14443_4aErrorNone) {
            break;
        }
        const uint8_t* raw_response = bit_buffer_get_data(poller->rx_data);
        trans_record = simple_array_get(poller->card_data->transaction_history, i);
        trans_record->datetime = malloc(sizeof(TuLlaveTime));
        // Parse the transaction date
        tullave_data_uint8_to_time(
            &raw_response[26], TULLAVE_TIME_BYTES_LEN, trans_record->datetime);

        // Parse the remaining balance
        memcpy(c_bal, &raw_response[3], 3);
        trans_record->final_balance = tullave_data_uint8_to_balance(c_bal, TULLAVE_BAL_TRAN_LEN);

        // Parse the transaction cost
        memset(c_bal, 0x0, TULLAVE_BAL_TRAN_LEN);
        memcpy(c_bal, &raw_response[11], 3);
        trans_record->cost = tullave_data_uint8_to_balance(c_bal, TULLAVE_BAL_TRAN_LEN);

        // Lastly, parse the transaction type.
        // If the enum does not have the specific number, this could generate unexpected results
        trans_record->type = raw_response[0];
    }
    // This is the last command in the read chain
    poller->read_state = TuLlaveStateReadCompleted;
    return NfcCommandStop;
}

const TuLlaveReadHandler tullave_read_handler[TuLlaveStateReadNum] = {
    [TuLlaveStateReadIdle] = tullave_poller_read_initialize,
    [TuLlaveStateReadCardNumber] = tullave_poller_read_card_number,
    [TuLlaveStateReadBalance] = tullave_poller_read_balance,
    [TuLlaveStateReadHistory] = tullave_poller_read_history,
};

NfcCommand tullave_poller_run(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolIso14443_4a);
    TuLlavePoller* instance = context;
    furi_assert(instance);
    furi_assert(instance->callback);
    const Iso14443_4aPollerEvent* iso14443_4a_event = event.event_data;
    furi_assert(iso14443_4a_event);

    instance->iso_poller = event.instance;
    NfcCommand command = NfcCommandContinue;

    if(iso14443_4a_event->type == Iso14443_4aPollerEventTypeReady) {
        command = tullave_read_handler[instance->read_state](instance);
    }
    if(command == NfcCommandStop) {
        instance->callback(event, instance);
    }
    return command;
}
