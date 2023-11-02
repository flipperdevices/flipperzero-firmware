#include "nfc_crocodile_worker_i.h"

#define TAG "NfcCrocodileWorker"

NfcCrocodileWorker* nfc_crocodile_worker_alloc() {
    NfcCrocodileWorker* nfc_crocodile_worker =
        (NfcCrocodileWorker*)malloc(sizeof(NfcCrocodileWorker));

    uint8_t uid_data[] = {0x04, 0x59, 0x44, 0xc9, 0x1a, 0xe0, 0x8c};
    memcpy(nfc_crocodile_worker->uid_data, uid_data, 7);
    nfc_crocodile_worker->atqa_data[0] = 0x44;
    nfc_crocodile_worker->atqa_data[1] = 0x00;
    nfc_crocodile_worker->sak = 0x0;

    nfc_crocodile_worker->thread = furi_thread_alloc_ex(
        "NfcCrocodileWorker", 8192, nfc_crocodile_worker_task, nfc_crocodile_worker);

    return nfc_crocodile_worker;
}

void nfc_crocodile_worker_free(NfcCrocodileWorker* nfc_crocodile_worker) {
    furi_assert(nfc_crocodile_worker);

    free(nfc_crocodile_worker->card_content);
    furi_thread_free(nfc_crocodile_worker->thread);
    free(nfc_crocodile_worker);
}

void nfc_crocodile_worker_start(
    NfcCrocodileWorker* nfc_crocodile_worker,
    NfcCrocodileWorkerState state,
    NfcCrocodileStorageType storage_type,
    char* str,
    void* callback,
    void* context) {
    nfc_crocodile_worker->storage_type = storage_type;
    nfc_crocodile_worker->card_content = str;
    nfc_crocodile_worker->callback = callback;
    nfc_crocodile_worker->context = context;
    nfc_crocodile_worker_change_state(nfc_crocodile_worker, state);
    furi_thread_start(nfc_crocodile_worker->thread);
}

void nfc_crocodile_worker_stop(NfcCrocodileWorker* nfc_crocodile_worker) {
    furi_assert(nfc_crocodile_worker);
    furi_assert(nfc_crocodile_worker->thread);
    FURI_LOG_D(TAG, "Stopping thread");

    if(furi_thread_get_state(nfc_crocodile_worker->thread) != FuriThreadStateStopped) {
        FURI_LOG_D(TAG, "Thread is not stopped yet!");
        furi_hal_nfc_stop();
        nfc_crocodile_worker_change_state(nfc_crocodile_worker, NfcCrocodileWorkerStateStop);
        furi_thread_join(nfc_crocodile_worker->thread);
    }
    FURI_LOG_I(TAG, "Stopped!");
}

void nfc_crocodile_worker_change_state(
    NfcCrocodileWorker* nfc_crocodile_worker,
    NfcCrocodileWorkerState state) {
    nfc_crocodile_worker->state = state;
}

static void nfc_crocodile_worker_card_init_row(
    uint8_t* data,
    uint8_t row,
    uint8_t val1,
    uint8_t val2,
    uint8_t val3,
    uint8_t val4) {
    data[row * 4 + 0] = val1;
    data[row * 4 + 1] = val2;
    data[row * 4 + 2] = val3;
    data[row * 4 + 3] = val4;
}

