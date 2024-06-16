#include "compress.h"

#include <furi.h>
#include <lib/heatshrink/heatshrink_encoder.h>
#include <lib/heatshrink/heatshrink_decoder.h>
#include <stdint.h>

#define TAG "Compress"

/** Defines encoder and decoder window size */
#define COMPRESS_EXP_BUFF_SIZE_LOG (8u)

/** Defines encoder and decoder lookahead buffer size */
#define COMPRESS_LOOKAHEAD_BUFF_SIZE_LOG (4u)

#define COMPRESS_ICON_ENCODED_BUFF_SIZE (256u)

static const CompressConfigHeatshrink COMPRESS_CONFIG_HEATSHRINK = {
    .window_sz2 = COMPRESS_EXP_BUFF_SIZE_LOG,
    .lookahead_sz2 = COMPRESS_LOOKAHEAD_BUFF_SIZE_LOG,
    .input_buffer_sz = COMPRESS_ICON_ENCODED_BUFF_SIZE,
};

const CompressConfigHeatshrink* CompressConfigHeatshrinkDefault = &COMPRESS_CONFIG_HEATSHRINK;

/** Buffer size for input data */
static bool compress_decode_internal(
    heatshrink_decoder* decoder,
    const uint8_t* data_in,
    size_t data_in_size,
    uint8_t* data_out,
    size_t data_out_size,
    size_t* data_res_size);

typedef struct {
    uint8_t is_compressed;
    uint8_t reserved;
    uint16_t compressed_buff_size;
} CompressHeader;

_Static_assert(sizeof(CompressHeader) == 4, "Incorrect CompressHeader size");

struct CompressIcon {
    heatshrink_decoder* decoder;
    uint8_t* buffer;
    size_t buffer_size;
};

CompressIcon* compress_icon_alloc(size_t decode_buf_size) {
    CompressIcon* instance = malloc(sizeof(CompressIcon));
    instance->decoder = heatshrink_decoder_alloc(
        COMPRESS_ICON_ENCODED_BUFF_SIZE,
        COMPRESS_EXP_BUFF_SIZE_LOG,
        COMPRESS_LOOKAHEAD_BUFF_SIZE_LOG);
    heatshrink_decoder_reset(instance->decoder);

    instance->buffer_size = decode_buf_size + 4; /* To account for heatshrink's poller quirks */
    instance->buffer = malloc(instance->buffer_size);

    return instance;
}

void compress_icon_free(CompressIcon* instance) {
    furi_check(instance);
    free(instance->buffer);
    heatshrink_decoder_free(instance->decoder);
    free(instance);
}

void compress_icon_decode(CompressIcon* instance, const uint8_t* icon_data, uint8_t** output) {
    furi_check(instance);
    furi_check(icon_data);
    furi_check(output);

    CompressHeader* header = (CompressHeader*)icon_data;
    if(header->is_compressed) {
        size_t decoded_size = 0;
        /* If decompression fails - check that decode_buf_size is large enough */
        furi_check(compress_decode_internal(
            instance->decoder,
            icon_data,
            /* Decoder will check/process headers again - need to pass them */
            sizeof(CompressHeader) + header->compressed_buff_size,
            instance->buffer,
            instance->buffer_size,
            &decoded_size));
        *output = instance->buffer;
    } else {
        *output = (uint8_t*)&icon_data[1];
    }
}

struct Compress {
    const void* config;
    heatshrink_encoder* encoder;
    heatshrink_decoder* decoder;
};

Compress* compress_alloc(CompressType type, const void* config) {
    furi_check(type == COMPRESS_TYPE_HEATSHRINK);
    furi_check(config);

    Compress* compress = malloc(sizeof(Compress));
    compress->config = config;
    compress->encoder = NULL;
    compress->decoder = NULL;

    // CompressConfigHeatshrink* hs_config = (CompressConfigHeatshrink*)config;
    // compress->encoder = heatshrink_encoder_alloc(hs_config->window_sz2, hs_config->lookahead_sz2);
    // compress->decoder = heatshrink_decoder_alloc(
    //     hs_config->input_buffer_sz, hs_config->window_sz2, hs_config->lookahead_sz2);

    return compress;
}

