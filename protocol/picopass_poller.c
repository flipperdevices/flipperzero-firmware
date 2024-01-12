#include "picopass_i.h"
#include "picopass_poller_i.h"

#include "../loclass/optimized_cipher.h"

#include <furi/furi.h>

#define TAG "Picopass"

typedef NfcCommand (*PicopassPollerStateHandler)(PicopassPoller* instance);

static void picopass_poller_reset(PicopassPoller* instance) {
    instance->current_block = 0;
}

static void picopass_poller_prepare_read(PicopassPoller* instance) {
    instance->app_limit = instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[0] <
                                  PICOPASS_MAX_APP_LIMIT ?
                              instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[0] :
                              PICOPASS_MAX_APP_LIMIT;
    instance->current_block = 2;
}

NfcCommand picopass_poller_request_mode_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->event.type = PicopassPollerEventTypeRequestMode;
    command = instance->callback(instance->event, instance->context);
    instance->mode = instance->event_data.req_mode.mode;
    instance->state = PicopassPollerStateDetect;

    return command;
}

NfcCommand picopass_poller_detect_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;
    PicopassError error = picopass_poller_actall(instance);

    if(error == PicopassErrorNone) {
        instance->state = PicopassPollerStateSelect;
        instance->event.type = PicopassPollerEventTypeCardDetected;
        command = instance->callback(instance->event, instance->context);
    } else {
        furi_delay_ms(100);
    }

    return command;
}

NfcCommand picopass_poller_select_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        PicopassError error = picopass_poller_identify(instance, &instance->col_res_serial_num);
        if(error != PicopassErrorNone) {
            instance->state = PicopassPollerStateFail;
            break;
        }

        error =
            picopass_poller_select(instance, &instance->col_res_serial_num, &instance->serial_num);
        if(error != PicopassErrorNone) {
            instance->state = PicopassPollerStateFail;
            break;
        }

        instance->state = PicopassPollerStatePreAuth;
    } while(false);

    return command;
}

NfcCommand picopass_poller_pre_auth_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;
    PicopassError error = PicopassErrorNone;

    do {
        memcpy(
            instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data,
            instance->serial_num.data,
            sizeof(PicopassSerialNum));
        FURI_LOG_D(
            TAG,
            "csn %02x%02x%02x%02x%02x%02x%02x%02x",
            instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data[0],
            instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data[1],
            instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data[2],
            instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data[3],
            instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data[4],
            instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data[5],
            instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data[6],
            instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data[7]);

        PicopassBlock block = {};
        error = picopass_poller_read_block(instance, PICOPASS_CONFIG_BLOCK_INDEX, &block);
        if(error != PicopassErrorNone) {
            instance->state = PicopassPollerStateFail;
            break;
        }
        memcpy(
            instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data,
            block.data,
            sizeof(PicopassBlock));
        FURI_LOG_D(
            TAG,
            "config %02x%02x%02x%02x%02x%02x%02x%02x",
            instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[0],
            instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[1],
            instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[2],
            instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[3],
            instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[4],
            instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[5],
            instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[6],
            instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[7]);

        error = picopass_poller_read_block(instance, PICOPASS_SECURE_EPURSE_BLOCK_INDEX, &block);
        if(error != PicopassErrorNone) {
            instance->state = PicopassPollerStateFail;
            break;
        }
        memcpy(
            instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data,
            block.data,
            sizeof(PicopassBlock));
        FURI_LOG_D(
            TAG,
            "epurse %02x%02x%02x%02x%02x%02x%02x%02x",
            instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data[0],
            instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data[1],
            instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data[2],
            instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data[3],
            instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data[4],
            instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data[5],
            instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data[6],
            instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data[7]);

        error = picopass_poller_read_block(instance, PICOPASS_SECURE_AIA_BLOCK_INDEX, &block);
        if(error != PicopassErrorNone) {
            instance->state = PicopassPollerStateFail;
            break;
        }
        memcpy(
            instance->data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data,
            block.data,
            sizeof(PicopassBlock));
        FURI_LOG_D(
            TAG,
            "aia %02x%02x%02x%02x%02x%02x%02x%02x",
            instance->data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[0],
            instance->data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[1],
            instance->data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[2],
            instance->data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[3],
            instance->data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[4],
            instance->data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[5],
            instance->data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[6],
            instance->data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[7]);

        instance->state = PicopassPollerStateCheckSecurity;
    } while(false);

    return command;
}

