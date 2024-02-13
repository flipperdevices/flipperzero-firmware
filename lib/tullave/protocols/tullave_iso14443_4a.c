#include "tullave_iso14443_4a.h"
#include <lib/toolbox/hex.h>

/**
 * ADPU Command to read tullave balance. 
 * The iso_poller is obtained in the NFCPoller callback
 * 
 * This assumes you have executed the tullave_read_info_iso14443_4a_card_number
 * Since it select first the APDU appliation
 * 
 * @param iso_poller
 *          The poller obtained in the NFCPoller callback
 * @param data
 *          Data where the balance will be stored
 * 
*/
void tullave_read_info_iso14443_4a_balance(Iso14443_4aPoller* iso_poller, TuLlaveApp* instance) {
    furi_assert(instance->card_data);

    //Debug command: nfc apdu 904c000004
    //Response example: 00000A8C9000
    const uint8_t read_bal_cmd[] = {0x90, 0x4c, 0x00, 0x00, 0x04};

    TuLlaveData* data = instance->card_data;

    BitBuffer* tx_data = bit_buffer_alloc(sizeof(read_bal_cmd));
    BitBuffer* rx_data = bit_buffer_alloc(TULLAVE_RX_MAX_BUFFER);

    bit_buffer_copy_bytes(tx_data, read_bal_cmd, sizeof(read_bal_cmd));
    Iso14443_4aError error = iso14443_4a_poller_send_block(iso_poller, tx_data, rx_data);

    if(error == Iso14443_4aErrorNone) {
        uint8_t c_bal[TULLAVE_BAL_LEN] = {0x0};
        const uint8_t* raw_bal_response = bit_buffer_get_data(rx_data);
        memcpy(c_bal, &raw_bal_response[5], 2);
        memcpy(&c_bal[2], raw_bal_response, 4);
        data->balance = uint8_to_integer_big_endian(c_bal, TULLAVE_BAL_LEN);
    }
}

void tullave_read_info_iso14443_4a_card_number(Iso14443_4aPoller* iso_poller, TuLlaveApp* instance) {
    furi_assert(instance->card_data);

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

    TuLlaveData* data = instance->card_data;

    BitBuffer* tx_data = bit_buffer_alloc(sizeof(read_card_number_cmd));
    BitBuffer* rx_data = bit_buffer_alloc(TULLAVE_RX_MAX_BUFFER);

    bit_buffer_copy_bytes(tx_data, read_card_number_cmd, sizeof(read_card_number_cmd));

    Iso14443_4aError error = iso14443_4a_poller_send_block(iso_poller, tx_data, rx_data);
    const uint8_t* apdu_resp = bit_buffer_get_data(rx_data);

    if(error == Iso14443_4aErrorNone && apdu_resp[0] == APDU_SW1_GOOD_RESPONSE) {
        uint8_t hexa_card_num[TULLAVE_CARD_NUM_LEN];
        const uint8_t* raw_card_num = &apdu_resp[TULLAVE_NUM_CARD_OFFSET];
        uint8_to_hex_chars(raw_card_num, hexa_card_num, TULLAVE_CARD_NUM_LEN);
        data->card_number = furi_string_alloc_printf("%s", hexa_card_num);
        instance->err_code = TuLlaveErrorNone;
    } else {
        instance->err_code = TuLlaveErrorCardNotSupported;
    }

    bit_buffer_free(tx_data);
    bit_buffer_free(rx_data);
}

void tullave_read_info_iso14443_4a(Iso14443_4aPoller* iso_poller, TuLlaveApp* instance) {
    furi_assert(instance->nfc_poller);
    furi_assert(iso_poller);

    // This changes the detect popup state
    view_dispatcher_send_custom_event(instance->view_dispatcher, NfcPollerEventReadingData);

    nfc_device_set_data(
        instance->nfc_device, NfcProtocolIso14443_4a, nfc_poller_get_data(instance->nfc_poller));
    const Iso14443_4aData* data =
        nfc_device_get_data(instance->nfc_device, NfcProtocolIso14443_4a);

    //Initializes the card data structure
    instance->card_data = tullave_data_alloc();

    // The UID can be read directly from the NFC-generic Poller
    tullave_iso14443_4a_format_bytes(
        instance->card_data->nfc_uid,
        data->iso14443_3a_data->uid,
        data->iso14443_3a_data->uid_len);

    // Read card number
    tullave_read_info_iso14443_4a_card_number(iso_poller, instance);
    // If card number could not be read, it does not make sense to try reading other sections
    if(instance->err_code != TuLlaveErrorNone) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcPollerEventError);
        return;
    }

    // Read card balance
    tullave_read_info_iso14443_4a_balance(iso_poller, instance);

    view_dispatcher_send_custom_event(instance->view_dispatcher, NfcPollerEventSuccess);
}

NfcCommand tullave_scene_read_poller_callback_iso14443_4a(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolIso14443_4a);
    TuLlaveApp* instance = context;

    const Iso14443_4aPollerEvent* iso_event = event.event_data;
    if(iso_event->type == Iso14443_4aPollerEventTypeReady) {
        // This is the only way to take the protocol-specific poller. This is only possible into the callback
        // Direct implementation of the Firmware APIs like allocating a new poller, is not recommended.
        Iso14443_4aPoller* iso_poller = event.instance;
        tullave_read_info_iso14443_4a(iso_poller, instance);
        return NfcCommandStop;
    }
    return NfcCommandContinue;
}

void tullave_scene_read_on_enter_iso14443_4a(TuLlaveApp* instance) {
    furi_assert(instance->nfc_poller);
    nfc_poller_start(
        instance->nfc_poller, tullave_scene_read_poller_callback_iso14443_4a, instance);
}
