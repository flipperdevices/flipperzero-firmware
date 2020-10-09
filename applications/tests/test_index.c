#include <stdio.h>
#include "flipper.h"
#include "log.h"

// #include "flipper-core.h" TODO: Rust build disabled

int run_minunit();

void flipper_test_app(void* p) {
    set_exitcode(run_minunit());
    furiac_exit(NULL);
}