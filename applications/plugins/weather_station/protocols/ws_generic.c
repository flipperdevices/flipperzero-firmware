#include "ws_generic.h"
#include <lib/toolbox/stream/stream.h>
#include <lib/flipper_format/flipper_format_i.h>
#include "../helpers/weather_station_types.h"

#define TAG "WSBlockGeneric"

void ws_block_generic_get_preset_name(const char* preset_name, FuriString* preset_str) {
    const char* preset_name_temp;
    if(!strcmp(preset_name, "AM270")) {
        preset_name_temp = "FuriHalSubGhzPresetOok270Async";
    } else if(!strcmp(preset_name, "AM650")) {
        preset_name_temp = "FuriHalSubGhzPresetOok650Async";
    } else if(!strcmp(preset_name, "FM238")) {
        preset_name_temp = "FuriHalSubGhzPreset2FSKDev238Async";
    } else if(!strcmp(preset_name, "FM476")) {
        preset_name_temp = "FuriHalSubGhzPreset2FSKDev476Async";
    } else {
        preset_name_temp = "FuriHalSubGhzPresetCustom";
    }
    furi_string_set(preset_str, preset_name_temp);
}

SubGhzProtocolError ws_block_generic_serialize(
    WSBlockGeneric* instance,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(instance);
    SubGhzProtocolError res = SubGhzProtocolErrorUnknown;
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    do {
        stream_clean(flipper_format_get_raw_stream(flipper_format));
        if(!flipper_format_write_header_cstr(
               flipper_format, WS_KEY_FILE_TYPE, WS_KEY_FILE_VERSION)) {
            FURI_LOG_E(TAG, "Unable to add header");
            res = SubGhzProtocolErrorHeader;
            break;
        }

        if(!flipper_format_write_uint32(flipper_format, "Frequency", &preset->frequency, 1)) {
            FURI_LOG_E(TAG, "Unable to add Frequency");
            res = SubGhzProtocolErrorFrequency;
            break;
        }

        ws_block_generic_get_preset_name(furi_string_get_cstr(preset->name), temp_str);
        if(!flipper_format_write_string_cstr(
               flipper_format, "Preset", furi_string_get_cstr(temp_str))) {
            FURI_LOG_E(TAG, "Unable to add Preset");
            res = SubGhzProtocolErrorPreset;
            break;
        }
        if(!strcmp(furi_string_get_cstr(temp_str), "FuriHalSubGhzPresetCustom")) {
            if(!flipper_format_write_string_cstr(
                   flipper_format, "Custom_preset_module", "CC1101")) {
                FURI_LOG_E(TAG, "Unable to add Custom_preset_module");
                res = SubGhzProtocolErrorCustomPreset;
                break;
            }
            if(!flipper_format_write_hex(
                   flipper_format, "Custom_preset_data", preset->data, preset->data_size)) {
                FURI_LOG_E(TAG, "Unable to add Custom_preset_data");
                res = SubGhzProtocolErrorCustomPreset;
                break;
            }
        }
        if(!flipper_format_write_string_cstr(flipper_format, "Protocol", instance->protocol_name)) {
            FURI_LOG_E(TAG, "Unable to add Protocol");
            res = SubGhzProtocolErrorProtocolName;
            break;
        }

        uint32_t temp_data = instance->id;
        if(!flipper_format_write_uint32(flipper_format, "Id", &temp_data, 1)) {
            FURI_LOG_E(TAG, "Unable to add Id");
            res = SubGhzProtocolErrorOthers;
            break;
        }

        temp_data = instance->data_count_bit;
        if(!flipper_format_write_uint32(flipper_format, "Bit", &temp_data, 1)) {
            FURI_LOG_E(TAG, "Unable to add Bit");
            res = SubGhzProtocolErrorBit;
            break;
        }

        uint8_t key_data[sizeof(uint64_t)] = {0};
        for(size_t i = 0; i < sizeof(uint64_t); i++) {
            key_data[sizeof(uint64_t) - i - 1] = (instance->data >> (i * 8)) & 0xFF;
        }

        if(!flipper_format_write_hex(flipper_format, "Data", key_data, sizeof(uint64_t))) {
            FURI_LOG_E(TAG, "Unable to add Data");
            res = SubGhzProtocolErrorOthers;
            break;
        }

        temp_data = instance->battery_low;
        if(!flipper_format_write_uint32(flipper_format, "Batt", &temp_data, 1)) {
            FURI_LOG_E(TAG, "Unable to add Battery_low");
            res = SubGhzProtocolErrorOthers;
            break;
        }

        temp_data = instance->humidity;
        if(!flipper_format_write_uint32(flipper_format, "Hum", &temp_data, 1)) {
            FURI_LOG_E(TAG, "Unable to add Humidity");
            res = SubGhzProtocolErrorOthers;
            break;
        }

        //DATE AGE set
        FuriHalRtcDateTime curr_dt;
        furi_hal_rtc_get_datetime(&curr_dt);
        uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);

        temp_data = curr_ts;
        if(!flipper_format_write_uint32(flipper_format, "Ts", &temp_data, 1)) {
            FURI_LOG_E(TAG, "Unable to add timestamp");
            res = SubGhzProtocolErrorOthers;
            break;
        }

        temp_data = instance->channel;
        if(!flipper_format_write_uint32(flipper_format, "Ch", &temp_data, 1)) {
            FURI_LOG_E(TAG, "Unable to add Channel");
            res = SubGhzProtocolErrorOthers;
            break;
        }

        temp_data = instance->btn;
        if(!flipper_format_write_uint32(flipper_format, "Btn", &temp_data, 1)) {
            FURI_LOG_E(TAG, "Unable to add Btn");
            res = SubGhzProtocolErrorOthers;
            break;
        }

        float temp = instance->temp;
        if(!flipper_format_write_float(flipper_format, "Temp", &temp, 1)) {
            FURI_LOG_E(TAG, "Unable to add Temperature");
            res = SubGhzProtocolErrorOthers;
            break;
        }

        res = SubGhzProtocolErrorNoError;
    } while(false);
    furi_string_free(temp_str);
    return res;
}

