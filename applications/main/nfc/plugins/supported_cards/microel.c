#include "nfc_supported_card_plugin.h"
#include <flipper_application.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_sync.h>
#include <bit_lib.h>

#define TAG "Microel"

#define KEY_LENGTH 6
#define UID_LENGTH 4
// Offset per la conversione dell'anno (5 bit: da 0 a 31, quindi range 2010–2041)
#define BASE_YEAR 2010

typedef struct {
    uint64_t a;
    uint64_t b;
} MfClassicKeyPair;

static MfClassicKeyPair microel_1k_keys[] = {
    {.a = 0x000000000000, .b = 0x000000000000}, // 000
    {.a = 0x000000000000, .b = 0x000000000000}, // 001
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 002
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 003
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 004
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 005
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 006
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 007
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 008
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 009
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 010
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 011
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 012
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 013
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 014
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 015
};

const uint8_t verify_sector = 1;

void calculateSumHex(const uint8_t* uid, size_t uidSize, uint8_t sumHex[]) {
    const uint8_t xorKey[] = {0x01, 0x92, 0xA7, 0x75, 0x2B, 0xF9};
    int sum = 0;

    for(size_t i = 0; i < uidSize; i++) {
        sum += uid[i];
    }

    int sumTwoDigits = sum % 256;

    if(sumTwoDigits % 2 == 1) {
        sumTwoDigits += 2;
    }

    for(size_t i = 0; i < sizeof(xorKey); i++) {
        sumHex[i] = sumTwoDigits ^ xorKey[i];
    }
}

void generateKeyA(const uint8_t* uid, uint8_t uidSize, uint8_t keyA[]) {
    uint8_t sumHex[6];
    calculateSumHex(uid, uidSize, sumHex);
    uint8_t firstCharacter = (sumHex[0] >> 4) & 0xF;

    if(firstCharacter == 0x2 || firstCharacter == 0x3 || firstCharacter == 0xA ||
       firstCharacter == 0xB) {
        // XOR con 0x40
        for(size_t i = 0; i < sizeof(sumHex); i++) {
            keyA[i] = 0x40 ^ sumHex[i];
        }
    } else if(
        firstCharacter == 0x6 || firstCharacter == 0x7 || firstCharacter == 0xE ||
        firstCharacter == 0xF) {
        // XOR con 0xC0
        for(size_t i = 0; i < sizeof(sumHex); i++) {
            keyA[i] = 0xC0 ^ sumHex[i];
        }
    } else {
        // KeyA uguale a sumHex
        for(size_t i = 0; i < sizeof(sumHex); i++) {
            keyA[i] = sumHex[i];
        }
    }
}

void generateKeyB(uint8_t keyA[], size_t keyASize, uint8_t keyB[]) {
    for(size_t i = 0; i < keyASize; i++) {
        keyB[i] = 0xFF ^ keyA[i];
    }
}

