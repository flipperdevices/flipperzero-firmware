#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <mbedtls/des.h>
#include "uhf_tag.h"

#define UHF_DEV_NAME_MAX_LEN 22
#define UHF_EPC_BANK_LENGTH_LABEL "EPC_LENGTH"
#define UHF_TID_BANK_LENGTH_LABEL "TID_LENGTH"
#define UHF_USER_BANK_LENGTH_LABEL "USER_LENGTH"
#define UHF_EPC_PC_LABEL "PC"
#define UHF_EPC_CRC_LABEL "CRC"
#define UHF_RFU_BANK_LABEL "RFU"
#define UHF_EPC_BANK_LABEL "EPC"
#define UHF_TID_BANK_LABEL "TID"
#define UHF_USER_BANK_LABEL "USER"

#define UHF_APP_EXTENSION ".uhf"

typedef void (*UHFLoadingCallback)(void* context, bool state);

typedef struct {
    Storage* storage;
    DialogsApp* dialogs;
    UHFTagWrapper* uhf_tag_wrapper;
    char dev_name[UHF_DEV_NAME_MAX_LEN + 1];
    FuriString* load_path;
    UHFLoadingCallback loading_cb;
    void* loading_cb_ctx;
} UHFDevice;

UHFDevice* uhf_device_alloc();

void uhf_device_free(UHFDevice* uhf_dev);

void uhf_device_set_name(UHFDevice* dev, const char* name);

bool uhf_device_save(UHFDevice* dev, const char* dev_name);

bool uhf_file_select(UHFDevice* dev);

// void uhf_device_data_clear(PicopassDeviceData* dev_data);

void uhf_device_clear(UHFDevice* dev);

bool uhf_device_delete(UHFDevice* dev, bool use_load_path);

void uhf_device_set_loading_callback(UHFDevice* dev, UHFLoadingCallback callback, void* context);

// ReturnCode uhf_device_parse_credential(PicopassBlock* AA1, PicopassPacs* pacs);
// ReturnCode uhf_device_parse_wiegand(uint8_t* data, PicopassWiegandRecord* record);
