#pragma once

#include <furi.h>

typedef struct FxbmFile FxbmFile;

FxbmFile* fxbm_alloc(const char* path);
void fxbm_free(FxbmFile* fxbm);
uint32_t fxbm_get_width(FxbmFile* fxbm);
uint32_t fxbm_get_height(FxbmFile* fxbm);
uint8_t* fxbm_get_data(FxbmFile* fxbm);
