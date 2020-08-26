#include <stdio.h>
#include "flipper.h"
#include "log.h"

#include "flipper-core.h"

bool test_furi_ac_create_kill(FILE* log);
bool test_furi_ac_switch_exit(FILE* log);

bool test_furi_pipe_record(FILE* log);
bool test_furi_holding_data(FILE* log);
bool test_furi_concurrent_access(FILE* log);
bool test_furi_nonexistent_data(FILE* log);
bool test_furi_mute_algorithm(FILE* log);

void flipper_test_app(void* p) {
    FILE* log = get_default_log();
    
    if(test_furi_ac_create_kill(log)) {
        fprintf(log, "[TEST] test_furi_ac_create_kill PASSED\n");
    } else {
        fprintf(log, "[TEST] test_furi_ac_create_kill FAILED\n");
    }

    if(test_furi_ac_switch_exit(log)) {
        fprintf(log, "[TEST] test_furi_ac_switch_exit PASSED\n");
    } else {
        fprintf(log, "[TEST] test_furi_ac_switch_exit FAILED\n");
    }

    if(test_furi_pipe_record(log)) {
        fprintf(log, "[TEST] test_furi_pipe_record PASSED\n");
    } else {
        fprintf(log, "[TEST] test_furi_pipe_record FAILED\n");
    }

    if(test_furi_holding_data(log)) {
        fprintf(log, "[TEST] test_furi_holding_data PASSED\n");
    } else {
        fprintf(log, "[TEST] test_furi_holding_data FAILED\n");
    }

    if(test_furi_concurrent_access(log)) {
        fprintf(log, "[TEST] test_furi_concurrent_access PASSED\n");
    } else {
        fprintf(log, "[TEST] test_furi_concurrent_access FAILED\n");
    }

    if(test_furi_nonexistent_data(log)) {
        fprintf(log, "[TEST] test_furi_nonexistent_data PASSED\n");
    } else {
        fprintf(log, "[TEST] test_furi_nonexistent_data FAILED\n");
    }

    if(test_furi_mute_algorithm(log)) {
        fprintf(log, "[TEST] test_furi_mute_algorithm PASSED\n");
    } else {
        fprintf(log, "[TEST] test_furi_mute_algorithm FAILED\n");
    }

    if(add(1, 2) == 3) {
        fprintf(log, "[TEST] Rust add PASSED\n");
    } else {
        fprintf(log, "[TEST] Rust add FAILED\n");
    }

    furiac_exit(NULL);
}