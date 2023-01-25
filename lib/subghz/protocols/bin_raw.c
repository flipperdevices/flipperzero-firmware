#include "bin_raw.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"
#include <lib/toolbox/float_tools.h>
#include <lib/toolbox/stream/stream.h>
#include <lib/flipper_format/flipper_format_i.h>

#define TAG "SubGhzProtocolBinRAW"

//change very carefully, RAM ends at the most inopportune moment
#define BIN_RAW_BUF_RAW_SIZE 2048
#define BIN_RAW_BUF_DATA_SIZE 512

#define BIN_RAW_THRESHOLD_RSSI -80.0f
#define BIN_RAW_SEARCH_CLASSES 10
#define BIN_RAW_TE_MIN_COUNT 40
#define BIN_RAW_BUF_MIN_DATA_COUNT 128
#define BIN_RAW_MAX_MARKUP_COUNT 20

static const SubGhzBlockConst subghz_protocol_bin_raw_const = {
    .te_short = 30,
    .te_long = 65000,
    .te_delta = 0,
    .min_count_bit_for_found = 0,
};

typedef enum {
    BinRAWDecoderStepReset = 0,
    BinRAWDecoderStepWrite,
    BinRAWDecoderStepBufFull,
} BinRAWDecoderStep;

struct BinRAW_Markup {
    uint16_t byte_bias;
    uint16_t bit_count;
};
typedef struct BinRAW_Markup BinRAW_Markup;

struct SubGhzProtocolDecoderBinRAW {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;
    int32_t* data_raw;
    uint8_t* data;
    BinRAW_Markup data_markup[BIN_RAW_MAX_MARKUP_COUNT];
    size_t data_raw_ind;
    uint32_t te;
};

struct SubGhzProtocolEncoderBinRAW {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;

    uint8_t* data;
    BinRAW_Markup data_markup[BIN_RAW_MAX_MARKUP_COUNT];
    uint32_t te;
};

const SubGhzProtocolDecoder subghz_protocol_bin_raw_decoder = {
    .alloc = subghz_protocol_decoder_bin_raw_alloc,
    .free = subghz_protocol_decoder_bin_raw_free,

    .feed = subghz_protocol_decoder_bin_raw_feed,
    .reset = subghz_protocol_decoder_bin_raw_reset,

    .get_hash_data = subghz_protocol_decoder_bin_raw_get_hash_data,
    .serialize = subghz_protocol_decoder_bin_raw_serialize,
    .deserialize = subghz_protocol_decoder_bin_raw_deserialize,
    .get_string = subghz_protocol_decoder_bin_raw_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_bin_raw_encoder = {
    .alloc = subghz_protocol_encoder_bin_raw_alloc,
    .free = subghz_protocol_encoder_bin_raw_free,

    .deserialize = subghz_protocol_encoder_bin_raw_deserialize,
    .stop = subghz_protocol_encoder_bin_raw_stop,
    .yield = subghz_protocol_encoder_bin_raw_yield,
};

const SubGhzProtocol subghz_protocol_bin_raw = {
    .name = SUBGHZ_PROTOCOL_BIN_RAW_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_FM | SubGhzProtocolFlag_Decodable |
            SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Send,

    .decoder = &subghz_protocol_bin_raw_decoder,
    .encoder = &subghz_protocol_bin_raw_encoder,
};

static uint16_t subghz_protocol_bin_raw_get_full_byte(uint16_t bit_count) {
    if(bit_count & 0x7) {
        return (bit_count >> 3) + 1;
    } else {
        return (bit_count >> 3);
    }
}

void* subghz_protocol_encoder_bin_raw_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderBinRAW* instance = malloc(sizeof(SubGhzProtocolEncoderBinRAW));

    instance->base.protocol = &subghz_protocol_bin_raw;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->encoder.repeat = 10;
    instance->encoder.size_upload = BIN_RAW_BUF_DATA_SIZE * 5;
    //printf("Free heap size alloc: %zu\r\n", memmgr_get_free_heap());
    instance->encoder.upload = malloc(instance->encoder.size_upload * sizeof(LevelDuration));
    instance->data = malloc(instance->encoder.size_upload * sizeof(uint8_t));
    //printf("Free heap size alloc: %zu\r\n", memmgr_get_free_heap());
    memset(instance->data_markup, 0x00, BIN_RAW_MAX_MARKUP_COUNT * sizeof(BinRAW_Markup));
    instance->encoder.is_running = false;
    return instance;
}

void subghz_protocol_encoder_bin_raw_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderBinRAW* instance = context;
    printf("Free heap size: %zu\r\n", memmgr_get_free_heap());
    free(instance->encoder.upload);
    free(instance->data);
    free(instance);
}

/**
 * Generating an upload from data.
 * @param instance Pointer to a SubGhzProtocolEncoderBinRAW instance
 * @return true On success
 */
