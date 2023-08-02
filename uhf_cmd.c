#include "uhf_cmd.h"
// #include "uhf_app_i.h"

// char* uhf_get_dev_info(UHFApp* uhf_app) {
// }

// uint8_t* uhf_single_poll(UHFApp* uhf_app){

// }
uint8_t calculate_checksum(uint8_t* arr, size_t length) {
    // CheckSum8 Modulo 256
    // Sum of Bytes % 256
    uint8_t sum_val = 0x00;
    for(size_t i = 0; i < length; i++) {
        sum_val += arr[i];
    }
    return sum_val % 256;
}