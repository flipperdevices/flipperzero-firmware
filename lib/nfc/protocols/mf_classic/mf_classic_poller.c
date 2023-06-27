#include "mf_classic_poller_i.h"

#include <nfc/protocols/nfc_poller_base.h>

#include <furi.h>

#define TAG "MfClassicPoller"

#define MF_CLASSIC_MAX_BUFF_SIZE (64)

typedef NfcCommand (*MfClassicPollerReadHandler)(MfClassicPoller* instance);

MfClassicPoller* mf_classic_poller_alloc(Iso14443_3aPoller* iso14443_3a_poller) {
    furi_assert(iso14443_3a_poller);

    MfClassicPoller* instance = malloc(sizeof(MfClassicPoller));
    instance->iso14443_3a_poller = iso14443_3a_poller;
    instance->data = mf_classic_alloc();
    instance->crypto = crypto1_alloc();
    instance->tx_plain_buffer = bit_buffer_alloc(MF_CLASSIC_MAX_BUFF_SIZE);
    instance->tx_encrypted_buffer = bit_buffer_alloc(MF_CLASSIC_MAX_BUFF_SIZE);
    instance->rx_plain_buffer = bit_buffer_alloc(MF_CLASSIC_MAX_BUFF_SIZE);
    instance->rx_encrypted_buffer = bit_buffer_alloc(MF_CLASSIC_MAX_BUFF_SIZE);

    instance->mfc_event.data = &instance->mfc_event_data;

    instance->general_event.protocol_type = NfcProtocolTypeMfClassic;
    instance->general_event.data = &instance->mfc_event;
    instance->general_event.poller = instance;

    return instance;
}

void mf_classic_poller_free(MfClassicPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->crypto);
    furi_assert(instance->tx_plain_buffer);
    furi_assert(instance->rx_plain_buffer);
    furi_assert(instance->tx_encrypted_buffer);
    furi_assert(instance->rx_encrypted_buffer);

    mf_classic_free(instance->data);
    crypto1_free(instance->crypto);
    bit_buffer_free(instance->tx_plain_buffer);
    bit_buffer_free(instance->rx_plain_buffer);
    bit_buffer_free(instance->tx_encrypted_buffer);
    bit_buffer_free(instance->rx_encrypted_buffer);

    free(instance);
}

NfcCommand mf_classic_poller_handler_idle(MfClassicPoller* instance) {
    iso14443_3a_copy(
        instance->data->iso14443_3a_data,
        iso14443_3a_poller_get_data(instance->iso14443_3a_poller));
    NfcCommand command = NfcCommandContinue;

    if(mf_classic_detect_protocol(instance->data->iso14443_3a_data, &instance->data->type)) {
        if(instance->card_state == MfClassicCardStateNotDetected) {
            instance->card_state = MfClassicCardStateDetected;
            instance->mfc_event.type = MfClassicPollerEventTypeCardDetected;
            command = instance->callback(instance->general_event, instance->context);
        }
        instance->state = instance->prev_state;
    }

    return command;
}

NfcCommand mf_classic_poller_handler_start(MfClassicPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->sectors_read = 0;
    instance->sectors_total = mf_classic_get_total_sectors_num(instance->data->type);

    instance->mfc_event_data.start_data.type = instance->data->type;
    command = instance->callback(instance->general_event, instance->context);

    instance->prev_state = MfClassicPollerStateStart;
    instance->state = MfClassicPollerStateNewSector;

    return command;
}

NfcCommand mf_classic_poller_handler_new_sector(MfClassicPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    if(instance->read_mode == MfClassicReadModeKeyReuse) {
        instance->key_reuse_sector++;
        if(instance->key_reuse_sector == instance->sectors_total) {
            instance->read_mode = MfClassicReadModeDictAttack;
            instance->mfc_event.type = MfClassicPollerEventTypeKeyAttackStop;
            command = instance->callback(instance->general_event, instance->context);
        } else if(mf_classic_is_sector_read(instance->data, instance->sectors_read)) {
            instance->mfc_event.type = MfClassicPollerEventTypeKeyAttackNextSector;
            command = instance->callback(instance->general_event, instance->context);
        } else {
            instance->state = MfClassicPollerStateAuthKeyA;
            instance->mfc_event.type = MfClassicPollerEventTypeKeyAttackNextSector;
            command = instance->callback(instance->general_event, instance->context);
        }
    } else {
        if(instance->sectors_read == instance->sectors_total) {
            instance->state = MfClassicPollerStateReadComplete;
        } else if(mf_classic_is_sector_read(instance->data, instance->sectors_read)) {
            instance->sectors_read++;
            instance->mfc_event.type = MfClassicPollerEventTypeNewSector;
            command = instance->callback(instance->general_event, instance->context);
        } else {
            instance->state = MfClassicPollerStateRequestKey;
        }
    }
    instance->prev_state = instance->state;

    return command;
}