static bool subghz_protocol_encoder_bin_raw_get_upload(SubGhzProtocolEncoderBinRAW* instance) {
    furi_assert(instance);
    FURI_LOG_E("instance->encoder.size_upload", "%d", instance->encoder.size_upload);
    //склеиваем все кусочики посылки в 1 длинную последовательность с выравниванием по левому краю, в заруженых данных у нас выравнивание по правому краю.

    uint16_t i = 0;
    uint16_t ind = 0;
    while((i < BIN_RAW_MAX_MARKUP_COUNT) && (instance->data_markup[i].bit_count != 0)) {
        uint8_t bit_bias =
            subghz_protocol_bin_raw_get_full_byte(instance->data_markup[i].bit_count) * 8 -
            instance->data_markup[i].bit_count;
        printf(
            "%d  %d  %d ",
            bit_bias,
            instance->data_markup[i].bit_count,
            instance->data_markup[i].byte_bias);
        for(uint16_t y = instance->data_markup[i].byte_bias * 8;
            y < instance->data_markup[i].byte_bias * 8 +
                    subghz_protocol_bin_raw_get_full_byte(instance->data_markup[i].bit_count) * 8 -
                    bit_bias;
            y++) {
            //printf("%02x  ", instance->data[y]);
            subghz_protocol_blocks_set_bit_array(
                subghz_protocol_blocks_get_bit_array(instance->data, y + bit_bias),
                instance->data,
                ind++,
                BIN_RAW_BUF_DATA_SIZE);
        }
        printf("--------------- \r\n");
        i++;
    }

    for(uint16_t y = 0; y < subghz_protocol_bin_raw_get_full_byte(ind); y++) {
        printf("%02x  ", instance->data[y]);
    }
    printf("--------------- \r\n");

    instance->encoder.size_upload = subghz_protocol_blocks_get_upload_from_bit_array(
        instance->data,
        ind,
        instance->encoder.upload,
        instance->encoder.size_upload,
        instance->te,
        SubGhzProtocolBlockAlignBitLeft);

    printf(
        "Free heap size subghz_protocol_encoder_bin_raw_get_upload : %zu\r\n",
        memmgr_get_free_heap());
    FURI_LOG_E("instance->generic.data_count_bit", "%d", instance->generic.data_count_bit);
    FURI_LOG_E("instance->encoder.size_upload", "%d", instance->encoder.size_upload);
    furi_check(instance->encoder.size_upload <= BIN_RAW_BUF_DATA_SIZE * 5);
    return true;
}

bool subghz_protocol_encoder_bin_raw_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolEncoderBinRAW* instance = context;

    bool res = false;
    uint32_t temp_data = 0;

    do {
        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            break;
        }
        if(!flipper_format_read_uint32(flipper_format, "Bit", (uint32_t*)&temp_data, 1)) {
            FURI_LOG_E(TAG, "Missing Bit");
            break;
        }

        //  if(subghz_protocol_bin_raw_get_full_byte(instance->generic.data_count_bit) >=
        //        BIN_RAW_BUF_DATA_SIZE * 8) {
        //         FURI_LOG_E(TAG, "Wrong number of bits in key");
        //         break;
        //     }

        instance->generic.data_count_bit = (uint16_t)temp_data;

        if(!flipper_format_read_uint32(flipper_format, "TE", (uint32_t*)&instance->te, 1)) {
            FURI_LOG_E(TAG, "Missing TE");
            break;
        }

        temp_data = 0;
        uint16_t ind = 0;
        uint16_t byte_bias = 0;
        uint16_t byte_count = 0;
        memset(instance->data_markup, 0x00, BIN_RAW_MAX_MARKUP_COUNT * sizeof(BinRAW_Markup));
        while(flipper_format_read_uint32(flipper_format, "Bit_RAW", (uint32_t*)&temp_data, 1)) {
            if(ind > BIN_RAW_MAX_MARKUP_COUNT) {
                FURI_LOG_E(TAG, "Markup overflow");
                break;
            }
            byte_count += subghz_protocol_bin_raw_get_full_byte(temp_data);
            if(byte_count > BIN_RAW_BUF_DATA_SIZE) {
                FURI_LOG_E(TAG, "Receive buffer overflow");
                break;
            }

            instance->data_markup[ind].bit_count = temp_data;
            instance->data_markup[ind].byte_bias = byte_bias;
            byte_bias += subghz_protocol_bin_raw_get_full_byte(temp_data);

            if(!flipper_format_read_hex(
                   flipper_format,
                   "Data_RAW",
                   instance->data + instance->data_markup[ind].byte_bias,
                   subghz_protocol_bin_raw_get_full_byte(temp_data))) {
                instance->data_markup[ind].bit_count = 0;
                FURI_LOG_E(TAG, "Missing Data_RAW");
                break;
            }
            ind++;
        }

        //вывод
        uint16_t i = 0;
        while((i < BIN_RAW_MAX_MARKUP_COUNT) && (instance->data_markup[i].bit_count != 0)) {
            printf(
                "bit_count=%d  byte_bias=%d  ",
                instance->data_markup[i].bit_count,
                instance->data_markup[i].byte_bias);
            for(uint16_t y = instance->data_markup[i].byte_bias;
                y < instance->data_markup[i].byte_bias +
                        subghz_protocol_bin_raw_get_full_byte(instance->data_markup[i].bit_count);
                y++) {
                printf("%02x  ", instance->data[y]);
            }
            printf("--------------- \r\n");
            i++;
        }

        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            break;
        }
        //optional parameter parameter
        flipper_format_read_uint32(
            flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 1);

        if(!subghz_protocol_encoder_bin_raw_get_upload(instance)) break;
        instance->encoder.is_running = true;

        res = true;
    } while(0);

    return res;
}

