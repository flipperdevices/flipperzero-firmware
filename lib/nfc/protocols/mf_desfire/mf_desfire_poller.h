#pragma once

#include "mf_desfire.h"

#include <lib/nfc/protocols/iso14443_4a/iso14443_4a_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MfDesfirePoller MfDesfirePoller;

typedef enum {
    MfDesfirePollerEventTypeReadSuccess,
    MfDesfirePollerEventTypeReadFailed,
} MfDesfirePollerEventType;

typedef struct {
    union {
        MfDesfireError error;
    };
} MfDesfirePollerEventData;

typedef struct {
    MfDesfirePollerEventType type;
    MfDesfirePollerEventData* data;
} MfDesfirePollerEvent;

MfDesfireError mf_desfire_send_chunks(
    MfDesfirePoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer);

MfDesfireError mf_desfire_poller_read_version(MfDesfirePoller* instance, MfDesfireVersion* data);

MfDesfireError
    mf_desfire_poller_read_free_memory(MfDesfirePoller* instance, MfDesfireFreeMemory* data);

MfDesfireError
    mf_desfire_poller_read_key_settings(MfDesfirePoller* instance, MfDesfireKeySettings* data);

MfDesfireError mf_desfire_poller_read_key_versions(
    MfDesfirePoller* instance,
    SimpleArray* data,
    uint32_t count);

MfDesfireError
    mf_desfire_poller_read_application_ids(MfDesfirePoller* instance, SimpleArray* data);

MfDesfireError mf_desfire_poller_select_application(
    MfDesfirePoller* instance,
    const MfDesfireApplicationId* id);

MfDesfireError mf_desfire_poller_read_file_ids(MfDesfirePoller* instance, SimpleArray* data);

MfDesfireError mf_desfire_poller_read_file_settings(
    MfDesfirePoller* instance,
    MfDesfireFileId id,
    MfDesfireFileSettings* data);

MfDesfireError mf_desfire_poller_read_file_settings_multi(
    MfDesfirePoller* instance,
    const SimpleArray* file_ids,
    SimpleArray* data);

MfDesfireError mf_desfire_poller_read_file_data(
    MfDesfirePoller* instance,
    MfDesfireFileId id,
    uint32_t offset,
    size_t size,
    MfDesfireFileData* data);

MfDesfireError mf_desfire_poller_read_file_value(
    MfDesfirePoller* instance,
    MfDesfireFileId id,
    MfDesfireFileData* data);

MfDesfireError mf_desfire_poller_read_file_records(
    MfDesfirePoller* instance,
    MfDesfireFileId id,
    uint32_t offset,
    size_t size,
    MfDesfireFileData* data);

MfDesfireError mf_desfire_poller_read_file_data_multi(
    MfDesfirePoller* instance,
    const SimpleArray* file_ids,
    const SimpleArray* file_settings,
    SimpleArray* data);

MfDesfireError
    mf_desfire_poller_read_application(MfDesfirePoller* instance, MfDesfireApplication* data);

MfDesfireError mf_desfire_poller_read_applications(
    MfDesfirePoller* instance,
    const SimpleArray* app_ids,
    SimpleArray* data);

#ifdef __cplusplus
}
#endif
