#include "subghz_keystore.h"

#include <furi.h>
#include <furi-hal.h>

#include <storage/storage.h>
#include <lib/toolbox/flipper-file.h>

#define SUBGHZ_KEYSTORE_TAG "SubGhzParser"

#define FILE_BUFFER_SIZE 64

#define SUBGHZ_KEYSTORE_FILE_TYPE "Flipper SubGhz Keystore File"
#define SUBGHZ_KEYSTORE_FILE_VERSION 0

#define SUBGHZ_KEYSTORE_FILE_DECRYPTED_LINE_SIZE 512
#define SUBGHZ_KEYSTORE_FILE_ENCRYPTED_LINE_SIZE (SUBGHZ_KEYSTORE_FILE_DECRYPTED_LINE_SIZE*2)

typedef enum {
    SubGhzKeystoreEncryptionNone,
    SubGhzKeystoreEncryptionAES256,
} SubGhzKeystoreEncryption;

struct SubGhzKeystore {
    SubGhzKeyArray_t data;
};

SubGhzKeystore* subghz_keystore_alloc() {
    SubGhzKeystore* instance = furi_alloc(sizeof(SubGhzKeystore));

    SubGhzKeyArray_init(instance->data);

    return instance;
}

void subghz_keystore_free(SubGhzKeystore* instance) {
    furi_assert(instance);

    for
    M_EACH(manufacture_code, instance->data, SubGhzKeyArray_t) {
        string_clear(manufacture_code->name);
        manufacture_code->key = 0;
    }
    SubGhzKeyArray_clear(instance->data);

    free(instance);
}

static void subghz_keystore_add_key(SubGhzKeystore* instance, const char* name, uint64_t key, uint16_t type) {
    SubGhzKey* manufacture_code = SubGhzKeyArray_push_raw(instance->data);
    string_init_set_str(manufacture_code->name, name);
    manufacture_code->key = key;
    manufacture_code->type = type;
}

static void subghz_keystore_process_line(SubGhzKeystore* instance, char* line) {
    uint64_t key = 0;
    uint16_t type = 0;
    char skey[17] = {0};
    char name[65] = {0};
    int ret = sscanf(line, "%16s:%hu:%64s", skey, &type, name);
    key = strtoull(skey, NULL, 16);
    if (ret == 3) {
        subghz_keystore_add_key(instance, name, key, type);
    } else {
        FURI_LOG_E(SUBGHZ_KEYSTORE_TAG, "Failed to load line: %s\r\n", line);
    }
}

bool subghz_keystore_load(SubGhzKeystore* instance, const char* file_name) {
    furi_assert(instance);
    bool result = false;
    char buffer[FILE_BUFFER_SIZE];
    uint8_t iv[16];
    size_t ret;
    uint32_t version;
    SubGhzKeystoreEncryption encryption;

    string_t filetype;
    string_init(filetype);
    string_t line;
    string_init(line);

    Storage* storage = furi_record_open("storage");

    FlipperFile* flipper_file = flipper_file_alloc(storage);
    do {
        if(!flipper_file_open_read(flipper_file, file_name)) {
            FURI_LOG_E(SUBGHZ_KEYSTORE_TAG, "Unable to open file for read: %s", file_name);
            break;
        }
        if(!flipper_file_read_header(flipper_file, filetype, &version)) {
            FURI_LOG_E(SUBGHZ_KEYSTORE_TAG, "Unable to get header");
            break;
        }
        if(!flipper_file_read_uint32(flipper_file, "Encryption", (uint32_t*)&encryption)) {
            FURI_LOG_E(SUBGHZ_KEYSTORE_TAG, "Unable to get encryption");
            break;
        }

        if (strcmp(string_get_cstr(filetype), SUBGHZ_KEYSTORE_FILE_TYPE) != 0
            || version != SUBGHZ_KEYSTORE_FILE_VERSION) {
            FURI_LOG_E(SUBGHZ_KEYSTORE_TAG, "Type or version mismatch");
            break;
        }

        File* file = flipper_file_get_file(flipper_file);
        if (encryption == SubGhzKeystoreEncryptionNone) {
            do {
                ret = storage_file_read(file, buffer, FILE_BUFFER_SIZE);
                for (uint16_t i=0; i < ret; i++) {
                    if (buffer[i] == '\n' && string_size(line) > 0) {
                        subghz_keystore_process_line(instance, string_get_cstr(line));
                        string_clean(line);
                    } else {
                        string_push_back(line, buffer[i]);
                    }
                }
            } while(ret > 0);
        }else if (encryption == SubGhzKeystoreEncryptionAES256) {
            furi_hal_crypto_store_load_key(1, iv);
            do {
                ret = storage_file_read(file, buffer, FILE_BUFFER_SIZE);
                for (uint16_t i=0; i < ret; i++) {
                    if (buffer[i] == '\n' && string_size(line) > 0) {
                        subghz_keystore_process_line(instance, line);
                        string_clean(line);
                    } else {
                        string_push_back(line, buffer[i]);
                    }
                }
            } while(ret > 0);
            furi_hal_crypto_store_unload_key(1);
        } else {
            break;
        }

        result = true;
    } while(0);
    flipper_file_close(flipper_file);
    flipper_file_free(flipper_file);

    furi_record_close("storage");

    string_clear(line);
    string_clear(filetype);

    return result;
}

