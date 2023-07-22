#pragma once
#include <stdint.h>
#include <stdlib.h>

typedef struct UHFData {
    uint8_t data[32];
    size_t length;
    struct UHFData* next;
} UHFData;

typedef struct UHFResponseData {
    UHFData* data;
    size_t size;

} UHFResponseData;

UHFData* uhf_data_alloc();
int uhf_data_append(UHFData* uhf_data, uint8_t data);
void uhf_data_free(UHFData* uhf_data);

UHFResponseData* uhf_response_data_alloc();
UHFData* add_uhf_data_to_uhf_response_data(UHFResponseData* uhf_response_data);
void uhf_response_data_free(UHFResponseData* uhf_response_data);