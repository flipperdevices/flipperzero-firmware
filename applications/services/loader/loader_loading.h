#pragma once
#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LoaderLoading LoaderLoading;

LoaderLoading* loader_loading_alloc();

void loader_loading_free(LoaderLoading* loader_loading);

#ifdef __cplusplus
}
#endif