void subghz_protocol_encoder_bin_raw_stop(void* context) {
    SubGhzProtocolEncoderBinRAW* instance = context;
    instance->encoder.is_running = false;
}

LevelDuration subghz_protocol_encoder_bin_raw_yield(void* context) {
    SubGhzProtocolEncoderBinRAW* instance = context;

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

void* subghz_protocol_decoder_bin_raw_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolDecoderBinRAW* instance = malloc(sizeof(SubGhzProtocolDecoderBinRAW));
    instance->base.protocol = &subghz_protocol_bin_raw;
    instance->generic.protocol_name = instance->base.protocol->name;
    instance->data_raw_ind = 0;
    instance->data_raw = malloc(BIN_RAW_BUF_RAW_SIZE * sizeof(int32_t));
    instance->data = malloc(BIN_RAW_BUF_RAW_SIZE * sizeof(uint8_t));
    memset(instance->data_markup, 0x00, BIN_RAW_MAX_MARKUP_COUNT * sizeof(BinRAW_Markup));
    return instance;
}

void subghz_protocol_decoder_bin_raw_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderBinRAW* instance = context;
    free(instance->data_raw);
    free(instance->data);
    free(instance);
}

void subghz_protocol_decoder_bin_raw_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderBinRAW* instance = context;
    UNUSED(instance);
    //instance->decoder.parser_step = BinRAWDecoderStepReset;
    //instance->data_raw_ind = 0;
}

void subghz_protocol_decoder_bin_raw_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderBinRAW* instance = context;

    if(instance->decoder.parser_step == BinRAWDecoderStepWrite) {
        if(instance->data_raw_ind == BIN_RAW_BUF_RAW_SIZE) {
            instance->decoder.parser_step = BinRAWDecoderStepBufFull;
        } else {
            instance->data_raw[instance->data_raw_ind++] = (level ? duration : -duration);
        }
    }
}

/** 
 * Analysis of received data
 * @param instance Pointer to a SubGhzProtocolDecoderBinRAW* instance
 */
