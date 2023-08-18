#include "uhf_tag.h"
#include <stdlib.h>
#include <string.h>

UHFTagWrapper* uhf_tag_wrapper_alloc() {
    UHFTagWrapper* uhf_tag_wrapper = (UHFTagWrapper*)malloc(sizeof(UHFTagWrapper));
    uhf_tag_wrapper->uhf_tag = NULL;
    return uhf_tag_wrapper;
}

void uhf_tag_wrapper_set_tag(UHFTagWrapper* uhf_tag_wrapper, UHFTag* uhf_tag) {
    uhf_tag_wrapper->uhf_tag = uhf_tag;
}

void uhf_tag_wrapper_free(UHFTagWrapper* uhf_tag_wrapper) {
    uhf_tag_free(uhf_tag_wrapper->uhf_tag);
    free(uhf_tag_wrapper);
}

UHFTag* uhf_tag_alloc() {
    UHFTag* uhf_tag = (UHFTag*)malloc(sizeof(UHFTag));
    uhf_tag->reserved = (ReservedMemoryBank*)malloc(sizeof(ReservedMemoryBank));
    uhf_tag->epc = (EPCMemoryBank*)malloc(sizeof(EPCMemoryBank));
    uhf_tag->tid = (TIDMemoryBank*)malloc(sizeof(TIDMemoryBank));
    uhf_tag->user = (UserMemoryBank*)malloc(sizeof(UserMemoryBank));
    return uhf_tag;
}

void uhf_tag_reset(UHFTag* uhf_tag) {
    uhf_tag->epc->crc = 0;
    uhf_tag->epc->pc = 0;
    uhf_tag->epc->size = 0;
    uhf_tag->tid->size = 0;
    uhf_tag->user->size = 0;
}

void uhf_tag_free(UHFTag* uhf_tag) {
    if(uhf_tag == NULL) return;
    free(uhf_tag->reserved);
    free(uhf_tag->epc);
    free(uhf_tag->tid);
    free(uhf_tag->user);
    free(uhf_tag);
}

void uhf_tag_set_epc_pc(UHFTag* uhf_tag, uint16_t pc) {
    uhf_tag->epc->pc = pc;
}

void uhf_tag_set_epc_crc(UHFTag* uhf_tag, uint16_t crc) {
    uhf_tag->epc->crc = crc;
}

void uhf_tag_set_epc(UHFTag* uhf_tag, uint8_t* data_in, size_t size) {
    memcpy(uhf_tag->epc->data, data_in, size);
    uhf_tag->epc->size = size;
}