#include "si4463_ext_interconnect.h"
#include "si4463_ext.h"
#include "config/si4463_config.h"

#define TAG "SubGhzDeviceSi4463Ext"

static bool subghz_device_si4463_ext_interconnect_is_frequency_valid(uint32_t frequency) {
    bool ret = subghz_device_si4463_ext_is_frequency_valid(frequency);
    if(!ret) {
        furi_crash("SubGhz: Incorrect frequency.");
    }
    return ret;
}

static uint32_t subghz_device_si4463_ext_interconnect_set_frequency(uint32_t frequency) {
    subghz_device_si4463_ext_interconnect_is_frequency_valid(frequency);
    return subghz_device_si4463_ext_set_frequency_and_path(frequency);
}

static bool subghz_device_si4463_ext_interconnect_start_async_tx(void* callback, void* context) {
    return subghz_device_si4463_ext_start_async_tx(
        (SubGhzDeviceSi4463ExtCallback)callback, context);
}

static void subghz_device_si4463_ext_interconnect_start_async_rx(void* callback, void* context) {
    subghz_device_si4463_ext_start_async_rx(
        (SubGhzDeviceSi4463ExtCaptureCallback)callback, context);
}

static void subghz_device_si4463_ext_interconnect_load_preset(
    FuriHalSubGhzPreset preset,
    uint8_t* preset_data) {
    switch(preset) {
    case FuriHalSubGhzPresetOok650Async:
        subghz_device_si4463_ext_load_config(subghz_device_si4463_preset_ook_650khz_async_regs);
        subghz_device_si4463_ext_mod_gpio_for_async(SI446X_MODEM_MOD_TYPE_MOD_TYPE_OOK);
        break;
    case FuriHalSubGhzPresetOok270Async:
        subghz_device_si4463_ext_load_config(subghz_device_si4463_preset_ook_270khz_async_regs);
        subghz_device_si4463_ext_mod_gpio_for_async(SI446X_MODEM_MOD_TYPE_MOD_TYPE_OOK);
        break;
    case FuriHalSubGhzPreset2FSKDev238Async:
        subghz_device_si4463_ext_load_config(
            subghz_device_si4463_preset_2fsk_dev2_38khz_async_regs);
        subghz_device_si4463_ext_mod_gpio_for_async(SI446X_MODEM_MOD_TYPE_MOD_TYPE_2FSK);
        break;
    case FuriHalSubGhzPreset2FSKDev476Async:
        subghz_device_si4463_ext_load_config(
            subghz_device_si4463_preset_2fsk_dev47_6khz_async_regs);
        subghz_device_si4463_ext_mod_gpio_for_async(SI446X_MODEM_MOD_TYPE_MOD_TYPE_2FSK);
        break;
        // case FuriHalSubGhzPresetMSK99_97KbAsync:
        //     subghz_device_si4463_ext_load_custom_preset(
        //         subghz_device_si4463_preset_msk_99_97kb_async_regs);
        //     break;
        // case FuriHalSubGhzPresetGFSK9_99KbAsync:
        //     subghz_device_si4463_ext_load_custom_preset(
        //         subghz_device_si4463_preset_gfsk_9_99kb_async_regs);
        //     break;

    default:
        //furi_crash("Si4463. need to implement subghz_device_si4463_ext_mod_gpio_for_async");
        //subghz_device_si4463_ext_load_config(preset_data);
        UNUSED(preset_data);
        subghz_device_si4463_ext_load_config(subghz_device_si4463_preset_ook_650khz_async_regs);
        subghz_device_si4463_ext_mod_gpio_for_async(SI446X_MODEM_MOD_TYPE_MOD_TYPE_OOK);
    }
    subghz_device_si4463_set_pa(SI446X_SET_MAX_PA);
}

