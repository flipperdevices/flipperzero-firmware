#include <furi-hal-compress.h>

#include <furi.h>
#include <lib/heatshrink/heatshrink_encoder.h>
#include <lib/heatshrink/heatshrink_decoder.h>

#define FURI_HAL_COMPRESS_ICON_ENCODED_BUFF_SIZE (512)
#define FURI_HAL_COMPRESS_ICON_DECODED_BUFF_SIZE (1024)

#define FURI_HAL_COMPRESS_EXP_BUFF_SIZE (1 << FURI_HAL_COMPRESS_EXP_BUFF_SIZE_LOG)

typedef struct {
    uint8_t is_compressed;
    uint8_t reserved;
    uint16_t compressed_buff_size;
} FuriHalCompressHeader;

typedef struct {
    heatshrink_decoder* decoder;
    uint8_t compress_buff[FURI_HAL_COMPRESS_EXP_BUFF_SIZE + FURI_HAL_COMPRESS_ICON_ENCODED_BUFF_SIZE];
    uint8_t decoded_buff[FURI_HAL_COMPRESS_ICON_DECODED_BUFF_SIZE];
} FuriHalCompressIcon;

struct FuriHalCompress {
    heatshrink_encoder* encoder;
    heatshrink_decoder* decoder;
    uint8_t *compress_buff;
    uint16_t compress_buff_size;
};

static FuriHalCompressIcon* icon_decoder;

static void furi_hal_compress_reset(FuriHalCompress* compress) {
    furi_assert(compress);
    heatshrink_encoder_reset(compress->encoder);
    heatshrink_decoder_reset(compress->decoder);
    memset(compress->compress_buff, 0, compress->compress_buff_size);
}

void furi_hal_compress_init() {
    icon_decoder = furi_alloc(sizeof(FuriHalCompressIcon));
    icon_decoder->decoder = heatshrink_decoder_alloc(
        icon_decoder->compress_buff,
        FURI_HAL_COMPRESS_ICON_ENCODED_BUFF_SIZE,
        FURI_HAL_COMPRESS_EXP_BUFF_SIZE_LOG,
        FURI_HAL_COMPRESS_LOOKAHEAD_BUFF_SIZE_LOG);
    heatshrink_decoder_reset(icon_decoder->decoder);
    memset(icon_decoder->decoded_buff, 0, sizeof(icon_decoder->decoded_buff));
    FURI_LOG_I("FuriHalCompress", "Init OK");
}

void furi_hal_compress_icon_decode(const uint8_t* icon_data, uint8_t** decoded_buff) { 
    furi_assert(icon_data);
    furi_assert(decoded_buff);

    FuriHalCompressHeader* header = (FuriHalCompressHeader*) icon_data;
    if(header->is_compressed) {
        size_t data_processed = 0;
        heatshrink_decoder_sink(icon_decoder->decoder, (uint8_t*)&icon_data[4], header->compressed_buff_size, &data_processed);
        while(
            heatshrink_decoder_poll(
                icon_decoder->decoder,
                icon_decoder->decoded_buff,
                sizeof(icon_decoder->decoded_buff),
                &data_processed) == HSDR_POLL_MORE
            ) {};
        heatshrink_decoder_reset(icon_decoder->decoder);
        *decoded_buff = icon_decoder->decoded_buff;
    } else {
        *decoded_buff = (uint8_t*)&icon_data[1];
    }
}

FuriHalCompress* furi_hal_compress_alloc(uint16_t copress_buff_size) {
    FuriHalCompress* compress = furi_alloc(sizeof(FuriHalCompress));
    compress->compress_buff = furi_alloc(copress_buff_size + FURI_HAL_COMPRESS_EXP_BUFF_SIZE);
    compress->encoder = heatshrink_encoder_alloc(compress->compress_buff, FURI_HAL_COMPRESS_EXP_BUFF_SIZE_LOG, FURI_HAL_COMPRESS_LOOKAHEAD_BUFF_SIZE_LOG);
    compress->decoder = heatshrink_decoder_alloc(compress->compress_buff, copress_buff_size, FURI_HAL_COMPRESS_EXP_BUFF_SIZE_LOG, FURI_HAL_COMPRESS_LOOKAHEAD_BUFF_SIZE_LOG);

    return compress;
}

void furi_hal_compress_free(FuriHalCompress* compress) {
    furi_assert(compress);

    heatshrink_encoder_free(compress->encoder);
    heatshrink_decoder_free(compress->decoder);
    free(compress->compress_buff);
    free(compress);
}

bool furi_hal_compress_encode(FuriHalCompress* compress, uint8_t* data_in, size_t data_in_size, uint8_t* data_out, size_t data_out_size, size_t* data_res_size) {
    furi_assert(compress);
    furi_assert(data_in);
    furi_assert(data_in_size);

    size_t sink_size = 0;
    size_t poll_size = 0;
    HSE_sink_res sink_res;
    HSE_poll_res poll_res;
    bool encode_failed = false;

    size_t sunk = 0;
    size_t res_buff_size = 0;
    while((sunk < data_in_size) && !encode_failed) {
        sink_res = heatshrink_encoder_sink(compress->encoder, &data_in[sunk], data_in_size - sunk, &sink_size);
        if(sink_res != HSER_SINK_OK) {
            encode_failed = true;
            break;
        }
        sunk += sink_size;
        do {
            poll_res = heatshrink_encoder_poll(compress->encoder, &data_out[4], data_out_size - 4, &poll_size);
            if(poll_res < 0) {
                encode_failed = true;
                break;
            }
            res_buff_size += poll_size;
        } while(poll_res == HSER_POLL_MORE);
    }

    bool result = true;
    if(!encode_failed && (res_buff_size < data_in_size + 1)) {
        FuriHalCompressHeader header = {.is_compressed = 0x01, .reserved = 0x00, .compressed_buff_size = res_buff_size};
        memcpy(data_out, &header, sizeof(header));
        *data_res_size = res_buff_size + 4;
    } else if (data_out_size > data_in_size) {
        data_out[0] = 0x00;
        memcpy(&data_out[1], data_in, data_in_size);
        *data_res_size = data_in_size + 1;
    } else {
        *data_res_size = 0;
        result = false;
    }
    furi_hal_compress_reset(compress);

    return result;
}

bool furi_hal_compress_decode(FuriHalCompress* compress, uint8_t* data_in, size_t data_in_size, uint8_t* data_out, size_t data_out_size, size_t* data_res_size) {
    return true;
}
