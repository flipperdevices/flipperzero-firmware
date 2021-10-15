#include <furi-hal-compress.h>

#include <furi.h>

static FuriHalCompressIcon icon_decoder;

typedef struct {
    uint8_t is_compressed;
    uint8_t reserved;
    uint16_t compressed_buff_size;
} FuriHalCompressHeader;

void furi_hal_compress_init() {
    heatshrink_decoder_reset(&icon_decoder.decoder);
    memset(icon_decoder.decoded_buff, 0, sizeof(icon_decoder.decoded_buff));
    icon_decoder.decoded_buff_size = 0;
}

void furi_hal_compress_encode() {

}

void furi_hal_compress_icon_decode(const uint8_t* icon_data, uint8_t** decoded_buff) { 
    furi_assert(icon_data);
    furi_assert(decoded_buff);

    FuriHalCompressHeader* header = (FuriHalCompressHeader*) icon_data;
    if(header->is_compressed) {
        size_t data_processed = 0;
        heatshrink_decoder_sink(&icon_decoder.decoder, (uint8_t*)&icon_data[4], header->compressed_buff_size, &data_processed);
        while(heatshrink_decoder_poll(&icon_decoder.decoder, icon_decoder.decoded_buff, sizeof(icon_decoder.decoded_buff), &data_processed) == HSDR_POLL_MORE) {};
        heatshrink_decoder_reset(&icon_decoder.decoder);
        *decoded_buff = icon_decoder.decoded_buff;
    } else {
        *decoded_buff = (uint8_t*)&icon_data[1];
    }
}
