#include "data_raw.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"
#include <lib/toolbox/float_tools.h>

#define TAG "SubGhzProtocolDataRAW"

#define DATA_RAW_BUF_RAW_SIZE 2048
#define DATA_RAW_BUF_DATA_SIZE 1024
#define DATA_RAW_THRESHOLD_RSSI -85.0f
#define DATA_RAW_SEARCH_CLASSES 10
#define DATA_RAW_TE_MIN_COUNT 40
#define DATA_RAW_BUF_DATA_COUNT 128
#define DATA_RAW_MAX_FOUND_GAP_COUNT 10

static const SubGhzBlockConst subghz_protocol_data_raw_const = {
    .te_short = 30,
    .te_long = 65000,
    .te_delta = 0,
    .min_count_bit_for_found = 0,
};

typedef enum {
    DataRAWDecoderStepReset = 0,
    DataRAWDecoderStepWrite,
    DataRAWDecoderStepBufFull,
    // DataRAWDecoderStepCheckDuration,
} DataRAWDecoderStep;

struct SubGhzProtocolDecoderDataRAW {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;
    int32_t* data_raw;
    uint8_t* data;
    size_t data_raw_ind;
    uint32_t te;
};

struct SubGhzProtocolEncoderDataRAW {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

const SubGhzProtocolDecoder subghz_protocol_data_raw_decoder = {
    .alloc = subghz_protocol_decoder_data_raw_alloc,
    .free = subghz_protocol_decoder_data_raw_free,

    .feed = subghz_protocol_decoder_data_raw_feed,
    .reset = subghz_protocol_decoder_data_raw_reset,

    .get_hash_data = subghz_protocol_decoder_data_raw_get_hash_data,
    .serialize = subghz_protocol_decoder_data_raw_serialize,
    .deserialize = subghz_protocol_decoder_data_raw_deserialize,
    .get_string = subghz_protocol_decoder_data_raw_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_data_raw_encoder = {
    .alloc = subghz_protocol_encoder_data_raw_alloc,
    .free = subghz_protocol_encoder_data_raw_free,

    .deserialize = subghz_protocol_encoder_data_raw_deserialize,
    .stop = subghz_protocol_encoder_data_raw_stop,
    .yield = subghz_protocol_encoder_data_raw_yield,
};

const SubGhzProtocol subghz_protocol_data_raw = {
    .name = SUBGHZ_PROTOCOL_DATA_RAW_NAME,
    .type = SubGhzProtocolTypeUnknown,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_FM | SubGhzProtocolFlag_Decodable |
            SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Send,

    .decoder = &subghz_protocol_data_raw_decoder,
    .encoder = &subghz_protocol_data_raw_encoder,
};

void* subghz_protocol_encoder_data_raw_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderDataRAW* instance = malloc(sizeof(SubGhzProtocolEncoderDataRAW));

    instance->base.protocol = &subghz_protocol_data_raw;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->encoder.repeat = 10;
    instance->encoder.size_upload = 52; //max 24bit*2 + 2 (start, stop)
    instance->encoder.upload = malloc(instance->encoder.size_upload * sizeof(LevelDuration));
    instance->encoder.is_running = false;
    return instance;
}

void subghz_protocol_encoder_data_raw_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderDataRAW* instance = context;
    free(instance->encoder.upload);
    free(instance);
}

/**
 * Generating an upload from data.
 * @param instance Pointer to a SubGhzProtocolEncoderDataRAW instance
 * @return true On success
 */
static bool subghz_protocol_encoder_data_raw_get_upload(SubGhzProtocolEncoderDataRAW* instance) {
    furi_assert(instance);
    size_t index = 0;
    size_t size_upload = (instance->generic.data_count_bit * 2) + 2;
    if(size_upload > instance->encoder.size_upload) {
        FURI_LOG_E(TAG, "Size upload exceeds allocated encoder buffer.");
        return false;
    } else {
        instance->encoder.size_upload = size_upload;
    }
    //Send header
    instance->encoder.upload[index++] =
        level_duration_make(false, (uint32_t)subghz_protocol_data_raw_const.te_short * 49);
    //Send start bit
    instance->encoder.upload[index++] =
        level_duration_make(true, (uint32_t)subghz_protocol_data_raw_const.te_long);
    //Send key data
    for(uint8_t i = instance->generic.data_count_bit; i > 0; i--) {
        if(bit_read(instance->generic.data, i - 1)) {
            //send bit 1
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)subghz_protocol_data_raw_const.te_long);
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)subghz_protocol_data_raw_const.te_short);
        } else {
            //send bit 0
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)subghz_protocol_data_raw_const.te_short);
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)subghz_protocol_data_raw_const.te_long);
        }
    }
    return true;
}

