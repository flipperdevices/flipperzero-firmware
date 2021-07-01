#include "subghz_protocol_nice_flor_s.h"
/*
 * https://phreakerclub.com/1615
 * https://phreakerclub.com/forum/showthread.php?t=2360
 * https://vrtp.ru/index.php?showtopic=27867
 */

struct SubGhzProtocolNiceFlorS {
    SubGhzProtocolCommon common;
};

SubGhzProtocolNiceFlorS* subghz_protocol_nice_flor_s_alloc() {
    SubGhzProtocolNiceFlorS* instance = furi_alloc(sizeof(SubGhzProtocolNiceFlorS));

    instance->common.name = "Nice FloR S";
    instance->common.code_min_count_bit_for_found = 52;
    instance->common.te_shot = 500;
    instance->common.te_long = 1000;
    instance->common.te_delta = 300;

    return instance;
}

void subghz_protocol_nice_flor_s_free(SubGhzProtocolNiceFlorS* instance) {
    furi_assert(instance);
    free(instance);
}

void subghz_protocol_nice_flor_s_send_bit(SubGhzProtocolNiceFlorS* instance, uint8_t bit) {
    if (bit) {
        //send bit 1
        SUBGHZ_TX_PIN_HIGTH();
        delay_us(instance->common.te_long);
        SUBGHZ_TX_PIN_LOW();
        delay_us(instance->common.te_shot);
    } else {
        //send bit 0
        SUBGHZ_TX_PIN_HIGTH();
        delay_us(instance->common.te_shot);
        SUBGHZ_TX_PIN_LOW();
        delay_us(instance->common.te_long);
    }
}

void subghz_protocol_nice_flor_s_send_key(SubGhzProtocolNiceFlorS* instance, uint64_t key, uint8_t bit, uint8_t repeat) {
    while (repeat--) {
        //Send header
        SUBGHZ_TX_PIN_LOW();
        delay_us(instance->common.te_shot * 34);
        //Send Start Bit
        SUBGHZ_TX_PIN_HIGTH();
        delay_us(instance->common.te_shot*3);
        SUBGHZ_TX_PIN_LOW();
        delay_us(instance->common.te_shot*3);
        //Send key data
        for (uint8_t i = bit; i > 0; i--) {
            subghz_protocol_nice_flor_s_send_bit(instance, bit_read(key, i - 1));
        }
        //Send Stop Bit
        SUBGHZ_TX_PIN_HIGTH();
        delay_us(instance->common.te_shot*3);
        SUBGHZ_TX_PIN_LOW();
        delay_us(instance->common.te_shot*3);
    }
}


void subghz_nice_flor_s_decoder_decrypt (SubGhzProtocolNiceFlorS* instance){
    //P2 (4-бита) - часть серийного номера, P2 = (K ^ S3) & 0xF;
    //P3 (байт) - старшая часть зашифрованного индекса.
    //P4 (байт) - младшая часть зашифрованного индекса.
    //P5 (байт) - часть серийного номера, P5 = K ^ S2;
    //P6 (байт) - часть серийного номера, P6 = K ^ S1;
    //P7 (байт) - часть серийного номера, P7 = K ^ S0;
    //K (байт) - зависит от P3 и P4, K = Fk(P3, P4);
    //S3,S2,S1,S0 - серийный номер пульта 28 бит.

    uint16_t p3p4 = (uint16_t)(instance->common.code_found>>24);
    instance->common.cnt = 0x00 << 8 | 0x00; //nice_flor_srainbow_table_for_search[p3p4]; тут надо считать поле с файла причем адрес надо у множить на 2
    //instance->common.cnt = nice_flor_srainbow_table_for_search[p3p4]; тут надо считать поле с файла
    
    //uint8_t  k =(uint8_t)(p3p4 &0x00FF) ^ nice_flor_srainbow_table_for_search[0x10000|subghz_protocol_nice_flor_s.cnt & 0x00ff];
    //тут надо считать поле в конце таблицы [0х20000 | instance->common.cnt & 0x00ff] ВРОДЕ ТАК ПРОВЕРИТЬ
    uint8_t  k =(uint8_t)(p3p4 &0x00FF) ^ 0x00; //nice_flor_srainbow_table_for_search[0x10000|subghz_protocol_nice_flor_s.cnt & 0x00ff];

    
    uint8_t s3= ((uint8_t)(instance->common.code_found>>40) ^k)&0x0f;
    uint8_t s2= ((uint8_t)(instance->common.code_found>>16) ^k);
    uint8_t s1= ((uint8_t)(instance->common.code_found>>8)  ^k);
    uint8_t s0= ((uint8_t)(instance->common.code_found)     ^k);
    instance->common.serial = s3<<24 | s2<<16 | s1<<8 | s0;

    instance->common.btn = (instance->common.code_found >> 48) &0x0f;
    if (instance->common.callback) instance->common.callback((SubGhzProtocolCommon*)instance, instance->common.context);
}

void subghz_protocol_nice_flor_s_parse(SubGhzProtocolNiceFlorS* instance, LevelPair data) {
    switch (instance->common.parser_step) {
    case 0:
        if ((data.level == ApiHalSubGhzCaptureLevelLow)
                && (DURATION_DIFF(data.duration,instance->common.te_shot * 38)< instance->common.te_delta * 38)) {
            //Found start header Nice Flor-S
            instance->common.parser_step = 1;
        } else {
            instance->common.parser_step = 0;
        }
        break;
    case 1:
        if ((data.level == ApiHalSubGhzCaptureLevelHigh)
                && (DURATION_DIFF(data.duration,instance->common.te_shot * 3)< instance->common.te_delta * 3)) {
            //Found next header Nice Flor-S
            instance->common.parser_step = 2;
        } else {
            instance->common.parser_step = 0;
        }
        break;
    case 2:
        if ((data.level == ApiHalSubGhzCaptureLevelLow)
                && (DURATION_DIFF(data.duration,instance->common.te_shot * 3)< instance->common.te_delta * 3)) {
            //Found header Nice Flor-S
            instance->common.parser_step = 3;
            instance->common.code_found = 0;
            instance->common.code_count_bit = 0;
        } else {
            instance->common.parser_step = 0;
        }
        break;
    case 3:
        if (data.level == ApiHalSubGhzCaptureLevelHigh) {
            if(DURATION_DIFF(data.duration,instance->common.te_shot*3) < instance->common.te_delta){
                //Found STOP bit
                instance->common.parser_step = 0;
                if (instance->common.code_count_bit>= instance->common.code_min_count_bit_for_found) {

                    //ToDo out data display
                    subghz_nice_flor_s_decoder_decrypt(instance);
                }
                break;
            } else {
                //save interval
                instance->common.te_last = data.duration;
                instance->common.parser_step = 4;
            }
        }
        break;
    case 4:
        if (data.level == ApiHalSubGhzCaptureLevelLow) {
            if ((DURATION_DIFF(instance->common.te_last,instance->common.te_shot) < instance->common.te_delta)
                    && (DURATION_DIFF(data.duration,instance->common.te_long)< instance->common.te_delta)) {
                subghz_protocol_common_add_bit(&instance->common, 0);
                instance->common.parser_step = 3;
            } else if ((DURATION_DIFF(instance->common.te_last,instance->common.te_long)< instance->common.te_delta)
                    && (DURATION_DIFF(data.duration,instance->common.te_shot)< instance->common.te_delta)) {
                subghz_protocol_common_add_bit(&instance->common, 1);
                instance->common.parser_step = 3;
            } else
                instance->common.parser_step = 0;
        } else {
            instance->common.parser_step = 0;
        }
        break;
    }
}
