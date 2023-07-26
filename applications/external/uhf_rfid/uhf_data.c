#include <furi.h>
#include "uhf_data.h"
// #include "uhf_cmd.h"

UHFData* uhf_data_alloc() {
    UHFData* uhf_data = (UHFData*)malloc(sizeof(UHFData));
    uhf_data->length = 0;
    uhf_data->data_full = false;
    uhf_data->next = NULL;
    return uhf_data;
}

int uhf_data_append(UHFData* uhf_data, uint8_t data) {
    if(uhf_data->length >= MAX_DATA_SIZE || uhf_data->data_full) return 0;
    uhf_data->data[uhf_data->length++] = data;
    if(data == 0x7E) {
        uhf_data->data_full = true;
    }
    return 1;
}

void uhf_data_free(UHFData* uhf_data) {
    if(uhf_data != NULL) {
        while(uhf_data != NULL) {
            UHFData* next = uhf_data->next;
            free(uhf_data);
            uhf_data = next;
        }
    }
}

UHFResponseData* uhf_response_data_alloc() {
    UHFResponseData* uhf_response_data = (UHFResponseData*)malloc(sizeof(UHFResponseData));
    uhf_response_data->data = uhf_data_alloc();
    uhf_response_data->size = 0;
    return uhf_response_data;
}

UHFData* add_uhf_data_to_uhf_response_data(UHFResponseData* uhf_response_data) {
    UHFData* next = uhf_response_data->data;
    while(next->next != NULL) {
        next = next->next;
    }
    next->next = uhf_data_alloc();
    uhf_response_data->size++;
    return next->next;
}

void uhf_response_data_free(UHFResponseData* uhf_response_data) {
    uhf_data_free(uhf_response_data->data);
    free(uhf_response_data);
}