bool subghz_protocol_encoder_data_raw_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolEncoderDataRAW* instance = context;
    bool res = false;
    do {
        if(!subghz_block_generic_deserialize(&instance->generic, flipper_format)) {
            FURI_LOG_E(TAG, "Deserialize error");
            break;
        }
        if(instance->generic.data_count_bit !=
           subghz_protocol_data_raw_const.min_count_bit_for_found) {
            FURI_LOG_E(TAG, "Wrong number of bits in key");
            break;
        }
        //optional parameter parameter
        flipper_format_read_uint32(
            flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 1);

        if(!subghz_protocol_encoder_data_raw_get_upload(instance)) break;
        instance->encoder.is_running = true;

        res = true;
    } while(false);

    return res;
}

void subghz_protocol_encoder_data_raw_stop(void* context) {
    SubGhzProtocolEncoderDataRAW* instance = context;
    instance->encoder.is_running = false;
}

LevelDuration subghz_protocol_encoder_data_raw_yield(void* context) {
    SubGhzProtocolEncoderDataRAW* instance = context;

    if(instance->encoder.repeat == 0 || !instance->encoder.is_running) {
        instance->encoder.is_running = false;
        return level_duration_reset();
    }

    LevelDuration ret = instance->encoder.upload[instance->encoder.front];

    if(++instance->encoder.front == instance->encoder.size_upload) {
        instance->encoder.repeat--;
        instance->encoder.front = 0;
    }

    return ret;
}

void* subghz_protocol_decoder_data_raw_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolDecoderDataRAW* instance = malloc(sizeof(SubGhzProtocolDecoderDataRAW));
    instance->base.protocol = &subghz_protocol_data_raw;
    instance->generic.protocol_name = instance->base.protocol->name;
    instance->data_raw_ind = 0;
    instance->data_raw = malloc(DATA_RAW_BUF_RAW_SIZE * sizeof(int32_t));
    instance->data = malloc(DATA_RAW_BUF_RAW_SIZE * sizeof(uint8_t));
    return instance;
}

void subghz_protocol_decoder_data_raw_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderDataRAW* instance = context;
    free(instance->data_raw);
    free(instance->data);
    free(instance);
}

void subghz_protocol_decoder_data_raw_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderDataRAW* instance = context;
    UNUSED(instance);
    //instance->decoder.parser_step = DataRAWDecoderStepReset;
    //instance->data_raw_ind = 0;
}

void subghz_protocol_decoder_data_raw_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderDataRAW* instance = context;

    if(instance->decoder.parser_step == DataRAWDecoderStepWrite) {
        if(instance->data_raw_ind == DATA_RAW_BUF_RAW_SIZE) {
            instance->decoder.parser_step = DataRAWDecoderStepBufFull;
        } else {
            instance->data_raw[instance->data_raw_ind++] = (level ? duration : -duration);
        }
    }
}

static uint16_t subghz_protocol_data_raw_get_full_byte(uint16_t bit_count) {
    if(bit_count & 0x7) {
        return (bit_count >> 3) + 1;
    } else {
        return (bit_count >> 3);
    }
}

/** 
 * Analysis of received data
 * @param instance Pointer to a SubGhzProtocolDecoderDataRAW* instance
 */
