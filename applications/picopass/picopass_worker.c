#include "picopass_worker_i.h"

#include <flipper_format/flipper_format.h>

#define TAG "PicopassWorker"

const uint8_t picopass_iclass_key[] = {0xaf, 0xa7, 0x85, 0xa7, 0xda, 0xb3, 0x33, 0x78};
const uint8_t picopass_factory_key[] = {0x76, 0x65, 0x54, 0x43, 0x32, 0x21, 0x10, 0x00};

static void picopass_worker_enable_field() {
    st25r3916TxRxOn();
    rfalLowPowerModeStop();
    rfalWorker();
}

static ReturnCode picopass_worker_disable_field(ReturnCode rc) {
    st25r3916TxRxOff();
    rfalLowPowerModeStart();
    return rc;
}

/***************************** Picopass Worker API *******************************/

PicopassWorker* picopass_worker_alloc() {
    PicopassWorker* picopass_worker = malloc(sizeof(PicopassWorker));

    // Worker thread attributes
    picopass_worker->thread = furi_thread_alloc();
    furi_thread_set_name(picopass_worker->thread, "PicopassWorker");
    furi_thread_set_stack_size(picopass_worker->thread, 8192);
    furi_thread_set_callback(picopass_worker->thread, picopass_worker_task);
    furi_thread_set_context(picopass_worker->thread, picopass_worker);

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
    if(picopass_worker->state == PicopassWorkerStateBroken ||
       picopass_worker->state == PicopassWorkerStateReady) {
        return;
    }
    picopass_worker_disable_field(ERR_NONE);

    picopass_worker_change_state(picopass_worker, PicopassWorkerStateStop);
    furi_thread_join(picopass_worker->thread);
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

ReturnCode picopass_check_elite_key(PicopassWorker* picopass_worker, uint8_t* key) {
    UNUSED(picopass_worker);
    rfalPicoPassIdentifyRes idRes;
    rfalPicoPassSelectRes selRes;
    rfalPicoPassReadCheckRes rcRes;
    rfalPicoPassCheckRes chkRes;

    ReturnCode err;

    uint8_t div_key[8] = {0};
    uint8_t mac[4] = {0};
    uint8_t ccnr[12] = {0};

    err = picopass_detect_card(0);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "picopass_detect_card error %d", err);
        return err;
    }

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

    loclass_iclass_calc_div_key(selRes.CSN, key, div_key, true);
    loclass_opt_doReaderMAC(ccnr, div_key, mac);

    err = rfalPicoPassPollerCheck(mac, &chkRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerCheck error %d", err);
        return err;
    }

    return ERR_NONE;
}

ReturnCode picopass_try_elite_keys(PicopassWorker* picopass_worker) {
    size_t index = 0;
    uint8_t key[PICOPASS_BLOCK_LEN] = {0};
    ReturnCode err = ERR_PARAM;

    if(!iclass_elite_dict_check_presence(IclassEliteDictTypeFlipper)) {
        FURI_LOG_E(TAG, "Dictionary not found");
        return ERR_PARAM;
    }

    IclassEliteDict* dict = iclass_elite_dict_alloc(IclassEliteDictTypeFlipper);
    if(!dict) {
        FURI_LOG_E(TAG, "Dictionary not allocated");
        return ERR_PARAM;
    }

    FURI_LOG_D(
        TAG, "Start Dictionary attack, Key Count %d", iclass_elite_dict_get_total_keys(dict));
    while(iclass_elite_dict_get_next_key(dict, key)) {
        FURI_LOG_D(
            TAG,
            "Try to auth with key %d %02x%02x%02x%02x%02x%02x%02x%02x",
            index++,
            key[0],
            key[1],
            key[2],
            key[3],
            key[4],
            key[5],
            key[6],
            key[7]);

        err = picopass_check_elite_key(picopass_worker, key);
        if(err == ERR_NONE) {
            break;
        }
    }

    if(dict) {
        iclass_elite_dict_free(dict);
    }

    return err;
}

ReturnCode picopass_read_card(PicopassWorker* picopass_worker, PicopassBlock* AA1) {
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

    loclass_diversifyKey(selRes.CSN, picopass_iclass_key, div_key);
    loclass_opt_doReaderMAC(ccnr, div_key, mac);

    err = rfalPicoPassPollerCheck(mac, &chkRes);
    if(err != ERR_NONE) {
        FURI_LOG_E(TAG, "rfalPicoPassPollerCheck error %d", err);
        err = picopass_try_elite_keys(picopass_worker);
        if(err != ERR_NONE) {
            return err;
        }
    }

    rfalPicoPassReadBlockRes csn;
    err = rfalPicoPassPollerReadBlock(PICOPASS_CSN_BLOCK_INDEX, &csn);
    memcpy(AA1[PICOPASS_CSN_BLOCK_INDEX].data, csn.data, sizeof(csn.data));

    rfalPicoPassReadBlockRes cfg;
    err = rfalPicoPassPollerReadBlock(PICOPASS_CONFIG_BLOCK_INDEX, &cfg);
    memcpy(AA1[PICOPASS_CONFIG_BLOCK_INDEX].data, cfg.data, sizeof(cfg.data));

    size_t app_limit = cfg.data[0] < PICOPASS_MAX_APP_LIMIT ? cfg.data[0] : PICOPASS_MAX_APP_LIMIT;

    for(size_t i = 2; i < app_limit; i++) {
        rfalPicoPassReadBlockRes block;
        err = rfalPicoPassPollerReadBlock(i, &block);
        if(err != ERR_NONE) {
            FURI_LOG_E(TAG, "rfalPicoPassPollerReadBlock error %d", err);
            return err;
        }

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

    loclass_diversifyKey(selRes.CSN, picopass_iclass_key, div_key);
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
        memcpy(data + 1, AA1[i].data, RFAL_PICOPASS_MAX_BLOCK_LEN);
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

int32_t picopass_worker_task(void* context) {
    PicopassWorker* picopass_worker = context;

    picopass_worker_enable_field();
    if(picopass_worker->state == PicopassWorkerStateDetect) {
        picopass_worker_detect(picopass_worker);
    } else if(picopass_worker->state == PicopassWorkerStateWrite) {
        picopass_worker_write(picopass_worker);
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

    PicopassWorkerEvent nextState = PicopassWorkerEventSuccess;

    while(picopass_worker->state == PicopassWorkerStateDetect) {
        if(picopass_detect_card(1000) == ERR_NONE) {
            // Process first found device
            err = picopass_read_card(picopass_worker, AA1);
            if(err != ERR_NONE) {
                FURI_LOG_E(TAG, "picopass_read_card error %d", err);
                nextState = PicopassWorkerEventFail;
            }

            if(nextState == PicopassWorkerEventSuccess) {
                err = picopass_device_parse_credential(AA1, pacs);
            }
            if(err != ERR_NONE) {
                FURI_LOG_E(TAG, "picopass_device_parse_credential error %d", err);
                nextState = PicopassWorkerEventFail;
            }

            if(nextState == PicopassWorkerEventSuccess) {
                err = picopass_device_parse_wiegand(pacs->credential, &pacs->record);
            }
            if(err != ERR_NONE) {
                FURI_LOG_E(TAG, "picopass_device_parse_wiegand error %d", err);
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
