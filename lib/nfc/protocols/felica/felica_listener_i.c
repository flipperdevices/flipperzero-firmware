#include "felica_listener_i.h"

#include <nfc/helpers/felica_crc.h>

#define TAG "FelicaListener"

#define FELICA_CHECK_MC_VALUE

static uint32_t felica_wcnt_get_max_value(const FelicaData* data) {
    const uint8_t mc = data->data.fs.mc.data[2];

    if(mc != 0xFF && mc != 0x00) {
        furi_crash("Reserved value is forbidden");
    }
    return (mc == 0xFF) ? 0x00FFFFFF : 0x00FFFE00;
}

void felica_wcnt_increment(FelicaData* data) {
    const uint32_t wcnt_max = felica_wcnt_get_max_value(data);
    uint32_t* wcnt_ptr = (uint32_t*)data->data.fs.wcnt.data;
    if(*wcnt_ptr < wcnt_max) {
        *wcnt_ptr += 1;
    }
}

bool felica_wcnt_check_warning_boundary(const FelicaData* data) {
    const uint8_t mc = data->data.fs.mc.data[2];
    const uint32_t* wcnt_ptr = (uint32_t*)data->data.fs.wcnt.data;
    bool res = false;
    if(mc == 0x00) {
        if(*wcnt_ptr > 0x00001027 && *wcnt_ptr < 0x00FFFDFF) {
            res = true;
        }
    }

    return res;
    //return ((mc == 00) && ((*wcnt_ptr > 0x00001027) && (*wcnt_ptr < 0x00FFFDFF)));
}

bool felica_wcnt_check_error_boundary(const FelicaData* data) {
    const uint32_t wcnt_max = felica_wcnt_get_max_value(data);
    const uint32_t* wcnt_ptr = (const uint32_t*)data->data.fs.wcnt.data;

    return *wcnt_ptr != wcnt_max;
}

void felica_wcnt_post_process(FelicaData* data) {
    uint32_t* wcnt_ptr = (uint32_t*)data->data.fs.wcnt.data;

    if((data->data.fs.mc.data[2] == 0x00) && (*wcnt_ptr > 0x00FFFE00)) {
        *wcnt_ptr = 0;
    }
}