static bool
    subghz_protocol_data_raw_check_remote_controller(SubGhzProtocolDecoderDataRAW* instance) {
    struct {
        uint32_t data;
        uint16_t count;
    } classes[DATA_RAW_SEARCH_CLASSES];

    struct {
        uint32_t bit_ind;
        uint16_t bit_count;
    } ind_new_data[DATA_RAW_MAX_FOUND_GAP_COUNT];

    size_t ind = 0;
    uint16_t ind_new_data_index = 0;

    memset(classes, 0x00, sizeof(classes));
    memset(classes, 0x00, sizeof(ind_new_data));

    if(instance->data_raw_ind < 512) {
        ind =
            instance->data_raw_ind -
            100; //there is usually garbage at the end of the record, we exclude it from the classification
    } else {
        ind = 512;
    }
    //sort the durations to find the shortest correlated interval
    for(size_t i = 0; i < ind; i++) {
        for(size_t k = 0; k < DATA_RAW_SEARCH_CLASSES; k++) {
            if(classes[k].count == 0) {
                classes[k].data = abs(instance->data_raw[i]);
                classes[k].count++;
                break;
            } else if(
                DURATION_DIFF(abs(instance->data_raw[i]), classes[k].data) <
                (classes[k].data / 5)) { //if the test value does not differ by more than 25%
                classes[k].data = (classes[k].data + abs(instance->data_raw[i])) /
                                  2; //adding and averaging values
                classes[k].count++;
                break;
            }
        }
    }

    // if(classes[DATA_RAW_SEARCH_CLASSES - 1].count != 0) {
    //     //filling the classifier, it means that they received an unclean signal
    //     return false;
    // }

    //looking for the minimum te with an occurrence greater than DATA_RAW_TE_MIN_COUNT
    instance->te = subghz_protocol_data_raw_const.te_long * 2;

    bool te_ok = false;
    uint16_t gap_ind = 0;
    uint32_t gap = 0;
    int data_temp = 0;

    for(size_t k = 0; k < DATA_RAW_SEARCH_CLASSES; k++) {
        if((classes[k].count > DATA_RAW_TE_MIN_COUNT) && (classes[k].data < instance->te)) {
            instance->te = classes[k].data;
            te_ok = true;
        } else if((classes[k].count > 2) && (classes[k].data > gap)) {
            gap = classes[k].data;
        }
    }

    if(!te_ok) {
        //did not find the minimum TE satisfying the condition
        return false;
    }

    if((gap / instance->te) < 10) {
        gap = 0; //check that our signal has a gap greater than 10*TE
    } else {
        //looking for the last occurrence of gap
        ind = instance->data_raw_ind - 1;
        while((ind > 0) && (DURATION_DIFF(abs(instance->data_raw[ind]), gap) > (gap >> 2))) {
            ind--;
        }
        gap_ind = ind;
    }

    // FURI_LOG_I("te", " %ld ", instance->te);
    // FURI_LOG_I("gap", " %ld ", gap);
    // FURI_LOG_I("gap_ind", " %d ", gap_ind);
    // int data = (int)(round((float)(instance->data_raw[gap_ind]) / instance->te));
    // FURI_LOG_W("gap/ind", " %d ", data);

    // if(te_ok) {
    //     for(size_t i = gap_ind; i < instance->data_raw_ind - 1; i++) {
    //         int data_temp = (int)(round((float)(instance->data_raw[i]) / instance->te));
    //         //printf("%d  ", data_temp);

    //         for(size_t i = 0; i < abs(data_temp); i++) {
    //             if(data_temp > 0) {
    //                 subghz_protocol_blocks_set_bit_array(
    //                     true, instance->data, ind++, DATA_RAW_BUF_DATA_SIZE);
    //             } else {
    //                 subghz_protocol_blocks_set_bit_array(
    //                     false, instance->data, ind++, DATA_RAW_BUF_DATA_SIZE);
    //             }
    //         }
    //         //    // printf("%d  \r\n", ind);

    //         //if we consider that there is a gap, then we divide the signal with respect to this gap
    //         if(gap !=0) {
    //             if(DURATION_DIFF(abs(instance->data_raw[i + 1]), gap) < (gap >> 2)) {
    //                 FURI_LOG_E(
    //                     "ind", "  %ld %ld %ld ", (instance->data_raw[ind]), gap, (gap >> 2));
    //                 instance->generic.data_count_bit = ind;
    //                 FURI_LOG_I("ind", " %d ", ind);
    //                 if(ind & 0x7) ind = ((ind >> 3) + 1) << 3;
    //                 FURI_LOG_I("ind", " %d ", ind);
    //             }
    //         }
    //     }
    //printf("--------------- \r\n");

    //if(te_ok) {

    //if we consider that there is a gap, then we divide the signal with respect to this gap
    if(gap != 0) {
        ind = (DATA_RAW_BUF_DATA_SIZE * 8);
        uint16_t bit_count = 0;
        do {
            gap_ind--;
            data_temp = (int)(round((float)(instance->data_raw[gap_ind]) / instance->te));
            //printf("%d  ", data_temp);
            if(data_temp == 0) bit_count++; //there is noise in the package
            for(size_t i = 0; i < abs(data_temp); i++) {
                bit_count++;
                if(ind) {
                    ind--;
                } else {
                    break;
                }
                if(data_temp > 0) {
                    subghz_protocol_blocks_set_bit_array(
                        true, instance->data, ind, DATA_RAW_BUF_DATA_SIZE);
                } else {
                    subghz_protocol_blocks_set_bit_array(
                        false, instance->data, ind, DATA_RAW_BUF_DATA_SIZE);
                }
            }

            if(DURATION_DIFF(abs(instance->data_raw[gap_ind]), gap) < (gap >> 2)) {
                ind_new_data[ind_new_data_index].bit_ind = ind >> 3;
                ind_new_data[ind_new_data_index++].bit_count = bit_count;
                bit_count = 0;

                if(ind_new_data_index == DATA_RAW_MAX_FOUND_GAP_COUNT) break;
                ind &= 0xFFFFFFF8; //jump to the pre whole byte
            }
        } while(gap_ind != 0);
        if(ind_new_data_index != DATA_RAW_MAX_FOUND_GAP_COUNT) {
            ind_new_data[ind_new_data_index].bit_ind = ind >> 3;
            ind_new_data[ind_new_data_index++].bit_count = bit_count;
        }

        //printf("-------%d-------- \r\n", ind);

        //reset the classifier and classify the received data
        memset(classes, 0x00, sizeof(classes));
        for(size_t i = 0; i < ind_new_data_index; i++) {
            for(size_t k = 0; k < DATA_RAW_SEARCH_CLASSES; k++) {
                if(classes[k].count == 0) {
                    classes[k].data = ind_new_data[i].bit_count;
                    classes[k].count++;
                    break;
                } else if(ind_new_data[i].bit_count == classes[k].data) {
                    classes[k].count++;
                    break;
                }
            }
        }

        // for(size_t h = 0; h < DATA_RAW_SEARCH_CLASSES; h++) {
        //     FURI_LOG_I("K", "%d, %ld %d", h, classes[h].data, classes[h].count);
        // }

        //choose the value with the maximum repetition
        data_temp = 0;
        for(size_t i = 0; i < DATA_RAW_SEARCH_CLASSES; i++) {
            if((classes[i].count > 1) && (data_temp < classes[i].count))
                data_temp = classes[i].data;
        }

        //compare data in chunks with the same number of bits
        if(data_temp == 0) return false;
        for(uint16_t i = 0; i < ind_new_data_index - 1; i++) {
            if(ind_new_data[i].bit_count == data_temp) {
                for(uint16_t y = i + 1; y < ind_new_data_index; y++) {
                    if(ind_new_data[y].bit_count == data_temp) {
                        uint16_t byte_count =
                            subghz_protocol_data_raw_get_full_byte(ind_new_data[y].bit_count);
                        if(memcmp(
                               instance->data + ind_new_data[i].bit_ind,
                               instance->data + ind_new_data[y].bit_ind,
                               byte_count) == 0) {
                            memcpy(
                                instance->data,
                                instance->data + ind_new_data[i].bit_ind,
                                byte_count);
                            instance->generic.data_count_bit = ind_new_data[i].bit_count;
                            return true;
                            //i = ind_new_data_index;
                            //break;
                        }
                    }
                }
            }
        }

        // if(ind_new_data_index == 0) return;
        // ind_new_data_index--;
        // for(size_t i = (ind / 8) - 1; i < DATA_RAW_BUF_DATA_SIZE; i++) {
        //     printf("%02x ", instance->data[i]);
        //     if(ind_new_data[ind_new_data_index].bit_ind == i + 1) {
        //         printf(
        //             "\r\n:    %ld , %d   : ",
        //             ind_new_data[ind_new_data_index].bit_ind,
        //             ind_new_data[ind_new_data_index].bit_count);
        //         if(ind_new_data_index != 0) ind_new_data_index--;
        //     }
        // }

        // printf("--------------- \r\n");

        // for(size_t i = 0; i < instance->generic.data_count_bit; i++) {
        //     printf("%02x  ", instance->data[i]);
        // }

        // printf("--------------- \r\n");
        // //}
    }
    return false;
}

