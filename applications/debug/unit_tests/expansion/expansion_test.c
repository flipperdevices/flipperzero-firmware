#include "../minunit.h"

#include <furi.h>
#include <expansion/expansion_protocol.h>

MU_TEST(test_expansion_encoded_size) {
    ExpansionFrame frame;

    frame.header.type = ExpansionFrameTypeHeartbeat;
    mu_assert_int_eq(1, expansion_frame_get_encoded_size(&frame));

    frame.header.type = ExpansionFrameTypeStatus;
    mu_assert_int_eq(2, expansion_frame_get_encoded_size(&frame));

    frame.header.type = ExpansionFrameTypeHold;
    mu_assert_int_eq(5, expansion_frame_get_encoded_size(&frame));

    frame.header.type = ExpansionFrameTypeBaudRate;
    mu_assert_int_eq(5, expansion_frame_get_encoded_size(&frame));

    frame.header.type = ExpansionFrameTypeControl;
    mu_assert_int_eq(2, expansion_frame_get_encoded_size(&frame));

    frame.header.type = ExpansionFrameTypeData;
    for(size_t i = 0; i <= EXPANSION_MAX_DATA_SIZE; ++i) {
        frame.content.data.size = i;
        mu_assert_int_eq(i + 3, expansion_frame_get_encoded_size(&frame));
    }
}

MU_TEST(test_expansion_remaining_size) {
    ExpansionFrame frame;

    mu_assert_int_eq(1, expansion_frame_get_remaining_size(&frame, 0));

    frame.header.type = ExpansionFrameTypeHeartbeat;
    mu_assert_int_eq(1, expansion_frame_get_remaining_size(&frame, 0));
    mu_assert_int_eq(0, expansion_frame_get_remaining_size(&frame, 1));
    mu_assert_int_eq(0, expansion_frame_get_remaining_size(&frame, 100));

    frame.header.type = ExpansionFrameTypeStatus;
    mu_assert_int_eq(1, expansion_frame_get_remaining_size(&frame, 0));
    mu_assert_int_eq(1, expansion_frame_get_remaining_size(&frame, 1));
    mu_assert_int_eq(0, expansion_frame_get_remaining_size(&frame, 2));
    mu_assert_int_eq(0, expansion_frame_get_remaining_size(&frame, 100));

    frame.header.type = ExpansionFrameTypeHold;
    mu_assert_int_eq(1, expansion_frame_get_remaining_size(&frame, 0));
    mu_assert_int_eq(4, expansion_frame_get_remaining_size(&frame, 1));
    mu_assert_int_eq(0, expansion_frame_get_remaining_size(&frame, 5));
    mu_assert_int_eq(0, expansion_frame_get_remaining_size(&frame, 100));

    frame.header.type = ExpansionFrameTypeBaudRate;
    mu_assert_int_eq(1, expansion_frame_get_remaining_size(&frame, 0));
    mu_assert_int_eq(4, expansion_frame_get_remaining_size(&frame, 1));
    mu_assert_int_eq(0, expansion_frame_get_remaining_size(&frame, 5));
    mu_assert_int_eq(0, expansion_frame_get_remaining_size(&frame, 100));

    frame.header.type = ExpansionFrameTypeControl;
    mu_assert_int_eq(1, expansion_frame_get_remaining_size(&frame, 0));
    mu_assert_int_eq(1, expansion_frame_get_remaining_size(&frame, 1));
    mu_assert_int_eq(0, expansion_frame_get_remaining_size(&frame, 2));
    mu_assert_int_eq(0, expansion_frame_get_remaining_size(&frame, 100));

    frame.header.type = ExpansionFrameTypeData;
    frame.content.data.size = EXPANSION_MAX_DATA_SIZE;
    mu_assert_int_eq(1, expansion_frame_get_remaining_size(&frame, 0));
    mu_assert_int_eq(2, expansion_frame_get_remaining_size(&frame, 1));
    mu_assert_int_eq(1, expansion_frame_get_remaining_size(&frame, 2));
    mu_assert_int_eq(EXPANSION_MAX_DATA_SIZE, expansion_frame_get_remaining_size(&frame, 3));
    for(size_t i = 0; i <= EXPANSION_MAX_DATA_SIZE; ++i) {
        mu_assert_int_eq(
            EXPANSION_MAX_DATA_SIZE - i, expansion_frame_get_remaining_size(&frame, i + 3));
    }
    mu_assert_int_eq(0, expansion_frame_get_remaining_size(&frame, 100));
}

typedef struct {
    void* data_out;
    size_t size_available;
    size_t size_sent;
} TestExpansionSendStream;

static size_t test_expansion_send_callback(const uint8_t* data, size_t data_size, void* context) {
    TestExpansionSendStream* stream = context;
    const size_t size_sent = MIN(data_size, stream->size_available);

    memcpy(stream->data_out + stream->size_sent, data, size_sent);

    stream->size_available -= size_sent;
    stream->size_sent += size_sent;

    return size_sent;
}

MU_TEST(test_expansion_encode_frame) {
    const ExpansionFrame frame_in = {
        .header.type = ExpansionFrameTypeData,
        .content.data.size = 8,
        .content.data.bytes = {0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, 0xca, 0xfe},
    };

    ExpansionFrame frame_out;

    TestExpansionSendStream stream = {
        .data_out = &frame_out,
        .size_available = sizeof(frame_out),
        .size_sent = 0,
    };

    mu_assert(
        expansion_frame_encode(&frame_in, test_expansion_send_callback, &stream), "encode failed");
    mu_assert_int_eq(expansion_frame_get_encoded_size(&frame_in), stream.size_sent);
    mu_assert_mem_eq(&frame_in, &frame_out, stream.size_sent);
}

typedef struct {
    const void* data_in;
    size_t size_available;
    size_t size_received;
} TestExpansionReceiveStream;

static size_t test_expansion_receive_callback(uint8_t* data, size_t data_size, void* context) {
    TestExpansionReceiveStream* stream = context;
    const size_t size_received = MIN(data_size, stream->size_available);

    memcpy(data, stream->data_in + stream->size_received, size_received);

    stream->size_available -= size_received;
    stream->size_received += size_received;

    return size_received;
}

MU_TEST(test_expansion_decode_frame) {
    const ExpansionFrame frame_in = {
        .header.type = ExpansionFrameTypeData,
        .content.data.size = 8,
        .content.data.bytes = {0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, 0xca, 0xfe},
    };

    TestExpansionReceiveStream stream = {
        .data_in = &frame_in,
        .size_available = expansion_frame_get_encoded_size(&frame_in),
        .size_received = 0,
    };

    ExpansionFrame frame_out;
    mu_assert(
        expansion_frame_decode(&frame_out, test_expansion_receive_callback, &stream),
        "decode failed");
    mu_assert_int_eq(expansion_frame_get_encoded_size(&frame_in), stream.size_received);
    mu_assert_mem_eq(&frame_in, &frame_out, stream.size_received);
}

MU_TEST_SUITE(test_expansion_suite) {
    MU_RUN_TEST(test_expansion_encoded_size);
    MU_RUN_TEST(test_expansion_remaining_size);
    MU_RUN_TEST(test_expansion_encode_frame);
    MU_RUN_TEST(test_expansion_decode_frame);
}

int run_minunit_test_expansion() {
    MU_RUN_SUITE(test_expansion_suite);
    return MU_EXIT_CODE;
}
