#include <furi.h>
#include <furi_hal.h>

#include "ibutton_worker_i.h"

#include "protocols/ibutton_protocols.h"

void ibutton_worker_mode_idle_start(iButtonWorker* worker);
void ibutton_worker_mode_idle_tick(iButtonWorker* worker);
void ibutton_worker_mode_idle_stop(iButtonWorker* worker);

void ibutton_worker_mode_emulate_start(iButtonWorker* worker);
void ibutton_worker_mode_emulate_tick(iButtonWorker* worker);
void ibutton_worker_mode_emulate_stop(iButtonWorker* worker);

void ibutton_worker_mode_read_start(iButtonWorker* worker);
void ibutton_worker_mode_read_tick(iButtonWorker* worker);
void ibutton_worker_mode_read_stop(iButtonWorker* worker);

void ibutton_worker_mode_write_start(iButtonWorker* worker);
void ibutton_worker_mode_write_tick(iButtonWorker* worker);
void ibutton_worker_mode_write_stop(iButtonWorker* worker);

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
        .start = ibutton_worker_mode_write_start,
        .tick = ibutton_worker_mode_write_tick,
        .stop = ibutton_worker_mode_write_stop,
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

typedef struct {
    uint32_t last_dwt_value;
    FuriStreamBuffer* stream;
} iButtonReadContext;

void ibutton_worker_comparator_callback(bool level, void* context) {
    iButtonReadContext* read_context = context;

    uint32_t current_dwt_value = DWT->CYCCNT;

    LevelDuration data =
        level_duration_make(level, current_dwt_value - read_context->last_dwt_value);
    furi_stream_buffer_send(read_context->stream, &data, sizeof(LevelDuration), 0);

    read_context->last_dwt_value = current_dwt_value;
}

// bool ibutton_worker_read_comparator(iButtonWorker* worker) {
//     bool result = false;
//
//     protocol_dict_decoders_start(worker->protocols);
//
//     furi_hal_rfid_pins_reset();
//     // pulldown pull pin, we sense the signal through the analog part of the RFID schematic
//     furi_hal_rfid_pin_pull_pulldown();
//
//     iButtonReadContext read_context = {
//         .last_dwt_value = DWT->CYCCNT,
//         .stream = furi_stream_buffer_alloc(sizeof(LevelDuration) * 512, 1),
//     };
//
//     furi_hal_rfid_comp_set_callback(ibutton_worker_comparator_callback, &read_context);
//     furi_hal_rfid_comp_start();
//
//     uint32_t tick_start = furi_get_tick();
//     while(true) {
//         LevelDuration level;
//         size_t ret =
//             furi_stream_buffer_receive(read_context.stream, &level, sizeof(LevelDuration), 100);
//
//         if((furi_get_tick() - tick_start) > 100) {
//             break;
//         }
//
//         if(ret > 0) {
//             ProtocolId decoded_index = protocol_dict_decoders_feed(
//                 worker->protocols,
//                 level_duration_get_level(level),
//                 level_duration_get_duration(level));
//
//             if(decoded_index == PROTOCOL_NO) continue;
//
//             protocol_dict_get_data(
//                 worker->protocols, decoded_index, worker->key_data, ibutton_key_get_max_size());
//
//             switch(decoded_index) {
//             case iButtonProtocolCyfral:
//                 furi_check(worker->key_p != NULL);
//                 ibutton_key_set_type(worker->key_p, iButtonKeyCyfral);
//                 ibutton_key_set_data(worker->key_p, worker->key_data, ibutton_key_get_max_size());
//                 result = true;
//                 break;
//             case iButtonProtocolMetakom:
//                 furi_check(worker->key_p != NULL);
//                 ibutton_key_set_type(worker->key_p, iButtonKeyMetakom);
//                 ibutton_key_set_data(worker->key_p, worker->key_data, ibutton_key_get_max_size());
//                 result = true;
//                 break;
//             default:
//                 break;
//             }
//         }
//     }
//
//     furi_hal_rfid_comp_stop();
//     furi_hal_rfid_comp_set_callback(NULL, NULL);
//     furi_hal_rfid_pins_reset();
//
//     furi_stream_buffer_free(read_context.stream);
//
//     return result;
// }

bool ibutton_worker_read_dallas(iButtonWorker* worker) {
    bool result = false;

    uint8_t *rom_data = NULL;
    void *user_data = NULL;

    onewire_host_start(worker->host);
    furi_delay_ms(100);

    FURI_CRITICAL_ENTER();

    do {
        if(!onewire_host_search(worker->host, worker->rom_data, OneWireHostSearchModeNormal)) {
            break;
        }

        if(!onewire_host_reset(worker->host)) {
            break;
        }

        const uint8_t family_code = worker->rom_data[0];
        const size_t protocol_index = ibutton_protocols_get_index_by_family_code(family_code);

        if(protocol_index == (size_t)iButtonProtocolMax) {
            break;
        }

        rom_data = malloc(ibutton_protocols_get_rom_size(protocol_index));

        if(!ibutton_protocols_read_rom(worker->host, rom_data, protocol_index)) {
            break;
        }

        //TODO: check if the ROM data matches

        const size_t user_data_size = ibutton_protocols_get_user_data_size(protocol_index);
        if(user_data_size > 0) {
            user_data = malloc(user_data_size);
            if(!ibutton_protocols_read_user_data(worker->host, user_data, protocol_index)) {
                break;
            }
        }

        //TODO: set key data

        result = true;
    } while(false);

    if(rom_data) {
        free(rom_data);
    }

    if(user_data) {
        free(user_data);
    }

    onewire_host_reset_search(worker->host);
    onewire_host_stop(worker->host);

    FURI_CRITICAL_EXIT();

    return result;
}