void subghz_protocol_decoder_data_input_rssi(SubGhzProtocolDecoderDataRAW* instance, float rssi) {
    furi_assert(instance);
    switch(instance->decoder.parser_step) {
    case DataRAWDecoderStepReset:
        if(rssi > DATA_RAW_THRESHOLD_RSSI) {
            instance->data_raw_ind = 0;
            memset(instance->data_raw, 0x00, DATA_RAW_BUF_RAW_SIZE * sizeof(int32_t));
            memset(instance->data, 0x00, DATA_RAW_BUF_RAW_SIZE * sizeof(uint8_t));
            instance->decoder.parser_step = DataRAWDecoderStepWrite;
        }
        break;
    case DataRAWDecoderStepBufFull:
    case DataRAWDecoderStepWrite:
        if(rssi < DATA_RAW_THRESHOLD_RSSI) {
            // for(size_t i = 0; i < instance->data_raw_ind; i++) {
            //     printf("%ld ", instance->data_raw[i]);
            // }
            // printf("---%d----------- \r\n", instance->data_raw_ind);
            instance->decoder.parser_step = DataRAWDecoderStepReset;
            if(instance->data_raw_ind >= DATA_RAW_BUF_DATA_COUNT) {
                if(subghz_protocol_data_raw_check_remote_controller(instance)) {
                    for(uint16_t i = 0; i < subghz_protocol_data_raw_get_full_byte(
                                                instance->generic.data_count_bit);
                        i++) {
                        printf("%02x  ", instance->data[i]);
                    }

                    printf("--------------- \r\n");

                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
            }
        }

        break;

    default:
        break;
    }
}

uint8_t subghz_protocol_decoder_data_raw_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderDataRAW* instance = context;
    return subghz_protocol_blocks_add_bytes(
        instance->data, subghz_protocol_data_raw_get_full_byte(instance->generic.data_count_bit));
}

