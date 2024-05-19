#pragma once

typedef struct {
    int (*run)(void);
    int (*get_minunit_run)(void);
    int (*get_minunit_assert)(void);
    int (*get_minunit_fail)(void);
    int (*get_minunit_status)(void);
} TestApi;