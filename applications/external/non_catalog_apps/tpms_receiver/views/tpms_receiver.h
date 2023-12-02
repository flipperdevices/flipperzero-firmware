#pragma once

#include <gui/view.h>
#include "../helpers/tpms_types.h"
#include "../helpers/tpms_event.h"

typedef struct TPMSReceiver TPMSReceiver;

typedef void (*TPMSReceiverCallback)(TPMSCustomEvent event, void* context);

void tpms_view_receiver_set_rssi(TPMSReceiver* instance, float rssi);

void tpms_view_receiver_set_lock(TPMSReceiver* tpms_receiver, TPMSLock keyboard);

void tpms_view_receiver_set_callback(
    TPMSReceiver* tpms_receiver,
    TPMSReceiverCallback callback,
    void* context);

TPMSReceiver* tpms_view_receiver_alloc();

void tpms_view_receiver_free(TPMSReceiver* tpms_receiver);

View* tpms_view_receiver_get_view(TPMSReceiver* tpms_receiver);

void tpms_view_receiver_add_data_statusbar(
    TPMSReceiver* tpms_receiver,
    const char* frequency_str,
    const char* preset_str,
    const char* history_stat_str,
    bool external);

void tpms_view_receiver_add_item_to_menu(
    TPMSReceiver* tpms_receiver,
    const char* name,
    uint8_t type);

uint16_t tpms_view_receiver_get_idx_menu(TPMSReceiver* tpms_receiver);

void tpms_view_receiver_set_idx_menu(TPMSReceiver* tpms_receiver, uint16_t idx);

void tpms_view_receiver_exit(void* context);
