#pragma once

#include <furi.h>

bool genie_save(uint32_t count, FuriString* key);
uint32_t genie_save_bin(const char* key);
uint32_t genie_load();
void genie_file_init();