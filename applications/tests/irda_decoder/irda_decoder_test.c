#if 1
#include <string.h>
#include <furi.h>
#include "../minunit.h"
#include "irda.h"
#include "irda_decoder_nec_test_data.h"


#define RUN_DECODER(data,expected)      run_decoder((data), COUNT_OF(data), (expected), COUNT_OF(expected))
#define COUNT_OF(x)                     (sizeof(x) / sizeof(x[0]))


static IrdaHandler* decoder;


static void test_setup(void) {
    decoder = irda_init_decoder();
}

static void test_teardown(void) {
    irda_free_decoder(decoder);
}

static void compare_message_results(const IrdaMessage *message_decoded,
                                    const IrdaMessage *message_expected) {
    mu_check(!strcmp(message_decoded->protocol_name, message_expected->protocol_name));
    mu_check(message_decoded->command == message_expected->command);
    mu_check(message_decoded->address == message_expected->address);
    mu_check(message_decoded->repeat == message_expected->repeat);
}

static void run_decoder(const uint32_t* input_delays, uint32_t input_delays_len,
                        const IrdaMessage* message_expected, uint32_t message_expected_len) {
    const IrdaMessage* message_decoded = 0;
    bool level = 1;
    uint32_t message_counter = 0;

    for (uint32_t i = 0; i < input_delays_len; ++i) {
        message_decoded = irda_decode(decoder, level, input_delays[i]);
        if (message_decoded) {
            mu_check(message_counter < message_expected_len);
            compare_message_results(message_decoded, &message_expected[message_counter]);
            ++message_counter;
        }
        level = !level;
    }

    mu_check(message_counter == message_expected_len);
}

MU_TEST(test_nec1) {
    RUN_DECODER(test_nec_input1, test_nec_expected1);
}

MU_TEST(test_nec2) {
    RUN_DECODER(test_nec_input2, test_nec_expected2);
}

MU_TEST(test_unexpected_end_in_sequence) {
    RUN_DECODER(test_nec_input1, test_nec_expected1);  // test_nec_input1 and test_nec_input2 shuts unexpected
    RUN_DECODER(test_nec_input1, test_nec_expected1);
    RUN_DECODER(test_nec_input2, test_nec_expected2);
    RUN_DECODER(test_nec_input2, test_nec_expected2);
}

MU_TEST_SUITE(test_irda_decoder) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

    MU_RUN_TEST(test_unexpected_end_in_sequence);
    MU_RUN_TEST(test_nec1);
    MU_RUN_TEST(test_nec2);
}

int run_minunit_test_irda_decoder() {
    MU_RUN_SUITE(test_irda_decoder);
    MU_REPORT();

    return MU_EXIT_CODE;
}
#endif
