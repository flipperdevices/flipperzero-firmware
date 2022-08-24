#include "mfkey32.h"

#include <furi/furi.h>
#include <storage/storage.h>
#include <stream/stream.h>
#include <stream/buffered_file_stream.h>
#include <m-array.h>

#define MFKEY32_LOGS_PATH EXT_PATH("nfc/.mfkey32.log")

typedef enum {
    Mfkey32StateIdle,
    Mfkey32StateAuthReceived,
    Mfkey32StateAuthNtSent,
    Mfkey32StateAuthArNrReceived,
} Mfkey32State;

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
} Mfkey32Params;

ARRAY_DEF(Mfkey32Params, Mfkey32Params, M_POD_OPLIST);

struct Mfkey32 {
    Mfkey32State state;
    Storage* storage;
    Stream* file_stream;
    Mfkey32Params_t params_arr;
    uint32_t cuid;
    Mfkey32ParseDataCallback callback;
    void* context;
};

Mfkey32* mfkey32_alloc(uint32_t cuid) {
    Mfkey32* instance = malloc(sizeof(Mfkey32));
    instance->cuid = cuid;
    instance->state = Mfkey32StateIdle;
    instance->storage = furi_record_open(RECORD_STORAGE);
    instance->file_stream = buffered_file_stream_alloc(instance->storage);
    if(!buffered_file_stream_open(
           instance->file_stream, MFKEY32_LOGS_PATH, FSAM_WRITE, FSOM_OPEN_APPEND)) {
        buffered_file_stream_close(instance->file_stream);
        stream_free(instance->file_stream);
        free(instance);
        instance = NULL;
    } else {
        Mfkey32Params_init(instance->params_arr);
    }

    furi_record_close(RECORD_STORAGE);

    return instance;
}

void mfkey32_free(Mfkey32* instance) {
    furi_assert(instance != NULL);

    Mfkey32Params_clear(instance->params_arr);
    buffered_file_stream_close(instance->file_stream);
    stream_free(instance->file_stream);
    free(instance);
}

void mfkey32_set_callback(Mfkey32* instance, Mfkey32ParseDataCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

void mfkey32_process_data(
    Mfkey32* instance,
    uint8_t* data,
    uint16_t len,
    bool reader_to_tag,
    bool crc_dropped) {
    furi_assert(instance);
    furi_assert(data);

    uint16_t data_len = len;
    if((data_len > 3) && !crc_dropped) {
        data_len = ;
    }
    if(instance->state == Mfkey32StateIdle) {
        
    }
}
