#include "picopass_worker_i.h"

#include <flipper_format/flipper_format.h>
#include <lib/nfc/protocols/nfcv.h>

#define TAG "PicopassWorker"

#define HAS_MASK(x, b) ((x & b) == b)

// CSNs from Proxmark3 repo
static const uint8_t loclass_csns[LOCLASS_NUM_CSNS][RFAL_PICOPASS_BLOCK_LEN] = {
    {0x01, 0x0A, 0x0F, 0xFF, 0xF7, 0xFF, 0x12, 0xE0},
    {0x0C, 0x06, 0x0C, 0xFE, 0xF7, 0xFF, 0x12, 0xE0},
    {0x10, 0x97, 0x83, 0x7B, 0xF7, 0xFF, 0x12, 0xE0},
    {0x13, 0x97, 0x82, 0x7A, 0xF7, 0xFF, 0x12, 0xE0},
    {0x07, 0x0E, 0x0D, 0xF9, 0xF7, 0xFF, 0x12, 0xE0},
    {0x14, 0x96, 0x84, 0x76, 0xF7, 0xFF, 0x12, 0xE0},
    {0x17, 0x96, 0x85, 0x71, 0xF7, 0xFF, 0x12, 0xE0},
    {0xCE, 0xC5, 0x0F, 0x77, 0xF7, 0xFF, 0x12, 0xE0},
    {0xD2, 0x5A, 0x82, 0xF8, 0xF7, 0xFF, 0x12, 0xE0},
};

static void picopass_worker_enable_field() {
    furi_hal_nfc_exit_sleep();
    furi_hal_nfc_ll_txrx_on();
    furi_hal_nfc_ll_poll();
}

static ReturnCode picopass_worker_disable_field(ReturnCode rc) {
    furi_hal_nfc_ll_txrx_off();
    furi_hal_nfc_start_sleep();
    return rc;
}

/***************************** Picopass Worker API *******************************/

PicopassWorker* picopass_worker_alloc() {
    PicopassWorker* picopass_worker = malloc(sizeof(PicopassWorker));

    // Worker thread attributes
    picopass_worker->thread =
        furi_thread_alloc_ex("PicopassWorker", 8 * 1024, picopass_worker_task, picopass_worker);

    picopass_worker->callback = NULL;
    picopass_worker->context = NULL;
    picopass_worker->storage = furi_record_open(RECORD_STORAGE);

    picopass_worker_change_state(picopass_worker, PicopassWorkerStateReady);

    return picopass_worker;
}

void picopass_worker_free(PicopassWorker* picopass_worker) {
    furi_assert(picopass_worker);

    furi_thread_free(picopass_worker->thread);

    furi_record_close(RECORD_STORAGE);

    free(picopass_worker);
}

PicopassWorkerState picopass_worker_get_state(PicopassWorker* picopass_worker) {
    return picopass_worker->state;
}

void picopass_worker_start(
    PicopassWorker* picopass_worker,
    PicopassWorkerState state,
    PicopassDeviceData* dev_data,
    PicopassWorkerCallback callback,
    void* context) {
    furi_assert(picopass_worker);
    furi_assert(dev_data);

    picopass_worker->callback = callback;
    picopass_worker->context = context;
    picopass_worker->dev_data = dev_data;
    picopass_worker_change_state(picopass_worker, state);
    furi_thread_start(picopass_worker->thread);
}

void picopass_worker_stop(PicopassWorker* picopass_worker) {
    furi_assert(picopass_worker);
    furi_assert(picopass_worker->thread);

    if(furi_thread_get_state(picopass_worker->thread) == FuriThreadStateStopped) {
        return;
    }

    if(picopass_worker->state == PicopassWorkerStateBroken ||
       picopass_worker->state == PicopassWorkerStateReady) {
        return;
    }

    if(picopass_worker->state != PicopassWorkerStateEmulate &&
       picopass_worker->state != PicopassWorkerStateLoclass) {
        // Can't do this while emulating in transparent mode as SPI isn't active
        picopass_worker_disable_field(ERR_NONE);
    }

    if(furi_thread_get_state(picopass_worker->thread) != FuriThreadStateStopped) {
        picopass_worker_change_state(picopass_worker, PicopassWorkerStateStop);
        furi_thread_join(picopass_worker->thread);
    }
}

void picopass_worker_change_state(PicopassWorker* picopass_worker, PicopassWorkerState state) {
    picopass_worker->state = state;
}

/***************************** Picopass Worker Thread *******************************/

ReturnCode picopass_detect_card(int timeout) {
    UNUSED(timeout);

    ReturnCode err;

    err = rfalPicoPassPollerInitialize();
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerInitialize error %d", err);
        return err;
    }

    err = rfalFieldOnAndStartGT();
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalFieldOnAndStartGT error %d", err);
        return err;
    }

    err = rfalPicoPassPollerCheckPresence();
    if(err != ERR_RF_COLLISION) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerCheckPresence error %d", err);
        return err;
    }

    return ERR_NONE;
}

ReturnCode picopass_read_preauth(PicopassBlock* AA1) {
    rfalPicoPassIdentifyRes idRes;
    rfalPicoPassSelectRes selRes;

    ReturnCode err;

    err = rfalPicoPassPollerIdentify(&idRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerIdentify error %d", err);
        return err;
    }

    err = rfalPicoPassPollerSelect(idRes.CSN, &selRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerSelect error %d", err);
        return err;
    }

    memcpy(AA1[PICOPASS_CSN_BLOCK_INDEX].data, selRes.CSN, sizeof(selRes.CSN));
    FURI_LOG_D(
        TAG,
        "csn %02x%02x%02x%02x%02x%02x%02x%02x",
        AA1[PICOPASS_CSN_BLOCK_INDEX].data[0],
        AA1[PICOPASS_CSN_BLOCK_INDEX].data[1],
        AA1[PICOPASS_CSN_BLOCK_INDEX].data[2],
        AA1[PICOPASS_CSN_BLOCK_INDEX].data[3],
        AA1[PICOPASS_CSN_BLOCK_INDEX].data[4],
        AA1[PICOPASS_CSN_BLOCK_INDEX].data[5],
        AA1[PICOPASS_CSN_BLOCK_INDEX].data[6],
        AA1[PICOPASS_CSN_BLOCK_INDEX].data[7]);

    rfalPicoPassReadBlockRes cfg = {0};
    rfalPicoPassPollerReadBlock(PICOPASS_CONFIG_BLOCK_INDEX, &cfg);
    memcpy(AA1[PICOPASS_CONFIG_BLOCK_INDEX].data, cfg.data, sizeof(cfg.data));
    FURI_LOG_D(
        TAG,
        "config %02x%02x%02x%02x%02x%02x%02x%02x",
        AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[0],
        AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[1],
        AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[2],
        AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[3],
        AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[4],
        AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[5],
        AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[6],
        AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[7]);

    rfalPicoPassReadBlockRes aia;
    rfalPicoPassPollerReadBlock(PICOPASS_SECURE_AIA_BLOCK_INDEX, &aia);
    memcpy(AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data, aia.data, sizeof(aia.data));
    FURI_LOG_D(
        TAG,
        "aia %02x%02x%02x%02x%02x%02x%02x%02x",
        AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[0],
        AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[1],
        AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[2],
        AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[3],
        AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[4],
        AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[5],
        AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[6],
        AA1[PICOPASS_SECURE_AIA_BLOCK_INDEX].data[7]);

    return ERR_NONE;
}

