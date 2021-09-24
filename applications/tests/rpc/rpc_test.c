#include "furi-hal-delay.h"
#include "furi/check.h"
#include "furi/record.h"
#include "rpc/rpc.h"
#include <furi.h>
#include "../minunit.h"
#include <stdint.h>
#include <stream_buffer.h>

static RpcInstance* rpc = NULL;
static RpcSession* session = NULL;
static StreamBufferHandle_t output_stream = NULL;

#define MAX_RECEIVE_OUTPUT_TIMEOUT      3000

#define DEFINE_BYTES(x, b)                  \
RpcTestBytes (x) = {                        \
    .bytes = (b),                           \
    .size = sizeof(b),                      \
};


static void test_setup(void) {
    furi_assert(!rpc);
    furi_assert(!session);
    furi_assert(!output_stream);

    rpc = furi_record_open("rpc");
    for (int i = 0; !session && (i < 10000); ++i) {
        session = rpc_open_session(rpc);
        delay(1);
    }

    output_stream = xStreamBufferCreate(256, 1);
    mu_assert(session, "failed to start session");
}

static void test_teardown(void) {
    rpc_close_session(session);
    furi_record_close("rpc");
    vStreamBufferDelete(output_stream);
    output_stream = NULL;
    rpc = NULL;
    session = NULL;
}

typedef struct {
    uint8_t* bytes;
    size_t size;
} RpcTestBytes;

static void output_bytes_callback(void* ctx, uint8_t* got_bytes, size_t got_size) {
    size_t bytes_sent = xStreamBufferSend(output_stream, got_bytes, got_size, osWaitForever);
    furi_assert(bytes_sent == got_size);
}

static void compare_expected_bytes(RpcTestBytes* expected) {
    furi_assert(expected);
    uint8_t* got_bytes = furi_alloc(expected->size);

    size_t got_size = xStreamBufferReceive(output_stream, got_bytes, expected->size, MAX_RECEIVE_OUTPUT_TIMEOUT);
    mu_check(got_size == expected->size);

    for (size_t i = 0; i < got_size; ++i) {
        mu_check(got_bytes[i] == expected->bytes[i]);
    }

    free(got_bytes);
}

//static void run_single_message_session(uint8_t* send_bytes, size_t send_bytes_size, uint8_t* expected_bytes, size_t expected_bytes_size) {
//}

MU_TEST(test_ping_raw) {

// command_id: 0x123
// command_status: 4
// not_last: 0
// ping_request: {
// }
    static uint8_t input_ping_request[] = {0x07, 0x08, 0xA3, 0x02, 0x10, 0x04, 0x22, 0x00};

// command_id: 0x123
// command_status: 0
// not_last: 0
// ping_response: {
// }
    static uint8_t expected_ping_response[] = {0x05, 0x08, 0xA3, 0x02, 0x2A, 0x00};

    DEFINE_BYTES(input, input_ping_request);
    DEFINE_BYTES(expected, expected_ping_response);
    rpc_set_send_bytes_callback(session, output_bytes_callback, NULL);
    size_t bytes_sent = rpc_feed_bytes(session, input.bytes, input.size, 0);
    furi_check(bytes_sent == input.size);

    compare_expected_bytes(&expected);
}


MU_TEST(test_storage_list) {
// command_id: 0x123
// command_status: 0
// not_last: 0
// storage_list_request: {
//     path:"/E/music/kirkorov"
// }
    static uint8_t input_bytes_list[] = {0x08, 0xA3, 0x02, 0x32, 0x13, 0x0A, 0x11, 0x2F, 0x45, 0x2F, 0x6D, 0x75, 0x73, 0x69, 0x63, 0x2F, 0x6B, 0x69, 0x72, 0x6B, 0x6F, 0x72, 0x6F, 0x76};

    DEFINE_BYTES(input_bytes, input_bytes_list);
    size_t bytes_sent = rpc_feed_bytes(session, input_bytes.bytes, input_bytes.size, 0);
    furi_check(bytes_sent == input_bytes.size);
}

MU_TEST_SUITE(test_irda_decoder_encoder) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

    volatile int a = 1;
    if (a) {
        MU_RUN_TEST(test_ping_raw);
    } else {
        MU_RUN_TEST(test_storage_list);
    }
}

int run_minunit_test_rpc() {
    MU_RUN_SUITE(test_irda_decoder_encoder);
    MU_REPORT();

    return MU_EXIT_CODE;
}