NfcCommand picopass_poller_check_security(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->secured = true;

    uint8_t crypt =
        (instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[7] & PICOPASS_FUSE_CRYPT10);
    switch(crypt) {
    case 0:
        FURI_LOG_D(TAG, "Secured page - Authentication disabled");
        // Well this is awkward... We can try anyway though I guess...
        break;
    case PICOPASS_FUSE_CRYPT0:
        FURI_LOG_D(TAG, "Non-secured page, skipping auth");
        instance->secured = false;
        picopass_poller_prepare_read(instance);
        instance->state = PicopassPollerStateReadBlock;
        return command;
    case PICOPASS_FUSE_CRYPT0 | PICOPASS_FUSE_CRYPT1:
        FURI_LOG_D(TAG, "Secured page - keys modifiable");
        break;
    case PICOPASS_FUSE_CRYPT1:
        FURI_LOG_D(TAG, "Secured page - keys locked");
    }

    // Thank you proxmark!
    PicopassBlock temp_block = {};
    memset(temp_block.data, 0xff, sizeof(PicopassBlock));
    instance->data->pacs.legacy =
        (memcmp(
             instance->data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data,
             temp_block.data,
             sizeof(PicopassBlock)) == 0);

    temp_block.data[3] = 0x00;
    temp_block.data[4] = 0x06;
    instance->data->pacs.se_enabled =
        (memcmp(
             instance->data->AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data,
             temp_block.data,
             sizeof(PicopassBlock)) == 0);

    if(instance->data->pacs.se_enabled) {
        FURI_LOG_D(TAG, "SE enabled");
    }

    if(instance->mode == PicopassPollerModeRead) {
        // Always try the NR-MAC auth in case we have the file.
        instance->state = PicopassPollerStateNrMacAuth;
    } else {
        // NR-MAC auth doesn't allow for writing, so don't try
        instance->state = PicopassPollerStateAuth;
    }
    return command;
}