static ReturnCode
    picopass_auth_dict(PicopassWorker* picopass_worker, IclassEliteDictType dict_type) {
    rfalPicoPassReadCheckRes rcRes;
    rfalPicoPassCheckRes chkRes;
    bool elite = (dict_type != IclassStandardDictTypeFlipper);

    PicopassDeviceData* dev_data = picopass_worker->dev_data;
    PicopassBlock* AA1 = dev_data->AA1;
    PicopassPacs* pacs = &dev_data->pacs;

    uint8_t* csn = AA1[PICOPASS_CSN_BLOCK_INDEX].data;
    uint8_t* div_key = AA1[PICOPASS_SECURE_KD_BLOCK_INDEX].data;

    ReturnCode err = ERR_PARAM;

    uint8_t mac[4] = {0};
    uint8_t ccnr[12] = {0};

    size_t index = 0;
    uint8_t key[RFAL_PICOPASS_BLOCK_LEN] = {0};

    if(!iclass_elite_dict_check_presence(dict_type)) {
        FURI_LOG_E(TAG, "Dictionary not found");
        return ERR_PARAM;
    }

    IclassEliteDict* dict = iclass_elite_dict_alloc(dict_type);
    if(!dict) {
        FURI_LOG_E(TAG, "Dictionary not allocated");
        return ERR_PARAM;
    }

    FURI_LOG_D(TAG, "Loaded %lu keys", iclass_elite_dict_get_total_keys(dict));
    while(iclass_elite_dict_get_next_key(dict, key)) {
        FURI_LOG_D(
            TAG,
            "Try to %s auth with key %zu %02x%02x%02x%02x%02x%02x%02x%02x",
            elite ? "elite" : "standard",
            index++,
            key[0],
            key[1],
            key[2],
            key[3],
            key[4],
            key[5],
            key[6],
            key[7]);

        err = rfalPicoPassPollerReadCheck(&rcRes);
        if(err != ERR_NONE) {
            FURI_LOG_E(TAG, "rfalPicoPassPollerReadCheck error %d", err);
            break;
        }
        memcpy(ccnr, rcRes.CCNR, sizeof(rcRes.CCNR)); // last 4 bytes left 0

        loclass_iclass_calc_div_key(csn, key, div_key, elite);
        loclass_opt_doReaderMAC(ccnr, div_key, mac);

        err = rfalPicoPassPollerCheck(mac, &chkRes);
        if(err == ERR_NONE) {
            memcpy(pacs->key, key, RFAL_PICOPASS_BLOCK_LEN);
            break;
        }

        if(picopass_worker->state != PicopassWorkerStateDetect) break;
    }

    iclass_elite_dict_free(dict);

    return err;
}

ReturnCode picopass_auth(PicopassWorker* picopass_worker) {
    ReturnCode err;

    FURI_LOG_I(TAG, "Starting system dictionary attack [Standard KDF]");
    err = picopass_auth_dict(picopass_worker, IclassStandardDictTypeFlipper);
    if(err == ERR_NONE) {
        return ERR_NONE;
    }

    FURI_LOG_I(TAG, "Starting user dictionary attack [Elite KDF]");
    err = picopass_auth_dict(picopass_worker, IclassEliteDictTypeUser);
    if(err == ERR_NONE) {
        return ERR_NONE;
    }

    FURI_LOG_I(TAG, "Starting system dictionary attack [Elite KDF]");
    err = picopass_auth_dict(picopass_worker, IclassEliteDictTypeFlipper);
    if(err == ERR_NONE) {
        return ERR_NONE;
    }

    return err;
}

ReturnCode picopass_read_card(PicopassBlock* AA1) {
    ReturnCode err;

    size_t app_limit = AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[0] < PICOPASS_MAX_APP_LIMIT ?
                           AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[0] :
                           PICOPASS_MAX_APP_LIMIT;

    for(size_t i = 2; i < app_limit; i++) {
        if(i == PICOPASS_SECURE_KD_BLOCK_INDEX) {
            // Skip over Kd block which is populated earlier (READ of Kd returns all FF's)
            continue;
        }

        rfalPicoPassReadBlockRes block;
        err = rfalPicoPassPollerReadBlock(i, &block);
        if(err != ERR_NONE) {
            FURI_LOG_E(TAG, "rfalPicoPassPollerReadBlock error %d", err);
            return err;
        }

        FURI_LOG_D(
            TAG,
            "rfalPicoPassPollerReadBlock %d %02x%02x%02x%02x%02x%02x%02x%02x",
            i,
            block.data[0],
            block.data[1],
            block.data[2],
            block.data[3],
            block.data[4],
            block.data[5],
            block.data[6],
            block.data[7]);

        memcpy(AA1[i].data, block.data, sizeof(block.data));
    }

    return ERR_NONE;
}

