#pragma once

#include <furi.h>

typedef struct GenieFile GenieFile;

GenieFile* genie_file_load(const char* path);
void genie_file_free(GenieFile* file);
uint32_t genie_file_get_key_hi(GenieFile* file);
uint32_t genie_file_get_key_lo(GenieFile* file);
uint16_t genie_file_get_last_sent(GenieFile* file);
uint16_t genie_file_get_rec_count(GenieFile* file);
void genie_file_set_last_sent(const char* genie_path, uint16_t last_sent);

bool genie_save(uint32_t count, FuriString* key);
uint32_t genie_save_bin(const char* key);
uint32_t genie_load();
void genie_file_init();