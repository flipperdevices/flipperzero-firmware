#include "infrared_protocol_sirc_i.h"

const InfraredCommonProtocolSpec protocol_sirc = {
    .timings =
        {
            .preamble_mark = INFRARED_SIRC_PREAMBLE_MARK,
            .preamble_space = INFRARED_SIRC_PREAMBLE_SPACE,
            .bit1_mark = INFRARED_SIRC_BIT1_MARK,
            .bit1_space = INFRARED_SIRC_BIT1_SPACE,
            .bit0_mark = INFRARED_SIRC_BIT0_MARK,
            .bit0_space = INFRARED_SIRC_BIT0_SPACE,
            .preamble_tolerance = INFRARED_SIRC_PREAMBLE_TOLERANCE,
            .bit_tolerance = INFRARED_SIRC_BIT_TOLERANCE,
            .silence_time = INFRARED_SIRC_SILENCE,
            .min_split_time = INFRARED_SIRC_MIN_SPLIT_TIME,
        },
    .databit_len[0] = 20,
    .databit_len[1] = 15,
    .databit_len[2] = 12,
    .no_stop_bit = true,
    .decode = infrared_common_decode_pdwm,
    .encode = infrared_common_encode_pdwm,
    .interpret = infrared_decoder_sirc_interpret,
    .decode_repeat = NULL,
    .encode_repeat = infrared_encoder_sirc_encode_repeat,
};

static const InfraredProtocolSpecification infrared_sirc_protocol_specification = {
    .name = "SIRC",
    .address_length = 5,
    .command_length = 7,
    .frequency = INFRARED_SIRC_CARRIER_FREQUENCY,
    .duty_cycle = INFRARED_SIRC_DUTY_CYCLE,
};

static const InfraredProtocolSpecification infrared_sirc15_protocol_specification = {
    .name = "SIRC15",
    .address_length = 8,
    .command_length = 7,
    .frequency = INFRARED_SIRC_CARRIER_FREQUENCY,
    .duty_cycle = INFRARED_SIRC_DUTY_CYCLE,
};

static const InfraredProtocolSpecification infrared_sirc20_protocol_specification = {
    .name = "SIRC20",
    .address_length = 13,
    .command_length = 7,
    .frequency = INFRARED_SIRC_CARRIER_FREQUENCY,
    .duty_cycle = INFRARED_SIRC_DUTY_CYCLE,
};

const InfraredProtocolSpecification* infrared_sirc_get_spec(InfraredProtocol protocol) {
    if(protocol == InfraredProtocolSIRC)
        return &infrared_sirc_protocol_specification;
    else if(protocol == InfraredProtocolSIRC15)
        return &infrared_sirc15_protocol_specification;
    else if(protocol == InfraredProtocolSIRC20)
        return &infrared_sirc20_protocol_specification;
    else
        return NULL;
}
