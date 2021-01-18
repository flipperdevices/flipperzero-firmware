#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void furi_record_init();

void furi_record_create(const char* name, void* data);

bool furi_record_destroy(const char* name);

void* furi_record_open(const char* name);

void furi_record_close(const char* name);

static inline bool furi_create(const char* name, void* data) {
    furi_record_create(name, data);
    return true;
}

#define furi_open furi_record_open

#ifdef __cplusplus
}
#endif
