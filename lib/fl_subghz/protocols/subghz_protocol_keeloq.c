#include "subghz_protocol_keeloq.h"

#include <m-string.h>
#include <m-array.h>

#include <furi.h>

typedef struct {
    string_t name;
    uint64_t key;
} KeeLoqManufactureCode;

ARRAY_DEF(KeeLoqManufactureCodeArray, KeeLoqManufactureCode, M_POD_OPLIST)
#define M_OPL_KeeLoqManufactureCodeArray_t() ARRAY_OPLIST(KeeLoqManufactureCodeArray, M_POD_OPLIST)

struct SubGhzProtocolKeeloq {
    SubGhzProtocolCommon common;
    KeeLoqManufactureCodeArray_t manufacture_codes;
};

/*
 * keelog
 * https://ru.wikipedia.org/wiki/KeeLoq
 *
 */
#define KeeLoq_NLF         0x3A5C742E
#define bit(x,n)           (((x)>>(n))&1)
#define g5(x,a,b,c,d,e)    (bit(x,a)+bit(x,b)*2+bit(x,c)*4+bit(x,d)*8+bit(x,e)*16)

//Simple Learning
inline uint32_t subghz_protocol_keeloq_encrypt(const uint32_t data, const uint64_t key) {
    uint32_t x = data, r;
    for (r = 0; r < 528; r++)
        x = (x>>1)^((bit(x,0)^bit(x,16)^(uint32_t)bit(key,r&63)^bit(KeeLoq_NLF,g5(x,1,9,20,26,31)))<<31);
    return x;
}

inline uint32_t subghz_protocol_keeloq_decrypt(const uint32_t data, const uint64_t key) {
    uint32_t x = data, r;
    for (r = 0; r < 528; r++)
        x = (x<<1)^bit(x,31)^bit(x,15)^(uint32_t)bit(key,(15-r)&63)^bit(KeeLoq_NLF,g5(x,0,8,19,25,30));
    return x;
}

/** Normal_Learning
 * https://phreakerclub.com/forum/showthread.php?t=67
 * @param data - serial number (28bit)
 * @param key - manufacture (64bit)
 * @return ?
 */
uint64_t subghz_protocol_keeloq_normal_learning(uint32_t data, const uint64_t key){
    uint32_t k1,k2;

    data&=0x0FFFFFFF;
    data|=0x20000000;
    k1=subghz_protocol_keeloq_decrypt(data, key);

    data&=0x0FFFFFFF;
    data|=0x60000000;
    k2=subghz_protocol_keeloq_decrypt(data, key);

    return ((uint64_t)k2<<32)| k1; // key - shifrovanoya
}

/*
 * Help
 * https://phreakerclub.com/forum/showthread.php?t=1094
 *
 */
SubGhzProtocolKeeloq* subghz_protocol_keeloq_alloc() {
    SubGhzProtocolKeeloq* instance = furi_alloc(sizeof(SubGhzProtocolKeeloq));

    snprintf(instance->common.name, SUBGHZ_PROTOCOL_NAME_LEN, "KEELOQ");
    instance->common.code_min_count_bit_for_found = 64;
    instance->common.te_shot = 400;
    instance->common.te_long = 800;
    instance->common.te_delta = 140;

    KeeLoqManufactureCodeArray_init(instance->manufacture_codes);

    return instance;
}

void subghz_protocol_keeloq_free(SubGhzProtocolKeeloq* instance) {
    furi_assert(instance);
    for
        M_EACH(manufacture_code, instance->manufacture_codes, KeeLoqManufactureCodeArray_t) {
            string_clear(manufacture_code->name);
            manufacture_code->key = 0;
    }
    KeeLoqManufactureCodeArray_clear(instance->manufacture_codes);
    free(instance);
}