void compress_free(Compress* compress) {
    furi_check(compress);

    if(compress->encoder) {
        heatshrink_encoder_free(compress->encoder);
    }
    if(compress->decoder) {
        heatshrink_decoder_free(compress->decoder);
    }
    free(compress);
}

static bool compress_encode_internal(
    heatshrink_encoder* encoder,
    uint8_t* data_in,
    size_t data_in_size,
    uint8_t* data_out,
    size_t data_out_size,
    size_t* data_res_size) {
    furi_check(encoder);
    furi_check(data_in);
    furi_check(data_in_size);

    size_t sink_size = 0;
    size_t poll_size = 0;
    HSE_sink_res sink_res;
    HSE_poll_res poll_res;
    HSE_finish_res finish_res;
    bool encode_failed = false;
    size_t sunk = 0;
    size_t res_buff_size = sizeof(CompressHeader);

    heatshrink_encoder_reset(encoder);
    /* Sink data to encoding buffer */
    while((sunk < data_in_size) && !encode_failed) {
        sink_res =
            heatshrink_encoder_sink(encoder, &data_in[sunk], data_in_size - sunk, &sink_size);
        if(sink_res != HSER_SINK_OK) {
            encode_failed = true;
            break;
        }
        sunk += sink_size;
        do {
            poll_res = heatshrink_encoder_poll(
                encoder, &data_out[res_buff_size], data_out_size - res_buff_size, &poll_size);
            if(poll_res < 0) {
                encode_failed = true;
                break;
            }
            res_buff_size += poll_size;
        } while(poll_res == HSER_POLL_MORE);
    }

    /* Notify sinking complete and poll encoded data */
    finish_res = heatshrink_encoder_finish(encoder);
    if(finish_res < 0) {
        encode_failed = true;
    } else {
        do {
            poll_res = heatshrink_encoder_poll(
                encoder, &data_out[res_buff_size], data_out_size - res_buff_size, &poll_size);
            if(poll_res < 0) {
                encode_failed = true;
                break;
            }
            res_buff_size += poll_size;
            finish_res = heatshrink_encoder_finish(encoder);
        } while(finish_res != HSER_FINISH_DONE);
    }

    bool result = true;
    /* Write encoded data to output buffer if compression is efficient. Otherwise, write header and original data */
    if(!encode_failed && (res_buff_size < data_in_size + 1)) {
        CompressHeader header = {
            .is_compressed = 0x01,
            .reserved = 0x00,
            .compressed_buff_size = res_buff_size - sizeof(CompressHeader)};
        memcpy(data_out, &header, sizeof(header));
        *data_res_size = res_buff_size;
    } else if(data_out_size > data_in_size) {
        data_out[0] = 0x00;
        memcpy(&data_out[1], data_in, data_in_size);
        *data_res_size = data_in_size + 1;
    } else {
        *data_res_size = 0;
        result = false;
    }
    return result;
}

static inline bool compress_decoder_poll(
    heatshrink_decoder* decoder,
    uint8_t* decompressed_chunk,
    size_t decomp_buffer_size,
    compress_io_cb_t write_cb,
    void* context) {
    HSD_poll_res poll_res;
    size_t poll_size;

    do {
        poll_res =
            heatshrink_decoder_poll(decoder, decompressed_chunk, decomp_buffer_size, &poll_size);
        if(poll_res < 0) {
            return false;
        }

        size_t write_size = write_cb(decompressed_chunk, poll_size, context);
        if(write_size != poll_size) {
            return false;
        }
    } while(poll_res == HSDR_POLL_MORE);

    return true;
}

