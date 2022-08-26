#include "reader_analyzer.h"
#include <stream_buffer.h>
#include <lib/nfc/protocols/nfc_util.h>
#include <lib/nfc/protocols/mifare_classic.h>
#include <m-array.h>

#include "mfkey32.h"
#include "nfc_debug_pcap.h"

#define TAG "ReaderAnalyzer"

#define READER_ANALYZER_MAX_BUFF_SIZE (256)

typedef struct {
    bool reader_to_tag;
    bool crc_dropped;
    uint16_t len;
} ReaderAnalyzerHeader;

typedef enum {
    ReaderAnalyzerNfcDataMfClassic,
} ReaderAnalyzerNfcData;

struct ReaderAnalyzer {
    FuriHalNfcDevData nfc_data;

    bool alive;
    StreamBufferHandle_t stream;
    FuriThread* thread;

    ReaderAnalyzerParseDataCallback callback;
    void* context;

    ReaderAnalyzerMode mode;
    Mfkey32* mfkey32;
    NfcDebugPcap* pcap;
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

void reader_analyzer_parse(ReaderAnalyzer* instance, uint8_t* buffer, size_t size) {
    if(size < sizeof(ReaderAnalyzerHeader)) return;

    size_t bytes_i = 0;
    while(bytes_i < size) {
        ReaderAnalyzerHeader* header = (ReaderAnalyzerHeader*)&buffer[bytes_i];
        uint16_t len = header->len;
        if(bytes_i + len > size) break;
        bytes_i += sizeof(ReaderAnalyzerHeader);
        if(instance->mfkey32) {
            mfkey32_process_data(
                instance->mfkey32,
                &buffer[bytes_i],
                len,
                header->reader_to_tag,
                header->crc_dropped);
        }
        if(instance->pcap) {
            nfc_debug_pcap_process_data(
                instance->pcap, &buffer[bytes_i], len, header->reader_to_tag, header->crc_dropped);
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

ReaderAnalyzer* reader_analyzer_alloc() {
    ReaderAnalyzer* instance = malloc(sizeof(ReaderAnalyzer));

    instance->nfc_data = reader_analyzer_nfc_data[ReaderAnalyzerNfcDataMfClassic];
    instance->alive = false;
    instance->stream = xStreamBufferCreate(1024, 1);

    instance->thread = furi_thread_alloc();
    furi_thread_set_name(instance->thread, "ReaderAnalyzerWorker");
    furi_thread_set_stack_size(instance->thread, 2048);
    furi_thread_set_callback(instance->thread, reader_analyzer_thread);
    furi_thread_set_context(instance->thread, instance);
    furi_thread_set_priority(instance->thread, FuriThreadPriorityLow);

    return instance;
}

void reader_analyzer_start(ReaderAnalyzer* instance, ReaderAnalyzerMode mode) {
    furi_assert(instance);

    if(mode & ReaderAnalyzerModeMfkey) {
        instance->mfkey32 = mfkey32_alloc(instance->nfc_data.cuid);
    }
    if(mode & ReaderAnalyzerModePcap) {
        instance->pcap = nfc_debug_pcap_alloc();
    }
    instance->alive = true;
    furi_thread_start(instance->thread);
}

void reader_analyzer_stop(ReaderAnalyzer* instance) {
    furi_assert(instance);

    if(instance->mfkey32) {
        mfkey32_free(instance->mfkey32);
        instance->mfkey32 = NULL;
    }
    if(instance->pcap) {
        nfc_debug_pcap_free(instance->pcap);
    }

    instance->alive = false;
    furi_thread_join(instance->thread);
}

void reader_analyzer_free(ReaderAnalyzer* instance) {
    furi_assert(instance);

    reader_analyzer_stop(instance);
    furi_thread_free(instance->thread);
    vStreamBufferDelete(instance->stream);
    free(instance);
}

void reader_analyzer_set_callback(
    ReaderAnalyzer* instance,
    ReaderAnalyzerParseDataCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

NfcProtocol
    reader_analyzer_guess_protocol(ReaderAnalyzer* instance, uint8_t* buff_rx, uint16_t len) {
    furi_assert(instance);
    furi_assert(buff_rx);
    UNUSED(len);
    NfcProtocol protocol = NfcDeviceProtocolUnknown;

    if((buff_rx[0] == 0x60) || (buff_rx[0] == 0x61)) {
        protocol = NfcDeviceProtocolMifareClassic;
    }

    return protocol;
}

FuriHalNfcDevData* reader_analyzer_get_nfc_data(ReaderAnalyzer* instance) {
    furi_assert(instance);

    return &instance->nfc_data;
}

static void reader_analyzer_write(
    ReaderAnalyzer* instance,
    uint8_t* data,
    uint16_t len,
    bool reader_to_tag,
    bool crc_dropped) {
    ReaderAnalyzerHeader header = {
        .reader_to_tag = reader_to_tag, .crc_dropped = crc_dropped, .len = len};
    xStreamBufferSend(instance->stream, &header, sizeof(ReaderAnalyzerHeader), FuriWaitForever);
    xStreamBufferSend(instance->stream, data, len, FuriWaitForever);
}

static void
    reader_analyzer_write_rx(uint8_t* data, uint16_t bits, bool crc_dropped, void* context) {
    UNUSED(crc_dropped);
    ReaderAnalyzer* reader_analyzer = context;
    reader_analyzer_write(reader_analyzer, data, bits / 8, false, crc_dropped);
}

static void
    reader_analyzer_write_tx(uint8_t* data, uint16_t bits, bool crc_dropped, void* context) {
    UNUSED(crc_dropped);
    ReaderAnalyzer* reader_analyzer = context;
    reader_analyzer_write(reader_analyzer, data, bits / 8, true, crc_dropped);
}

void reader_analyzer_prepare_tx_rx(
    ReaderAnalyzer* instance,
    FuriHalNfcTxRxContext* tx_rx,
    bool is_picc) {
    furi_assert(instance);
    furi_assert(tx_rx);

    if(is_picc) {
        tx_rx->sniff_tx = reader_analyzer_write_rx;
        tx_rx->sniff_rx = reader_analyzer_write_tx;
    } else {
        tx_rx->sniff_rx = reader_analyzer_write_rx;
        tx_rx->sniff_tx = reader_analyzer_write_tx;
    }

    tx_rx->sniff_context = instance;
}