ReturnCode picopass_write_card(PicopassBlock* AA1) {
    rfalPicoPassIdentifyRes idRes;
    rfalPicoPassSelectRes selRes;
    rfalPicoPassReadCheckRes rcRes;
    rfalPicoPassCheckRes chkRes;

    ReturnCode err;

    uint8_t div_key[8] = {0};
    uint8_t mac[4] = {0};
    uint8_t ccnr[12] = {0};

    err = rfalPicoPassPollerIdentify(&idRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerIdentify error %d", err);
        return err;
    }

    err = rfalPicoPassPollerSelect(idRes.CSN, &selRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerSelect error %d", err);
        return err;
    }

    err = rfalPicoPassPollerReadCheck(&rcRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerReadCheck error %d", err);
        return err;
    }
    memcpy(ccnr, rcRes.CCNR, sizeof(rcRes.CCNR)); // last 4 bytes left 0

    loclass_iclass_calc_div_key(selRes.CSN, (uint8_t*)picopass_iclass_key, div_key, false);
    loclass_opt_doReaderMAC(ccnr, div_key, mac);

    err = rfalPicoPassPollerCheck(mac, &chkRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerCheck error %d", err);
        return err;
    }

    for(size_t i = 6; i < 10; i++) {
        FURI_LOG_D(TAG, "rfalPicoPassPollerWriteBlock %d", i);
        uint8_t data[9] = {0};
        data[0] = i;
        memcpy(data + 1, AA1[i].data, RFAL_PICOPASS_BLOCK_LEN);
        loclass_doMAC_N(data, sizeof(data), div_key, mac);
        FURI_LOG_D(
            TAG,
            "loclass_doMAC_N %d %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x",
            i,
            data[1],
            data[2],
            data[3],
            data[4],
            data[5],
            data[6],
            data[7],
            data[8],
            mac[0],
            mac[1],
            mac[2],
            mac[3]);

        err = rfalPicoPassPollerWriteBlock(i, AA1[i].data, mac);
        if(err != ERR_NONE) {
            FURI_LOG_E(TAG, "rfalPicoPassPollerWriteBlock error %d", err);
            return err;
        }
    }

    return ERR_NONE;
}

ReturnCode picopass_write_block(PicopassBlock* AA1, uint8_t blockNo, uint8_t* newBlock) {
    rfalPicoPassIdentifyRes idRes;
    rfalPicoPassSelectRes selRes;
    rfalPicoPassReadCheckRes rcRes;
    rfalPicoPassCheckRes chkRes;

    ReturnCode err;

    uint8_t mac[4] = {0};
    uint8_t ccnr[12] = {0};

    err = rfalPicoPassPollerIdentify(&idRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerIdentify error %d", err);
        return err;
    }

    err = rfalPicoPassPollerSelect(idRes.CSN, &selRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerSelect error %d", err);
        return err;
    }

    err = rfalPicoPassPollerReadCheck(&rcRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerReadCheck error %d", err);
        return err;
    }
    memcpy(ccnr, rcRes.CCNR, sizeof(rcRes.CCNR)); // last 4 bytes left 0

    if(memcmp(selRes.CSN, AA1[PICOPASS_CSN_BLOCK_INDEX].data, RFAL_PICOPASS_BLOCK_LEN) != 0) {
        FURI_LOG_E(TAG, "Wrong CSN for write");
        return ERR_REQUEST;
    }

    loclass_opt_doReaderMAC(ccnr, AA1[PICOPASS_SECURE_KD_BLOCK_INDEX].data, mac);
    err = rfalPicoPassPollerCheck(mac, &chkRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerCheck error %d", err);
        return err;
    }

    FURI_LOG_D(TAG, "rfalPicoPassPollerWriteBlock %d", blockNo);
    uint8_t data[9] = {
        blockNo,
        newBlock[0],
        newBlock[1],
        newBlock[2],
        newBlock[3],
        newBlock[4],
        newBlock[5],
        newBlock[6],
        newBlock[7]};
    loclass_doMAC_N(data, sizeof(data), AA1[PICOPASS_SECURE_KD_BLOCK_INDEX].data, mac);
    FURI_LOG_D(
        TAG,
        "loclass_doMAC_N %d %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x",
        blockNo,
        data[1],
        data[2],
        data[3],
        data[4],
        data[5],
        data[6],
        data[7],
        data[8],
        mac[0],
        mac[1],
        mac[2],
        mac[3]);

    err = rfalPicoPassPollerWriteBlock(data[0], data + 1, mac);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerWriteBlock error %d", err);
        return err;
    }

    return ERR_NONE;
}