static bool compress_decode_stream_internal(
    heatshrink_decoder* decoder,
    compress_io_cb_t read_cb,
    compress_io_cb_t write_cb,
    void* context) {
    bool decode_failed = false;
    HSD_sink_res sink_res;
    HSD_finish_res finish_res;
    size_t read_size = 0;
    size_t sink_size = 0;

    const size_t WORK_BUFFER_SIZE = COMPRESS_ICON_ENCODED_BUFF_SIZE;

    uint8_t* compressed_chunk = malloc(WORK_BUFFER_SIZE);
    uint8_t* decompressed_chunk = malloc(WORK_BUFFER_SIZE);

    heatshrink_decoder_reset(decoder);
    /* Sink data to decoding buffer */
    do {
        read_size = read_cb(compressed_chunk, WORK_BUFFER_SIZE, context);

        size_t sunk = 0;
        while(sunk < read_size && !decode_failed) {
            sink_res = heatshrink_decoder_sink(
                decoder, &compressed_chunk[sunk], read_size - sunk, &sink_size);
            if(sink_res < 0) {
                decode_failed = true;
                break;
            }
            sunk += sink_size;

            if(!compress_decoder_poll(
                   decoder, decompressed_chunk, WORK_BUFFER_SIZE, write_cb, context)) {
                decode_failed = true;
                break;
            }
        }
    } while(!decode_failed && read_size);

    /* Notify sinking complete and poll decoded data */
    if(!decode_failed) {
        while((finish_res = heatshrink_decoder_finish(decoder)) != HSDR_FINISH_DONE) {
            if(finish_res < 0) {
                decode_failed = true;
                break;
            }

            if(!compress_decoder_poll(
                   decoder, decompressed_chunk, WORK_BUFFER_SIZE, write_cb, context)) {
                decode_failed = true;
                break;
            }
        }
    }

    free(compressed_chunk);
    free(decompressed_chunk);

    return !decode_failed;
}

static size_t compress_decode_hs_old(
    heatshrink_decoder* decoder,
    const uint8_t* compressed_data,
    size_t compressed_size,
    uint8_t* data_out,
    size_t data_out_size,
    size_t* data_res_size) {
    bool decode_failed = false;
    HSD_sink_res sink_res;
    HSD_poll_res poll_res;
    HSD_finish_res finish_res;
    size_t sink_size = 0;
    size_t res_buff_size = 0;
    size_t poll_size = 0;

    heatshrink_decoder_reset(decoder);
    /* Sink data to decoding buffer */
    size_t sunk = 0;
    while(sunk < compressed_size && !decode_failed) {
        sink_res = heatshrink_decoder_sink(
            decoder, (uint8_t*)&compressed_data[sunk], compressed_size - sunk, &sink_size);
        if(sink_res < 0) {
            decode_failed = true;
            break;
        }
        sunk += sink_size;
        do {
            poll_res = heatshrink_decoder_poll(
                decoder, &data_out[res_buff_size], data_out_size - res_buff_size, &poll_size);
            if((poll_res < 0) || ((data_out_size - res_buff_size) == 0)) {
                decode_failed = true;
                break;
            }
            res_buff_size += poll_size;
        } while(poll_res == HSDR_POLL_MORE);
    }
    /* Notify sinking complete and poll decoded data */
    if(!decode_failed) {
        finish_res = heatshrink_decoder_finish(decoder);
        if(finish_res < 0) {
            decode_failed = true;
        } else {
            do {
                poll_res = heatshrink_decoder_poll(
                    decoder, &data_out[res_buff_size], data_out_size - res_buff_size, &poll_size);
                if(poll_res < 0) {
                    decode_failed = true;
                    break;
                }
                res_buff_size += poll_size;
                finish_res = heatshrink_decoder_finish(decoder);
            } while(finish_res != HSDR_FINISH_DONE);
        }
    }
    *data_res_size = res_buff_size;
    return !decode_failed;
}

typedef struct {
    uint8_t* compressed_data;
    size_t compressed_data_size;
    uint8_t* data_out;
    size_t data_out_size;
} stream_state;

