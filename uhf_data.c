#include <furi.h>
#include "uhf_data.h"

UHFData* uhf_data_alloc() {
    UHFData* uhf_data = (UHFData*)malloc(sizeof(UHFData));
    uhf_data->length = 0;
    uhf_data->start = false;
    uhf_data->end = false;
    uhf_data->next = NULL;
    return uhf_data;
}

int uhf_data_append(UHFData* uhf_data, uint8_t data) {
    if(data == 0xBB) {
        uhf_data->start = true;
    }
    if(!uhf_data->start) return 0;
    if(uhf_data->end) return 0;
    if(uhf_data->length >= MAX_DATA_SIZE) return 0;
    if(data == 0x7E) {
        uhf_data->end = true;
    }
    uhf_data->data[uhf_data->length++] = data;
    return 1;
}

void uhf_data_reset(UHFData* uhf_data) {
    for(size_t i = 0; i < uhf_data->length; i++) {
        uhf_data->data[i] = 0x00;
    }
    uhf_data->start = false;
    uhf_data->end = false;
    uhf_data->length = 0;
    uhf_data->next = NULL;
}

void uhf_data_free(UHFData* uhf_data) {
    if(uhf_data == NULL) return;
    while(uhf_data != NULL) {
        UHFData* next = uhf_data->next;
        free(uhf_data);
        uhf_data = next;
    }
}

UHFResponseData* uhf_response_data_alloc() {
    UHFResponseData* uhf_response_data = (UHFResponseData*)malloc(sizeof(UHFResponseData));
    uhf_response_data->head = uhf_data_alloc();
    uhf_response_data->tail = uhf_response_data->head;
    uhf_response_data->size = 1;
    return uhf_response_data;
}

UHFData* uhf_response_data_add_new_uhf_data(UHFResponseData* uhf_response_data) {
    UHFData* temp = uhf_response_data->head;
    while(temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = uhf_data_alloc();
    uhf_response_data->size++;
    uhf_response_data->tail = temp->next;
    return temp->next;
}

UHFData* uhf_response_data_get_uhf_data(UHFResponseData* uhf_response_data, uint index) {
    if(uhf_response_data == NULL || uhf_response_data->size <= index) return NULL;
    UHFData* uhf_data = uhf_response_data->head;
    if(index == 0) return uhf_data;
    while(uhf_data != NULL && index >= 1) {
        uhf_data = uhf_data->next;
        index--;
    }
    return uhf_data;
}

void uhf_response_data_reset(UHFResponseData* uhf_response_data) {
    uhf_data_reset(uhf_response_data->head);
    if(uhf_response_data->size == 1) {
        return;
    }
    uhf_data_free(uhf_response_data->head->next);
    uhf_response_data->size = 1;
}

void uhf_response_data_free(UHFResponseData* uhf_response_data) {
    uhf_data_free(uhf_response_data->head);
    free(uhf_response_data);
}