void subghz_protocol_keeloq_add_manafacture_key(SubGhzProtocolKeeloq* instance, const char* name, uint64_t key) {
    KeeLoqManufactureCode* manufacture_code = KeeLoqManufactureCodeArray_push_raw(instance->manufacture_codes);
    string_init_set_str(manufacture_code->name, name);
    manufacture_code->key = key;
}

uint8_t subghz_protocol_keeloq_check_remote_controller_selector(SubGhzProtocolKeeloq* instance, uint32_t fix , uint32_t hop) {
      uint16_t end_serial =(uint16_t)(fix&0x3FF);
      uint8_t btn = (uint8_t)(fix>>28);
      uint32_t decript=0;

      for
        M_EACH(manufacture_code, instance->manufacture_codes, KeeLoqManufactureCodeArray_t) {

          decript=subghz_protocol_keeloq_decrypt(hop, manufacture_code->key);
          if( (decript>>28 ==btn) && ((((uint16_t)(decript>>16))&0x3FF)==end_serial)){
              snprintf(instance->common.name_remote_controller, SUBGHZ_PROTOCOL_NAME_LEN, string_get_cstr(manufacture_code->name));
              instance->common.cnt= decript&0x0000FFFF;
              return 1;
          }

          // проверка на зеркальный мак
          uint64_t man_rev=0;
          uint64_t man_rev_byte=0;
          for(uint8_t i=0; i<64; i+=8){
              man_rev_byte=(uint8_t)(manufacture_code->key >> i);
              man_rev = man_rev  | man_rev_byte << (56-i);
          }
          decript=subghz_protocol_keeloq_decrypt(hop, man_rev);
          if( (decript>>28 ==btn) && ((((uint16_t)(decript>>16))&0x3FF)==end_serial)){
                  snprintf(instance->common.name_remote_controller, SUBGHZ_PROTOCOL_NAME_LEN, string_get_cstr(manufacture_code->name));
                  instance->common.cnt= decript&0x0000FFFF;
                  return 1;
              }
          ////////////////////////////////////
      }
      snprintf(instance->common.name_remote_controller, SUBGHZ_PROTOCOL_NAME_LEN, "Unknown");
      instance->common.cnt=0;
      return 0;
}

void subghz_protocol_keeloq_check_remote_controller(SubGhzProtocolKeeloq* instance) {
    uint64_t key = subghz_protocol_common_reverse_key(instance->common.code_found, instance->common.code_count_bit);
    uint32_t key_fix = key >> 32;
    uint32_t key_hop = key & 0x00000000ffffffff;
    //check key AN-Motors
    if((key_hop >> 24) == ((key_hop>>16)&0x00ff) && (key_fix>>28) ==((key_hop>>12)&0x0f) ){
        snprintf(instance->common.name_remote_controller, SUBGHZ_PROTOCOL_NAME_LEN, "AN-Motors");
        instance->common.cnt = key_hop>>16;
    } else {
        subghz_protocol_keeloq_check_remote_controller_selector(instance, key_fix, key_hop);
    }
    subghz_protocol_common_printf(&instance->common);
}

void subghz_protocol_keeloq_send_bit(SubGhzProtocolKeeloq* instance, uint8_t bit) {
    if (bit) {
        //send bit 1
        SUBGHZ_TX_PIN_HIGTH();
        delay_us(instance->common.te_shot);
        SUBGHZ_TX_PIN_LOW();
        delay_us(instance->common.te_long);
    } else {
        //send bit 0
        SUBGHZ_TX_PIN_HIGTH();
        delay_us(instance->common.te_long);
        SUBGHZ_TX_PIN_LOW();
        delay_us(instance->common.te_shot);
    }
}

