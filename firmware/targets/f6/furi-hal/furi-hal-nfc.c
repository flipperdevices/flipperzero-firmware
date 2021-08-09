#include "furi-hal-nfc.h"
#include <st25r3916.h>
#include <stm32wbxx_ll_lptim.h>
#include <furi-hal-interrupt.h>

#define FURI_HAL_NFC_FLAG_INT (0x01)
#define FURI_HAL_NFC_FLAG_TIMER (0x02)
#define FURI_HAL_NFC_FLAG_STOP (0x04)
#define FURI_HAL_NFC_RFAL_FLAG_ALL (FURI_HAL_NFC_FLAG_INT | FURI_HAL_NFC_FLAG_TIMER | FURI_HAL_NFC_FLAG_STOP)

static const uint32_t clocks_in_ms = 64 * 1000;

static volatile osThreadId_t furi_hal_nfc_thread_id = NULL;
static uint32_t timer_ms = 0;

static void furi_hal_nfc_rfal_int_callback() {
    // furi_assert(furi_hal_nfc_thread_id);
    if(furi_hal_nfc_thread_id) {
        osThreadFlagsSet(furi_hal_nfc_thread_id, FURI_HAL_NFC_FLAG_INT);
    }
}

static void furi_hal_nfc_timer_ISR() {
    if(LL_LPTIM_IsActiveFlag_ARRM(LPTIM1)) {
        LL_LPTIM_ClearFLAG_ARRM(LPTIM1);
        timer_ms--;
        if(!timer_ms) {
            LL_LPTIM_DeInit(LPTIM1);
            // furi_assert(furi_hal_nfc_thread_id);
            if(furi_hal_nfc_thread_id) {
                osThreadFlagsSet(furi_hal_nfc_thread_id, FURI_HAL_NFC_FLAG_TIMER);
            }
        } else {
            LL_LPTIM_SetAutoReload(LPTIM1, 1000);
            LL_LPTIM_StartCounter(LPTIM1, LL_LPTIM_OPERATING_MODE_ONESHOT);
        }
    }
}

void furi_hal_nfc_stop() {
    furi_assert(furi_hal_nfc_thread_id);
    osThreadFlagsSet(furi_hal_nfc_thread_id, FURI_HAL_NFC_FLAG_STOP);
}

static void furi_hal_nfc_timer_init() {
    // Configure clock source
    LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM1_CLKSOURCE_PCLK1);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPTIM1);
    // Configure interrupt
    furi_hal_interrupt_set_lptimer_isr(LPTIM1, furi_hal_nfc_timer_ISR);
    NVIC_SetPriority(LPTIM1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 4, 0));
    // NVIC_SetPriority(LPTIM1_IRQn, 4);

    NVIC_EnableIRQ(LPTIM1_IRQn);
}

void furi_hal_nfc_timer_start(uint32_t time_ms) {
    // Setup LPTIM peripheral
    LL_LPTIM_SetClockSource(LPTIM1, LL_LPTIM_CLK_SOURCE_INTERNAL);
    LL_LPTIM_SetCounterMode(LPTIM1, LL_LPTIM_COUNTER_MODE_INTERNAL);
    LL_LPTIM_SetUpdateMode(LPTIM1, LL_LPTIM_UPDATE_MODE_IMMEDIATE);
    LL_LPTIM_SetPrescaler(LPTIM1, LL_LPTIM_PRESCALER_DIV64);
    LL_LPTIM_EnableIT_ARRM(LPTIM1);
    LL_LPTIM_Enable(LPTIM1);
    while(!LL_LPTIM_IsEnabled(LPTIM1));
    // Init static timer variable in ms
    timer_ms = time_ms;
    // Setup ARR register to generate update event each ms
    LL_LPTIM_SetAutoReload(LPTIM1, 1000);
    LL_LPTIM_StartCounter(LPTIM1, LL_LPTIM_OPERATING_MODE_ONESHOT);
}


bool furi_hal_nfc_timer_expired() {
    // return LL_LPTIM_IsEnabled(LPTIM1);
    return timer_ms == 0;
}

void furi_hal_nfc_init() {
    furi_hal_nfc_timer_init();
    rfalSetUpperLayerCallback(furi_hal_nfc_rfal_int_callback);
    ReturnCode ret = rfalNfcInitialize();
    if(ret == ERR_NONE) {
        furi_hal_nfc_start_sleep();
        FURI_LOG_I("FuriHalNfc", "Init OK");
    } else {
        FURI_LOG_W("FuriHalNfc", "Initialization failed, RFAL returned: %d", ret);
    }
}

bool furi_hal_nfc_is_busy() {
    return rfalNfcGetState() != RFAL_NFC_STATE_IDLE;
}

void furi_hal_nfc_field_on() {
    furi_hal_nfc_exit_sleep();
    st25r3916TxRxOn();
}

void furi_hal_nfc_field_off() {
    st25r3916TxRxOff();
    furi_hal_nfc_start_sleep();
}

void furi_hal_nfc_start_sleep() {
    rfalLowPowerModeStart();
}

void furi_hal_nfc_exit_sleep() {
    rfalLowPowerModeStop();
    // TODO check do we need this
    rfalSetUpperLayerCallback(furi_hal_nfc_rfal_int_callback);
}

