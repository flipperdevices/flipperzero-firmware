#include <stdio.h>
#include "flipper.h"
#include "log.h"

#include "minunit.h"

bool test_furi_ac_create_kill(FuriRecordSubscriber* log);
bool test_furi_ac_switch_exit(FuriRecordSubscriber* log);

bool test_furi_pipe_record(FuriRecordSubscriber* log);
bool test_furi_holding_data(FuriRecordSubscriber* log);
bool test_furi_concurrent_access(FuriRecordSubscriber* log);
bool test_furi_nonexistent_data(FuriRecordSubscriber* log);
bool test_furi_mute_algorithm(FuriRecordSubscriber* log);

static int foo = 0;

FuriRecordSubscriber* test_log = NULL;

void test_setup(void) {
	foo = 7;
    test_log = get_default_log();
}

void test_teardown(void) {
	/* Nothing */
}

MU_TEST(test_check) {
	mu_check(foo == 6);
}

MU_TEST(mu_test_furi_ac_create_kill) {
	mu_assert_int_eq(test_furi_ac_create_kill(test_log), true);
}

MU_TEST(mu_test_furi_ac_switch_exit) {
	mu_assert_int_eq(test_furi_ac_switch_exit(test_log), true);
}

MU_TEST(mu_test_furi_pipe_record) {
	mu_assert_int_eq(test_furi_pipe_record(test_log), true);
}

MU_TEST(mu_test_furi_holding_data) {
	mu_assert_int_eq(test_furi_holding_data(test_log), true);
}

MU_TEST(mu_test_furi_concurrent_access) {
	mu_assert_int_eq(test_furi_concurrent_access(test_log), true);
}

MU_TEST(mu_test_furi_nonexistent_data) {
	mu_assert_int_eq(test_furi_nonexistent_data(test_log), true);
}

/*
MU_TEST(mu_test_furi_mute_algorithm) {
	mu_assert_int_eq(test_furi_mute_algorithm(test_log), true);
}
*/

MU_TEST_SUITE(test_suite) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test_check);
    MU_RUN_TEST(mu_test_furi_ac_create_kill);
	MU_RUN_TEST(mu_test_furi_ac_switch_exit);
	MU_RUN_TEST(mu_test_furi_pipe_record);
	MU_RUN_TEST(mu_test_furi_holding_data);
	MU_RUN_TEST(mu_test_furi_concurrent_access);
	MU_RUN_TEST(mu_test_furi_nonexistent_data);
	// MU_RUN_TEST(mu_test_furi_mute_algorithm);
}

int run_minunit() {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	fuprintf(test_log, "\n\n%d tests, %d assertions, %d failures\n", 0, 1, 2);
	delay(100);
	return MU_EXIT_CODE;
}