NfcCommand picopass_poller_nr_mac_auth(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;
    Picopass* picopass = instance->context;
    PicopassDevice* dev = picopass->dev;

    uint8_t* csn = instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data;
    uint8_t* epurse = instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data;

    FuriString* temp_str = furi_string_alloc();
    FuriString* filename = furi_string_alloc();
    FlipperFormat* file = flipper_format_file_alloc(dev->storage);
    PicopassMac mac = {};

    for(size_t i = 0; i < PICOPASS_BLOCK_LEN; i++) {
        furi_string_cat_printf(filename, "%02x", csn[i]);
    }
    furi_string_cat_printf(filename, "_");
    for(size_t i = 0; i < PICOPASS_BLOCK_LEN; i++) {
        furi_string_cat_printf(filename, "%02x", epurse[i]);
    }

    furi_string_printf(
        temp_str, "%s/%s%s", STORAGE_APP_DATA_PATH_PREFIX, furi_string_get_cstr(filename), ".mac");

    FURI_LOG_D(TAG, "Looking for %s", furi_string_get_cstr(temp_str));
    uint8_t nr_mac[PICOPASS_BLOCK_LEN];

    // Set next state so breaking do/while will jump to it. If successful, do/while will set to ReadBlock
    if(instance->data->pacs.se_enabled) {
        instance->state = PicopassPollerStateAuthFail;
    } else {
        // For non-SE, run through normal key check
        instance->state = PicopassPollerStateAuth;
    }
    do {
        //check for file
        if(!flipper_format_file_open_existing(file, furi_string_get_cstr(temp_str))) break;
        // FURI_LOG_D(TAG, "Found %s", furi_string_get_cstr(temp_str));

        furi_string_printf(temp_str, "NR-MAC");
        if(!flipper_format_read_hex(
               file, furi_string_get_cstr(temp_str), nr_mac, PICOPASS_BLOCK_LEN))
            break;
        memcpy(mac.data, nr_mac + 4, PICOPASS_MAC_LEN);
        /*
        FURI_LOG_D(
            TAG,
            "Read nr-mac: %02x %02x %02x %02x %02x %02x %02x %02x",
            nr_mac[0],
            nr_mac[1],
            nr_mac[2],
            nr_mac[3],
            nr_mac[4],
            nr_mac[5],
            nr_mac[6],
            nr_mac[7]);
        FURI_LOG_D(
            TAG, "MAC: %02x %02x %02x %02x", mac.data[0], mac.data[1], mac.data[2], mac.data[3]);
        */

        uint8_t ccnr[12] = {};
        PicopassReadCheckResp read_check_resp = {};
        PicopassError error = picopass_poller_read_check(instance, &read_check_resp);
        if(error == PicopassErrorTimeout) {
            instance->event.type = PicopassPollerEventTypeCardLost;
            instance->callback(instance->event, instance->context);
            instance->state = PicopassPollerStateDetect;
            break;
        } else if(error != PicopassErrorNone) {
            FURI_LOG_E(TAG, "Read check failed: %d", error);
            break;
        }
        memcpy(ccnr, read_check_resp.data, sizeof(PicopassReadCheckResp)); // last 4 bytes left 0

        /*
        FURI_LOG_D(
            TAG,
            "CCNR: %02x %02x %02x %02x %02x %02x %02x %02x",
            ccnr[0],
            ccnr[1],
            ccnr[2],
            ccnr[3],
            ccnr[4],
            ccnr[5],
            ccnr[6],
            ccnr[7]);
            */

        //use mac
        PicopassCheckResp check_resp = {};
        error = picopass_poller_check(instance, nr_mac, &mac, &check_resp);
        if(error == PicopassErrorNone) {
            memcpy(instance->mac.data, mac.data, sizeof(PicopassMac));
            if(instance->mode == PicopassPollerModeRead) {
                picopass_poller_prepare_read(instance);
                instance->state = PicopassPollerStateReadBlock;
                // Set to non-zero keys to allow emulation
                memset(
                    instance->data->AA1[PICOPASS_SECURE_KD_BLOCK_INDEX].data,
                    0xff,
                    PICOPASS_BLOCK_LEN);
                memset(
                    instance->data->AA1[PICOPASS_SECURE_KC_BLOCK_INDEX].data,
                    0xff,
                    PICOPASS_BLOCK_LEN);
            }
        }

    } while(false);
    furi_string_free(temp_str);
    furi_string_free(filename);
    flipper_format_free(file);

    return command;
}

