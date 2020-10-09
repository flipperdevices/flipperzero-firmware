#include <stdio.h>
#include <string.h>
#include "flipper_v2.h"
#include "log.h"

bool test_furi_valuemutex() {
    const uint32_t init_value = 0xdeadbeef;
    const uint32_t changed_value = 0x12345678;

    uint32_t value = init_value;
    bool result;
    ValueMutex valuemutex;

    // init mutex case
    result = init_mutex(&valuemutex, &value, sizeof(value));
    if(!result) {
        printf("init mutex failed\n");
        return false;
    }

    // acquire mutex case
    uint32_t* value_pointer = acquire_mutex(&valuemutex, 100);
    if(value_pointer != &value) {
        printf("acquire mutex is NULL\n");
        return false;
    }

    // second acquire mutex case
    uint32_t* value_pointer_second = acquire_mutex(&valuemutex, 100);
    if(value_pointer_second != NULL) {
        printf("acquire mutex is not NULL\n");
        return false;
    }

    // change value case
    *value_pointer = changed_value;
    if(value != changed_value) {
        printf("value wasn't changed\n");
        return false;
    }

    // release mutex case
    result = release_mutex(&valuemutex, &value);
    if(!result) {
        printf("release mutex failed\n");
        return false;
    }

    // TODO
    //acquire mutex blocking case
    //write mutex blocking case
    //read mutex blocking case

    return true;
}
