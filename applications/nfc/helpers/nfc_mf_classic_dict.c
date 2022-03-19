#include "nfc_mf_classic_dict.h"

#include <flipper_format/flipper_format.h>
#include <lib/toolbox/args.h>

#define NFC_MF_CLASSIC_DICT_PATH "/ext/nfc/assets/mf_classic_dict.nfc"

#define NFC_MF_CLASSIC_LINE_LEN (13)

bool nfc_mf_classic_dict_check_presence(Storage* storage) {
    furi_assert(storage);
    return storage_common_stat(storage, NFC_MF_CLASSIC_DICT_PATH, NULL) == FSE_OK;
}

bool nfc_mf_classic_dict_open_file(Storage* storage, File* dict_file) {
    furi_assert(storage);
    furi_assert(dict_file);
    return storage_file_open(dict_file, NFC_MF_CLASSIC_DICT_PATH, FSAM_READ, FSOM_OPEN_EXISTING);
}

void nfc_mf_classic_dict_close_file(File* dict_file) {
    furi_assert(dict_file);
    storage_file_close(dict_file);
}

bool nfc_mf_classic_dict_get_next_key(File* dict_file, uint64_t* key) {
    furi_assert(dict_file);
    char key_str[NFC_MF_CLASSIC_LINE_LEN];
    *key = 0;
    uint8_t key_byte = 0;
    // TODO rework with correctly file comments processing
    uint16_t read_bytes = storage_file_read(dict_file, (void*)key_str, NFC_MF_CLASSIC_LINE_LEN);
    if(read_bytes != NFC_MF_CLASSIC_LINE_LEN) {
        return false;
    }
    for(uint8_t i = 0; i < 12; i += 2) {
        args_char_to_hex(key_str[i], key_str[i + 1], &key_byte);
        *key |= (uint64_t)key_byte << 8 * (5 - i / 2);
    }
    return true;
}

void nfc_mf_classic_dict_reset(File* dict_file) {
    furi_assert(dict_file);
    storage_file_seek(dict_file, 0, true);
}