NfcCommand picopass_poller_auth_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        // Request key
        instance->event.type = PicopassPollerEventTypeRequestKey;
        command = instance->callback(instance->event, instance->context);
        if(command != NfcCommandContinue) break;

        if(!instance->event_data.req_key.is_key_provided) {
            instance->state = PicopassPollerStateAuthFail;
            break;
        }

        FURI_LOG_D(
            TAG,
            "Try to %s auth with key %02x%02x%02x%02x%02x%02x%02x%02x",
            instance->event_data.req_key.is_elite_key ? "elite" : "standard",
            instance->event_data.req_key.key[0],
            instance->event_data.req_key.key[1],
            instance->event_data.req_key.key[2],
            instance->event_data.req_key.key[3],
            instance->event_data.req_key.key[4],
            instance->event_data.req_key.key[5],
            instance->event_data.req_key.key[6],
            instance->event_data.req_key.key[7]);

        PicopassReadCheckResp read_check_resp = {};
        uint8_t* csn = instance->serial_num.data;
        memset(instance->div_key, 0, sizeof(instance->div_key));
        uint8_t* div_key = NULL;

        if(instance->mode == PicopassPollerModeRead) {
            div_key = instance->data->AA1[PICOPASS_SECURE_KD_BLOCK_INDEX].data;
        } else {
            div_key = instance->div_key;
        }

        uint8_t ccnr[12] = {};
        PicopassMac mac = {};

        PicopassError error = picopass_poller_read_check(instance, &read_check_resp);
        if(error == PicopassErrorTimeout) {
            instance->event.type = PicopassPollerEventTypeCardLost;
            command = instance->callback(instance->event, instance->context);
            instance->state = PicopassPollerStateDetect;
            break;
        } else if(error != PicopassErrorNone) {
            FURI_LOG_E(TAG, "Read check failed: %d", error);
            break;
        }
        memcpy(ccnr, read_check_resp.data, sizeof(PicopassReadCheckResp)); // last 4 bytes left 0

        loclass_iclass_calc_div_key(
            csn,
            instance->event_data.req_key.key,
            div_key,
            instance->event_data.req_key.is_elite_key);
        loclass_opt_doReaderMAC(ccnr, div_key, mac.data);

        PicopassCheckResp check_resp = {};
        error = picopass_poller_check(instance, NULL, &mac, &check_resp);
        if(error == PicopassErrorNone) {
            FURI_LOG_I(TAG, "Found key");
            memcpy(instance->mac.data, mac.data, sizeof(PicopassMac));
            if(instance->mode == PicopassPollerModeRead) {
                memcpy(
                    instance->data->pacs.key, instance->event_data.req_key.key, PICOPASS_KEY_LEN);
                instance->data->pacs.elite_kdf = instance->event_data.req_key.is_elite_key;
                picopass_poller_prepare_read(instance);
                instance->state = PicopassPollerStateReadBlock;
            } else if(instance->mode == PicopassPollerModeWrite) {
                instance->state = PicopassPollerStateWriteBlock;
            } else {
                instance->state = PicopassPollerStateWriteKey;
            }
        }

    } while(false);

    return command;
}

NfcCommand picopass_poller_read_block_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        if(instance->current_block == instance->app_limit) {
            if(instance->secured) {
                instance->state = PicopassPollerStateParseCredential;
            } else {
                instance->state = PicopassPollerStateSuccess;
            }
            break;
        }

        if(instance->secured && instance->current_block == PICOPASS_SECURE_KD_BLOCK_INDEX) {
            // Skip over Kd block which is populated earlier (READ of Kd returns all FF's)
            instance->current_block++;
        }

        PicopassBlock block = {};
        PicopassError error =
            picopass_poller_read_block(instance, instance->current_block, &block);
        if(error != PicopassErrorNone) {
            FURI_LOG_E(TAG, "Failed to read block %d: %d", instance->current_block, error);
            instance->state = PicopassPollerStateFail;
            break;
        }
        FURI_LOG_D(
            TAG,
            "Block %d: %02x%02x%02x%02x%02x%02x%02x%02x",
            instance->current_block,
            block.data[0],
            block.data[1],
            block.data[2],
            block.data[3],
            block.data[4],
            block.data[5],
            block.data[6],
            block.data[7]);
        memcpy(
            instance->data->AA1[instance->current_block].data, block.data, sizeof(PicopassBlock));
        instance->current_block++;
    } while(false);

    return command;
}

NfcCommand picopass_poller_parse_credential_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    picopass_device_parse_credential(instance->data->AA1, &instance->data->pacs);
    instance->state = PicopassPollerStateParseWiegand;
    return command;
}

NfcCommand picopass_poller_parse_wiegand_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    picopass_device_parse_wiegand(instance->data->pacs.credential, &instance->data->pacs);
    instance->state = PicopassPollerStateSuccess;
    return command;
}

