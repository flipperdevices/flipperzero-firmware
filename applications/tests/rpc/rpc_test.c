#include "furi/record.h"
#include "rpc/rpc.h"
#include <furi.h>
#include "../minunit.h"
#include <stdint.h>

static RpcInstance* rpc;
static RpcSession* session;

static void test_setup(void) {
    rpc = furi_record_open("rpc");
    session = rpc_open_session(rpc);
}

static void test_teardown(void) {
    rpc_close_session(session);
    furi_record_close("rpc");
    rpc = NULL;
    session = NULL;
}

typedef struct {
    uint8_t* bytes;
    size_t size;
} RpcTestBytes;

static void compare_expected_bytes(void* ctx, uint8_t* got_bytes, size_t got_size) {
    RpcTestBytes* expected = ctx;
    mu_check(got_size == expected->size);

    for (size_t i = 0; i < got_size; ++i) {
        mu_check(got_bytes[i] ==  expected->bytes[i]);
    }
}

#define DEFINE_BYTES(x, b)                  \
RpcTestBytes (x) = {                        \
    .bytes = (b),                           \
    .size = sizeof(b),                      \
};


//static void run_single_message_session(uint8_t* send_bytes, size_t send_bytes_size, uint8_t* expected_bytes, size_t expected_bytes_size) {
//}

MU_TEST(test_ping_raw) {

// command_id: 0x123
// command_status: 4
// not_last: 0
// ping_request: {
// }
    static uint8_t input_bytes_ping_request[] = {0x08, 0xA3, 0x02, 0x10, 0x04, 0x22, 0x00};

// command_id: 0x123
// command_status: 0
// not_last: 0
// ping_response: {
// }
    static uint8_t expected_bytes_ping_response[] = {0x08, 0xA3, 0x02, 0x2A, 0x00};

    DEFINE_BYTES(input_bytes, input_bytes_ping_request);
    DEFINE_BYTES(expected_bytes, expected_bytes_ping_response);
    rpc_set_send_bytes_callback(session, compare_expected_bytes, &expected_bytes);
    size_t bytes_sent = rpc_feed_bytes(session, input_bytes.bytes, input_bytes.size, 0);
    furi_check(bytes_sent == input_bytes.size);
}

MU_TEST_SUITE(test_irda_decoder_encoder) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

    MU_RUN_TEST(test_ping_raw);
}

int run_minunit_test_rpc() {
    MU_RUN_SUITE(test_irda_decoder_encoder);
    MU_REPORT();

    return MU_EXIT_CODE;
}
