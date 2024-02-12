#include "../../../tullave_i.h"
#include "tullave_data.h"

// I have not seeing a response greater than 53 bytes
#define TULLAVE_RX_MAX_BUFFER 53
// The card number in the initial response is after the first 8 bytes
#define TULLAVE_NUM_CARD_OFFSET 8
// As I know all cards contain only 16 digits
#define TULLAVE_CARD_NUM_LEN 16
// Apparently The highest balance supported by TuLlave is 999999 COP
#define TULLAVE_BAL_LEN 6

// APDU SW1 byte indicating the response from the card was successful
// This is used in the first read of the card (read card number)
#define APDU_SW1_GOOD_RESPONSE 0x6f

/**
 * Sends the ADPU Command to read tullave balance and parses the result
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
void tullave_read_info_iso14443_4a_balance(Iso14443_4aPoller* iso_poller, TuLlaveData* data);

/**
 * Sends the APDU Command to read card number and initially selecting the APDU app 
 * Read Balance or History only will work if this method is invoked first
 *
 * The iso_poller is obtained in the NFCPoller callback
 * 
 * @param iso_poller
 *          The poller obtained in the NFCPoller callback
 * 
 * @param data 
 *          Data where the card number will be stored
 * 
 * @return true if card number was read, otherwise return false
 * 
*/
bool tullave_read_info_iso14443_4a_card_number(Iso14443_4aPoller* iso_poller, TuLlaveData* data);

/**
 * Read all known info of TuLlave card (card_number, balance, history)
 * it uses a protocol-specific poller to send ISO14443-4a as transport
 * APDU commands are sent through ISO14443-4a blocks
 * 
 * @param iso_poller
 *          The poller obtained in the NFCPoller callback
 * 
 * @instance 
 *          TuLlave instance
 * 
*/
void tullave_read_info_iso14443_4a(Iso14443_4aPoller* iso_poller, TuLlaveApp* instance);

/**
 * Poller callback. This method is passed as delegate in the nfc_poller_start function
 * This method will be invoked after a event occurs in the NFCPoller, through the NfcGenericEvent
 * the protocol-specific poller can be obtained
 * 
 * @param event
 *          Generic and opaque event for NFC
 * 
 * @param context
 *          TuLlave instance
 * 
*/
NfcCommand tullave_scene_read_poller_callback_iso14443_4a(NfcGenericEvent event, void* context);

/**
 * Entrypoint for the communication protocol, it initializes the NFCPoller which 
 * should be previously allocated
 * 
 * @param instance
 *          TuLlave instance
 * 
*/
void tullave_scene_read_on_enter_iso14443_4a(TuLlaveApp* instance);