NfcCommand picopass_poller_write_block_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;
    PicopassError error = PicopassErrorNone;

    do {
        instance->event.type = PicopassPollerEventTypeRequestWriteBlock;
        command = instance->callback(instance->event, instance->context);
        if(command != NfcCommandContinue) break;

        PicopassPollerEventDataRequestWriteBlock* write_block = &instance->event_data.req_write;
        if(!write_block->perform_write) {
            instance->state = PicopassPollerStateSuccess;
            break;
        }

        FURI_LOG_D(TAG, "Writing %d block", write_block->block_num);
        uint8_t data[9] = {};
        data[0] = write_block->block_num;
        memcpy(&data[1], write_block->block->data, PICOPASS_BLOCK_LEN);
        loclass_doMAC_N(data, sizeof(data), instance->div_key, instance->mac.data);
        FURI_LOG_D(
            TAG,
            "loclass_doMAC_N %d %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x",
            write_block->block_num,
            data[1],
            data[2],
            data[3],
            data[4],
            data[5],
            data[6],
            data[7],
            data[8],
            instance->mac.data[0],
            instance->mac.data[1],
            instance->mac.data[2],
            instance->mac.data[3]);
        error = picopass_poller_write_block(
            instance, write_block->block_num, write_block->block, &instance->mac);
        if(error != PicopassErrorNone) {
            FURI_LOG_E(TAG, "Failed to write block %d. Error %d", write_block->block_num, error);
            instance->state = PicopassPollerStateFail;
            break;
        }

    } while(false);

    return command;
}

NfcCommand picopass_poller_write_key_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;
    PicopassError error = PicopassErrorNone;

    do {
        instance->event.type = PicopassPollerEventTypeRequestWriteKey;
        command = instance->callback(instance->event, instance->context);
        if(command != NfcCommandContinue) break;

        const PicopassDeviceData* picopass_data = instance->event_data.req_write_key.data;
        const uint8_t* new_key = instance->event_data.req_write_key.key;
        bool is_elite_key = instance->event_data.req_write_key.is_elite_key;

        const uint8_t* csn = picopass_data->AA1[PICOPASS_CSN_BLOCK_INDEX].data;
        const uint8_t* config_block = picopass_data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data;
        uint8_t fuses = config_block[7];
        const uint8_t* old_key = picopass_data->AA1[PICOPASS_SECURE_KD_BLOCK_INDEX].data;

        PicopassBlock new_block = {};
        loclass_iclass_calc_div_key(csn, new_key, new_block.data, is_elite_key);

        if((fuses & 0x80) == 0x80) {
            FURI_LOG_D(TAG, "Plain write for personalized mode key change");
        } else {
            FURI_LOG_D(TAG, "XOR write for application mode key change");
            // XOR when in application mode
            for(size_t i = 0; i < PICOPASS_BLOCK_LEN; i++) {
                new_block.data[i] ^= old_key[i];
            }
        }

        FURI_LOG_D(TAG, "Writing key to %d block", PICOPASS_SECURE_KD_BLOCK_INDEX);
        uint8_t data[9] = {};
        data[0] = PICOPASS_SECURE_KD_BLOCK_INDEX;
        memcpy(&data[1], new_block.data, PICOPASS_BLOCK_LEN);
        loclass_doMAC_N(data, sizeof(data), instance->div_key, instance->mac.data);
        FURI_LOG_D(
            TAG,
            "loclass_doMAC_N %d %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x",
            PICOPASS_SECURE_KD_BLOCK_INDEX,
            data[1],
            data[2],
            data[3],
            data[4],
            data[5],
            data[6],
            data[7],
            data[8],
            instance->mac.data[0],
            instance->mac.data[1],
            instance->mac.data[2],
            instance->mac.data[3]);
        error = picopass_poller_write_block(
            instance, PICOPASS_SECURE_KD_BLOCK_INDEX, &new_block, &instance->mac);
        if(error != PicopassErrorNone) {
            FURI_LOG_E(
                TAG, "Failed to write block %d. Error %d", PICOPASS_SECURE_KD_BLOCK_INDEX, error);
            instance->state = PicopassPollerStateFail;
            break;
        }
        instance->state = PicopassPollerStateSuccess;

    } while(false);

    return command;
}

NfcCommand picopass_poller_success_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->event.type = PicopassPollerEventTypeSuccess;
    command = instance->callback(instance->event, instance->context);
    furi_delay_ms(100);

    return command;
}

NfcCommand picopass_poller_fail_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandReset;

    instance->event.type = PicopassPollerEventTypeFail;
    command = instance->callback(instance->event, instance->context);
    picopass_poller_reset(instance);
    instance->state = PicopassPollerStateDetect;

    return command;
}