void picopass_worker_elite_dict_attack(PicopassWorker* picopass_worker) {
    furi_assert(picopass_worker);
    furi_assert(picopass_worker->callback);

    picopass_device_data_clear(picopass_worker->dev_data);
    PicopassDeviceData* dev_data = picopass_worker->dev_data;
    PicopassBlock* AA1 = dev_data->AA1;
    PicopassPacs* pacs = &dev_data->pacs;

    for(size_t i = 0; i < PICOPASS_MAX_APP_LIMIT; i++) {
        memset(AA1[i].data, 0, sizeof(AA1[i].data));
    }
    memset(pacs, 0, sizeof(PicopassPacs));

    IclassEliteDictAttackData* dict_attack_data =
        &picopass_worker->dev_data->iclass_elite_dict_attack_data;
    bool elite = (dict_attack_data->type != IclassStandardDictTypeFlipper);

    rfalPicoPassReadCheckRes rcRes;
    rfalPicoPassCheckRes chkRes;

    ReturnCode err;
    uint8_t mac[4] = {0};
    uint8_t ccnr[12] = {0};

    size_t index = 0;
    uint8_t key[RFAL_PICOPASS_BLOCK_LEN] = {0};

    // Load dictionary
    IclassEliteDict* dict = dict_attack_data->dict;
    if(!dict) {
        FURI_LOG_E(TAG, "Dictionary not found");
        picopass_worker->callback(PicopassWorkerEventNoDictFound, picopass_worker->context);
        return;
    }

    do {
        if(picopass_detect_card(1000) == ERR_NONE) {
            picopass_worker->callback(PicopassWorkerEventCardDetected, picopass_worker->context);

            // Process first found device
            err = picopass_read_preauth(AA1);
            if(err != ERR_NONE) {
                FURI_LOG_E(TAG, "picopass_read_preauth error %d", err);
                picopass_worker->callback(PicopassWorkerEventAborted, picopass_worker->context);
                return;
            }

            // Thank you proxmark!
            pacs->legacy = picopass_is_memset(AA1[5].data, 0xFF, 8);
            pacs->se_enabled = (memcmp(AA1[5].data, "\xff\xff\xff\x00\x06\xff\xff\xff", 8) == 0);
            if(pacs->se_enabled) {
                FURI_LOG_D(TAG, "SE enabled");
                picopass_worker->callback(PicopassWorkerEventAborted, picopass_worker->context);
                return;
            }

            break;
        } else {
            picopass_worker->callback(PicopassWorkerEventNoCardDetected, picopass_worker->context);
        }
        if(picopass_worker->state != PicopassWorkerStateEliteDictAttack) break;

        furi_delay_ms(100);
    } while(true);

    FURI_LOG_D(
        TAG, "Start Dictionary attack, Key Count %lu", iclass_elite_dict_get_total_keys(dict));
    while(iclass_elite_dict_get_next_key(dict, key)) {
        FURI_LOG_T(TAG, "Key %zu", index);
        if(++index % PICOPASS_DICT_KEY_BATCH_SIZE == 0) {
            picopass_worker->callback(
                PicopassWorkerEventNewDictKeyBatch, picopass_worker->context);
        }

        err = rfalPicoPassPollerReadCheck(&rcRes);
        if(err != ERR_NONE) {
            FURI_LOG_E(TAG, "rfalPicoPassPollerReadCheck error %d", err);
            break;
        }
        memcpy(ccnr, rcRes.CCNR, sizeof(rcRes.CCNR)); // last 4 bytes left 0

        uint8_t* csn = AA1[PICOPASS_CSN_BLOCK_INDEX].data;
        uint8_t* div_key = AA1[PICOPASS_SECURE_KD_BLOCK_INDEX].data;

        loclass_iclass_calc_div_key(csn, key, div_key, elite);
        loclass_opt_doReaderMAC(ccnr, div_key, mac);

        err = rfalPicoPassPollerCheck(mac, &chkRes);
        if(err == ERR_NONE) {
            FURI_LOG_I(TAG, "Found key");
            memcpy(pacs->key, key, RFAL_PICOPASS_BLOCK_LEN);
            pacs->elite_kdf = elite;
            err = picopass_read_card(AA1);
            if(err != ERR_NONE) {
                FURI_LOG_E(TAG, "picopass_read_card error %d", err);
                picopass_worker->callback(PicopassWorkerEventFail, picopass_worker->context);
                break;
            }

            err = picopass_device_parse_credential(AA1, pacs);
            if(err != ERR_NONE) {
                FURI_LOG_E(TAG, "picopass_device_parse_credential error %d", err);
                picopass_worker->callback(PicopassWorkerEventFail, picopass_worker->context);
                break;
            }

            err = picopass_device_parse_wiegand(pacs->credential, &pacs->record);
            if(err != ERR_NONE) {
                FURI_LOG_E(TAG, "picopass_device_parse_wiegand error %d", err);
                picopass_worker->callback(PicopassWorkerEventFail, picopass_worker->context);
                break;
            }
            picopass_worker->callback(PicopassWorkerEventAborted, picopass_worker->context);
            break;
        }

        if(picopass_worker->state != PicopassWorkerStateEliteDictAttack) break;
    }
    FURI_LOG_D(TAG, "Dictionary complete");
    if(picopass_worker->state == PicopassWorkerStateEliteDictAttack) {
        picopass_worker->callback(PicopassWorkerEventSuccess, picopass_worker->context);
    } else {
        picopass_worker->callback(PicopassWorkerEventAborted, picopass_worker->context);
    }
}

int32_t picopass_worker_task(void* context) {
    PicopassWorker* picopass_worker = context;

    if(picopass_worker->state == PicopassWorkerStateDetect) {
        picopass_worker_enable_field();
        picopass_worker_detect(picopass_worker);
    } else if(picopass_worker->state == PicopassWorkerStateWrite) {
        picopass_worker_enable_field();
        picopass_worker_write(picopass_worker);
    } else if(picopass_worker->state == PicopassWorkerStateWriteKey) {
        picopass_worker_enable_field();
        picopass_worker_write_key(picopass_worker);
    } else if(picopass_worker->state == PicopassWorkerStateEliteDictAttack) {
        picopass_worker_enable_field();
        picopass_worker_elite_dict_attack(picopass_worker);
    } else if(picopass_worker->state == PicopassWorkerStateEmulate) {
        picopass_worker_emulate(picopass_worker, false);
    } else if(picopass_worker->state == PicopassWorkerStateLoclass) {
        picopass_worker_emulate(picopass_worker, true);
    } else if(picopass_worker->state == PicopassWorkerStateStop) {
        FURI_LOG_D(TAG, "Worker state stop");
        // no-op
    } else {
        FURI_LOG_W(TAG, "Unknown state %d", picopass_worker->state);
    }
    picopass_worker_disable_field(ERR_NONE);
    picopass_worker_change_state(picopass_worker, PicopassWorkerStateReady);

    return 0;
}

void picopass_worker_detect(PicopassWorker* picopass_worker) {
    picopass_device_data_clear(picopass_worker->dev_data);
    PicopassDeviceData* dev_data = picopass_worker->dev_data;

    PicopassBlock* AA1 = dev_data->AA1;
    PicopassPacs* pacs = &dev_data->pacs;
    ReturnCode err;

    // reset device data
    for(size_t i = 0; i < PICOPASS_MAX_APP_LIMIT; i++) {
        memset(AA1[i].data, 0, sizeof(AA1[i].data));
    }
    memset(pacs, 0, sizeof(PicopassPacs));

    PicopassWorkerEvent nextState = PicopassWorkerEventSuccess;

    while(picopass_worker->state == PicopassWorkerStateDetect) {
        if(picopass_detect_card(1000) == ERR_NONE) {
            // Process first found device
            err = picopass_read_preauth(AA1);
            if(err != ERR_NONE) {
                FURI_LOG_E(TAG, "picopass_read_preauth error %d", err);
                nextState = PicopassWorkerEventFail;
            }

            // Thank you proxmark!
            pacs->legacy = picopass_is_memset(AA1[5].data, 0xFF, 8);
            pacs->se_enabled = (memcmp(AA1[5].data, "\xff\xff\xff\x00\x06\xff\xff\xff", 8) == 0);
            if(pacs->se_enabled) {
                FURI_LOG_D(TAG, "SE enabled");
                nextState = PicopassWorkerEventFail;
            }

            if(nextState == PicopassWorkerEventSuccess) {
                err = picopass_auth(picopass_worker);
                if(err != ERR_NONE) {
                    FURI_LOG_E(TAG, "picopass_try_auth error %d", err);
                    nextState = PicopassWorkerEventFail;
                }
            }

            if(nextState == PicopassWorkerEventSuccess) {
                err = picopass_read_card(AA1);
                if(err != ERR_NONE) {
                    FURI_LOG_E(TAG, "picopass_read_card error %d", err);
                    nextState = PicopassWorkerEventFail;
                }
            }

            if(nextState == PicopassWorkerEventSuccess) {
                err = picopass_device_parse_credential(AA1, pacs);
                if(err != ERR_NONE) {
                    FURI_LOG_E(TAG, "picopass_device_parse_credential error %d", err);
                    nextState = PicopassWorkerEventFail;
                }
            }

            if(nextState == PicopassWorkerEventSuccess) {
                err = picopass_device_parse_wiegand(pacs->credential, &pacs->record);
                if(err != ERR_NONE) {
                    FURI_LOG_E(TAG, "picopass_device_parse_wiegand error %d", err);
                    nextState = PicopassWorkerEventFail;
                }
            }

            // Notify caller and exit
            if(picopass_worker->callback) {
                picopass_worker->callback(nextState, picopass_worker->context);
            }
            break;
        }
        furi_delay_ms(100);
    }
}