static bool
    subghz_protocol_bin_raw_check_remote_controller(SubGhzProtocolDecoderBinRAW* instance) {
    struct {
        float data;
        uint16_t count;
    } classes[BIN_RAW_SEARCH_CLASSES];

    size_t ind = 0;

    memset(classes, 0x00, sizeof(classes));

    uint16_t data_markup_ind = 0;
    memset(instance->data_markup, 0x00, BIN_RAW_MAX_MARKUP_COUNT * sizeof(BinRAW_Markup));

    if(instance->data_raw_ind < 512) {
        ind =
            instance->data_raw_ind -
            100; //there is usually garbage at the end of the record, we exclude it from the classification
    } else {
        ind = 512;
    }

    //sort the durations to find the shortest correlated interval
    for(size_t i = 0; i < ind; i++) {
        for(size_t k = 0; k < BIN_RAW_SEARCH_CLASSES; k++) {
            if(classes[k].count == 0) {
                classes[k].data = (float)(abs(instance->data_raw[i]));
                classes[k].count++;
                break;
            } else if(
                DURATION_DIFF((float)(abs(instance->data_raw[i])), (classes[k].data)) <
                (classes[k].data / 4)) { //if the test value does not differ by more than 25%
                classes[k].data += ((float)(abs(instance->data_raw[i])) - classes[k].data) *
                                   0.05f; //running average k=0.05
                classes[k].count++;
                break;
            }
        }
    }

    // if(classes[BIN_RAW_SEARCH_CLASSES - 1].count != 0) {
    //     //filling the classifier, it means that they received an unclean signal
    //     return false;
    // }

    //looking for the minimum te with an occurrence greater than BIN_RAW_TE_MIN_COUNT
    instance->te = subghz_protocol_bin_raw_const.te_long * 2;

    bool te_ok = false;
    uint16_t gap_ind = 0;
    uint16_t gap_delta = 0;
    uint32_t gap = 0;
    int data_temp = 0;

    //sort by number of occurrences
    bool swap = true;
    while(swap) {
        swap = false;
        for(size_t i = 1; i < BIN_RAW_SEARCH_CLASSES; i++) {
            if(classes[i].count > classes[i - 1].count) {
                uint32_t data = classes[i - 1].data;
                uint32_t count = classes[i - 1].count;
                classes[i - 1].data = classes[i].data;
                classes[i - 1].count = classes[i].count;
                classes[i].data = data;
                classes[i].count = count;
                swap = true;
            }
        }
    }

    for(size_t k = 0; k < BIN_RAW_SEARCH_CLASSES; k++) {
        FURI_LOG_W("Class", "%d\t%d\t%ld", k, classes[k].count, (uint32_t)classes[k].data);
    }

    if((classes[0].count > BIN_RAW_TE_MIN_COUNT) && (classes[1].count == 0)) {
        //adopted only the preamble
        instance->te = (uint32_t)classes[0].data;
        te_ok = true;
        gap = 0; //gap no
    } else {
        //take the 2 most common durations
        //check that there are enough
        if((classes[0].count < BIN_RAW_TE_MIN_COUNT) ||
           (classes[1].count < (BIN_RAW_TE_MIN_COUNT >> 1)))
            return false;
        //arrange the first 2 date values ??in ascending order
        if(classes[0].data > classes[1].data) {
            uint32_t data = classes[1].data;
            classes[0].data = classes[1].data;
            classes[1].data = data;
        }

        //determine the value to be corrected
        for(uint8_t k = 1; k < 5; k++) {
            FURI_LOG_I("K-div", " %f ", (double)(classes[1].data / (classes[0].data / k)));
            float delta = (classes[1].data / (classes[0].data / k)) -
                          ((uint32_t)classes[1].data / ((uint32_t)classes[0].data / k));
            if((delta < 0.25) || (delta > 0.75)) {
                instance->te = (uint32_t)classes[0].data / k;
                FURI_LOG_I("K=", " %d ", k);
                te_ok = true;
                break;
            }
        }

        //looking for a gap
        for(size_t k = 2; k < BIN_RAW_SEARCH_CLASSES; k++) {
            if((classes[k].count > 2) && (classes[k].data > gap)) {
                gap = (uint32_t)classes[k].data;
                gap_delta = gap / 5;
            }
        }

        if((gap / instance->te) < 10) {
            gap = 0; //check that our signal has a gap greater than 10*TE
        } else {
            //looking for the last occurrence of gap
            ind = instance->data_raw_ind - 1;
            while((ind > 0) && (DURATION_DIFF(abs(instance->data_raw[ind]), gap) > gap_delta)) {
                ind--;
            }
            gap_ind = ind;
        }
    }

    // for(size_t k = 0; k < BIN_RAW_SEARCH_CLASSES; k++) {
    //     //find the 2 most common durations
    //     if(classes[k].count > instance->data_markup[0].count) {
    //         instance->data_markup[1].data = instance->data_markup[0].data;
    //         instance->data_markup[1].count = instance->data_markup[0].count;
    //         instance->data_markup[0].data = classes[k].data;
    //         instance->data_markup[0].count = classes[k].count;
    //     }
    //     //find the longest duration
    //     if((classes[k].count > 2) && (classes[k].data > gap)) {
    //         gap = classes[k].data;
    //         gap_delta = gap / 5;
    //     }
    // }

    // for(size_t k = 0; k < BIN_RAW_MAX_MARKUP_COUNT; k++) {
    //     FURI_LOG_E("instance->data_markup", "%d %d %ld", k, instance->data_markup[k].count, instance->data_markup[k].data);
    // }

    if(!te_ok) {
        //did not find the minimum TE satisfying the condition
        return false;
    }

    //if(te_ok) {

    //if we consider that there is a gap, then we divide the signal with respect to this gap
    if(gap != 0) {
        ind = (BIN_RAW_BUF_DATA_SIZE * 8);
        uint16_t bit_count = 0;
        do {
            gap_ind--;
            data_temp = (int)(round((float)(instance->data_raw[gap_ind]) / instance->te));
            //printf("\033[0;32m%d\033[0m(%0.2f)  ", data_temp, (double)((float)(instance->data_raw[gap_ind]) / instance->te));
            // printf(
            //     "%d(%0.2f)  ",
            //     data_temp,
            //     (double)((float)(instance->data_raw[gap_ind]) / instance->te));
            printf("%d  ", data_temp);
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
                        true, instance->data, ind, BIN_RAW_BUF_DATA_SIZE);
                } else {
                    subghz_protocol_blocks_set_bit_array(
                        false, instance->data, ind, BIN_RAW_BUF_DATA_SIZE);
                }
            }

            if(DURATION_DIFF(abs(instance->data_raw[gap_ind]), gap) < gap_delta) {
                instance->data_markup[data_markup_ind].byte_bias = ind >> 3;
                instance->data_markup[data_markup_ind++].bit_count = bit_count;
                bit_count = 0;

                if(data_markup_ind == BIN_RAW_MAX_MARKUP_COUNT) break;
                ind &= 0xFFFFFFF8; //jump to the pre whole byte
            }
        } while(gap_ind != 0);
        if(data_markup_ind != BIN_RAW_MAX_MARKUP_COUNT) {
            instance->data_markup[data_markup_ind].byte_bias = ind >> 3;
            instance->data_markup[data_markup_ind++].bit_count = bit_count;
        }

        printf("-------%d-------- \r\n", (BIN_RAW_BUF_DATA_SIZE * 8) - ind);

        //reset the classifier and classify the received data
        memset(classes, 0x00, sizeof(classes));
        for(size_t i = 0; i < data_markup_ind; i++) {
            for(size_t k = 0; k < BIN_RAW_SEARCH_CLASSES; k++) {
                if(classes[k].count == 0) {
                    classes[k].data = instance->data_markup[i].bit_count;
                    classes[k].count++;
                    break;
                } else if(instance->data_markup[i].bit_count == classes[k].data) {
                    classes[k].count++;
                    break;
                }
            }
        }

        for(size_t h = 0; h < BIN_RAW_SEARCH_CLASSES; h++) {
            FURI_LOG_I("K", "%d, %d %ld ", h, classes[h].count, (uint32_t)classes[h].data);
        }

        //choose the value with the maximum repetition
        data_temp = 0;
        for(size_t i = 0; i < BIN_RAW_SEARCH_CLASSES; i++) {
            if((classes[i].count > 1) && (data_temp < classes[i].count))
                data_temp = (int)classes[i].data;
        }

        //if(data_markup_ind == 0) return false;
        //вывод в обратном порятке
        uint16_t data_markup_ind_temp = data_markup_ind;
        if(data_markup_ind) {
            data_markup_ind_temp--;
            for(size_t i = (ind / 8) - 1; i < BIN_RAW_BUF_DATA_SIZE; i++) {
                printf("%02x ", instance->data[i]);
                if(instance->data_markup[data_markup_ind_temp].byte_bias == i + 1) {
                    printf(
                        "\r\n ind %d :\t%d %d   :\t",
                        data_markup_ind_temp,
                        instance->data_markup[data_markup_ind_temp].byte_bias,
                        instance->data_markup[data_markup_ind_temp].bit_count);
                    if(data_markup_ind_temp != 0) data_markup_ind_temp--;
                }
            }

            printf("--------------- \r\n");
        }

        //compare data in chunks with the same number of bits
        //if(data_temp == 0) data_temp = (int)classes[0].data;
        FURI_LOG_E("data_temp", "%d", data_temp);
        uint8_t data_type = 0;

        if(data_temp != 0) {
            //проверка, что данные в передаче повторяются каждый пакет
            for(uint16_t i = 0; i < data_markup_ind - 1; i++) {
                if((instance->data_markup[i].bit_count == data_temp) &&
                   (instance->data_markup[i + 1].bit_count == data_temp)) {
                    //если количество бит соседних посылках одинаково сравниваем данные
                    FURI_LOG_E(
                        "Comparison 1+1",
                        "i=%d y=%d %02X=%02X .... %02X=%02X",
                        i,
                        i + 1,
                        instance->data[instance->data_markup[i].byte_bias],
                        instance->data[instance->data_markup[i + 1].byte_bias],
                        instance->data
                            [instance->data_markup[i].byte_bias +
                             subghz_protocol_bin_raw_get_full_byte(
                                 instance->data_markup[i].bit_count) -
                             1],
                        instance->data
                            [instance->data_markup[i + 1].byte_bias +
                             subghz_protocol_bin_raw_get_full_byte(
                                 instance->data_markup[i + 1].bit_count) -
                             1]);

                    uint16_t byte_count =
                        subghz_protocol_bin_raw_get_full_byte(instance->data_markup[i].bit_count);
                    if(memcmp(
                           instance->data + instance->data_markup[i].byte_bias,
                           instance->data + instance->data_markup[i + 1].byte_bias,
                           byte_count - 1) == 0) {
                        FURI_LOG_I("found 1+1", " ");
                        // memcpy(
                        //     instance->data,
                        //     instance->data + instance->data_markup[i].byte_bias,
                        //     byte_count);
                        // instance->generic.data_count_bit = instance->data_markup[i].bit_count;

                        //размещаем в 1 элементе смещение до валидных данных
                        instance->data_markup[0].bit_count = instance->data_markup[i].bit_count;
                        instance->data_markup[0].byte_bias = instance->data_markup[i].byte_bias;
                        //конец
                        instance->data_markup[1].bit_count = 0;
                        instance->data_markup[1].byte_bias = 0;

                        data_type = 1;
                        i = data_markup_ind;
                        break;
                    }
                }
            }
        }
        if(data_type == 0) {
            //проверяем что повтор  происходит каждые n пакетов
            for(uint16_t i = 0; i < data_markup_ind - 2; i++) {
                uint16_t byte_count =
                    subghz_protocol_bin_raw_get_full_byte(instance->data_markup[i].bit_count);
                for(uint16_t y = i + 1; y < data_markup_ind - 1; y++) {
                    FURI_LOG_E(
                        "Comparison",
                        "i=%d y=%d %02X=%02X .... %02X=%02X",
                        i,
                        y,
                        instance->data[instance->data_markup[i].byte_bias],
                        instance->data[instance->data_markup[y].byte_bias],
                        instance->data
                            [instance->data_markup[i].byte_bias +
                             subghz_protocol_bin_raw_get_full_byte(
                                 instance->data_markup[i].bit_count) -
                             1],
                        instance->data
                            [instance->data_markup[y].byte_bias +
                             subghz_protocol_bin_raw_get_full_byte(
                                 instance->data_markup[y].bit_count) -
                             1]);

                    if(byte_count ==
                       subghz_protocol_bin_raw_get_full_byte(
                           instance->data_markup[y].bit_count)) { //если длинна в байтах совпадает

                        if((memcmp(
                                instance->data + instance->data_markup[i].byte_bias,
                                instance->data + instance->data_markup[y].byte_bias,
                                byte_count - 1) == 0) &&
                           (memcmp(
                                instance->data + instance->data_markup[i + 1].byte_bias,
                                instance->data + instance->data_markup[y + 1].byte_bias,
                                byte_count - 1) == 0)) {
                            // memcpy(
                            //     instance->data,
                            //     instance->data + instance->data_markup[i].data,
                            //     byte_count);
                            //instance->generic.data_count_bit = instance->data_markup[i].bit_count;
                            FURI_LOG_I("found", " ");
                            //вывод в обратном порятке
                            uint8_t index = y - 1;
                            for(size_t z = instance->data_markup[y].byte_bias + byte_count;
                                z < instance->data_markup[i].byte_bias + byte_count;
                                z++) {
                                if(instance->data_markup[index].byte_bias == z) {
                                    printf(
                                        "\r\n ind %d :\t%d %d   :\t",
                                        index,
                                        instance->data_markup[index].byte_bias,
                                        instance->data_markup[index].bit_count);
                                    if(index != 0) index--;
                                }
                                printf("%02x ", instance->data[z]);
                            }

                            printf("--------------- \r\n");

                            //todo оптимизировать
                            BinRAW_Markup markup_temp[BIN_RAW_MAX_MARKUP_COUNT];
                            memcpy(
                                markup_temp,
                                instance->data_markup,
                                BIN_RAW_MAX_MARKUP_COUNT * sizeof(BinRAW_Markup));
                            memset(
                                instance->data_markup,
                                0x00,
                                BIN_RAW_MAX_MARKUP_COUNT * sizeof(BinRAW_Markup));

                            for(index = i; index < y; index++) {
                                instance->data_markup[index - i].bit_count =
                                    markup_temp[y - index - 1].bit_count;
                                instance->data_markup[index - i].byte_bias =
                                    markup_temp[y - index - 1].byte_bias;
                            }

                            data_type = 2;
                            i = data_markup_ind;
                            break;
                        }
                    }
                }
            }
        }
        //todo оптимизировать
        if(data_type == 0) {
            if(data_temp != 0) {
                BinRAW_Markup markup_temp[BIN_RAW_MAX_MARKUP_COUNT];
                memcpy(
                    markup_temp,
                    instance->data_markup,
                    BIN_RAW_MAX_MARKUP_COUNT * sizeof(BinRAW_Markup));
                memset(
                    instance->data_markup, 0x00, BIN_RAW_MAX_MARKUP_COUNT * sizeof(BinRAW_Markup));
                uint16_t byte_count = subghz_protocol_bin_raw_get_full_byte(data_temp);
                uint16_t index = 0;
                for(int i = BIN_RAW_MAX_MARKUP_COUNT; i > -1; i--) {
                    if(subghz_protocol_bin_raw_get_full_byte(markup_temp[i].bit_count) ==
                       byte_count) {
                        instance->data_markup[index].bit_count = markup_temp[i].bit_count;
                        instance->data_markup[index].byte_bias = markup_temp[i].byte_bias;
                        index++;
                        data_type = 3;
                    }
                }
            }
        }
        FURI_LOG_I("data_type", " %d", data_type);
        if(data_type)
            return true;
        else
            return false;

    } else {
        FURI_LOG_I("-----", "  ");
        FURI_LOG_I("-----", "  ");
        FURI_LOG_I("-----", "  ");
        FURI_LOG_I("te", " %ld ", instance->te);
        FURI_LOG_I("gap", " %ld ", gap);
        FURI_LOG_I("gap_ind", " %d ", gap_ind);
        int data = (int)(round((float)(instance->data_raw[gap_ind]) / instance->te));
        FURI_LOG_W("gap/ind", " %d ", data);
        FURI_LOG_W("instance->data_raw_ind", " %d ", instance->data_raw_ind);
        ind = 0; //
        for(size_t i = 0; i < instance->data_raw_ind; i++) {
            int data_temp = (int)(round((float)(instance->data_raw[i]) / instance->te));
            if(data_temp == 0) break; //found an interval 2 times shorter than TE, this is noise
            printf("%d  ", data_temp);

            for(size_t k = 0; k < abs(data_temp); k++) {
                if(data_temp > 0) {
                    subghz_protocol_blocks_set_bit_array(
                        true, instance->data, ind++, BIN_RAW_BUF_DATA_SIZE);
                } else {
                    subghz_protocol_blocks_set_bit_array(
                        false, instance->data, ind++, BIN_RAW_BUF_DATA_SIZE);
                }
                if(ind == BIN_RAW_BUF_DATA_SIZE * 8) {
                    i = instance->data_raw_ind;
                    break;
                }
            }
        }
        printf("--------------- \r\n");

        FURI_LOG_W("ind count_byte", " %d  %d", ind, subghz_protocol_bin_raw_get_full_byte(ind));
        for(size_t i = 0; i < subghz_protocol_bin_raw_get_full_byte(ind); i++) {
            printf("%02x ", instance->data[i]);
        }
        printf("--------------- \r\n");

        //right alignment
        uint8_t bit_bias = (subghz_protocol_bin_raw_get_full_byte(ind) << 3) - ind;

        FURI_LOG_W(
            "bit_bias",
            "bit_bias= %d  bit_full_byte= %d bit= %d",
            bit_bias,
            subghz_protocol_bin_raw_get_full_byte(ind) << 3,
            ind);

        for(size_t i = subghz_protocol_bin_raw_get_full_byte(ind) - 1; i > 0; i--) {
            instance->data[i] = (instance->data[i - 1] << (8 - bit_bias)) |
                                (instance->data[i] >> bit_bias);
        }
        instance->data[0] = (instance->data[0] >> bit_bias);

        //распечаать
        for(size_t i = 0; i < subghz_protocol_bin_raw_get_full_byte(ind); i++) {
            printf("%02x ", instance->data[i]);
        }
        printf("--------------- \r\n");

        //memcpy(instance->data, instance->data + instance->data_markup[i].data, byte_count);

        if(ind != 0) {
            //проверить что не приходят одни 0
            //instance->generic.data_count_bit = ind;
            instance->data_markup[0].bit_count = ind;
            instance->data_markup[0].byte_bias = 0;

            return true;
        } else {
            return false;
        }
    }
    return false;
}

