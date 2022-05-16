#include "../minunit.h"
#include <furi.h>
#include <toolbox/pulse_joiner.h>

MU_TEST(test_pulse_joiner) {
    size_t period = 0;
    size_t pulse = 0;
    PulseJoiner* joiner = pulse_joiner_alloc();

    // 50 | 50
    //      ___
    // ___/
    mu_check(pulse_joiner_push_pulse(joiner, false, 100, 50) != 1);

    // 50 | 50
    // ___
    //    \___
    mu_check(pulse_joiner_push_pulse(joiner, true, 100, 50) != 1);

    // 10 | 10
    // ___
    //    \___
    mu_check(pulse_joiner_push_pulse(joiner, true, 20, 10) == 1);

    // total:
    // 100 | 50
    // ___
    //    \___
    pulse_joiner_pop_pulse(joiner, &period, &pulse);

    mu_assert_int_eq(150, period);
    mu_assert_int_eq(100, pulse);

    // 50 | 50
    // ___
    //    \___
    mu_check(pulse_joiner_push_pulse(joiner, true, 100, 50) == 1);

    // total:
    // 20 | 10
    // ___
    //    \___
    pulse_joiner_pop_pulse(joiner, &period, &pulse);

    mu_assert_int_eq(20, period);
    mu_assert_int_eq(10, pulse);

    // 50 | 50
    //      ___
    // ___/
    mu_check(pulse_joiner_push_pulse(joiner, false, 100, 50) == 1);

    // total:
    // 100 | 50
    // ___
    //    \___
    pulse_joiner_pop_pulse(joiner, &period, &pulse);

    mu_assert_int_eq(150, period);
    mu_assert_int_eq(50, pulse);

    // 50 | 50
    //      ___
    // ___/
    mu_check(pulse_joiner_push_pulse(joiner, false, 100, 50) != 1);

    // 50 | 50
    //      ___
    // ___/
    mu_check(pulse_joiner_push_pulse(joiner, false, 100, 50) == 1);

    // total:
    // 50 | 50
    // ___
    //    \___
    pulse_joiner_pop_pulse(joiner, &period, &pulse);

    mu_assert_int_eq(100, period);
    mu_assert_int_eq(50, pulse);

    pulse_joiner_free(joiner);
}

MU_TEST_SUITE(test_pulse_joiner_suite) {
    MU_RUN_TEST(test_pulse_joiner);
}

int run_minunit_test_pulse_joiner() {
    MU_RUN_SUITE(test_pulse_joiner_suite);
    return MU_EXIT_CODE;
}