void ibutton_worker_mode_read_start(iButtonWorker* worker) {
    UNUSED(worker);
    furi_hal_power_enable_otg();
}

void ibutton_worker_mode_read_tick(iButtonWorker* worker) {
    bool valid = false;
    if(ibutton_worker_read_dallas(worker)) {
        valid = true;
    // } else if(ibutton_worker_read_comparator(worker)) {
    //     valid = true;
    }

    if(valid) {
        if(worker->read_cb != NULL) {
            worker->read_cb(worker->cb_ctx);
        }

        ibutton_worker_switch_mode(worker, iButtonWorkerIdle);
    }
}

void ibutton_worker_mode_read_stop(iButtonWorker* worker) {
    UNUSED(worker);
    furi_hal_power_disable_otg();
}

/*********************** EMULATE ***********************/
static void onewire_slave_callback(void* context) {
    furi_assert(context);
    iButtonWorker* worker = context;
    ibutton_worker_notify_emulate(worker);
}

void ibutton_worker_emulate_dallas_start(iButtonWorker* worker) {
    // uint8_t* device_id = onewire_device_get_id_p(worker->device);
    // const uint8_t* key_id = ibutton_key_get_data_p(worker->key_p);
    // const uint8_t key_size = ibutton_key_get_max_size();
    // memcpy(device_id, key_id, key_size);

    onewire_slave_attach(worker->slave, worker->device);
    onewire_slave_set_result_callback(worker->slave, onewire_slave_callback, worker);
    onewire_slave_start(worker->slave);
}

void ibutton_worker_emulate_dallas_stop(iButtonWorker* worker) {
    onewire_slave_stop(worker->slave);
    onewire_slave_detach(worker->slave);
}

// void ibutton_worker_emulate_timer_cb(void* context) {
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

// void ibutton_worker_emulate_timer_start(iButtonWorker* worker) {
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

void ibutton_worker_emulate_timer_stop(iButtonWorker* worker) {
    UNUSED(worker);
    furi_hal_ibutton_emulate_stop();
}

void ibutton_worker_mode_emulate_start(iButtonWorker* worker) {
    furi_assert(worker->key_p);

    furi_hal_rfid_pins_reset();
    furi_hal_rfid_pin_pull_pulldown();

    // switch(ibutton_key_get_type(worker->key_p)) {
    // case iButtonKeyDS1990:
        ibutton_worker_emulate_dallas_start(worker);
        // break;
    // case iButtonKeyCyfral:
    // case iButtonKeyMetakom:
    //     ibutton_worker_emulate_timer_start(worker);
    //     break;
    // }
}

void ibutton_worker_mode_emulate_tick(iButtonWorker* worker) {
    UNUSED(worker);
}

void ibutton_worker_mode_emulate_stop(iButtonWorker* worker) {
    furi_assert(worker->key_p);

    furi_hal_rfid_pins_reset();

    // switch(ibutton_key_get_type(worker->key_p)) {
    // case iButtonKeyDS1990:
    //     ibutton_worker_emulate_dallas_stop(worker);
    //     break;
    // case iButtonKeyCyfral:
    // case iButtonKeyMetakom:
    //     ibutton_worker_emulate_timer_stop(worker);
    //     break;
    // }
}

/*********************** WRITE ***********************/

void ibutton_worker_mode_write_start(iButtonWorker* worker) {
    furi_hal_power_enable_otg();
    onewire_host_start(worker->host);
}

void ibutton_worker_mode_write_tick(iButtonWorker* worker) {
    furi_check(worker->key_p != NULL);
    iButtonWriterResult writer_result = ibutton_writer_write(worker->writer, worker->key_p);
    iButtonWorkerWriteResult result;
    switch(writer_result) {
    case iButtonWriterOK:
        result = iButtonWorkerWriteOK;
        break;
    case iButtonWriterSameKey:
        result = iButtonWorkerWriteSameKey;
        break;
    case iButtonWriterNoDetect:
        result = iButtonWorkerWriteNoDetect;
        break;
    case iButtonWriterCannotWrite:
        result = iButtonWorkerWriteCannotWrite;
        break;
    default:
        result = iButtonWorkerWriteNoDetect;
        break;
    }

    if(worker->write_cb != NULL) {
        worker->write_cb(worker->cb_ctx, result);
    }
}

void ibutton_worker_mode_write_stop(iButtonWorker* worker) {
    furi_hal_power_disable_otg();
    onewire_host_stop(worker->host);
}