static bool microel_read(Nfc* nfc, NfcDevice* device) {
    FURI_LOG_D(TAG, "Entering Microel KDF");

    furi_assert(nfc);
    furi_assert(device);

    bool is_read = false;

    MfClassicData* data = mf_classic_alloc();
    nfc_device_copy_data(device, NfcProtocolMfClassic, data);

    do {
        MfClassicType type = MfClassicType1k;
        MfClassicError error = mf_classic_poller_sync_detect_type(nfc, &type);
        if(error != MfClassicErrorNone) break;

        //Get UID and check if it is 4 bytes
        size_t uid_len;
        const uint8_t* uid = mf_classic_get_uid(data, &uid_len);
        FURI_LOG_D(TAG, "UID identified: %02X%02X%02X%02X", uid[0], uid[1], uid[2], uid[3]);
        if(uid_len != UID_LENGTH) break;

        // Generate keys
        uint8_t keyA[KEY_LENGTH];
        uint8_t keyB[KEY_LENGTH];
        generateKeyA(uid, UID_LENGTH, keyA);
        generateKeyB(keyA, KEY_LENGTH, keyB);

        // Check key 0a to verify if it is a microel card
        MfClassicKey key = {0};
        bit_lib_num_to_bytes_be(
            bit_lib_bytes_to_num_be(keyA, KEY_LENGTH), COUNT_OF(key.data), key.data);
        const uint8_t block_num = mf_classic_get_first_block_num_of_sector(0); // This is 0
        MfClassicAuthContext auth_context;
        error =
            mf_classic_poller_sync_auth(nfc, block_num, &key, MfClassicKeyTypeA, &auth_context);
        if(error != MfClassicErrorNone) {
            break;
        }

        // Save keys generated to stucture
        for(size_t i = 0; i < mf_classic_get_total_sectors_num(data->type); i++) {
            if(microel_1k_keys[i].a == 0x000000000000) {
                microel_1k_keys[i].a = bit_lib_bytes_to_num_be(keyA, KEY_LENGTH);
            }
            if(microel_1k_keys[i].b == 0x000000000000) {
                microel_1k_keys[i].b = bit_lib_bytes_to_num_be(keyB, KEY_LENGTH);
            }
        }
        MfClassicDeviceKeys keys = {};
        for(size_t i = 0; i < mf_classic_get_total_sectors_num(data->type); i++) {
            bit_lib_num_to_bytes_be(
                microel_1k_keys[i].a, sizeof(MfClassicKey), keys.key_a[i].data);
            FURI_BIT_SET(keys.key_a_mask, i);
            bit_lib_num_to_bytes_be(
                microel_1k_keys[i].b, sizeof(MfClassicKey), keys.key_b[i].data);
            FURI_BIT_SET(keys.key_b_mask, i);
        }

        error = mf_classic_poller_sync_read(nfc, &keys, data);
        if(error == MfClassicErrorNotPresent) {
            FURI_LOG_W(TAG, "Failed to read data");
            break;
        }

        nfc_device_set_data(device, NfcProtocolMfClassic, data);

        is_read = (error == MfClassicErrorNone);
    } while(false);

    mf_classic_free(data);

    return is_read;
}