static size_t comp_stream_read(uint8_t* dest, size_t read_size, void* context) {
    stream_state* state = (stream_state*)context;

    if(read_size > state->compressed_data_size) {
        read_size = state->compressed_data_size;
    }
    memcpy(dest, state->compressed_data, read_size);
    state->compressed_data += read_size;
    state->compressed_data_size -= read_size;
    return read_size;
}

static size_t comp_stream_write(uint8_t* src, size_t write_size, void* context) {
    stream_state* state = (stream_state*)context;

    if(write_size > state->data_out_size) {
        write_size = state->data_out_size;
    }
    memcpy(state->data_out, src, write_size);
    state->data_out += write_size;
    state->data_out_size -= write_size;
    return write_size;
}

bool compress_decode_hs_(
    heatshrink_decoder* decoder,
    const uint8_t* compressed_data,
    size_t compressed_size,
    uint8_t* data_out,
    size_t data_out_size,
    size_t* data_res_size) {
    UNUSED(compress_decode_hs_old);
    // Use stream decoding API
    stream_state state = {
        .compressed_data = (uint8_t*)compressed_data,
        .compressed_data_size = compressed_size,
        .data_out = data_out,
        .data_out_size = data_out_size,
    };
    if(compress_decode_stream_internal(decoder, comp_stream_read, comp_stream_write, &state)) {
        *data_res_size = data_out_size - state.data_out_size;
        return true;
    }
    return false;
}

static bool compress_decode_internal(
    heatshrink_decoder* decoder,
    const uint8_t* data_in,
    size_t data_in_size,
    uint8_t* data_out,
    size_t data_out_size,
    size_t* data_res_size) {
    furi_check(decoder);
    furi_check(data_in);
    furi_check(data_out);
    furi_check(data_res_size);

    bool result = false;

    CompressHeader* header = (CompressHeader*)data_in;
    if(header->is_compressed) {
        return compress_decode_hs_(
            decoder,
            &data_in[sizeof(CompressHeader)],
            header->compressed_buff_size,
            data_out,
            data_out_size,
            data_res_size);
    } else if(data_out_size >= data_in_size - 1) {
        memcpy(data_out, &data_in[1], data_in_size);
        *data_res_size = data_in_size - 1;
        result = true;
    } else {
        /* Not enough space in output buffer */
        result = false;
    }
    return result;
}

bool compress_encode(
    Compress* compress,
    uint8_t* data_in,
    size_t data_in_size,
    uint8_t* data_out,
    size_t data_out_size,
    size_t* data_res_size) {
    if(!compress->encoder) {
        CompressConfigHeatshrink* hs_config = (CompressConfigHeatshrink*)compress->config;
        compress->encoder =
            heatshrink_encoder_alloc(hs_config->window_sz2, hs_config->lookahead_sz2);
    }
    return compress_encode_internal(
        compress->encoder, data_in, data_in_size, data_out, data_out_size, data_res_size);
}

bool compress_decode(
    Compress* compress,
    uint8_t* data_in,
    size_t data_in_size,
    uint8_t* data_out,
    size_t data_out_size,
    size_t* data_res_size) {
    if(!compress->decoder) {
        CompressConfigHeatshrink* hs_config = (CompressConfigHeatshrink*)compress->config;
        compress->decoder = heatshrink_decoder_alloc(
            hs_config->input_buffer_sz, hs_config->window_sz2, hs_config->lookahead_sz2);
    }
    return compress_decode_internal(
        compress->decoder, data_in, data_in_size, data_out, data_out_size, data_res_size);
}

bool compress_decode_stream(
    Compress* compress,
    compress_io_cb_t read_cb,
    compress_io_cb_t write_cb,
    void* context) {
    if(!compress->decoder) {
        CompressConfigHeatshrink* hs_config = (CompressConfigHeatshrink*)compress->config;
        compress->decoder = heatshrink_decoder_alloc(
            hs_config->input_buffer_sz, hs_config->window_sz2, hs_config->lookahead_sz2);
    }

    return compress_decode_stream_internal(compress->decoder, read_cb, write_cb, context);
}