static bool subghz_device_si4463_ext_io_control(uint32_t io_control_code, void* in_out_data) {
    switch(io_control_code) {
    case SubGhzDeviceIOCTL_SI4463GetProperties:;
        SubGhzDeviceIOCTL_SI4463GetPropertiesData* get_data = in_out_data;
        furi_assert(get_data->size <= sizeof(get_data->data));
        return subghz_device_si4463_ext_get_properties(
            get_data->prop, get_data->data, get_data->size);
        break;
    case SubGhzDeviceIOCTL_SI4463SetProperties:;
        SubGhzDeviceIOCTL_SI4463SetPropertiesData* set_data = in_out_data;
        furi_assert(set_data->size <= sizeof(set_data->data));
        return subghz_device_si4463_ext_set_properties(
            set_data->prop, set_data->data, set_data->size);
        break;
    case SubGhzDeviceIOCTL_SI4463SetPath:;
        SubGhzDeviceIOCTL_SI4463SetPathData* set_path = in_out_data;
        if(set_path->path == SubGhzDeviceIOCTL_Si4463ExtPath433) {
            subghz_device_si4463_ext_set_path(SubGhzDeviceSi4463ExtPath433);
        } else if(set_path->path == SubGhzDeviceIOCTL_Si4463ExtPath315) {
            subghz_device_si4463_ext_set_path(SubGhzDeviceSi4463ExtPath315);
        } else if(set_path->path == SubGhzDeviceIOCTL_Si4463ExtPath868) {
            subghz_device_si4463_ext_set_path(SubGhzDeviceSi4463ExtPath868);
        } else if(set_path->path == SubGhzDeviceIOCTL_Si4463ExtPathIsolate) {
            subghz_device_si4463_ext_set_path(SubGhzDeviceSi4463ExtPathIsolate);
        } else {
            furi_crash("Si4464_ext. unknown path type");
        }
        return true;
        break;
    default:
        return false;
    }
}

const SubGhzDeviceInterconnect subghz_device_si4463_ext_interconnect = {
    .begin = subghz_device_si4463_ext_alloc,
    .end = subghz_device_si4463_ext_free,
    .is_connect = subghz_device_si4463_ext_is_connect,
    .reset = subghz_device_si4463_ext_reset,
    .sleep = subghz_device_si4463_ext_sleep,
    .idle = subghz_device_si4463_ext_idle,
    .load_preset = subghz_device_si4463_ext_interconnect_load_preset,
    .set_frequency = subghz_device_si4463_ext_interconnect_set_frequency,
    .is_frequency_valid = subghz_device_si4463_ext_is_frequency_valid,
    .set_async_mirror_pin = subghz_device_si4463_ext_set_async_mirror_pin,
    .get_data_gpio = subghz_device_si4463_ext_get_data_gpio,

    .set_tx = subghz_device_si4463_ext_tx,
    .flush_tx = subghz_device_si4463_ext_flush_tx,
    .start_async_tx = subghz_device_si4463_ext_interconnect_start_async_tx,
    .is_async_complete_tx = subghz_device_si4463_ext_is_async_tx_complete,
    .stop_async_tx = subghz_device_si4463_ext_stop_async_tx,

    .set_rx = subghz_device_si4463_ext_rx,
    .flush_rx = subghz_device_si4463_ext_flush_rx,
    .start_async_rx = subghz_device_si4463_ext_interconnect_start_async_rx,
    .stop_async_rx = subghz_device_si4463_ext_stop_async_rx,

    .get_rssi = subghz_device_si4463_ext_get_rssi,
    .get_lqi = subghz_device_si4463_ext_get_lqi,

    .rx_pipe_not_empty = subghz_device_si4463_ext_rx_pipe_not_empty,
    .is_rx_data_crc_valid = subghz_device_si4463_ext_is_rx_data_crc_valid,
    .read_packet = subghz_device_si4463_ext_read_packet,
    .write_packet = subghz_device_si4463_ext_write_packet,

    .device_io_control = subghz_device_si4463_ext_io_control,
};

const SubGhzDevice subghz_device_si4463_ext = {
    .name = SUBGHZ_DEVICE_SI4463_EXT_NAME,
    .interconnect = &subghz_device_si4463_ext_interconnect,
};

static const FlipperAppPluginDescriptor subghz_device_si4463_ext_descriptor = {
    .appid = SUBGHZ_RADIO_DEVICE_PLUGIN_APP_ID,
    .ep_api_version = SUBGHZ_RADIO_DEVICE_PLUGIN_API_VERSION,
    .entry_point = &subghz_device_si4463_ext,
};

const FlipperAppPluginDescriptor* subghz_device_si4463_ext_ep() {
    return &subghz_device_si4463_ext_descriptor;
}