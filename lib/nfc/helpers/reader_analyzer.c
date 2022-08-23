#include "reader_analyzer.h"
#include <stream_buffer.h>
#include <lib/nfc/protocols/nfc_util.h>
#include <lib/nfc/protocols/mifare_classic.h>
#include <m-array.h>

#define TAG "ReaderAnalyzer"

#define READER_ANALYZER_LOG_PATH EXT_PATH("nfc/detect_reader.log")

#define READER_ANALYZER_MAX_BUFF_SIZE (256)

#define READER_ANALYZER_IS_PCD (0x0000)
#define READER_ANALYZER_IS_PICC (0x0001)

typedef struct {
    uint32_t cuid;
    uint8_t sector;
    MfClassicKey key;
    uint32_t nt0;
    uint32_t nr0;
    uint32_t ar0;
    uint32_t nt1;
    uint32_t nr1;
    uint32_t ar1;
} ReaderAnalyzerMfkey32;

typedef struct {
    uint8_t sector;
    MfClassicKey key;
    uint32_t nt;
    uint32_t nr;
    uint32_t ar;
} ReaderAnalyzerMfkey32Nonce;

ARRAY_DEF(ReaderAnalyzerMfkey32, ReaderAnalyzerMfkey32, M_POD_OPLIST);

typedef struct {
    uint16_t is_picc;
    uint16_t len;
} ReaderAnalyzerHeader;

typedef enum {
    ReaderAnalyzerNfcDataMfClassic,
} ReaderAnalyzerNfcData;

typedef enum {
    ReaderAnalyzerIdle,
    ReaderAnalyzerStateMfClassic,
    ReaderAnalyzerStateMfClassicAuthReceived,
    ReaderAnalyzerStateMfClassicNtSent,
    ReaderAnalyzerStateMfClassicArNrReceived,
} ReaderAnalyzerState;

struct ReaderAnalyzer {
    ReaderAnalyzerState state;
    FuriHalNfcDevData nfc_data;

    bool alive;
    Storage* storage;
    Stream* file_stream;
    StreamBufferHandle_t stream;
    FuriThread* thread;

    ReaderAnalyzerParseDataCallback callback;
    void* context;

    ReaderAnalyzerMfkey32_t mfkey_params;
    ReaderAnalyzerMfkey32Nonce mfkey_nonce;
};

const FuriHalNfcDevData reader_analyzer_nfc_data[] = {
    [ReaderAnalyzerNfcDataMfClassic] =
        {.sak = 0x08,
         .atqa = {0x44, 0x00},
         .interface = FuriHalNfcInterfaceRf,
         .type = FuriHalNfcTypeA,
         .uid_len = 7,
         .uid = {0x04, 0x77, 0x70, 0x2A, 0x23, 0x4F, 0x80},
         .cuid = 0x2A234F80},
};

static bool reader_analyzer_write_mfkey_params(
    ReaderAnalyzer* reader_analyzer,
    ReaderAnalyzerMfkey32* params) {
    string_t str;
    string_init_printf(
        str,
        "Sector %d key %c cuid %08x nt0 %08x nr0 %08x ar0 %08x nt1 %08x nr1 %08x ar1 %08x\n",
        params->sector,
        params->key == MfClassicKeyA ? 'A' : 'B',
        params->cuid,
        params->nt0,
        params->nr0,
        params->ar0,
        params->nt1,
        params->nr1,
        params->ar1);
    bool write_success = stream_write_string(reader_analyzer->file_stream, str);
    string_clear(str);
    return write_success;
}

static void reader_analyzer_add_mfkey_params(
    ReaderAnalyzer* reader_analyzer,
    ReaderAnalyzerMfkey32Nonce* nonce) {
    bool nonce_added = false;
    // Search if we partially collected params
    if(ReaderAnalyzerMfkey32_size(reader_analyzer->mfkey_params)) {
        ReaderAnalyzerMfkey32_it_t it;
        for(ReaderAnalyzerMfkey32_it(it, reader_analyzer->mfkey_params);
            !ReaderAnalyzerMfkey32_end_p(it);
            ReaderAnalyzerMfkey32_next(it)) {
            ReaderAnalyzerMfkey32* params = ReaderAnalyzerMfkey32_ref(it);
            if((params->sector == nonce->sector) && (params->key == nonce->key)) {
                params->nt1 = nonce->nt;
                params->nr1 = nonce->nr;
                params->ar1 = nonce->ar;
                nonce_added = true;
                FURI_LOG_D(
                    TAG,
                    "Params for sector %d key %c collected",
                    params->sector,
                    params->key == MfClassicKeyA ? 'A' : 'B');
                // Write on sd card
                reader_analyzer_write_mfkey_params(reader_analyzer, params);
                ReaderAnalyzerMfkey32_remove(reader_analyzer->mfkey_params, it);
            }
        }
    }
    if(!nonce_added) {
        ReaderAnalyzerMfkey32 params = {
            .sector = nonce->sector,
            .key = nonce->key,
            .cuid = reader_analyzer->nfc_data.cuid,
            .nt0 = nonce->nt,
            .nr0 = nonce->nr,
            .ar0 = nonce->ar,
        };
        ReaderAnalyzerMfkey32_push_back(reader_analyzer->mfkey_params, params);
    }
}