void picopass_worker_write(PicopassWorker* picopass_worker) {
    PicopassDeviceData* dev_data = picopass_worker->dev_data;
    PicopassBlock* AA1 = dev_data->AA1;
    ReturnCode err;
    PicopassWorkerEvent nextState = PicopassWorkerEventSuccess;

    while(picopass_worker->state == PicopassWorkerStateWrite) {
        if(picopass_detect_card(1000) == ERR_NONE) {
            err = picopass_write_card(AA1);
            if(err != ERR_NONE) {
                FURI_LOG_E(TAG, "picopass_write_card error %d", err);
                nextState = PicopassWorkerEventFail;
            }

            // Notify caller and exit
            if(picopass_worker->callback) {
                picopass_worker->callback(nextState, picopass_worker->context);
            }
            break;
        }
        furi_delay_ms(100);
    }
}

void picopass_worker_write_key(PicopassWorker* picopass_worker) {
    PicopassDeviceData* dev_data = picopass_worker->dev_data;
    PicopassBlock* AA1 = dev_data->AA1;
    PicopassPacs* pacs = &dev_data->pacs;
    ReturnCode err;
    PicopassWorkerEvent nextState = PicopassWorkerEventSuccess;

    uint8_t* csn = AA1[PICOPASS_CSN_BLOCK_INDEX].data;
    uint8_t* configBlock = AA1[PICOPASS_CONFIG_BLOCK_INDEX].data;
    uint8_t fuses = configBlock[7];
    uint8_t* oldKey = AA1[PICOPASS_SECURE_KD_BLOCK_INDEX].data;

    uint8_t newKey[RFAL_PICOPASS_BLOCK_LEN] = {0};
    loclass_iclass_calc_div_key(csn, pacs->key, newKey, pacs->elite_kdf);

    if((fuses & 0x80) == 0x80) {
        FURI_LOG_D(TAG, "Plain write for personalized mode key change");
    } else {
        FURI_LOG_D(TAG, "XOR write for application mode key change");
        // XOR when in application mode
        for(size_t i = 0; i < RFAL_PICOPASS_BLOCK_LEN; i++) {
            newKey[i] ^= oldKey[i];
        }
    }

    while(picopass_worker->state == PicopassWorkerStateWriteKey) {
        if(picopass_detect_card(1000) == ERR_NONE) {
            err = picopass_write_block(AA1, PICOPASS_SECURE_KD_BLOCK_INDEX, newKey);
            if(err != ERR_NONE) {
                FURI_LOG_E(TAG, "picopass_write_block error %d", err);
                nextState = PicopassWorkerEventFail;
            }

            // Notify caller and exit
            if(picopass_worker->callback) {
                picopass_worker->callback(nextState, picopass_worker->context);
            }
            break;
        }
        furi_delay_ms(100);
    }
}

// from proxmark3 armsrc/iclass.c rotateCSN
static void picopass_anticoll_csn(uint8_t* rotated_csn, const uint8_t* original_csn) {
    for(uint8_t i = 0; i < 8; i++) {
        rotated_csn[i] = (original_csn[i] >> 3) | (original_csn[(i + 1) % 8] << 5);
    }
}

static void picopass_append_crc(uint8_t* buf, uint16_t size) {
    uint16_t crc = rfalPicoPassCalculateCcitt(0xE012, buf, size);

    buf[size] = crc & 0xFF;
    buf[size + 1] = crc >> 8;
}

static inline void picopass_emu_read_blocks(
    NfcVData* nfcv_data,
    uint8_t* buf,
    uint8_t block_num,
    uint8_t block_count) {
    memcpy(
        buf,
        nfcv_data->data + (block_num * RFAL_PICOPASS_BLOCK_LEN),
        block_count * RFAL_PICOPASS_BLOCK_LEN);
}

static inline void picopass_emu_write_blocks(
    NfcVData* nfcv_data,
    const uint8_t* buf,
    uint8_t block_num,
    uint8_t block_count) {
    memcpy(
        nfcv_data->data + (block_num * RFAL_PICOPASS_BLOCK_LEN),
        buf,
        block_count * RFAL_PICOPASS_BLOCK_LEN);
}

static void picopass_init_cipher_state(NfcVData* nfcv_data, PicopassEmulatorCtx* ctx) {
    uint8_t cc[RFAL_PICOPASS_BLOCK_LEN];
    uint8_t key[RFAL_PICOPASS_BLOCK_LEN];

    picopass_emu_read_blocks(nfcv_data, cc, PICOPASS_SECURE_EPURSE_BLOCK_INDEX, 1);
    picopass_emu_read_blocks(nfcv_data, key, ctx->key_block_num, 1);

    ctx->cipher_state = loclass_opt_doTagMAC_1(cc, key);
}

static void
    loclass_update_csn(FuriHalNfcDevData* nfc_data, NfcVData* nfcv_data, PicopassEmulatorCtx* ctx) {
    // collect two nonces in a row for each CSN
    uint8_t csn_num = (ctx->key_block_num / 2) % LOCLASS_NUM_CSNS;
    memcpy(nfc_data->uid, loclass_csns[csn_num], RFAL_PICOPASS_BLOCK_LEN);
    picopass_emu_write_blocks(nfcv_data, loclass_csns[csn_num], PICOPASS_CSN_BLOCK_INDEX, 1);
}

