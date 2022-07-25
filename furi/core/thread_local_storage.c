#include "thread_local_storage.h"
#include "FreeRTOS.h"
#include "task.h"
#include "check.h"

struct FuriThreadLocalStorage {
    string_t buffer;
};

void furi_thread_local_storage_init() {
    FuriThreadLocalStorage* tls = pvTaskGetThreadLocalStoragePointer(NULL, 0);
    furi_assert(tls == NULL);

    tls = malloc(sizeof(FuriThreadLocalStorage));
    string_init(tls->buffer);
    vTaskSetThreadLocalStoragePointer(NULL, 0, tls);
}

void furi_thread_local_storage_clear() {
    FuriThreadLocalStorage* tls = pvTaskGetThreadLocalStoragePointer(NULL, 0);
    furi_assert(tls != NULL);
    vTaskSetThreadLocalStoragePointer(NULL, 0, NULL);
    string_clear(tls->buffer);
    free(tls);
}

string_ptr furi_thread_local_storage_get_buffer() {
    FuriThreadLocalStorage* tls = pvTaskGetThreadLocalStoragePointer(NULL, 0);
    furi_assert(tls != NULL);
    return tls->buffer;
}