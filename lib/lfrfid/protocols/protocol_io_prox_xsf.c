#include <furi.h>
#include <toolbox/protocols/protocol.h>
#include <lfrfid/tools/fsk_demod.h>
#include <lfrfid/tools/fsk_osc.h>

typedef struct {
} ProtocolIOProxXSF;

void* protocol_io_prox_xsf_alloc(void) {
    ProtocolIOProxXSF* protocol = malloc(sizeof(ProtocolIOProxXSF));
    return protocol;
};

void protocol_io_prox_xsf_free(void* protocol) {
    free(protocol);
};

void protocol_io_prox_xsf_set_data(void* protocol, const uint8_t* data, size_t data_size) {
    UNUSED(protocol);
    UNUSED(data);
    UNUSED(data_size);
};

void protocol_io_prox_xsf_get_data(void* protocol, uint8_t* data, size_t data_size) {
    UNUSED(protocol);
    UNUSED(data);
    UNUSED(data_size);
};

size_t protocol_io_prox_xsf_get_data_size(void* protocol) {
    UNUSED(protocol);
    return 0;
};

const char* protocol_io_prox_xsf_get_name(void* protocol) {
    UNUSED(protocol);
    return "IO-Prox-XSF";
};

const char* protocol_io_prox_xsf_get_manufacturer(void* protocol) {
    UNUSED(protocol);
    return "IO-Prox";
};

void protocol_io_prox_xsf_decoder_start(void* protocol) {
    UNUSED(protocol);
};

bool protocol_io_prox_xsf_decoder_feed(void* protocol, bool level, uint32_t duration) {
    UNUSED(protocol);
    UNUSED(level);
    UNUSED(duration);
    return false;
};

void protocol_io_prox_xsf_decoder_reset(void* protocol) {
    UNUSED(protocol);
};

bool protocol_io_prox_xsf_encoder_start(void* protocol) {
    UNUSED(protocol);
    return false;
};

LevelDuration protocol_io_prox_xsf_encoder_yield(void* protocol) {
    UNUSED(protocol);
    return level_duration_make(false, 0);
};

void protocol_io_prox_xsf_encoder_reset(void* protocol) {
    UNUSED(protocol);
};

const ProtocolBase protocol_io_prox_xsf = {
    .alloc = (ProtocolAlloc)protocol_io_prox_xsf_alloc,
    .free = (ProtocolFree)protocol_io_prox_xsf_free,
    .set_data = (ProtocolSetData)protocol_io_prox_xsf_set_data,
    .get_data = (ProtocolGetData)protocol_io_prox_xsf_get_data,
    .get_data_size = (ProtocolGetDataSize)protocol_io_prox_xsf_get_data_size,
    .get_name = (ProtocolGetName)protocol_io_prox_xsf_get_name,
    .get_manufacturer = (ProtocolGetManufacturer)protocol_io_prox_xsf_get_manufacturer,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_io_prox_xsf_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_io_prox_xsf_decoder_feed,
            .reset = (ProtocolDecoderReset)protocol_io_prox_xsf_decoder_reset,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_io_prox_xsf_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_io_prox_xsf_encoder_yield,
            .reset = (ProtocolEncoderReset)protocol_io_prox_xsf_encoder_reset,
        },
};