void subghz_protocol_keeloq_send_key(SubGhzProtocolKeeloq* instance, uint64_t key, uint8_t bit, uint8_t repeat) {
    while (repeat--) {
        //Send header
        for (uint8_t i = 11; i > 0; i--) {
            SUBGHZ_TX_PIN_HIGTH();
            delay_us(instance->common.te_shot);
            SUBGHZ_TX_PIN_LOW();
            delay_us(instance->common.te_shot);
        }
        delay_us(instance->common.te_shot * 9); //+1 up Send header

        for (uint8_t i = bit; i > 0; i--) {
            subghz_protocol_keeloq_send_bit(instance, bit_read(key, i - 1));
        }
        //+send 2 status bit
        subghz_protocol_keeloq_send_bit(instance, 0);
        subghz_protocol_keeloq_send_bit(instance, 0);
        //send end
        subghz_protocol_keeloq_send_bit(instance, 0);
        delay_us(instance->common.te_shot * 2);   //+2 interval END SEND
    }
}

void subghz_protocol_keeloq_parse(SubGhzProtocolKeeloq* instance, LevelPair data) {
    switch (instance->common.parser_step) {
    case 0:
        if ((data.level == ApiHalSubGhzCaptureLevelHigh) && DURATION_DIFF(data.duration, instance->common.te_shot)< instance->common.te_delta) {
            instance->common.parser_step = 1;
            instance->common.header_count++;
        } else {
            instance->common.parser_step = 0;
        }

        break;
    case 1:
        if ((data.level == ApiHalSubGhzCaptureLevelLow) && (DURATION_DIFF(data.duration, instance->common.te_shot ) < instance->common.te_delta)) {
            instance->common.parser_step = 0;
            break;
        }
        if ((instance->common.header_count > 2) && ( DURATION_DIFF(data.duration, instance->common.te_shot * 10)< instance->common.te_delta * 10)) {
            //Found header
            instance->common.parser_step = 2;
            instance->common.code_found = 0;
            instance->common.code_count_bit = 0;
        } else {
            instance->common.parser_step = 0;
            instance->common.header_count = 0;
        }
        break;
    case 2:
        if (data.level == ApiHalSubGhzCaptureLevelHigh) {
            instance->common.te_last = data.duration;
            instance->common.parser_step = 3;
        }
        break;
    case 3:
        if (data.level == ApiHalSubGhzCaptureLevelLow) {
            if (data.duration >= (instance->common.te_shot*2+ instance->common.te_delta)) {
                //Found end TX
                instance->common.parser_step = 0;
                if (instance->common.code_count_bit >= instance->common.code_min_count_bit_for_found) {
                    //&& (instance->common.code_last_found != instance->common.code_found )) {
                    instance->common.code_last_found = instance->common.code_found;

                    //ToDo out data display
                    //subghz_protocol_common_printf(&instance->common);
                    subghz_protocol_keeloq_check_remote_controller(instance);

                    //Print_Code(&KEELOQ);
                    //Reverse_Code(KEELOQ.Code);
                    instance->common.code_found = 0;
                    instance->common.code_count_bit = 0;
                    instance->common.header_count = 0;
                }
                break;
            } else if ((DURATION_DIFF(instance->common.te_last, instance->common.te_shot) < instance->common.te_delta)
                    && (DURATION_DIFF(data.duration, instance->common.te_long) < instance->common.te_delta)) {
                if (instance->common.code_count_bit < instance->common.code_min_count_bit_for_found) {
                    subghz_protocol_common_add_bit(&instance->common, 1);
                }
                instance->common.parser_step = 2;
            } else if ((DURATION_DIFF(instance->common.te_last, instance->common.te_long) < instance->common.te_delta)
                    && (DURATION_DIFF(data.duration, instance->common.te_shot) < instance->common.te_delta)) {
                if (instance->common.code_count_bit < instance->common.code_min_count_bit_for_found) {
                    subghz_protocol_common_add_bit(&instance->common, 0);
                }
                instance->common.parser_step = 2;
            } else {
                instance->common.parser_step = 0;
                instance->common.header_count = 0;
            }
        } else {
            instance->common.parser_step = 0;
            instance->common.header_count = 0;
        }
        break;
    }
}