void reader_analyzer_parse(ReaderAnalyzer* reader_analyzer, uint8_t* buffer, size_t size) {
    if(size < sizeof(ReaderAnalyzerHeader)) return;

    size_t bytes_i = 0;
    while(bytes_i < size) {
        ReaderAnalyzerHeader* header = (ReaderAnalyzerHeader*)&buffer[bytes_i];
        uint16_t len = header->len;
        if(bytes_i + len > size) break;
        bytes_i += sizeof(ReaderAnalyzerHeader);

        if(buffer[bytes_i] == 0x60 || buffer[bytes_i] == 0x61) {
            if(buffer[bytes_i] == 0x60) {
                reader_analyzer->mfkey_nonce.key = MfClassicKeyA;
            } else {
                reader_analyzer->mfkey_nonce.key = MfClassicKeyB;
            }
            reader_analyzer->mfkey_nonce.sector =
                mf_classic_get_sector_by_block(buffer[bytes_i + 1]);
            reader_analyzer->state = ReaderAnalyzerStateMfClassicAuthReceived;
        } else if(reader_analyzer->state == ReaderAnalyzerStateMfClassicAuthReceived) {
            if(header->is_picc == READER_ANALYZER_IS_PICC) {
                if(len == 4) {
                    reader_analyzer->mfkey_nonce.nt = nfc_util_bytes2num(&buffer[bytes_i], 4);
                    reader_analyzer->state = ReaderAnalyzerStateMfClassicNtSent;
                }
            }
        } else if(reader_analyzer->state == ReaderAnalyzerStateMfClassicNtSent) {
            if(header->is_picc == READER_ANALYZER_IS_PCD) {
                if(len == 8) {
                    reader_analyzer->mfkey_nonce.nr = nfc_util_bytes2num(&buffer[bytes_i], 4);
                    reader_analyzer->mfkey_nonce.ar = nfc_util_bytes2num(&buffer[bytes_i + 4], 4);
                    reader_analyzer->state = ReaderAnalyzerStateMfClassicArNrReceived;
                    reader_analyzer_add_mfkey_params(
                        reader_analyzer, &reader_analyzer->mfkey_nonce);
                }
            }
        }
        bytes_i += len;
    }
}

int32_t reader_analyzer_thread(void* context) {
    ReaderAnalyzer* reader_analyzer = context;
    uint8_t buffer[READER_ANALYZER_MAX_BUFF_SIZE] = {};

    while(reader_analyzer->alive) {
        size_t ret = xStreamBufferReceive(
            reader_analyzer->stream, buffer, READER_ANALYZER_MAX_BUFF_SIZE, 50);
        if(ret) {
            reader_analyzer_parse(reader_analyzer, buffer, ret);
        }
    }

    return 0;
}

