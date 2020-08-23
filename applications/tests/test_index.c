#include <stdio.h>
#include "flipper.h"
#include "debug.h"

#include "furiac_test.c"

void flipper_test_app(void* p) {
    FILE* debug_uart = get_debug();

    if(furi_ac_create_kill(debug_uart)) {
        fprintf(debug_uart, "[TEST] furi_ac_create_kill PASSED\n");
    } else {
        fprintf(debug_uart, "[TEST] furi_ac_create_kill FAILED\n");
    }

    if(furi_ac_switch_exit(debug_uart)) {
        fprintf(debug_uart, "[TEST] furi_ac_switch_exit PASSED\n");
    } else {
        fprintf(debug_uart, "[TEST] furi_ac_switch_exit FAILED\n");
    }

    furiac_exit(NULL);
}