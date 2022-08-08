#include <furi.h>
#include "../minunit.h"
#include <toolbox/protocols/protocol_dict.h>

typedef enum {
    TestDictProtocol0,
    TestDictProtocol1,

    TestDictProtocolMax,
} TestDictProtocols;

/*********************** PROTOCOL 0 START ***********************/

typedef struct {
    uint32_t data;
    size_t encoder_counter;
} Protocol0Data;

static const uint32_t protocol_0_decoder_result = 0xDEADBEEF;

static void* protocol_0_alloc() {
    void* data = malloc(sizeof(Protocol0Data));
    return data;
}

static void protocol_0_free(Protocol0Data* data) {
    free(data);
}

static const char* protocol_0_get_name(Protocol0Data* data) {
    UNUSED(data);
    return "Protocol 0";
}

static const char* protocol_0_get_manufacturer(Protocol0Data* data) {
    UNUSED(data);
    return "Manufacturer 0";
}

static void protocol_0_set_data(Protocol0Data* data, const uint8_t* value, size_t data_size) {
    furi_check(data_size <= 4);
    data->data = 0;
    memcpy(&data->data, value, data_size);
}

static void protocol_0_get_data(Protocol0Data* data, uint8_t* value, size_t data_size) {
    furi_check(data_size >= 4);
    memcpy(value, &data->data, data_size);
}

static size_t protocol_0_get_data_size(Protocol0Data* data) {
    UNUSED(data);
    return 4;
}

static void protocol_0_decoder_start(Protocol0Data* data) {
    data->data = 0;
}

static bool protocol_0_decoder_feed(Protocol0Data* data, bool level, uint32_t duration) {
    if(level && duration == 666) {
        data->data = protocol_0_decoder_result;
        return true;
    } else {
        return false;
    }
}

static void protocol_0_decoder_reset(Protocol0Data* data) {
    data->data = 0;
}

static bool protocol_0_encoder_start(Protocol0Data* data) {
    data->encoder_counter = 0;
    return true;
}

static LevelDuration protocol_0_encoder_yield(Protocol0Data* data) {
    data->encoder_counter++;
    return level_duration_make(data->encoder_counter % 2, data->data);
}

static void protocol_0_encoder_reset(Protocol0Data* data) {
    data->encoder_counter = 0;
}

/*********************** PROTOCOL 1 START ***********************/

typedef struct {
    uint64_t data;
    size_t encoder_counter;
} Protocol1Data;

static const uint64_t protocol_1_decoder_result = 0x1234567890ABCDEF;

static void* protocol_1_alloc() {
    void* data = malloc(sizeof(Protocol1Data));
    return data;
}

static void protocol_1_free(Protocol1Data* data) {
    free(data);
}

static const char* protocol_1_get_name(Protocol1Data* data) {
    UNUSED(data);
    return "Protocol 1";
}

static const char* protocol_1_get_manufacturer(Protocol1Data* data) {
    UNUSED(data);
    return "Manufacturer 1";
}

static void protocol_1_set_data(Protocol1Data* data, const uint8_t* value, size_t data_size) {
    furi_check(data_size <= 4);
    data->data = 0;
    memcpy(&data->data, value, data_size);
}

static void protocol_1_get_data(Protocol1Data* data, uint8_t* value, size_t data_size) {
    furi_check(data_size >= 4);
    memcpy(value, &data->data, data_size);
}

static size_t protocol_1_get_data_size(Protocol1Data* data) {
    UNUSED(data);
    return 8;
}

static void protocol_1_decoder_start(Protocol1Data* data) {
    data->data = 0;
}

static bool protocol_1_decoder_feed(Protocol1Data* data, bool level, uint32_t duration) {
    if(level && duration == 543) {
        data->data = 0x1234567890ABCDEF;
        return true;
    } else {
        return false;
    }
}

static void protocol_1_decoder_reset(Protocol1Data* data) {
    data->data = 0;
}

static bool protocol_1_encoder_start(Protocol1Data* data) {
    data->encoder_counter = 0;
    return true;
}

static LevelDuration protocol_1_encoder_yield(Protocol1Data* data) {
    data->encoder_counter++;
    return level_duration_make(!(data->encoder_counter % 2), 100);
}

static void protocol_1_encoder_reset(Protocol1Data* data) {
    data->encoder_counter = 0;
}

/*********************** PROTOCOLS DESCRIPTION ***********************/
static const ProtocolBase protocol_0 = {
    .alloc = (ProtocolAlloc)protocol_0_alloc,
    .free = (ProtocolFree)protocol_0_free,
    .set_data = (ProtocolSetData)protocol_0_set_data,
    .get_data = (ProtocolGetData)protocol_0_get_data,
    .get_data_size = (ProtocolGetDataSize)protocol_0_get_data_size,
    .get_name = (ProtocolGetName)protocol_0_get_name,
    .get_manufacturer = (ProtocolGetManufacturer)protocol_0_get_manufacturer,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_0_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_0_decoder_feed,
            .reset = (ProtocolDecoderReset)protocol_0_decoder_reset,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_0_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_0_encoder_yield,
            .reset = (ProtocolEncoderReset)protocol_0_encoder_reset,
        },
};

