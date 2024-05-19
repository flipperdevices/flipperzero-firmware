#pragma once

typedef struct {
    int (*run)(void);
    int (*get_minunit_run)(void);
    int (*get_minunit_assert)(void);
    int (*get_minunit_fail)(void);
    int (*get_minunit_status)(void);
} TestApi;

#define TEST_API_DEFINE(entrypoint)               \
    TestApi test_api = {                          \
        .run = entrypoint,                        \
        .get_minunit_run = get_minunit_run,       \
        .get_minunit_assert = get_minunit_assert, \
        .get_minunit_fail = get_minunit_fail,     \
        .get_minunit_status = get_minunit_status, \
    };                                            \
    TestApi* get_api(void) {                      \
        return &test_api;                         \
    }
