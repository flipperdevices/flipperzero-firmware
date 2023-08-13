#include "uhf_data.h"

UHFData* uhf_data_alloc() {
    UHFData* uhf_data = (UHFData*)malloc(sizeof(UHFData));
    uhf_data->word_length = 0;
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

uint8_t uhf_data_calculate_checksum(UHFData* uhf_data) {
    // CheckSum8 Modulo 256
    // Sum of Bytes % 256
    uint8_t sum_val = 0x00;
    size_t length = uhf_data->length - 2;
    for(size_t i = 1; i < length; i++) {
        sum_val += uhf_data->data[i];
    }
    return sum_val % 256;
}

bool uhf_data_verfiy_checksum(UHFData* uhf_data) {
    uint8_t data_checksum = uhf_data->data[uhf_data->length - 2];
    uint8_t actual_checksum = uhf_data_calculate_checksum(uhf_data);
    return data_checksum == actual_checksum;
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

UHFTag* uhf_tag_alloc() {
    UHFTag* uhf_tag = (UHFTag*)malloc(sizeof(UHFTag));
    return uhf_tag;
}

void uhf_tag_set_epc(UHFTag* uhf_tag, uint8_t* data, size_t length) {
    memcpy(uhf_tag->crc, data, 2);
    data += 2;
    memcpy(uhf_tag->pc, data, 2);
    memcpy(uhf_tag->epc, data, length);
    uhf_tag->epc_length = length;
}

void uhf_tag_reset(UHFTag* uhf_tag) {
    for(int i = 0; i < 2; i++) {
        uhf_tag->crc[i] = 0;
        uhf_tag->pc[i] = 0;
        uhf_tag->xpc[i] = 0;
    }
    for(int i = 0; i < MAX_BANK_SIZE; i++) {
        uhf_tag->epc[i] = 0;
        uhf_tag->tid[i] = 0;
        uhf_tag->user[i] = 0;
    }
    uhf_tag->epc_length = 0;
    uhf_tag->tid_length = 0;
    uhf_tag->user_length = 0;
}

void uhf_tag_set_tid(UHFTag* uhf_tag, uint8_t* data, size_t length) {
    memcpy(uhf_tag->tid, data, length);
    uhf_tag->tid_length = length;
}

void uhf_tag_set_user(UHFTag* uhf_tag, uint8_t* data, size_t length) {
    memcpy(uhf_tag->user, data, length);
    uhf_tag->user_length = length;
}

void uhf_tag_free(UHFTag* uhf_tag) {
    free(uhf_tag);
}