void subghz_protocol_decoder_bin_raw_data_input_rssi(
    SubGhzProtocolDecoderBinRAW* instance,
    float rssi) {
    furi_assert(instance);
    switch(instance->decoder.parser_step) {
    case BinRAWDecoderStepReset:
        if(rssi > BIN_RAW_THRESHOLD_RSSI) {
            instance->data_raw_ind = 0;
            memset(instance->data_raw, 0x00, BIN_RAW_BUF_RAW_SIZE * sizeof(int32_t));
            memset(instance->data, 0x00, BIN_RAW_BUF_RAW_SIZE * sizeof(uint8_t));
            instance->decoder.parser_step = BinRAWDecoderStepWrite;
        }
        break;
    case BinRAWDecoderStepBufFull:
    case BinRAWDecoderStepWrite:
        if(rssi < BIN_RAW_THRESHOLD_RSSI) {
            for(size_t i = 0; i < instance->data_raw_ind; i++) {
                printf("%ld ", instance->data_raw[i]);
            }
            printf("---%d----------- \r\n", instance->data_raw_ind);

            instance->decoder.parser_step = BinRAWDecoderStepReset;
            instance->generic.data_count_bit = 0;
            if(instance->data_raw_ind >= BIN_RAW_BUF_MIN_DATA_COUNT) {
                if(subghz_protocol_bin_raw_check_remote_controller(instance)) {
                    uint16_t i = 0;
                    while((i < BIN_RAW_MAX_MARKUP_COUNT) &&
                          (instance->data_markup[i].bit_count != 0)) {
                        instance->generic.data_count_bit += instance->data_markup[i].bit_count;

                        //вывод
                        for(uint16_t y = instance->data_markup[i].byte_bias;
                            y < instance->data_markup[i].byte_bias +
                                    subghz_protocol_bin_raw_get_full_byte(
                                        instance->data_markup[i].bit_count);
                            y++) {
                            printf("%02x  ", instance->data[y]);
                        }
                        printf("--------------- \r\n");

                        i++;
                    }
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

uint8_t subghz_protocol_decoder_bin_raw_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderBinRAW* instance = context;
    return subghz_protocol_blocks_add_bytes(
        instance->data, subghz_protocol_bin_raw_get_full_byte(instance->generic.data_count_bit));
}

bool subghz_protocol_decoder_bin_raw_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    SubGhzProtocolDecoderBinRAW* instance = context;

    bool res = false;
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    do {
        stream_clean(flipper_format_get_raw_stream(flipper_format));
        if(!flipper_format_write_header_cstr(
               flipper_format, SUBGHZ_KEY_FILE_TYPE, SUBGHZ_KEY_FILE_VERSION)) {
            FURI_LOG_E(TAG, "Unable to add header");
            break;
        }

        if(!flipper_format_write_uint32(flipper_format, "Frequency", &preset->frequency, 1)) {
            FURI_LOG_E(TAG, "Unable to add Frequency");
            break;
        }

        subghz_block_generic_get_preset_name(furi_string_get_cstr(preset->name), temp_str);
        if(!flipper_format_write_string_cstr(
               flipper_format, "Preset", furi_string_get_cstr(temp_str))) {
            FURI_LOG_E(TAG, "Unable to add Preset");
            break;
        }
        if(!strcmp(furi_string_get_cstr(temp_str), "FuriHalSubGhzPresetCustom")) {
            if(!flipper_format_write_string_cstr(
                   flipper_format, "Custom_preset_module", "CC1101")) {
                FURI_LOG_E(TAG, "Unable to add Custom_preset_module");
                break;
            }
            if(!flipper_format_write_hex(
                   flipper_format, "Custom_preset_data", preset->data, preset->data_size)) {
                FURI_LOG_E(TAG, "Unable to add Custom_preset_data");
                break;
            }
        }
        if(!flipper_format_write_string_cstr(
               flipper_format, "Protocol", instance->generic.protocol_name)) {
            FURI_LOG_E(TAG, "Unable to add Protocol");
            break;
        }

        uint32_t temp = instance->generic.data_count_bit;
        if(!flipper_format_write_uint32(flipper_format, "Bit", &temp, 1)) {
            FURI_LOG_E(TAG, "Unable to add Bit");
            break;
        }

        if(!flipper_format_write_uint32(flipper_format, "TE", &instance->te, 1)) {
            FURI_LOG_E(TAG, "Unable to add TE");
            break;
        }

        uint16_t i = 0;
        while((i < BIN_RAW_MAX_MARKUP_COUNT) && (instance->data_markup[i].bit_count != 0)) {
            temp = instance->data_markup[i].bit_count;
            if(!flipper_format_write_uint32(flipper_format, "Bit_RAW", &temp, 1)) {
                FURI_LOG_E(TAG, "Bit_RAW");
                break;
            }
            if(!flipper_format_write_hex(
                   flipper_format,
                   "Data_RAW",
                   instance->data + instance->data_markup[i].byte_bias,
                   subghz_protocol_bin_raw_get_full_byte(instance->data_markup[i].bit_count))) {
                FURI_LOG_E(TAG, "Unable to add Data_RAW");
                break;
            }
            i++;
        }

        res = true;
    } while(false);
    furi_string_free(temp_str);
    return res;
}

bool subghz_protocol_decoder_bin_raw_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderBinRAW* instance = context;

    bool res = false;
    uint32_t temp_data = 0;

    do {
        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            break;
        }
        if(!flipper_format_read_uint32(flipper_format, "Bit", (uint32_t*)&temp_data, 1)) {
            FURI_LOG_E(TAG, "Missing Bit");
            break;
        }

        //  if(subghz_protocol_bin_raw_get_full_byte(instance->generic.data_count_bit) >=
        //        BIN_RAW_BUF_DATA_SIZE * 8) {
        //         FURI_LOG_E(TAG, "Wrong number of bits in key");
        //         break;
        //     }

        instance->generic.data_count_bit = (uint16_t)temp_data;

        if(!flipper_format_read_uint32(flipper_format, "TE", (uint32_t*)&instance->te, 1)) {
            FURI_LOG_E(TAG, "Missing TE");
            break;
        }

        temp_data = 0;
        uint16_t ind = 0;
        uint16_t byte_bias = 0;
        uint16_t byte_count = 0;
        memset(instance->data_markup, 0x00, BIN_RAW_MAX_MARKUP_COUNT * sizeof(BinRAW_Markup));
        while(flipper_format_read_uint32(flipper_format, "Bit_RAW", (uint32_t*)&temp_data, 1)) {
            if(ind > BIN_RAW_MAX_MARKUP_COUNT) {
                FURI_LOG_E(TAG, "Markup overflow");
                break;
            }
            byte_count += subghz_protocol_bin_raw_get_full_byte(temp_data);
            if(byte_count > BIN_RAW_BUF_DATA_SIZE) {
                FURI_LOG_E(TAG, "Receive buffer overflow");
                break;
            }

            instance->data_markup[ind].bit_count = temp_data;
            instance->data_markup[ind].byte_bias = byte_bias;
            byte_bias += subghz_protocol_bin_raw_get_full_byte(temp_data);

            if(!flipper_format_read_hex(
                   flipper_format,
                   "Data_RAW",
                   instance->data + instance->data_markup[ind].byte_bias,
                   subghz_protocol_bin_raw_get_full_byte(temp_data))) {
                instance->data_markup[ind].bit_count = 0;
                FURI_LOG_E(TAG, "Missing Data_RAW");
                break;
            }
            ind++;
        }

        res = true;
    } while(0);

    return res;
}

void subghz_protocol_decoder_bin_raw_get_string(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderBinRAW* instance = context;
    //subghz_protocol_bin_raw_check_remote_controller(&instance->generic);
    furi_string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:",
        instance->generic.protocol_name,
        instance->generic.data_count_bit);

    uint16_t byte_count = subghz_protocol_bin_raw_get_full_byte(instance->generic.data_count_bit);
    for(size_t i = 0; (byte_count < 36 ? i < byte_count : i < 36); i++) {
        furi_string_cat_printf(output, "%02X", instance->data[i]);
    }

    furi_string_cat_printf(output, "\r\nTe:%luus\r\n", instance->te);
}