ReaderAnalyzer* reader_analyzer_alloc(Storage* storage) {
    furi_assert(storage);

    ReaderAnalyzer* reader_analyzer = malloc(sizeof(ReaderAnalyzer));
    reader_analyzer->storage = storage;

    do {
        reader_analyzer->file_stream = buffered_file_stream_alloc(reader_analyzer->storage);
        if(!buffered_file_stream_open(
               reader_analyzer->file_stream,
               READER_ANALYZER_LOG_PATH,
               FSAM_WRITE,
               FSOM_OPEN_APPEND)) {
            buffered_file_stream_close(reader_analyzer->file_stream);
            stream_free(reader_analyzer->file_stream);
            free(reader_analyzer);
            reader_analyzer = NULL;
            break;
        }
        reader_analyzer->state = ReaderAnalyzerIdle;
        reader_analyzer->nfc_data = reader_analyzer_nfc_data[ReaderAnalyzerNfcDataMfClassic];
        reader_analyzer->alive = true;
        reader_analyzer->stream = xStreamBufferCreate(1024, 1);
        ReaderAnalyzerMfkey32_init(reader_analyzer->mfkey_params);

        reader_analyzer->thread = furi_thread_alloc();
        furi_thread_set_name(reader_analyzer->thread, "ReaderAnalyzerWorker");
        furi_thread_set_stack_size(reader_analyzer->thread, 2048);
        furi_thread_set_callback(reader_analyzer->thread, reader_analyzer_thread);
        furi_thread_set_context(reader_analyzer->thread, reader_analyzer);
        furi_thread_set_priority(reader_analyzer->thread, FuriThreadPriorityLow);
        furi_thread_start(reader_analyzer->thread);

    } while(false);

    return reader_analyzer;
}

void reader_analyzer_free(ReaderAnalyzer* reader_analyzer) {
    furi_assert(reader_analyzer);

    reader_analyzer->alive = false;
    furi_thread_join(reader_analyzer->thread);
    furi_thread_free(reader_analyzer->thread);
    vStreamBufferDelete(reader_analyzer->stream);
    ReaderAnalyzerMfkey32_clear(reader_analyzer->mfkey_params);
    buffered_file_stream_close(reader_analyzer->file_stream);
    stream_free(reader_analyzer->file_stream);
    free(reader_analyzer);
}

void reader_analyzer_set_callback(
    ReaderAnalyzer* reader_analyzer,
    ReaderAnalyzerParseDataCallback callback,
    void* context) {
    furi_assert(reader_analyzer);
    furi_assert(callback);

    reader_analyzer->callback = callback;
    reader_analyzer->context = context;
}

NfcProtocol reader_analyzer_guess_protocol(
    ReaderAnalyzer* reader_analyzer,
    uint8_t* buff_rx,
    uint16_t len) {
    furi_assert(reader_analyzer);
    furi_assert(buff_rx);
    UNUSED(len);
    NfcProtocol protocol = NfcDeviceProtocolUnknown;

    if((buff_rx[0] == 0x60) || (buff_rx[0] == 0x61)) {
        protocol = NfcDeviceProtocolMifareClassic;
        reader_analyzer->state = ReaderAnalyzerStateMfClassic;
    }

    return protocol;
}

FuriHalNfcDevData* reader_analyzer_get_nfc_data(ReaderAnalyzer* reader_analyzer) {
    furi_assert(reader_analyzer);

    return &reader_analyzer->nfc_data;
}

static void reader_analyzer_write(
    ReaderAnalyzer* reader_analyzer,
    uint8_t* data,
    uint16_t len,
    uint16_t is_picc) {
    ReaderAnalyzerHeader header = {.is_picc = is_picc, .len = len};
    xStreamBufferSend(
        reader_analyzer->stream, &header, sizeof(ReaderAnalyzerHeader), FuriWaitForever);
    xStreamBufferSend(reader_analyzer->stream, data, len, FuriWaitForever);
}

static void
    reader_analyzer_write_rx(uint8_t* data, uint16_t bits, bool crc_dropped, void* context) {
    UNUSED(crc_dropped);
    ReaderAnalyzer* reader_analyzer = context;
    reader_analyzer_write(reader_analyzer, data, bits / 8, READER_ANALYZER_IS_PICC);
}

static void
    reader_analyzer_write_tx(uint8_t* data, uint16_t bits, bool crc_dropped, void* context) {
    UNUSED(crc_dropped);
    ReaderAnalyzer* reader_analyzer = context;
    reader_analyzer_write(reader_analyzer, data, bits / 8, READER_ANALYZER_IS_PCD);
}

void reader_analyzer_prepare_tx_rx(
    ReaderAnalyzer* reader_analyzer,
    FuriHalNfcTxRxContext* tx_rx,
    bool is_picc) {
    furi_assert(reader_analyzer);
    furi_assert(tx_rx);

    if(is_picc) {
        tx_rx->sniff_tx = reader_analyzer_write_rx;
        tx_rx->sniff_rx = reader_analyzer_write_tx;
    } else {
        tx_rx->sniff_rx = reader_analyzer_write_rx;
        tx_rx->sniff_tx = reader_analyzer_write_tx;
    }

    tx_rx->sniff_context = reader_analyzer;
}