static const ProtocolBase protocol_1 = {
    .alloc = (ProtocolAlloc)protocol_1_alloc,
    .free = (ProtocolFree)protocol_1_free,
    .set_data = (ProtocolSetData)protocol_1_set_data,
    .get_data = (ProtocolGetData)protocol_1_get_data,
    .get_data_size = (ProtocolGetDataSize)protocol_1_get_data_size,
    .get_name = (ProtocolGetName)protocol_1_get_name,
    .get_manufacturer = (ProtocolGetManufacturer)protocol_1_get_manufacturer,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_1_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_1_decoder_feed,
            .reset = (ProtocolDecoderReset)protocol_1_decoder_reset,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_1_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_1_encoder_yield,
            .reset = (ProtocolEncoderReset)protocol_1_encoder_reset,
        },
};

static const ProtocolBase* test_protocols_base[] = {
    [TestDictProtocol0] = &protocol_0,
    [TestDictProtocol1] = &protocol_1,
};

MU_TEST(test_protocol_dict) {
    ProtocolDict* dict = protocol_dict_alloc(test_protocols_base, TestDictProtocolMax);
    size_t max_data_size = protocol_dict_get_max_data_size(dict);
    mu_assert_int_eq(8, max_data_size);
    uint8_t* data = malloc(max_data_size);

    protocol_dict_decoders_start(dict);
    ProtocolId protocol_id = PROTOCOL_NO;

    for(size_t i = 0; i < 100; i++) {
        protocol_id = protocol_dict_decoders_feed(dict, i % 2, 100);
        mu_assert_int_eq(PROTOCOL_NO, protocol_id);
    }

    // trigger protocol 1
    protocol_id = protocol_dict_decoders_feed(dict, true, 543);
    mu_assert_int_eq(TestDictProtocol1, protocol_id);

    mu_assert_string_eq("Protocol 1", protocol_dict_get_name(dict, protocol_id));
    mu_assert_string_eq("Manufacturer 1", protocol_dict_get_manufacturer(dict, protocol_id));

    size_t data_size = protocol_dict_get_data_size(dict, protocol_id);
    mu_assert_int_eq(8, data_size);

    protocol_dict_get_data(dict, protocol_id, data, data_size);
    mu_assert_mem_eq(&protocol_1_decoder_result, data, data_size);

    // trigger protocol 0
    protocol_id = protocol_dict_decoders_feed(dict, true, 666);
    mu_assert_int_eq(TestDictProtocol0, protocol_id);

    mu_assert_string_eq("Protocol 0", protocol_dict_get_name(dict, protocol_id));
    mu_assert_string_eq("Manufacturer 0", protocol_dict_get_manufacturer(dict, protocol_id));

    data_size = protocol_dict_get_data_size(dict, protocol_id);
    mu_assert_int_eq(4, data_size);

    protocol_dict_get_data(dict, protocol_id, data, data_size);
    mu_assert_mem_eq(&protocol_0_decoder_result, data, data_size);

    protocol_dict_decoders_reset(dict);

    protocol_id = TestDictProtocol0;

    const uint8_t protocol_0_test_data[4] = {100, 0, 0, 0};
    protocol_dict_set_data(dict, protocol_id, protocol_0_test_data, 4);

    mu_check(protocol_dict_encoder_start(dict, protocol_id));

    LevelDuration level;
    level = protocol_dict_encoder_yield(dict, protocol_id);
    mu_assert_int_eq(true, level_duration_get_level(level));
    mu_assert_int_eq(100, level_duration_get_duration(level));
    level = protocol_dict_encoder_yield(dict, protocol_id);
    mu_assert_int_eq(false, level_duration_get_level(level));
    mu_assert_int_eq(100, level_duration_get_duration(level));
    level = protocol_dict_encoder_yield(dict, protocol_id);
    mu_assert_int_eq(true, level_duration_get_level(level));
    mu_assert_int_eq(100, level_duration_get_duration(level));

    mu_check(protocol_dict_encoder_start(dict, protocol_id));
    level = protocol_dict_encoder_yield(dict, protocol_id);
    mu_assert_int_eq(true, level_duration_get_level(level));
    mu_assert_int_eq(100, level_duration_get_duration(level));

    protocol_dict_free(dict);
    free(data);
}

MU_TEST_SUITE(test_protocol_dict_suite) {
    MU_RUN_TEST(test_protocol_dict);
}

int run_minunit_test_protocol_dict() {
    MU_RUN_SUITE(test_protocol_dict_suite);
    return MU_EXIT_CODE;
}