bool subghz_protocol_decoder_data_raw_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    SubGhzProtocolDecoderDataRAW* instance = context;
    bool res = subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
    if(res && !flipper_format_write_uint32(flipper_format, "TE", &instance->te, 1)) {
        FURI_LOG_E(TAG, "Unable to add TE");
        res = false;
    }
    if(res && !flipper_format_write_hex(
                  flipper_format,
                  "Data_RAW",
                  instance->data,
                  subghz_protocol_data_raw_get_full_byte(instance->generic.data_count_bit))) {
        FURI_LOG_E(TAG, "Unable to add Data_RAW");
        res = false;
    }
    return res;
}

bool subghz_protocol_decoder_data_raw_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderDataRAW* instance = context;
    bool res = false;
    do {
        if(!subghz_block_generic_deserialize(&instance->generic, flipper_format)) {
            FURI_LOG_E(TAG, "Deserialize error");
            break;
        }
        if(subghz_protocol_data_raw_get_full_byte(instance->generic.data_count_bit) >=
           DATA_RAW_BUF_DATA_SIZE) {
            FURI_LOG_E(TAG, "Wrong number of bits in key");
            break;
        }
        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            break;
        }
        if(!flipper_format_read_uint32(flipper_format, "TE", (uint32_t*)&instance->te, 1)) {
            FURI_LOG_E(TAG, "Missing TE");
            break;
        }
        if(!flipper_format_read_hex(
               flipper_format,
               "Data_RAW",
               instance->data,
               subghz_protocol_data_raw_get_full_byte(instance->generic.data_count_bit))) {
            FURI_LOG_E(TAG, "Missing Data_RAW");
            break;
        }
        res = true;
    } while(false);

    return res;
}

void subghz_protocol_decoder_data_raw_get_string(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderDataRAW* instance = context;
    //subghz_protocol_data_raw_check_remote_controller(&instance->generic);
    furi_string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:",
        instance->generic.protocol_name,
        instance->generic.data_count_bit);

    uint16_t byte_count = subghz_protocol_data_raw_get_full_byte(instance->generic.data_count_bit);
    for(size_t i = 0; (byte_count < 36 ? i < byte_count : i < 36); i++) {
        furi_string_cat_printf(output, "%02X", instance->data[i]);
    }

    furi_string_cat_printf(output, "\r\nTe:%luus\r\n", instance->te);
}