bool furi_hal_nfc_detect(rfalNfcDevice **dev_list, uint8_t* dev_cnt, uint32_t timeout, bool deactivate) {
    furi_assert(dev_list);
    furi_assert(dev_cnt);

    furi_hal_nfc_thread_id = osThreadGetId();

    rfalLowPowerModeStop();
    rfalNfcState state = rfalNfcGetState();
    rfalNfcState state_prev = rfalNfcGetState();
    bool activated = false;
    if(state == RFAL_NFC_STATE_NOTINIT) {
        rfalNfcInitialize();
    }
    rfalNfcDiscoverParam params;
    params.compMode = RFAL_COMPLIANCE_MODE_EMV;
    params.techs2Find = RFAL_NFC_POLL_TECH_A | RFAL_NFC_POLL_TECH_B | RFAL_NFC_POLL_TECH_F |
                        RFAL_NFC_POLL_TECH_V | RFAL_NFC_POLL_TECH_AP2P | RFAL_NFC_POLL_TECH_ST25TB;
    params.totalDuration = 1000;
    params.devLimit = 3;
    params.wakeupEnabled = false;
    params.wakeupConfigDefault = true;
    params.nfcfBR = RFAL_BR_212;
    params.ap2pBR = RFAL_BR_424;
    params.maxBR = RFAL_BR_KEEP;
    params.GBLen = RFAL_NFCDEP_GB_MAX_LEN;
    params.notifyCb = NULL;

    // uint32_t start = DWT->CYCCNT;
    rfalNfcDiscover(&params);
    while(true) {
        rfalNfcWorker();
        state = rfalNfcGetState();
        FURI_LOG_D("HAL NFC", "Current state %d", state);
        while(state_prev != state) {
            state_prev = state;
            rfalNfcWorker();
            state = rfalNfcGetState();
            FURI_LOG_D("HAL NFC", "Current state from loop %d", state);
        }
        if(state == RFAL_NFC_STATE_POLL_SELECT) {
            rfalNfcSelect(0);
        } else if(state == RFAL_NFC_STATE_ACTIVATED) {
            activated = true;
            break;
        }
        uint32_t flag = osThreadFlagsWait(FURI_HAL_NFC_RFAL_FLAG_ALL, osFlagsWaitAny, osWaitForever);
        if(flag == FURI_HAL_NFC_FLAG_STOP) {
            break;
        }
    }

    rfalNfcGetDevicesFound(dev_list, dev_cnt);
    if(deactivate) {
        rfalNfcDeactivate(false);
        rfalLowPowerModeStart();
    }
    return activated;
}

bool furi_hal_nfc_listen(uint8_t* uid, uint8_t uid_len, uint8_t* atqa, uint8_t sak, uint32_t timeout) {
    rfalNfcState state = rfalNfcGetState();

    furi_hal_nfc_thread_id = osThreadGetId();

    if(state == RFAL_NFC_STATE_NOTINIT) {
        rfalNfcInitialize();
    } else if(state >= RFAL_NFC_STATE_ACTIVATED) {
        rfalNfcDeactivate(false);
    }
    rfalLowPowerModeStop();
    rfalNfcDiscoverParam params = {
        .compMode = RFAL_COMPLIANCE_MODE_NFC,
        .techs2Find = RFAL_NFC_LISTEN_TECH_A,
        .totalDuration = 1000,
        .devLimit = 1,
        .wakeupEnabled = false,
        .wakeupConfigDefault = true,
        .nfcfBR = RFAL_BR_212,
        .ap2pBR = RFAL_BR_424,
        .maxBR = RFAL_BR_KEEP,
        .GBLen = RFAL_NFCDEP_GB_MAX_LEN,
        .notifyCb = NULL,
    };
    params.lmConfigPA.nfcidLen = uid_len;
    memcpy(params.lmConfigPA.nfcid, uid, uid_len);
    params.lmConfigPA.SENS_RES[0] = atqa[0];
    params.lmConfigPA.SENS_RES[1] = atqa[1];
    params.lmConfigPA.SEL_RES = sak;
    rfalNfcDiscover(&params);

    uint32_t start = DWT->CYCCNT;
    while(state != RFAL_NFC_STATE_ACTIVATED) {
        rfalNfcWorker();
        state = rfalNfcGetState();
        FURI_LOG_D("HAL NFC", "Current state %d", state);
        if(DWT->CYCCNT - start > timeout * clocks_in_ms) {
            rfalNfcDeactivate(true);
            return false;
        }
        osThreadYield();
    }
    return true;
}

ReturnCode furi_hal_nfc_data_exchange(uint8_t* tx_buff, uint16_t tx_len, uint8_t** rx_buff, uint16_t** rx_len, bool deactivate) {
    furi_assert(rx_buff);
    furi_assert(rx_len);

    ReturnCode ret;
    rfalNfcState state = RFAL_NFC_STATE_ACTIVATED;
    ret = rfalNfcDataExchangeStart(tx_buff, tx_len, rx_buff, rx_len, 0);
    if(ret != ERR_NONE) {
        return ret;
    }
    uint32_t start = DWT->CYCCNT;
    while(state != RFAL_NFC_STATE_DATAEXCHANGE_DONE) {
        rfalNfcWorker();
        state = rfalNfcGetState();
        ret = rfalNfcDataExchangeGetStatus();
        FURI_LOG_D("HAL NFC", "Nfc st: %d Data st: %d", state, ret);
        if(ret > ERR_SLEEP_REQ) {
            return ret;
        }
        if(ret == ERR_BUSY) {
            if(DWT->CYCCNT - start > 1000 * clocks_in_ms) {
                return ERR_TIMEOUT;
            }
            continue;
        } else {
            start = DWT->CYCCNT;
        }
        taskYIELD();
    }
    if(deactivate) {
        rfalNfcDeactivate(false);
        rfalLowPowerModeStart();
    }
    return ERR_NONE;
}

void furi_hal_nfc_deactivate() {
    rfalNfcDeactivate(false);
    rfalLowPowerModeStart();
}