SubGhzProtocolError
    ws_block_generic_deserialize(WSBlockGeneric* instance, FlipperFormat* flipper_format) {
    furi_assert(instance);
    SubGhzProtocolError res = SubGhzProtocolErrorUnknown;
    uint32_t temp_data = 0;

    do {
        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            res = SubGhzProtocolErrorOthers;
            break;
        }

        if(!flipper_format_read_uint32(flipper_format, "Id", (uint32_t*)&temp_data, 1)) {
            FURI_LOG_E(TAG, "Missing Id");
            res = SubGhzProtocolErrorOthers;
            break;
        }
        instance->id = (uint32_t)temp_data;

        if(!flipper_format_read_uint32(flipper_format, "Bit", (uint32_t*)&temp_data, 1)) {
            FURI_LOG_E(TAG, "Missing Bit");
            res = SubGhzProtocolErrorBit;
            break;
        }
        instance->data_count_bit = (uint8_t)temp_data;

        uint8_t key_data[sizeof(uint64_t)] = {0};
        if(!flipper_format_read_hex(flipper_format, "Data", key_data, sizeof(uint64_t))) {
            FURI_LOG_E(TAG, "Missing Data");
            res = SubGhzProtocolErrorOthers;
            break;
        }

        for(uint8_t i = 0; i < sizeof(uint64_t); i++) {
            instance->data = instance->data << 8 | key_data[i];
        }

        if(!flipper_format_read_uint32(flipper_format, "Batt", (uint32_t*)&temp_data, 1)) {
            FURI_LOG_E(TAG, "Missing Battery_low");
            res = SubGhzProtocolErrorOthers;
            break;
        }
        instance->battery_low = (uint8_t)temp_data;

        if(!flipper_format_read_uint32(flipper_format, "Hum", (uint32_t*)&temp_data, 1)) {
            FURI_LOG_E(TAG, "Missing Humidity");
            res = SubGhzProtocolErrorOthers;
            break;
        }
        instance->humidity = (uint8_t)temp_data;

        if(!flipper_format_read_uint32(flipper_format, "Ts", (uint32_t*)&temp_data, 1)) {
            FURI_LOG_E(TAG, "Missing timestamp");
            res = SubGhzProtocolErrorOthers;
            break;
        }
        instance->timestamp = (uint32_t)temp_data;

        if(!flipper_format_read_uint32(flipper_format, "Ch", (uint32_t*)&temp_data, 1)) {
            FURI_LOG_E(TAG, "Missing Channel");
            res = SubGhzProtocolErrorOthers;
            break;
        }
        instance->channel = (uint8_t)temp_data;

        if(!flipper_format_read_uint32(flipper_format, "Btn", (uint32_t*)&temp_data, 1)) {
            FURI_LOG_E(TAG, "Missing Btn");
            res = SubGhzProtocolErrorOthers;
            break;
        }
        instance->btn = (uint8_t)temp_data;

        float temp;
        if(!flipper_format_read_float(flipper_format, "Temp", (float*)&temp, 1)) {
            FURI_LOG_E(TAG, "Missing Temperature");
            res = SubGhzProtocolErrorOthers;
            break;
        }
        instance->temp = temp;

        res = SubGhzProtocolErrorNoError;
    } while(0);

    return res;
}

SubGhzProtocolError ws_block_generic_deserialize_check_count_bit(
    WSBlockGeneric* instance,
    FlipperFormat* flipper_format,
    uint16_t count_bit) {
    SubGhzProtocolError ret = SubGhzProtocolErrorUnknown;
    do {
        ret = ws_block_generic_deserialize(instance, flipper_format);
        if(ret != SubGhzProtocolErrorNoError) {
            break;
        }
        if(instance->data_count_bit != count_bit) {
            FURI_LOG_E(TAG, "Wrong number of bits in key");
            ret = SubGhzProtocolErrorCountBit;
            break;
        }
    } while(false);
    return ret;
}