NfcCommand mf_classic_poller_handler_request_key(MfClassicPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->mfc_event_data.key_request_data.sector_num = instance->sectors_read;
    command = instance->callback(instance->general_event, instance->context);
    if(instance->mfc_event_data.key_request_data.key_provided) {
        instance->current_key = instance->mfc_event_data.key_request_data.key;
        instance->state = MfClassicPollerStateAuthKeyA;
    } else {
        instance->state = MfClassicPollerStateNewSector;
    }

    return command;
}

NfcCommand mf_classic_poller_handler_auth_a(MfClassicPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    uint8_t sector = 0;
    if(instance->read_mode == MfClassicReadModeKeyReuse) {
        sector = instance->key_reuse_sector;
    } else {
        sector = instance->sectors_read;
    }

    if(mf_classic_is_key_found(instance->data, sector, MfClassicKeyTypeA)) {
        instance->prev_state = instance->state;
        instance->state = MfClassicPollerStateAuthKeyB;
    } else {
        uint8_t block = mf_classic_get_first_block_num_of_sector(sector);
        uint64_t key = nfc_util_bytes2num(instance->current_key.data, 6);
        FURI_LOG_D(TAG, "Auth to block %d with key A: %06llx", block, key);

        MfClassicError error = mf_classic_async_auth(
            instance, block, &instance->current_key, MfClassicKeyTypeA, NULL);
        if(error == MfClassicErrorNone) {
            FURI_LOG_I(TAG, "Key A found");
            mf_classic_set_key_found(instance->data, sector, MfClassicKeyTypeA, key);
            if(instance->read_mode != MfClassicReadModeKeyReuse) {
                if(sector < instance->sectors_total - 1) {
                    instance->read_mode = MfClassicReadModeKeyReuse;
                    instance->key_reuse_sector = sector;
                    instance->mfc_event.type = MfClassicPollerEventTypeKeyAttackStart;
                    instance->mfc_event_data.key_attack_data.start_sector =
                        instance->key_reuse_sector;
                    command = instance->callback(instance->general_event, instance->context);
                }
            }
            instance->mfc_event.type = MfClassicPollerEventTypeFoundKeyA;
            command = instance->callback(instance->general_event, instance->context);
            instance->prev_state = instance->state;
            instance->state = MfClassicPollerStateReadSector;
        } else {
            if(instance->read_mode == MfClassicReadModeKeyReuse) {
                instance->state = MfClassicPollerStateAuthKeyB;
            } else {
                instance->state = MfClassicPollerStateRequestKey;
            }
        }
    }

    return command;
}

NfcCommand mf_classic_poller_handler_auth_b(MfClassicPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    uint8_t sector = 0;
    if(instance->read_mode == MfClassicReadModeKeyReuse) {
        sector = instance->key_reuse_sector;
    } else {
        sector = instance->sectors_read;
    }

    if(mf_classic_is_key_found(instance->data, sector, MfClassicKeyTypeB)) {
        instance->state = MfClassicPollerStateNewSector;
    } else {
        uint8_t block = mf_classic_get_first_block_num_of_sector(sector);
        uint64_t key = nfc_util_bytes2num(instance->current_key.data, 6);
        FURI_LOG_D(TAG, "Auth to block %d with key B: %06llx", block, key);

        MfClassicError error = mf_classic_async_auth(
            instance, block, &instance->current_key, MfClassicKeyTypeB, NULL);
        if(error == MfClassicErrorNone) {
            FURI_LOG_D(TAG, "Key B found");
            instance->mfc_event.type = MfClassicPollerEventTypeFoundKeyB;
            mf_classic_set_key_found(instance->data, sector, MfClassicKeyTypeB, key);
            if(instance->read_mode != MfClassicReadModeKeyReuse) {
                if(sector < instance->sectors_total - 1) {
                    instance->read_mode = MfClassicReadModeKeyReuse;
                    instance->key_reuse_sector = sector;

                    instance->mfc_event.type = MfClassicPollerEventTypeKeyAttackStart;
                    instance->mfc_event_data.key_attack_data.start_sector =
                        instance->key_reuse_sector;
                    command = instance->callback(instance->general_event, instance->context);
                }
            }
            command = instance->callback(instance->general_event, instance->context);
            instance->state = MfClassicPollerStateReadSector;
        } else {
            if(instance->read_mode == MfClassicReadModeKeyReuse) {
                instance->state = MfClassicPollerStateNewSector;
            } else {
                instance->state = MfClassicPollerStateRequestKey;
            }
        }
    }
    instance->prev_state = instance->state;

    return command;
}