void nfc_crocodile_worker_card_init(NfcCrocodileWorker* nfc_crocodile_worker) {
    // NTAG215, 7 bytes UID, 135*4 bytes memory
    furi_assert(nfc_crocodile_worker);
    furi_assert(nfc_crocodile_worker->card_content);
    uint8_t* uid_data = nfc_crocodile_worker->uid_data;
    uint8_t* card = nfc_crocodile_worker->card;

    uint8_t BCC0 = 0x88 ^ uid_data[0] ^ uid_data[1] ^ uid_data[2];
    uint8_t BCC1 = uid_data[3] ^ uid_data[4] ^ uid_data[5] ^ uid_data[6];
    nfc_crocodile_worker_card_init_row(
        card,
        0x0,
        uid_data[0], // SN0, manufacturer ID
        uid_data[1], // SN1
        uid_data[2], // SN2
        BCC0 // check byte 0
    );
    nfc_crocodile_worker_card_init_row(
        nfc_crocodile_worker->card,
        0x1,
        uid_data[3], // SN3
        uid_data[4], // SN4
        uid_data[5], // SN5
        uid_data[6] // SN6
    );
    nfc_crocodile_worker_card_init_row(
        card,
        0x2,
        BCC1, // check byte 1
        0x48, // fixed internal value
        0x0, // lock bytes
        0x0 // lock bytes
    );

    // Memory content at delivery
    nfc_crocodile_worker_card_init_row(
        card,
        0x3,
        0xe1, // Capacity container
        0x10, // CC
        0x3F, // CC
        0x00 // CC
    );
    // Memory content at delivery
    nfc_crocodile_worker_card_init_row(card, 0x4, 0x03, 0x00, 0xFE, 0x00);

    // Fill user space with zeros
    for(uint8_t i = 0x5; i < 0x82; i++) {
        nfc_crocodile_worker_card_init_row(card, i, 0x00, 0x00, 0x00, 0x00);
    }

    // Configuration pages
    nfc_crocodile_worker_card_init_row(card, 0x82, 0x00, 0x00, 0x00, 0xBD);
    nfc_crocodile_worker_card_init_row(card, 0x83, 0x04, 0x00, 0x00, 0xFF);
    nfc_crocodile_worker_card_init_row(card, 0x84, 0x00, 0x05, 0x00, 0x00);
    nfc_crocodile_worker_card_init_row(card, 0x85, 0xFF, 0xFF, 0xFF, 0xFF);
    nfc_crocodile_worker_card_init_row(card, 0x86, 0x00, 0x00, 0x00, 0x00);

    uint8_t* start = card + 0x5 * 4 + 0;
    uint8_t* ptr = start;

    // https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.0.0%2Fnfc_ndef_format_dox.html&anchor=nfc_ndef_format_flags
    if(nfc_crocodile_worker->storage_type == NfcCrocodileStorageText) {
        const char* mime = "T";
        const uint32_t mime_len = strlen(mime);
        const char* lang_code = "en";
        const uint32_t lang_code_len = strlen(lang_code);
        const char* content = nfc_crocodile_worker->card_content;
        const uint32_t content_length = strlen(content);

        // flags, ME + MD + /*SR +*/ Media type
        *ptr++ = 0b10000000 | 0b01000000 | /*0b00010000 |*/ 0x01;
        // type length
        *ptr++ = mime_len;
        // payload length
        uint32_t payload_length = content_length + lang_code_len + 1;
        *ptr++ = (payload_length >> 24) & 0xFF;
        *ptr++ = (payload_length >> 16) & 0xFF;
        *ptr++ = (payload_length >> 8) & 0xFF;
        *ptr++ = payload_length & 0xFF;
        // ID length
        // skipped
        // Payload type
        memcpy(ptr, mime, mime_len);
        ptr += mime_len;
        // Payload
        // Status byte. UTF8, length = 2
        *ptr++ = lang_code_len;
        // lang code
        memcpy(ptr, lang_code, lang_code_len);
        ptr += lang_code_len;
        // true payload
        memcpy(ptr, content, content_length);
        ptr += content_length;
    } else if(nfc_crocodile_worker->storage_type == NfcCrocodileStorageURL) {
        const char* mime = "U";
        const uint8_t mime_len = strlen(mime);
        const char* site = "duckduckgo.com/?q=";
        const uint8_t site_len = strlen(site);
        const char* content = nfc_crocodile_worker->card_content;
        const uint8_t content_length = strlen(content);

        // flags, ME + MD + /*SR +*/ Media type
        *ptr++ = 0b10000000 | 0b01000000 | /*0b00010000 |*/ 0x01;
        // type length
        *ptr++ = mime_len;
        // payload length
        uint32_t payload_length = content_length + site_len + 1;
        *ptr++ = (payload_length >> 24) & 0xFF;
        *ptr++ = (payload_length >> 16) & 0xFF;
        *ptr++ = (payload_length >> 8) & 0xFF;
        *ptr++ = payload_length & 0xFF;
        // ID length
        // skipped
        // Payload type
        memcpy(ptr, mime, mime_len);
        ptr += mime_len;
        // Payload
        // Scheme, https://
        *ptr++ = 0x04;
        // lang code
        memcpy(ptr, site, site_len);
        ptr += site_len;
        // true payload
        memcpy(ptr, content, content_length);
        ptr += content_length;
    }
    // fill containter size
    uint32_t container_length = ptr - start;
    nfc_crocodile_worker->card[0x4 * 4 + 1] = 0xFF;
    nfc_crocodile_worker->card[0x4 * 4 + 2] = (container_length >> 8) & 0xFF;
    nfc_crocodile_worker->card[0x4 * 4 + 3] = container_length & 0xFF;
    // Message end
    *ptr++ = 0xFE;
}

