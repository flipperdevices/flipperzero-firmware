#include "subghz_protocol_common.h"
#include <stdio.h>

void subghz_protocol_common_add_bit(SubGhzProtocolCommon *pr, uint8_t bit){
    pr->code_found = pr->code_found <<1 | bit;
    pr->code_count_bit++;
}

uint8_t subghz_protocol_common_check_interval (SubGhzProtocolCommon *pr,uint32_t interval, uint16_t intervel_check){
    if ((intervel_check >= (interval - pr->te_delta))&&(intervel_check <= (interval + pr->te_delta))){
        return 1;
    } else {
        return 0;
    }
}

uint64_t subghz_protocol_common_reverse_key(uint64_t key, uint8_t count_bit){
    uint64_t key_reverse=0;
    for (uint8_t i=0;i<count_bit;i++){
        key_reverse=key_reverse<<1|bit_read(key,i);
        }
    return key_reverse;
}

void subghz_protocol_common_printf (SubGhzProtocolCommon *pr){
    //snprintf(BufTX, sizeof(BufTX),"Protocol %s: %d Bit | KEY:0x%llX HEX  \n\r", pr->Name_Protocol, pr->Count_BIT, pr->Code);
    uint32_t code_found_hi = pr->code_found>>32;
    uint32_t code_found_lo = pr->code_found&0x00000000ffffffff;

    uint64_t code_found_reverse= subghz_protocol_common_reverse_key(pr->code_found,pr->code_count_bit);

    uint32_t code_found_reverse_hi = code_found_reverse>>32;
    uint32_t code_found_reverse_lo = code_found_reverse&0x00000000ffffffff;

    if(code_found_hi>0){
        printf ("Found protocol %s   %d Bit  KEY:0x%lX%08lX  Reverse KEY:0x%lX%08lX \r\n ",
                    pr->name,
                    pr->code_count_bit,
                    code_found_hi,
                    code_found_lo,
                    code_found_reverse_hi,
                    code_found_reverse_lo
                );
    } else{
        printf ("Found protocol %s   %d Bit  KEY:0x%lX%lX  Reverse KEY:0x%lX%lX \r\n ",
                pr->name,
                pr->code_count_bit,
                code_found_hi,
                code_found_lo,
                code_found_reverse_hi,
                code_found_reverse_lo
            );
    }
    if(pr->name[0] =='K'){
        printf ("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tFIX: %lX  \r\n"
                "\tremote controller:%s\t\t\t\t\t\t\t\tHOP: %lX  \r\n"
                "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tCNT: %04X  BTN: %02lX\r\n",
                code_found_reverse_hi,
                pr->name_remote_controller,
                code_found_reverse_lo,
                pr->cnt, //need manufacture code
                code_found_reverse_hi>>28);
    }
}
