#include <stdio.h>
#include <string.h>
#include "flipper_v2.h"
#include "log.h"
#include "minunit.h"
#include "furi-new.h"

const int int_value_init = 0x1234;
const int int_value_changed = 0x5678;

osSemaphoreId_t new_record_available;
void another_app(void* p) {
    int another_test_value = int_value_init;
    new_furi_create("test/another_app_record", &another_test_value);
    osSemaphoreRelease(new_record_available);
    while(1) {
        osDelay(100);
    }
}

void test_furi_new() {
    int test_value = int_value_init;

    // init core
    new_furi_init();

    // test that "create" wont affect pointer value
    new_furi_create("test/record", &test_value);
    mu_assert_int_eq(test_value, int_value_init);

    // test that we get correct pointer
    int* test_value_pointer = new_furi_open_block("test/record");
    mu_assert_pointers_not_eq(test_value_pointer, NULL);
    mu_assert_pointers_eq(test_value_pointer, &test_value);

    *test_value_pointer = int_value_changed;
    mu_assert_int_eq(test_value, int_value_changed);

    // start another app
    new_record_available = osSemaphoreNew(1, 1, NULL);
    osSemaphoreAcquire(new_record_available, osWaitForever);

    osThreadAttr_t another_app_attr = {.name = "another_app", .stack_size = 512};
    osThreadId_t player = osThreadNew(another_app, NULL, &another_app_attr);

    // wait until app create record
    osSemaphoreAcquire(new_record_available, osWaitForever);

    // open record, test that record pointed to int_value_init
    test_value_pointer = new_furi_open_block("test/another_app_record");
    mu_assert_pointers_not_eq(test_value_pointer, NULL);
    mu_assert_int_eq(*test_value_pointer, int_value_init);

    // test that we can close, (unsubscribe) from record
    bool close_result = new_furi_close("test/another_app_record");
    mu_assert(close_result, "cannot close record");
}