bool nfc_crocodile_worker_callback(
    uint8_t* buff_rx,
    uint16_t buff_rx_len,
    uint8_t* buff_tx,
    uint16_t* buff_tx_len,
    uint32_t* flags,
    void* context) {
    NfcCrocodileWorker* nfc_crocodile_worker = context;
    nfc_crocodile_worker->connection = true;

    *buff_tx_len = 0;
    *flags = FURI_HAL_NFC_TXRX_DEFAULT;

    if(buff_rx_len < 8) {
        FURI_LOG_D(TAG, "Empty RX");
        return true;
    }

    // READ
    if(buff_rx[0] == 0x30) {
        *buff_tx_len = 16 * 8;
        FURI_LOG_D(TAG, "READ addr=%x", buff_rx[1]);
        memcpy(buff_tx, nfc_crocodile_worker->card + buff_rx[1] * 4, sizeof(uint8_t) * 16);
        return true;
    }

    // FAST_READ
    if(buff_rx[0] == 0x3A) {
        uint8_t requested = buff_rx[2] - buff_rx[1] + 1;
        FURI_LOG_D(
            TAG,
            "FAST_READ StartDddr=%x, EndAddr=%x (total pages: %u)",
            buff_rx[1],
            buff_rx[2],
            requested);
        *buff_tx_len = requested * 4 * 8;
        memcpy(buff_tx, nfc_crocodile_worker->card + buff_tx[1], sizeof(uint8_t) * requested * 4);
        return true;
    }

    // GET_VERSION
    if(buff_rx[0] == 0x60) {
        FURI_LOG_D(TAG, "GET_VERSION");
        // NTAG215
        buff_tx[0] = 0x00;
        buff_tx[1] = 0x04; // Vendor: NXP
        buff_tx[2] = 0x04; // Product: NTAG
        buff_tx[3] = 0x02; // Subtype: 50 pF
        buff_tx[4] = 0x01; // Product major version
        buff_tx[5] = 0x00; // product minor version
        buff_tx[6] = 0x11; // NTAG 215
        buff_tx[7] = 0x03; // Protocol type: ISO/IEC 14443-3 compliant
        *buff_tx_len = 8 * 8;
        return true;
    }

    // PWD_AUTH
    if(buff_rx[0] == 0x1B) {
        FURI_LOG_D(TAG, "PWD_AUTH %x %x %x %x", buff_rx[1], buff_rx[2], buff_rx[3], buff_rx[4]);
        buff_tx[0] = 0x0;
        buff_tx[1] = 0x0;
        *buff_tx_len = 2 * 8;
    }

    FURI_LOG_D(TAG, "=== RX ===");
    FURI_LOG_D(TAG, "len = %i", buff_rx_len);
    for(uint8_t i = 0; i < buff_rx_len / 8; i++) {
        FURI_LOG_D(TAG, "rx[%u] = 0x%x", i, buff_rx[i]);
    }
    FURI_LOG_D(TAG, "=== TX ===");
    FURI_LOG_D(TAG, "len = %i", *buff_tx_len);

    *buff_tx_len = 4;
    buff_tx[0] = 0x0;

    return true;
}

int32_t nfc_crocodile_worker_task(void* context) {
    NfcCrocodileWorker* nfc_crocodile_worker = context;
    nfc_crocodile_worker_card_init(nfc_crocodile_worker);

    FURI_LOG_I(TAG, "Start! State: %i", nfc_crocodile_worker->state);

    furi_hal_nfc_exit_sleep();
    while(nfc_crocodile_worker->state == NfcCrocodileWorkerStateReady) {
        furi_hal_nfc_emulate_nfca(
            nfc_crocodile_worker->uid_data,
            7,
            nfc_crocodile_worker->atqa_data,
            nfc_crocodile_worker->sak,
            nfc_crocodile_worker_callback,
            nfc_crocodile_worker,
            1000);
        if(nfc_crocodile_worker->connection) {
            FURI_LOG_D(TAG, "Connection was closed!");
            nfc_crocodile_worker->connection = false;
            if(nfc_crocodile_worker->callback) {
                nfc_crocodile_worker->callback(nfc_crocodile_worker->context);
            }
        }
    }
    furi_hal_nfc_sleep();

    FURI_LOG_I(TAG, "Stop thread!");
    return 0;
}