bool subghz_keystore_save(SubGhzKeystore* instance, const char* file_name, uint8_t* iv) {
    furi_assert(instance);
    bool result = false;

    Storage* storage = furi_record_open("storage");
    char* decrypted_line = furi_alloc(SUBGHZ_KEYSTORE_FILE_DECRYPTED_LINE_SIZE);
    char* encrypted_line = furi_alloc(SUBGHZ_KEYSTORE_FILE_ENCRYPTED_LINE_SIZE);

    FlipperFile* flipper_file = flipper_file_alloc(storage);
    do {
        if(!flipper_file_new_write(flipper_file, file_name)) {
            FURI_LOG_E(SUBGHZ_KEYSTORE_TAG, "Unable to open file for write: %s", file_name);
            break;
        }
        if(!flipper_file_write_header_cstr(flipper_file, SUBGHZ_KEYSTORE_FILE_TYPE, SUBGHZ_KEYSTORE_FILE_VERSION)) {
            FURI_LOG_E(SUBGHZ_KEYSTORE_TAG, "Unable to add header");
            break;
        }
        if(!flipper_file_write_uint32(flipper_file, "Encryption", SubGhzKeystoreEncryptionAES256)) {
            FURI_LOG_E(SUBGHZ_KEYSTORE_TAG, "Unable to add Encryption");
            break;
        }
        if(!flipper_file_write_hex_array(flipper_file, "IV", iv, 16)) {
            FURI_LOG_E(SUBGHZ_KEYSTORE_TAG, "Unable to add IV");
            break;
        }

        if(!furi_hal_crypto_store_load_key(1, iv)) {
            FURI_LOG_E(SUBGHZ_KEYSTORE_TAG, "Unable to load encryption key");
            break;
        }

        File* file = flipper_file_get_file(flipper_file);
        size_t encrypted_line_count = 0;
        for
            M_EACH(
                key,
                instance->data,
                SubGhzKeyArray_t) {
                // Wipe buffer before packing
                memset(decrypted_line, 0, SUBGHZ_KEYSTORE_FILE_DECRYPTED_LINE_SIZE);
                memset(encrypted_line, 0, SUBGHZ_KEYSTORE_FILE_ENCRYPTED_LINE_SIZE);
                // Form unecreypted line
                int len = snprintf(
                    decrypted_line, SUBGHZ_KEYSTORE_FILE_DECRYPTED_LINE_SIZE, 
                    "%08lX%08lX:%hu:%s",
                    (uint32_t)(key->key>>32), (uint32_t)key->key, key->type, string_get_cstr(key->name));
                // Verify length and align
                furi_assert(len > 0);
                if (len % 16 != 0) {
                    len += (16 - len % 16);
                }
                furi_assert(len % 16 == 0);
                furi_assert(len <= SUBGHZ_KEYSTORE_FILE_DECRYPTED_LINE_SIZE);
                // Form encreypted line
                if(!furi_hal_crypto_encrypt((uint8_t*)decrypted_line, (uint8_t*)encrypted_line, len)) {
                    FURI_LOG_E(SUBGHZ_KEYSTORE_TAG, "Encryption failed");
                    break;
                }
                // HEX Encode encrypted line
                const char xx[]= "0123456789ABCDEF";
                for (size_t i=0; i<len; i++) {
                    size_t cursor = len - i - 1;
                    size_t hex_cursor = len*2 - i*2 - 1;
                    encrypted_line[hex_cursor] = xx[encrypted_line[cursor] & 0xF];
                    encrypted_line[hex_cursor-1] = xx[(encrypted_line[cursor]>>4) & 0xF];
                }
                storage_file_write(file, encrypted_line, strlen(encrypted_line));
                storage_file_write(file, "\n", 1);
                encrypted_line_count++;

                FURI_LOG_I(SUBGHZ_KEYSTORE_TAG, "Encrypted: `%s` -> `%s`", decrypted_line, encrypted_line);
            }
        furi_hal_crypto_store_unload_key(1);
        result = encrypted_line_count == SubGhzKeyArray_size(instance->data);
    } while(0);
    flipper_file_close(flipper_file);
    flipper_file_free(flipper_file);

    free(encrypted_line);
    free(decrypted_line);
    furi_record_close("storage");

    return result;
}

SubGhzKeyArray_t* subghz_keystore_get_data(SubGhzKeystore* instance) {
    furi_assert(instance);
    return &instance->data;
}
