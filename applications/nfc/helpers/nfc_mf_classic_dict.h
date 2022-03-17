#pragma once

#include <stdbool.h>
#include <storage/storage.h>

bool nfc_mf_classic_dict_check_presence(Storage* storage);

bool nfc_mf_classic_dict_open_file(Storage* storage, File* dict_file);

void nfc_mf_classic_dict_close_file(File* dict_file);

bool nfc_mf_classic_dict_get_next_key(File* dict_file, uint64_t* key);

void nfc_mf_classic_dict_reset(File* dict_file);