NfcCommand mf_classic_poller_handler_read_sector(MfClassicPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    uint8_t block_start = mf_classic_get_first_block_num_of_sector(instance->sectors_read);
    uint8_t total_blocks = mf_classic_get_blocks_num_in_sector(instance->sectors_read);
    MfClassicBlock block = {};

    for(size_t i = block_start; i < block_start + total_blocks; i++) {
        FURI_LOG_D(TAG, "Reding block %d", i);
        if(mf_classic_is_block_read(instance->data, i)) continue;
        MfClassicError error = mf_classic_async_read_block(instance, i, &block);
        if(error == MfClassicErrorNone) {
            mf_classic_set_block_read(instance->data, i, &block);
        } else {
            FURI_LOG_D(TAG, "Failed to read %d block", i);
            break;
        }
    }

    instance->prev_state = instance->state;
    if(instance->prev_state == MfClassicPollerStateAuthKeyA) {
        instance->state = MfClassicPollerStateAuthKeyB;
    } else {
        instance->state = MfClassicPollerStateNewSector;
    }

    return command;
}

NfcCommand mf_classic_poller_handler_read_complete(MfClassicPoller* instance) {
    NfcCommand command = NfcCommandContinue;
    instance->mfc_event.type = MfClassicPollerEventTypeReadComplete;
    command = instance->callback(instance->general_event, instance->context);

    return command;
}

static const MfClassicPollerReadHandler
    mf_classic_poller_dict_attack_handler[MfClassicPollerStateNum] = {
        [MfClassicPollerStateIdle] = mf_classic_poller_handler_idle,
        [MfClassicPollerStateStart] = mf_classic_poller_handler_start,
        [MfClassicPollerStateNewSector] = mf_classic_poller_handler_new_sector,
        [MfClassicPollerStateRequestKey] = mf_classic_poller_handler_request_key,
        [MfClassicPollerStateAuthKeyA] = mf_classic_poller_handler_auth_a,
        [MfClassicPollerStateAuthKeyB] = mf_classic_poller_handler_auth_b,
        [MfClassicPollerStateReadSector] = mf_classic_poller_handler_read_sector,
        [MfClassicPollerStateReadComplete] = mf_classic_poller_handler_read_complete,
};

NfcCommand mf_classsic_poller_run(NfcPollerEvent event, void* context) {
    furi_assert(event.data);
    furi_assert(event.protocol_type == NfcProtocolTypeIso14443_3a);
    furi_assert(context);

    MfClassicPoller* instance = context;
    Iso14443_3aPollerEvent* iso14443_3a_event = event.data;
    NfcCommand command = NfcCommandContinue;

    if(iso14443_3a_event->type == Iso14443_3aPollerEventTypeReady) {
        command = mf_classic_poller_dict_attack_handler[instance->state](instance);
    } else if(iso14443_3a_event->type == Iso14443_3aPollerEventTypeError) {
        if(iso14443_3a_event->data->error == Iso14443_3aErrorNotPresent) {
            if(instance->card_state == MfClassicCardStateDetected) {
                instance->card_state = MfClassicCardStateNotDetected;
                instance->mfc_event.type = MfClassicPollerEventTypeCardNotDetected;
                command = instance->callback(instance->general_event, instance->context);
                instance->state = MfClassicPollerStateIdle;
            }
        }
    }

    return command;
}

bool mf_classsic_poller_detect(NfcPollerEvent event, void* context) {
    furi_assert(event.data);
    furi_assert(event.protocol_type == NfcProtocolTypeIso14443_3a);
    furi_assert(context);

    return false;
}

void mf_classic_poller_set_callback(
    MfClassicPoller* instance,
    NfcPollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

const MfClassicData* mf_classic_poller_get_data(const MfClassicPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    return instance->data;
}

const NfcPollerBase mf_classic_poller = {
    .alloc = (NfcPollerAlloc)mf_classic_poller_alloc,
    .free = (NfcPollerFree)mf_classic_poller_free,
    .set_callback = (NfcPollerSetCallback)mf_classic_poller_set_callback,
    .run = (NfcPollerRun)mf_classsic_poller_run,
    .detect = (NfcPollerDetect)mf_classsic_poller_detect,
    .get_data = (NfcPollerGetData)mf_classic_poller_get_data,
};
