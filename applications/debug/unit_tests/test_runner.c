#include "test_runner.h"

#include <cli/cli.h>
#include <toolbox/path.h>
#include <loader/loader.h>
#include <storage/storage.h>
#include <notification/notification_messages.h>

#define TAG "TestRunner"

#define PLUGINS_PATH APP_DATA_PATH("plugins")

struct TestRunner {
    Storage* storage;
    Loader* loader;
    NotificationApp* notification;

    // Temporary used things
    Cli* cli;
    FuriString* args;

    // Report data
    int minunit_run;
    int minunit_assert;
    int minunit_fail;
    int minunit_status;
};

TestRunner* test_runner_alloc(Cli* cli, FuriString* args) {
    TestRunner* instance = malloc(sizeof(TestRunner));

    instance->storage = furi_record_open(RECORD_STORAGE);
    instance->loader = furi_record_open(RECORD_LOADER);
    instance->notification = furi_record_open(RECORD_NOTIFICATION);

    instance->cli = cli;
    instance->args = args;

    return instance;
}

void test_runner_free(TestRunner* instance) {
    furi_assert(instance);

    furi_record_close(RECORD_NOTIFICATION);
    instance->notification = NULL;

    furi_record_close(RECORD_LOADER);
    instance->loader = NULL;

    furi_record_close(RECORD_STORAGE);
    instance->storage = NULL;

    free(instance);
}

void test_runner_run_internal(TestRunner* instance) {
    furi_assert(instance);

    char file_name_buffer[256];
    FuriString* file_name = furi_string_alloc();
    File* directory = storage_file_alloc(instance->storage);

    do {
        if(!storage_dir_open(directory, PLUGINS_PATH)) {
            FURI_LOG_E(TAG, "Failed to open directory %s", PLUGINS_PATH);
            break;
        }

        while(true) {
            if(cli_cmd_interrupt_received(instance->cli)) {
                break;
            }

            if(!storage_dir_read(directory, NULL, file_name_buffer, sizeof(file_name_buffer))) {
                break;
            }

            furi_string_set(file_name, file_name_buffer);
            if(!furi_string_end_with_str(file_name, ".fal")) {
                continue;
            }

            path_concat(PLUGINS_PATH, file_name_buffer, file_name);
            FURI_LOG_D(TAG, "Loading %s", furi_string_get_cstr(file_name));

            // if(furi_string_size(instance->args)) {
            //     if(furi_string_cmp_str(instance->args, unit_tests[i].name) == 0) {
            //         unit_tests[i].entry();
            //     } else {
            //         printf("Skipping %s\r\n", unit_tests[i].name);
            //     }
            // } else {
            //     unit_tests[i].entry();
            // }
        }
    } while(false);

    storage_dir_close(directory);
    storage_file_free(directory);
    furi_string_free(file_name);
}

void test_runner_run(TestRunner* instance) {
    furi_assert(instance);

    // TODO FL-3491: lock device while test running
    if(loader_is_locked(instance->loader)) {
        printf("RPC: stop all applications to run tests\r\n");
        notification_message(instance->notification, &sequence_blink_magenta_100);
    } else {
        notification_message_block(instance->notification, &sequence_set_only_blue_255);

        uint32_t heap_before = memmgr_get_free_heap();
        uint32_t cycle_counter = furi_get_tick();

        test_runner_run_internal(instance);

        if(instance->minunit_run != 0) {
            printf("\r\nFailed tests: %u\r\n", instance->minunit_fail);

            // Time report
            cycle_counter = (furi_get_tick() - cycle_counter);
            printf("Consumed: %lu ms\r\n", cycle_counter);

            // Wait for tested services and apps to deallocate memory
            furi_delay_ms(200);
            uint32_t heap_after = memmgr_get_free_heap();
            printf("Leaked: %ld\r\n", heap_before - heap_after);

            // Final Report
            if(instance->minunit_fail == 0) {
                notification_message(instance->notification, &sequence_success);
                printf("Status: PASSED\r\n");
            } else {
                notification_message(instance->notification, &sequence_error);
                printf("Status: FAILED\r\n");
            }
        }
    }
}
