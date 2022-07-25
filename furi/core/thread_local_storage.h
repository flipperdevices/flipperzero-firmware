#pragma once
#include <m-string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FuriThreadLocalStorage FuriThreadLocalStorage;

void furi_thread_local_storage_init();

void furi_thread_local_storage_clear();

string_ptr furi_thread_local_storage_get_buffer();

#ifdef __cplusplus
}
#endif
