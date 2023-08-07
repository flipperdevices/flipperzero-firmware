#include <furi.h>
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

UHFTag* uhf_tag_alloc() {
    UHFTag* uhf_tag = (UHFTag*)malloc(sizeof(UHFTag));
    return uhf_tag;
}

void uhf_tag_set_epc(UHFTag* uhf_tag, uint8_t* data) {
    // ED 1A 34 00 22 11 22 11 22 11 22 11 22 11 22 11
    // memcpy((void*)&select_cmd->data, (void*)&CMD_SET_SELECT_PARAMETER.cmd[0], select_cmd->length);
    memcpy(&uhf_tag->CRC, &data, 2);
    data += 2;
    memcpy(&uhf_tag->PC, data, 2);
    uint16_t epc_length = uhf_tag->PC[0];
    epc_length <<= 1;
    epc_length += uhf_tag->PC[1] & 0x08;
    epc_length *= 2;
    uhf_tag->epc_length = epc_length;
    memcpy(&uhf_tag->EPC, &data, (size_t)epc_length);
}

void uhf_tag_set_tid(UHFTag* uhf_tag, uint8_t* data) {
    memcpy(&uhf_tag->CRC, &data, 2);
    data += 2;
    memcpy(&uhf_tag->PC, data, 2);
    uint16_t epc_length = uhf_tag->PC[0];
    epc_length <<= 1;
    epc_length += uhf_tag->PC[1] & 0x08;
    epc_length *= 2;
    uhf_tag->epc_length = epc_length;
    memcpy(&uhf_tag->EPC, &data, (size_t)epc_length);
}

void uhf_tag_free(UHFTag* uhf_tag) {
    free(uhf_tag);
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