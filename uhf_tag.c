#include "uhf_tag.h"

UHFTag* uhf_tag_alloc() {
    UHFTag* uhf_tag = (UHFTag*)malloc(sizeof(UHFTag));
    uhf_tag->reserved = (ReservedMemoryBank*)malloc(sizeof(ReservedMemoryBank));
    uhf_tag->epc = (EPCMemoryBank*)malloc(sizeof(EPCMemoryBank));
    uhf_tag->tid = (TIDMemoryBank*)malloc(sizeof(TIDMemoryBank));
    uhf_tag->user = (UserMemoryBank*)malloc(sizeof(UserMemoryBank));
    return uhf_tag;
}

void uhf_tag_free(UHFTag* uhf_tag) {
    free(uhf_tag->reserved);
    free(uhf_tag->epc);
    free(uhf_tag->tid);
    free(uhf_tag->user);
    free(uhf_tag);
}

UHFTagWrapper* uhf_tag_wrapper_alloc(){
    UHFTagWrapper* uhf_tag_wrapper = (UHFTagWrapper*)malloc(sizeof(UHFTagWrapper));
    uhf_tag_wrapper->uhf_tag = NULL;
    return uhf_tag_wrapper;
}

void uhf_tag_wrapper_set_tag(UHFTagWrapper* uhf_tag_wrapper, UHFTag* uhf_tag){
    uhf_tag_wrapper->uhf_tag = uhf_tag;
}

void uhf_tag_wrapper_free(UHFTagWrapper* uhf_tag_wrapper){
    if(uhf_tag_wrapper->uhf_tag != NULL)
        uhf_tag_free(uhf_tag_wrapper->uhf_tag);
    free(uhf_tag_wrapper);
}