NfcCommand picopass_poller_auth_fail_handler(PicopassPoller* instance) {
    NfcCommand command = NfcCommandReset;

    instance->event.type = PicopassPollerEventTypeAuthFail;
    command = instance->callback(instance->event, instance->context);
    picopass_poller_reset(instance);
    instance->state = PicopassPollerStateDetect;

    return command;
}

static const PicopassPollerStateHandler picopass_poller_state_handler[PicopassPollerStateNum] = {
    [PicopassPollerStateRequestMode] = picopass_poller_request_mode_handler,
    [PicopassPollerStateDetect] = picopass_poller_detect_handler,
    [PicopassPollerStateSelect] = picopass_poller_select_handler,
    [PicopassPollerStatePreAuth] = picopass_poller_pre_auth_handler,
    [PicopassPollerStateCheckSecurity] = picopass_poller_check_security,
    [PicopassPollerStateNrMacAuth] = picopass_poller_nr_mac_auth,
    [PicopassPollerStateAuth] = picopass_poller_auth_handler,
    [PicopassPollerStateReadBlock] = picopass_poller_read_block_handler,
    [PicopassPollerStateWriteBlock] = picopass_poller_write_block_handler,
    [PicopassPollerStateWriteKey] = picopass_poller_write_key_handler,
    [PicopassPollerStateParseCredential] = picopass_poller_parse_credential_handler,
    [PicopassPollerStateParseWiegand] = picopass_poller_parse_wiegand_handler,
    [PicopassPollerStateSuccess] = picopass_poller_success_handler,
    [PicopassPollerStateFail] = picopass_poller_fail_handler,
    [PicopassPollerStateAuthFail] = picopass_poller_auth_fail_handler,
};

static NfcCommand picopass_poller_callback(NfcEvent event, void* context) {
    furi_assert(context);

    PicopassPoller* instance = context;
    NfcCommand command = NfcCommandContinue;

    if(event.type == NfcEventTypePollerReady) {
        command = picopass_poller_state_handler[instance->state](instance);
    }

    if(instance->session_state == PicopassPollerSessionStateStopRequest) {
        command = NfcCommandStop;
    }

    return command;
}

void picopass_poller_start(
    PicopassPoller* instance,
    PicopassPollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->session_state == PicopassPollerSessionStateIdle);

    instance->callback = callback;
    instance->context = context;

    instance->session_state = PicopassPollerSessionStateActive;
    nfc_start(instance->nfc, picopass_poller_callback, instance);
}

void picopass_poller_stop(PicopassPoller* instance) {
    furi_assert(instance);

    instance->session_state = PicopassPollerSessionStateStopRequest;
    nfc_stop(instance->nfc);
    instance->session_state = PicopassPollerSessionStateIdle;
}

PicopassPoller* picopass_poller_alloc(Nfc* nfc) {
    furi_assert(nfc);

    PicopassPoller* instance = malloc(sizeof(PicopassPoller));
    instance->nfc = nfc;
    nfc_config(instance->nfc, NfcModePoller, NfcTechIso15693);
    nfc_set_guard_time_us(instance->nfc, 10000);
    nfc_set_fdt_poll_fc(instance->nfc, 5000);
    nfc_set_fdt_poll_poll_us(instance->nfc, 1000);

    instance->event.data = &instance->event_data;
    instance->data = malloc(sizeof(PicopassDeviceData));

    instance->tx_buffer = bit_buffer_alloc(PICOPASS_POLLER_BUFFER_SIZE);
    instance->rx_buffer = bit_buffer_alloc(PICOPASS_POLLER_BUFFER_SIZE);
    instance->tmp_buffer = bit_buffer_alloc(PICOPASS_POLLER_BUFFER_SIZE);

    return instance;
}

void picopass_poller_free(PicopassPoller* instance) {
    furi_assert(instance);

    free(instance->data);
    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);
    bit_buffer_free(instance->tmp_buffer);
    free(instance);
}

const PicopassDeviceData* picopass_poller_get_data(PicopassPoller* instance) {
    furi_assert(instance);

    return instance->data;
}
