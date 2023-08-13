#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_DATA_SIZE 128
#define MAX_BANK_SIZE 64
typedef unsigned int uint;

typedef struct UHFData {
    uint8_t data[MAX_DATA_SIZE];
    size_t word_length;
    size_t length;
    bool start;
    bool end;
    struct UHFData* next;
} UHFData;

typedef struct UHFResponseData {
    UHFData* head;
    UHFData* tail;
    size_t size;
} UHFResponseData;

typedef struct UHFTag {
    // RESERVED BANK (RFU) (00)
    uint8_t kill_pwd[2]; // 0x00-0x10
    uint8_t access_pwd[2]; // 0x10-0x20
    // EPC Bank
    uint8_t crc[2]; // 0x00-0x10
    uint8_t pc[2]; // 0x10-0x20
    uint8_t epc[MAX_BANK_SIZE]; // 0x20-0x210
    size_t epc_length;
    uint8_t xpc[2]; // 0x210-0x21F
    size_t xpc_length;
    // TID Bank
    uint8_t tid[MAX_BANK_SIZE]; // 0x00-END
    size_t tid_length;
    // USER Bank
    uint8_t user[MAX_BANK_SIZE]; // 0x00-END
    size_t user_length;
} UHFTag;

UHFData* uhf_data_alloc();
int uhf_data_append(UHFData* uhf_data, uint8_t data);
void uhf_data_reset(UHFData* uhf_data);
uint8_t uhf_data_calculate_checksum(UHFData* uhf_data);
bool uhf_data_verfiy_checksum(UHFData* uhf_data);
void uhf_data_free(UHFData* uhf_data);

UHFResponseData* uhf_response_data_alloc();
UHFData* uhf_response_data_add_new_uhf_data(UHFResponseData* uhf_response_data);
UHFData* uhf_response_data_get_uhf_data(UHFResponseData* uhf_response_data, uint index);
void uhf_response_data_reset(UHFResponseData* uhf_response_data);
void uhf_response_data_free(UHFResponseData* uhf_response_data);

UHFTag* uhf_tag_alloc();
void uhf_tag_reset(UHFTag* uhf_tag);
void uhf_tag_set_epc(UHFTag* uhf_tag, uint8_t* data, size_t length);
void uhf_tag_set_tid(UHFTag* uhf_tag, uint8_t* data, size_t length);
void uhf_tag_set_user(UHFTag* uhf_tag, uint8_t* data, size_t length);
void uhf_tag_free(UHFTag* uhf_tag);