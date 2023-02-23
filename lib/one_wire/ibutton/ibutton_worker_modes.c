#include "ibutton_worker_i.h"

#include <furi.h>
#include <furi_hal.h>

#include "ibutton_protocols.h"

static void ibutton_worker_mode_idle_start(iButtonWorker* worker);
static void ibutton_worker_mode_idle_tick(iButtonWorker* worker);
static void ibutton_worker_mode_idle_stop(iButtonWorker* worker);

static void ibutton_worker_mode_emulate_start(iButtonWorker* worker);
static void ibutton_worker_mode_emulate_tick(iButtonWorker* worker);
static void ibutton_worker_mode_emulate_stop(iButtonWorker* worker);

static void ibutton_worker_mode_read_start(iButtonWorker* worker);
static void ibutton_worker_mode_read_tick(iButtonWorker* worker);
static void ibutton_worker_mode_read_stop(iButtonWorker* worker);

static void ibutton_worker_mode_write_common_start(iButtonWorker* worker);
static void ibutton_worker_mode_write_blank_tick(iButtonWorker* worker);
static void ibutton_worker_mode_write_copy_tick(iButtonWorker* worker);
static void ibutton_worker_mode_write_common_stop(iButtonWorker* worker);

const iButtonWorkerModeType ibutton_worker_modes[] = {
    {
        .quant = FuriWaitForever,
        .start = ibutton_worker_mode_idle_start,
        .tick = ibutton_worker_mode_idle_tick,
        .stop = ibutton_worker_mode_idle_stop,
    },
    {
        .quant = 100,
        .start = ibutton_worker_mode_read_start,
        .tick = ibutton_worker_mode_read_tick,
        .stop = ibutton_worker_mode_read_stop,
    },
    {
        .quant = 1000,
        .start = ibutton_worker_mode_write_common_start,
        .tick = ibutton_worker_mode_write_blank_tick,
        .stop = ibutton_worker_mode_write_common_stop,
    },
    {
        .quant = 1000,
        .start = ibutton_worker_mode_write_common_start,
        .tick = ibutton_worker_mode_write_copy_tick,
        .stop = ibutton_worker_mode_write_common_stop,
    },
    {
        .quant = 1000,
        .start = ibutton_worker_mode_emulate_start,
        .tick = ibutton_worker_mode_emulate_tick,
        .stop = ibutton_worker_mode_emulate_stop,
    },
};

/*********************** IDLE ***********************/

void ibutton_worker_mode_idle_start(iButtonWorker* worker) {
    UNUSED(worker);
}

void ibutton_worker_mode_idle_tick(iButtonWorker* worker) {
    UNUSED(worker);
}

void ibutton_worker_mode_idle_stop(iButtonWorker* worker) {
    UNUSED(worker);
}

/*********************** READ ***********************/

void ibutton_worker_mode_read_start(iButtonWorker* worker) {
    UNUSED(worker);
    furi_hal_power_enable_otg();
}

void ibutton_worker_mode_read_tick(iButtonWorker* worker) {
    if(ibutton_protocols_read(worker->key)) {
        if(worker->read_cb != NULL) {
            worker->read_cb(worker->cb_ctx);
        }

        ibutton_worker_switch_mode(worker, iButtonWorkerModeIdle);
    }
}

void ibutton_worker_mode_read_stop(iButtonWorker* worker) {
    UNUSED(worker);
    furi_hal_power_disable_otg();
}

/*********************** EMULATE ***********************/
// static void onewire_slave_callback(void* context) {
//     furi_assert(context);
//     iButtonWorker* worker = context;
//     ibutton_worker_notify_emulate(worker);
// }

// static void ibutton_worker_emulate_dallas_start(iButtonWorker* worker) {
//     onewire_slave_set_result_callback(worker->bus, onewire_slave_callback, worker);
//     ibutton_key_emulate_start(worker->key, worker->bus);
// }

// static void ibutton_worker_emulate_dallas_stop(iButtonWorker* worker) {
//     onewire_slave_stop(worker->bus);
// }

// static void ibutton_worker_emulate_timer_cb(void* context) {
//     furi_assert(context);
//     iButtonWorker* worker = context;
//
//     const LevelDuration level_duration =
//         protocol_dict_encoder_yield(worker->protocols, worker->protocol_to_encode);
//
//     const bool level = level_duration_get_level(level_duration);
//
//     furi_hal_ibutton_emulate_set_next(level);
//     furi_hal_ibutton_pin_write(level);
// }

// static void ibutton_worker_emulate_timer_start(iButtonWorker* worker) {
//     furi_assert(worker->key_p);
//     const uint8_t* key_id = ibutton_key_get_data_p(worker->key_p);
//     const uint8_t key_size = ibutton_key_get_max_size();
//
//     switch(ibutton_key_get_type(worker->key_p)) {
//     case iButtonKeyDS1990:
//         return;
//         break;
//     case iButtonKeyCyfral:
//         worker->protocol_to_encode = iButtonProtocolCyfral;
//         break;
//     case iButtonKeyMetakom:
//         worker->protocol_to_encode = iButtonProtocolMetakom;
//         break;
//     }
//
//     protocol_dict_set_data(worker->protocols, worker->protocol_to_encode, key_id, key_size);
//     protocol_dict_encoder_start(worker->protocols, worker->protocol_to_encode);
//
//     furi_hal_ibutton_pin_configure();
//     furi_hal_ibutton_emulate_start(0, ibutton_worker_emulate_timer_cb, worker);
// }

// static void ibutton_worker_emulate_timer_stop(iButtonWorker* worker) {
//     UNUSED(worker);
//     furi_hal_ibutton_emulate_stop();
// }

void ibutton_worker_mode_emulate_start(iButtonWorker* worker) {
    furi_assert(worker->key);

    furi_hal_rfid_pins_reset();
    furi_hal_rfid_pin_pull_pulldown();

    ibutton_protocols_emulate_start(worker->key);
}

void ibutton_worker_mode_emulate_tick(iButtonWorker* worker) {
    UNUSED(worker);
}

void ibutton_worker_mode_emulate_stop(iButtonWorker* worker) {
    furi_assert(worker->key);

    ibutton_protocols_emulate_stop(worker->key);

    furi_hal_rfid_pins_reset();
}

/*********************** WRITE ***********************/

void ibutton_worker_mode_write_common_start(iButtonWorker* worker) { //-V524
    UNUSED(worker);
    furi_hal_power_enable_otg();
}

void ibutton_worker_mode_write_blank_tick(iButtonWorker* worker) {
    furi_assert(worker->key);

    const bool success = ibutton_protocols_write_blank(worker->key);
    // TODO: pass a proper result to the callback
    const iButtonWorkerWriteResult result = success ? iButtonWorkerWriteOK :
                                                      iButtonWorkerWriteNoDetect;
    if(worker->write_cb != NULL) {
        worker->write_cb(worker->cb_ctx, result);
    }
}

void ibutton_worker_mode_write_copy_tick(iButtonWorker* worker) {
    furi_assert(worker->key);

    const bool success = ibutton_protocols_write_copy(worker->key);
    // TODO: pass a proper result to the callback
    const iButtonWorkerWriteResult result = success ? iButtonWorkerWriteOK :
                                                      iButtonWorkerWriteNoDetect;
    if(worker->write_cb != NULL) {
        worker->write_cb(worker->cb_ctx, result);
    }
}

void ibutton_worker_mode_write_common_stop(iButtonWorker* worker) { //-V524
    UNUSED(worker);
    furi_hal_power_disable_otg();
}