static void picopass_emu_handle_packet(
    FuriHalNfcTxRxContext* tx_rx,
    FuriHalNfcDevData* nfc_data,
    void* nfcv_data_in) {
    NfcVData* nfcv_data = (NfcVData*)nfcv_data_in;
    PicopassEmulatorCtx* ctx = nfcv_data->emu_protocol_ctx;
    uint8_t response[34];
    uint8_t response_length = 0;
    uint8_t key_block_num = PICOPASS_SECURE_KD_BLOCK_INDEX;

    const uint8_t block_ff[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    if(nfcv_data->frame_length < 1) {
        return;
    }

    switch(nfcv_data->frame[0]) {
    case RFAL_PICOPASS_CMD_ACTALL: // No args
        if(nfcv_data->frame_length != 1) {
            return;
        }

        if(ctx->state != PicopassEmulatorStateHalt) {
            ctx->state = PicopassEmulatorStateActive;
        }

        // Send SOF only
        break;
    case RFAL_PICOPASS_CMD_ACT: // No args
        if(nfcv_data->frame_length != 1 || ctx->state != PicopassEmulatorStateActive) {
            return;
        }

        // Send SOF only
        break;
    case RFAL_PICOPASS_CMD_HALT: // No args
        if(nfcv_data->frame_length != 1 || ctx->state != PicopassEmulatorStateSelected) {
            return;
        }

        // Technically we should go to StateHalt, but since we can't detect the field dropping we drop to idle instead
        ctx->state = PicopassEmulatorStateIdle;

        // Send SOF only
        break;
    case RFAL_PICOPASS_CMD_READ_OR_IDENTIFY:
        if(nfcv_data->frame_length == 1 &&
           ctx->state == PicopassEmulatorStateActive) { // PICOPASS_CMD_IDENTIFY
            // ASNB(8) CRC16(2)
            picopass_anticoll_csn(response, nfc_data->uid);
            picopass_append_crc(response, RFAL_PICOPASS_BLOCK_LEN);
            response_length = RFAL_PICOPASS_BLOCK_LEN + 2;
            break;
        } else if(
            nfcv_data->frame_length == 4 &&
            ctx->state == PicopassEmulatorStateSelected) { // PICOPASS_CMD_READ ADDRESS(1) CRC16(2)
            if(nfcv_data->frame[1] >= PICOPASS_MAX_APP_LIMIT) {
                return;
            }

            // TODO: Check CRC?
            // TODO: Check auth?

            // DATA(8) CRC16(2)
            if(nfcv_data->frame[1] == PICOPASS_SECURE_KD_BLOCK_INDEX ||
               nfcv_data->frame[1] == PICOPASS_SECURE_KC_BLOCK_INDEX) {
                // Reading Kd or Kc blocks always returns FF's
                memcpy(response, block_ff, RFAL_PICOPASS_BLOCK_LEN);
            } else {
                picopass_emu_read_blocks(nfcv_data, response, nfcv_data->frame[1], 1);
            }
            picopass_append_crc(response, RFAL_PICOPASS_BLOCK_LEN);
            response_length = RFAL_PICOPASS_BLOCK_LEN + 2;
            break;
        }

        return;
    case RFAL_PICOPASS_CMD_READ4: // ADDRESS(1) CRC16(2)
        if(nfcv_data->frame_length != 4 || ctx->state != PicopassEmulatorStateSelected ||
           nfcv_data->frame[1] + 4 >= PICOPASS_MAX_APP_LIMIT) {
            return;
        }

        // TODO: Check CRC?
        // TODO: Check auth?

        uint8_t blockNum = nfcv_data->frame[1];

        // DATA(32) CRC16(2)
        picopass_emu_read_blocks(nfcv_data, response, blockNum, 4);
        if(blockNum == 4) {
            // Kc is block 4, so just redact first block of response
            memcpy(response, block_ff, RFAL_PICOPASS_BLOCK_LEN);
        } else if(blockNum < 4) {
            // Kd is block 3
            uint8_t* kdOffset = response + ((3 - blockNum) * RFAL_PICOPASS_BLOCK_LEN);
            memcpy(kdOffset, block_ff, RFAL_PICOPASS_BLOCK_LEN);
            if(blockNum != 0) {
                // Redact Kc
                memcpy(kdOffset + RFAL_PICOPASS_BLOCK_LEN, block_ff, RFAL_PICOPASS_BLOCK_LEN);
            }
        }
        picopass_append_crc(response, RFAL_PICOPASS_BLOCK_LEN * 4);
        response_length = (RFAL_PICOPASS_BLOCK_LEN * 4) + 2;
        break;
    case RFAL_PICOPASS_CMD_SELECT: // ASNB(8)|SERIALNB(8)
        if(nfcv_data->frame_length != 9) {
            return;
        }

        uint8_t select_csn[RFAL_PICOPASS_BLOCK_LEN];
        if(ctx->state == PicopassEmulatorStateHalt || ctx->state == PicopassEmulatorStateIdle) {
            memcpy(select_csn, nfc_data->uid, RFAL_PICOPASS_BLOCK_LEN);
        } else {
            picopass_anticoll_csn(select_csn, nfc_data->uid);
        }

        if(memcmp(nfcv_data->frame + 1, select_csn, RFAL_PICOPASS_BLOCK_LEN)) {
            if(ctx->state == PicopassEmulatorStateActive) {
                ctx->state = PicopassEmulatorStateIdle;
            } else if(ctx->state == PicopassEmulatorStateSelected) {
                // Technically we should go to StateHalt, but since we can't detect the field dropping we drop to idle instead
                ctx->state = PicopassEmulatorStateIdle;
            }

            return;
        }

        ctx->state = PicopassEmulatorStateSelected;

        // SERIALNB(8) CRC16(2)
        memcpy(response, nfc_data->uid, RFAL_PICOPASS_BLOCK_LEN);
        picopass_append_crc(response, RFAL_PICOPASS_BLOCK_LEN);

        response_length = RFAL_PICOPASS_BLOCK_LEN + 2;
        break;
    case RFAL_PICOPASS_CMD_READCHECK_KC: // ADDRESS(1)
        key_block_num = PICOPASS_SECURE_KC_BLOCK_INDEX;
        // fallthrough
    case RFAL_PICOPASS_CMD_READCHECK_KD: // ADDRESS(1)
        if(nfcv_data->frame_length != 2 ||
           nfcv_data->frame[1] != PICOPASS_SECURE_EPURSE_BLOCK_INDEX ||
           ctx->state != PicopassEmulatorStateSelected) {
            return;
        }

        if(ctx->key_block_num != key_block_num && !ctx->loclass_mode) {
            ctx->key_block_num = key_block_num;
            picopass_init_cipher_state(nfcv_data, ctx);
        }

        // DATA(8)
        picopass_emu_read_blocks(nfcv_data, response, nfcv_data->frame[1], 1);
        response_length = RFAL_PICOPASS_BLOCK_LEN;
        break;
    case RFAL_PICOPASS_CMD_CHECK: // CHALLENGE(4) READERSIGNATURE(4)
        if(nfcv_data->frame_length != 9 || ctx->state != PicopassEmulatorStateSelected) {
            return;
        }

        if(ctx->loclass_mode) {
            // LOCLASS Reader attack mode

            // Copy EPURSE
            uint8_t cc[RFAL_PICOPASS_BLOCK_LEN];
            picopass_emu_read_blocks(nfcv_data, cc, PICOPASS_SECURE_EPURSE_BLOCK_INDEX, 1);

            // Check if the nonce is from a standard key
            uint8_t key[RFAL_PICOPASS_BLOCK_LEN];
            loclass_iclass_calc_div_key(nfc_data->uid, picopass_iclass_key, key, false);
            ctx->cipher_state = loclass_opt_doTagMAC_1(cc, key);

            uint8_t rmac[4];
            loclass_opt_doBothMAC_2(ctx->cipher_state, nfcv_data->frame + 1, rmac, response, key);

            if(!memcmp(nfcv_data->frame + 5, rmac, 4)) {
                // MAC from reader matches Standard Key, keyroll mode or non-elite keyed reader.
                // Either way no point logging it.

                FURI_LOG_W(TAG, "loclass: standard key detected during collection");
                ctx->loclass_got_std_key = true;

                ctx->state = PicopassEmulatorStateIdle;
                return;
            }

            // Copy CHALLENGE (nr) and READERSIGNATURE (mac) from frame
            uint8_t nr[4];
            memcpy(nr, nfcv_data->frame + 1, 4);
            uint8_t mac[4];
            memcpy(mac, nfcv_data->frame + 5, 4);

            FURI_LOG_I(TAG, "loclass: got nr/mac pair");
            loclass_writer_write_params(
                ctx->loclass_writer, ctx->key_block_num, nfc_data->uid, cc, nr, mac);

            // Rotate to the next CSN
            ctx->key_block_num = (ctx->key_block_num + 1) % (LOCLASS_NUM_CSNS * 2);
            loclass_update_csn(nfc_data, nfcv_data, ctx);

            ctx->state = PicopassEmulatorStateIdle;

            return;
        }

        uint8_t key[RFAL_PICOPASS_BLOCK_LEN];
        picopass_emu_read_blocks(nfcv_data, key, ctx->key_block_num, 1);

        uint8_t rmac[4];
        loclass_opt_doBothMAC_2(ctx->cipher_state, nfcv_data->frame + 1, rmac, response, key);

        if(memcmp(nfcv_data->frame + 5, rmac, 4)) {
            // Bad MAC from reader, do not send a response.
            FURI_LOG_I(TAG, "Got bad MAC from reader");
#ifndef PICOPASS_DEBUG_IGNORE_BAD_RMAC
            return;
#endif
        }

        // CHIPRESPONSE(4)
        response_length = 4;
        break;
    case RFAL_PICOPASS_CMD_UPDATE: // ADDRESS(1) DATA(8) SIGN(4)|CRC16(2)
        if((nfcv_data->frame_length != 12 && nfcv_data->frame_length != 14) ||
           ctx->state != PicopassEmulatorStateSelected) {
            return;
        }

        if(nfcv_data->frame[1] >= PICOPASS_MAX_APP_LIMIT) {
            return;
        }

        uint8_t cfgBlock[RFAL_PICOPASS_BLOCK_LEN];
        picopass_emu_read_blocks(nfcv_data, cfgBlock, PICOPASS_CONFIG_BLOCK_INDEX, 1);
        bool persMode = HAS_MASK(cfgBlock[7], PICOPASS_FUSE_PERS);

        if((nfcv_data->frame[1] == PICOPASS_CSN_BLOCK_INDEX) // CSN is always read only
           ||
           (!persMode &&
            !HAS_MASK(cfgBlock[3], 0x80)) // Chip is in RO mode, no updated possible (even ePurse)
           || (!persMode &&
               nfcv_data->frame[1] ==
                   PICOPASS_SECURE_AIA_BLOCK_INDEX) // AIA can only be set in personalisation mode
           || (!persMode &&
               (nfcv_data->frame[1] == PICOPASS_SECURE_KD_BLOCK_INDEX ||
                nfcv_data->frame[1] == PICOPASS_SECURE_KC_BLOCK_INDEX) &&
               (!HAS_MASK(cfgBlock[7], PICOPASS_FUSE_CRYPT10)))) {
            return; // TODO: Is this the right response?
        }

        if(nfcv_data->frame[1] >= 6 && nfcv_data->frame[1] <= 12) {
            if(!HAS_MASK(
                   cfgBlock[3],
                   1 << (nfcv_data->frame[1] - 6))) { // bit0 is block6, up to bit6 being block12
                // Block is marked as read-only, deny writing
                return; // TODO: Is this the right response?
            }
        }

        // TODO: Check CRC/SIGN depending on if in secure mode
        // Check correct key
        // -> Kd only allows decrementing e-Purse
        // -> per-app controlled by key access config
        //bool keyAccess = HAS_MASK(cfgBlock[5], 0x01);
        // -> must auth with that key to change it

        uint8_t blockOffset = nfcv_data->frame[1];
        uint8_t block[RFAL_PICOPASS_BLOCK_LEN];
        switch(nfcv_data->frame[1]) {
        case PICOPASS_CONFIG_BLOCK_INDEX:
            block[0] = cfgBlock[0]; // Applications Limit
            block[1] = cfgBlock[1] & nfcv_data->frame[3]; // OTP
            block[2] = cfgBlock[2] & nfcv_data->frame[4]; // OTP
            block[3] = cfgBlock[3] & nfcv_data->frame[5]; // Block Write Lock
            block[4] = cfgBlock[4]; // Chip Config
            block[5] = cfgBlock[5]; // Memory Config
            block[6] = nfcv_data->frame[8]; // EAS
            block[7] = cfgBlock[7]; // Fuses

            // Some parts allow w (but not e) if in persMode
            if(persMode) {
                block[0] &= nfcv_data->frame[2]; // Applications Limit
                block[4] &= nfcv_data->frame[6]; // Chip Config
                block[5] &= nfcv_data->frame[7]; // Memory Config
                block[7] &= nfcv_data->frame[9]; // Fuses
            } else {
                // Fuses allows setting Crypt1/0 from 1 to 0 only during application mode
                block[7] &= nfcv_data->frame[9] | ~PICOPASS_FUSE_CRYPT10;
            }
            break;
        case PICOPASS_SECURE_EPURSE_BLOCK_INDEX:
            // ePurse updates swap first and second half of the block each update
            memcpy(block + 4, nfcv_data->frame + 2, 4);
            memcpy(block, nfcv_data->frame + 6, 4);
            break;
        case PICOPASS_SECURE_KD_BLOCK_INDEX:
            // fallthrough
        case PICOPASS_SECURE_KC_BLOCK_INDEX:
            if(!persMode) {
                picopass_emu_read_blocks(nfcv_data, block, blockOffset, 1);
                for(uint8_t i = 0; i < sizeof(RFAL_PICOPASS_BLOCK_LEN); i++)
                    block[i] ^= nfcv_data->frame[i + 2];
                break;
            }
            // Use default case when in personalisation mode
            // fallthrough
        default:
            memcpy(block, nfcv_data->frame + 2, RFAL_PICOPASS_BLOCK_LEN);
            break;
        }

        picopass_emu_write_blocks(nfcv_data, block, blockOffset, 1);

        if((nfcv_data->frame[1] == ctx->key_block_num ||
            nfcv_data->frame[1] == PICOPASS_SECURE_EPURSE_BLOCK_INDEX) &&
           !ctx->loclass_mode)
            picopass_init_cipher_state(nfcv_data, ctx);

        // DATA(8) CRC16(2)
        if(nfcv_data->frame[1] == PICOPASS_SECURE_KD_BLOCK_INDEX ||
           nfcv_data->frame[1] == PICOPASS_SECURE_KD_BLOCK_INDEX) {
            // Key updates always return FF's
            memcpy(response, block_ff, RFAL_PICOPASS_BLOCK_LEN);
        } else {
            memcpy(response, block, RFAL_PICOPASS_BLOCK_LEN);
        }
        picopass_append_crc(response, RFAL_PICOPASS_BLOCK_LEN);
        response_length = RFAL_PICOPASS_BLOCK_LEN + 2;
        break;
    case RFAL_PICOPASS_CMD_PAGESEL: // PAGE(1) CRC16(2)
        // Chips with a single page do not answer to this command
        // BLOCK1(8) CRC16(2)
        return;
    case RFAL_PICOPASS_CMD_DETECT:
        // TODO - not used by iClass though
        return;
    default:
        return;
    }

    NfcVSendFlags flags = NfcVSendFlagsSof | NfcVSendFlagsOneSubcarrier | NfcVSendFlagsHighRate;
    if(response_length > 0) {
        flags |= NfcVSendFlagsEof;
    }

    nfcv_emu_send(
        tx_rx,
        nfcv_data,
        response,
        response_length,
        flags,
        nfcv_data->eof_timestamp + NFCV_FDT_FC(4000)); // 3650 is ~254uS 4000 is ~283uS
}

void picopass_worker_emulate(PicopassWorker* picopass_worker, bool loclass_mode) {
    furi_hal_nfc_exit_sleep();

    FuriHalNfcTxRxContext tx_rx = {};
    PicopassEmulatorCtx emu_ctx = {
        .state = PicopassEmulatorStateIdle,
        .key_block_num = PICOPASS_SECURE_KD_BLOCK_INDEX,
        .loclass_mode = loclass_mode,
        .loclass_got_std_key = false,
        .loclass_writer = NULL,
    };
    FuriHalNfcDevData nfc_data = {
        .uid_len = RFAL_PICOPASS_UID_LEN,
    };
    NfcVData* nfcv_data = malloc(sizeof(NfcVData));
    nfcv_data->block_size = RFAL_PICOPASS_BLOCK_LEN;
    nfcv_data->emu_protocol_ctx = &emu_ctx;
    nfcv_data->emu_protocol_handler = &picopass_emu_handle_packet;

    PicopassDeviceData* dev_data = picopass_worker->dev_data;
    PicopassBlock* blocks = dev_data->AA1;

    if(loclass_mode) {
        // Setup blocks for loclass attack
        emu_ctx.key_block_num = 0;
        loclass_update_csn(&nfc_data, nfcv_data, &emu_ctx);

        uint8_t conf[8] = {0x12, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0xFF, 0x3C};
        picopass_emu_write_blocks(nfcv_data, conf, PICOPASS_CONFIG_BLOCK_INDEX, 1);

        uint8_t epurse[8] = {0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        picopass_emu_write_blocks(nfcv_data, epurse, PICOPASS_SECURE_EPURSE_BLOCK_INDEX, 1);

        uint8_t aia[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        picopass_emu_write_blocks(nfcv_data, aia, PICOPASS_SECURE_AIA_BLOCK_INDEX, 1);

        emu_ctx.loclass_writer = loclass_writer_alloc();
        loclass_writer_write_start_stop(emu_ctx.loclass_writer, true);
    } else {
        memcpy(nfc_data.uid, blocks[PICOPASS_CSN_BLOCK_INDEX].data, RFAL_PICOPASS_BLOCK_LEN);
        memcpy(nfcv_data->data, blocks, sizeof(dev_data->AA1));
        picopass_init_cipher_state(nfcv_data, &emu_ctx);
    }

    uint8_t last_loclass_csn_num = 0;
    bool loclass_got_std_key = false;

    nfcv_emu_init(&nfc_data, nfcv_data);
    while(picopass_worker->state == PicopassWorkerStateEmulate ||
          picopass_worker->state == PicopassWorkerStateLoclass) {
        if(nfcv_emu_loop(&tx_rx, &nfc_data, nfcv_data, 500)) {
            if(picopass_worker->callback) {
                if((loclass_mode) && (last_loclass_csn_num != emu_ctx.key_block_num)) {
                    last_loclass_csn_num = emu_ctx.key_block_num;
                    picopass_worker->callback(
                        PicopassWorkerEventLoclassGotMac, picopass_worker->context);
                } else if((loclass_mode) && !loclass_got_std_key && emu_ctx.loclass_got_std_key) {
                    loclass_got_std_key = true;
                    picopass_worker->callback(
                        PicopassWorkerEventLoclassGotStandardKey, picopass_worker->context);
                } else {
                    picopass_worker->callback(
                        PicopassWorkerEventSuccess, picopass_worker->context);
                }
            }
        }
    }

    if(emu_ctx.loclass_writer) {
        loclass_writer_write_start_stop(emu_ctx.loclass_writer, false);
        loclass_writer_free(emu_ctx.loclass_writer);
    }

    nfcv_emu_deinit(nfcv_data);
    free(nfcv_data);
}
