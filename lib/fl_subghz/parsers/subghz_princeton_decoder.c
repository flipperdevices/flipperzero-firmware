#include "subghz_princeton_decoder.h"

/*
 * Help
 * https://phreakerclub.com/447
 *
 */

CC1101ProtocolStruct subghz_protocol_princeton;

void subghz_princeton_decoder_init(void) {
    snprintf(subghz_protocol_princeton.name, CC1101_PROTOCOL_NAME_LEN,"PRINCETON");
    subghz_protocol_princeton.code_min_count_bit_for_found = 24;
    subghz_protocol_princeton.te_shot = 450;//150;
    subghz_protocol_princeton.te_long = 1350;//450;
    subghz_protocol_princeton.te_delta = 200;//50;
}

void subghz_princeton_decoder_send_bit(uint8_t bit) {
    if (bit) {
        //send bit 1
        SUBGHZ_TX_PIN_LOW();
        delay_us(subghz_protocol_princeton.te_long);
        SUBGHZ_TX_PIN_HIGTH();
        delay_us(subghz_protocol_princeton.te_shot);
    } else {
        //send bit 0
        SUBGHZ_TX_PIN_LOW();
        delay_us(subghz_protocol_princeton.te_shot);
        SUBGHZ_TX_PIN_HIGTH();
        delay_us(subghz_protocol_princeton.te_long);
    }
}

void subghz_princeton_decoder_send_key(uint64_t key, uint8_t bit,uint8_t repeat) {
    while (repeat--) {
        SUBGHZ_TX_PIN_LOW();
        //Send start bit
        subghz_princeton_decoder_send_bit(1);
        //Send header
        delay_us(subghz_protocol_princeton.te_shot * 33); //+2 interval v bit 1
        //Send key data
        for (uint8_t i = bit; i > 0; i--) {
            subghz_princeton_decoder_send_bit(bit_read(key, i - 1));
        }
    }
}

void subghz_princeton_decoder_parse(LevelPair data) {
    switch (subghz_protocol_princeton.parser_step) {
    case 0:
        if ((data.level == ApiHalSubGhzCaptureLevelLow)
                && (DURATION_DIFF(data.duration,subghz_protocol_princeton.te_shot * 36)< subghz_protocol_princeton.te_delta * 36)) {
            //Found Preambula
            subghz_protocol_princeton.parser_step = 1;
            subghz_protocol_princeton.code_found = 0;
            subghz_protocol_princeton.code_count_bit = 0;
        } else {
            subghz_protocol_princeton.parser_step = 0;
        }

        break;
    case 1:
        //save duration
        if (data.level == ApiHalSubGhzCaptureLevelHigh) {
            subghz_protocol_princeton.te_last = data.duration;
            subghz_protocol_princeton.parser_step = 2;
        }

        break;
    case 2:
        if (data.level == ApiHalSubGhzCaptureLevelLow) {
            if (data.duration>= (subghz_protocol_princeton.te_shot * 10+ subghz_protocol_princeton.te_delta)) {
                subghz_protocol_princeton.parser_step = 1;
                if (subghz_protocol_princeton.code_count_bit>= subghz_protocol_princeton.code_min_count_bit_for_found) {

                    //ToDo out data display
                    subghz_protocol_header_printf(&subghz_protocol_princeton);

                }
                subghz_protocol_princeton.code_found = 0;
                subghz_protocol_princeton.code_count_bit = 0;
                break;
            }

            if ((DURATION_DIFF(subghz_protocol_princeton.te_last,subghz_protocol_princeton.te_shot)< subghz_protocol_princeton.te_delta)
                    && (DURATION_DIFF(data.duration,subghz_protocol_princeton.te_long)< subghz_protocol_princeton.te_delta*3)) {
                subghz_protocol_header_add_bit(&subghz_protocol_princeton, 0);
                subghz_protocol_princeton.parser_step = 1;
            } else if ((DURATION_DIFF(subghz_protocol_princeton.te_last,subghz_protocol_princeton.te_long)< subghz_protocol_princeton.te_delta*3)
                    && (DURATION_DIFF(data.duration,subghz_protocol_princeton.te_shot)< subghz_protocol_princeton.te_delta)) {
                subghz_protocol_header_add_bit(&subghz_protocol_princeton, 1);
                subghz_protocol_princeton.parser_step = 1;
            } else {
                subghz_protocol_princeton.parser_step = 0;
            }
        } else {
            subghz_protocol_princeton.parser_step = 0;
        }

        break;
    }
}