static bool microel_parse(const NfcDevice* device, FuriString* parsed_data) {
    furi_assert(device);
    furi_assert(parsed_data);

    const MfClassicData* data = nfc_device_get_data(device, NfcProtocolMfClassic);

    bool parsed = false;

    do {
        //Get UID
        size_t uid_len;
        const uint8_t* uid = mf_classic_get_uid(data, &uid_len);
        if(uid_len != UID_LENGTH) break;

        // Generate key from uid
        uint8_t keyA[KEY_LENGTH];
        generateKeyA(uid, UID_LENGTH, keyA);

        // Verify key
        MfClassicSectorTrailer* sec_tr =
            mf_classic_get_sector_trailer_by_sector(data, verify_sector);
        uint64_t key = bit_lib_bytes_to_num_be(sec_tr->key_a.data, 6);
        uint64_t key_for_check_from_array = bit_lib_bytes_to_num_be(keyA, KEY_LENGTH);
        if(key != key_for_check_from_array) break;

        // Header info (UID, ATQA, SAK)
	furi_string_cat_printf(parsed_data, "\e#Microel Card\n");
	furi_string_cat_printf(parsed_data, "(Mifare Classic 1k)\n");
	furi_string_cat_printf(parsed_data, "====================\n");

	// UID
	furi_string_cat_printf(parsed_data, "UID:");
	for(size_t i = 0; i < UID_LENGTH; i++) {
    	    furi_string_cat_printf(parsed_data, " %02X", data->block[0].data[i]);
	}
	furi_string_cat_printf(parsed_data, "\n");

	uint8_t atqa_lsb = data->block[0].data[6];
	uint8_t atqa_msb = data->block[0].data[7];
	uint8_t sak = data->block[0].data[5];
	furi_string_cat_printf(parsed_data, "ATQA: %02X %02X ~ SAK: %02X\n", atqa_msb, atqa_lsb, sak);
	
	furi_string_cat_printf(parsed_data, "--------------------\n");
	
	// Credito disponibile
        uint16_t credito_raw = (data->block[4].data[6] << 8) | data->block[4].data[5];
	// Dati credito precedente (Block 5)
        uint16_t credito_precedente = (data->block[5].data[6] << 8) | data->block[5].data[5];
	// Data e ora (Block 4, bytes 7-10) in formato MicroEL
	uint32_t raw_data_op = (data->block[4].data[10] << 24) | (data->block[4].data[9] << 16) | (data->block[4].data[8] << 8) | data->block[4].data[7];
	// Ricava tutti i campi (scarta i primi 5 bit)
	uint32_t useful_bits = raw_data_op & 0x07FFFFFF; // ultimi 27 bit

	uint8_t minuti     = (useful_bits >> 21) & 0x3F; // 6 bit
	uint8_t ore        = (useful_bits >> 16) & 0x1F; // 5 bit
	uint8_t tipo_op    = (useful_bits >> 14) & 0x03; // 2 bit
	uint16_t anno      = ((useful_bits >> 9) & 0x1F) + BASE_YEAR; // 5 bit + offset
	uint8_t mese       = (useful_bits >> 5) & 0x0F;  // 4 bit
	uint8_t giorno     = useful_bits & 0x1F;         // ultimi 5 bit

	// Conversione tipo operazione in stringa
	const char* tipo_op_str;
	if(tipo_op == 0) tipo_op_str = "Firts operation"; // 00
	else if(tipo_op == 1) tipo_op_str = "Recharge";   // 01
	else tipo_op_str = "Payment";  // 10

        // Numero operazione (Block 4, bytes 0-1)
        uint16_t num_op = (data->block[4].data[1] << 8) | data->block[4].data[0];
        // Saldo punti (Block 4, bytes 11-12)
        uint16_t saldo_punti = (data->block[4].data[11] << 8) | data->block[4].data[12];
        // Ultima transazione (Block 4, bytes 13-14)
        uint16_t transazione_raw = (data->block[4].data[14] << 8) | data->block[4].data[13];
        // Somma in ingresso e cauzione (Block 4)
	uint16_t somma_raw = data->block[4].data[2] | (data->block[4].data[3] << 8);
	float somma = somma_raw / 10.0f;
	bool cauzione = data->block[4].data[4] != 0;
	uint8_t cauzione_raw = data->block[4].data[4];

        furi_string_cat_printf(parsed_data, "-> Credit Available: %d.%02d\n", credito_raw / 100, credito_raw % 100);
	furi_string_cat_printf(parsed_data, "-> Previous Credit: %d.%02d\n", credito_precedente / 100, credito_precedente % 100);
	furi_string_cat_printf(parsed_data, "--------------------\n");
	furi_string_cat_printf(parsed_data, "Last transaction: %d.%02d\n", transazione_raw / 100, transazione_raw % 100);
        furi_string_cat_printf(parsed_data, "Date: %02d/%02d/%04d ~ %02d:%02d\n", giorno, mese, anno, ore, minuti);
        furi_string_cat_printf(parsed_data, "Operation Nr: (%d)\n", num_op);
        furi_string_cat_printf(parsed_data, "Op. Type: [%s]\n", tipo_op_str);       
        furi_string_cat_printf(parsed_data, "====================\n");

	// Vendor ID - Block 1 (entire block)
        furi_string_cat_printf(parsed_data, "Vendor Univocal ID:\n");
	for (size_t i = 0; i < 16; i++) {
    	    furi_string_cat_printf(parsed_data, "%02X", data->block[1].data[i]);
    	    if (i % 8 == 7) {
                // Dopo ogni 6 byte, vai a capo
                furi_string_cat_printf(parsed_data, "\n");
    	    } else {
        	// Altrimenti, aggiungi uno spazio
        	furi_string_cat_printf(parsed_data, " ");
    	    }
        }
        
        furi_string_cat_printf(parsed_data, "--------------------\n");
        
        furi_string_cat_printf(parsed_data, "Points balance: (%d pt.)\n", saldo_punti);
	furi_string_cat_printf(parsed_data, "Admission credit: %d.%02d\n", (int)somma, (int)(somma * 100) % 100);
	
	// Gestione deposito aggiornata
	if (cauzione) {
	    furi_string_cat_printf(parsed_data, "Deposit: [Yes] ~ (%d.%02d)\n", 
	                          cauzione_raw / 100, cauzione_raw % 100);
	} else {
	    furi_string_cat_printf(parsed_data, "Deposit: [No]\n");
	}
	
	furi_string_cat_printf(parsed_data, "--------------------\n");
	
        parsed = true;
    } while(false);

    return parsed;
}

static const NfcSupportedCardsPlugin microel_plugin = {
    .protocol = NfcProtocolMfClassic,
    .verify = NULL,
    .read = microel_read,
    .parse = microel_parse,
};

static const FlipperAppPluginDescriptor microel_plugin_descriptor = {
    .appid = NFC_SUPPORTED_CARD_PLUGIN_APP_ID,
    .ep_api_version = NFC_SUPPORTED_CARD_PLUGIN_API_VERSION,
    .entry_point = &microel_plugin,
};

const FlipperAppPluginDescriptor* microel_plugin_ep(void) {
    return &microel_plugin_descriptor;
}
