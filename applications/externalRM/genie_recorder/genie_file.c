#include <storage/storage.h>
#include <flipper_format.h>

#define GENIE_APPS_DATA_FOLDER EXT_PATH("apps_data")
#define GENIE_SAVE_FOLDER      \
    GENIE_APPS_DATA_FOLDER "/" \
                           "genie"
#define GENIE_SAVE_NAME "keys"
#define GENIE_SAVE_EXTENSION ".txt"

// Should match application.fam, fap_version.
#define GENIE_MAJOR_VERSION 2
#define GENIE_MINOR_VERSION 5

#define GENIE_MAGIC_CODE 0x472A

#ifdef TAG
#undef TAG
#endif
#define TAG "GenieFile"

static void ensure_dir_exists(Storage* storage, char* dir) {
    if(!storage_dir_exists(storage, dir)) {
        FURI_LOG_I(TAG, "Creating directory: %s", dir);
        storage_simply_mkdir(storage, dir);
    } else {
        FURI_LOG_D(TAG, "Directory exists: %s", dir);
    }
}

static void ensure_save_folder_exists(Storage* storage) {
    ensure_dir_exists(storage, GENIE_APPS_DATA_FOLDER);
    ensure_dir_exists(storage, GENIE_SAVE_FOLDER);
}

void genie_file_init() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    ensure_save_folder_exists(storage);
    furi_record_close(RECORD_STORAGE);
}

static uint16_t storage_file_read16(File* file) {
    uint16_t read = 0;
    char buffer[2] = {0};
    storage_file_read(file, buffer, 2);
    read |= (buffer[0] << 8);
    read |= buffer[1];
    return read;
}

static bool storage_file_write16(File* file, uint16_t data) {
    char buffer[2] = {0};
    buffer[0] = (data >> 8) & 0xFF;
    buffer[1] = data & 0xFF;
    return storage_file_write(file, buffer, 2) == 2;
}

static uint32_t storage_file_read32(File* file) {
    uint32_t read = 0;
    char buffer[4] = {0};
    storage_file_read(file, buffer, 4);
    read = (buffer[0] << 24);
    read |= (buffer[1] << 16);
    read |= (buffer[2] << 8);
    read |= buffer[3];
    return read;
}

static bool storage_file_write32(File* file, uint32_t data) {
    char buffer[4] = {0};
    buffer[0] = (data >> 24) & 0xFF;
    buffer[1] = (data >> 16) & 0xFF;
    buffer[2] = (data >> 8) & 0xFF;
    buffer[3] = data & 0xFF;
    return storage_file_write(file, buffer, 4) == 4;
}

enum {
    GENIE_MAGIC = 0, // 2 bytes
    GENIE_VERSION = 2, // 2 bytes
    GENIE_SN = 4, // 4 bytes
    GENIE_LAST_SENT = 8, // 2 bytes
    GENIE_REC_COUNT = 10, // 2 bytes
    GENIE_RESERVED = 12, // 4 bytes
    GENIE_DATA = 16, // 64K bytes
} genie_file;

static void genie_create_file(Storage* storage, char* name, uint32_t low) {
    File* file = storage_file_alloc(storage);
    if(!file) {
        FURI_LOG_E(TAG, "Failed to alloc file");
        return;
    }

    if(storage_file_open(file, name, FSAM_READ_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write16(file, GENIE_MAGIC_CODE); // Magic value for Genie files.
        storage_file_write16(file,
                             GENIE_MAJOR_VERSION << 8 | GENIE_MINOR_VERSION); // Version info
        storage_file_write32(file, low); // Btn+SN
        storage_file_write16(file, 0); // Last send (where to start looking for keys)
        storage_file_write16(file, 0x0000); // Last recorded value (how many values we recorded)
        storage_file_write32(file, 0x00000000); // Reserved data
        char buffer[256] = {0}; // blocks of 0s.
        for(int i = 0; i < 1024; i++) {
            storage_file_write(file, buffer, 256);
        }
    }
    storage_file_close(file);
    storage_file_free(file);
}

static uint32_t hex_to_i32(const char* data) {
    uint32_t value = 0;
    for(int i = 0; i < 8; i++) {
        value = value << 4;
        value |= (data[i] >= 'a') ? 10 + data[i] - 'a' :
                 (data[i] >= 'A') ? 10 + data[i] - 'A' :
                                    data[i] - '0';
    }
    return value;
}

uint32_t genie_save_bin(const char* key) {
    uint32_t result = 0;
    uint32_t key_high = 0;
    uint32_t key_low = 0;
    key_high = hex_to_i32(key);
    key_low = hex_to_i32(key + 8);

    FURI_LOG_D(TAG, "Saving binary key. key: %s high: %08lX low: %08lX", key, key_high, key_low);

    Storage* storage = furi_record_open(RECORD_STORAGE);

    char buffer[128] = {0};
    snprintf(buffer, 128, "%s/%08lX.gne", GENIE_SAVE_FOLDER, key_low);

    File* file = NULL;
    do {
        if(!storage) {
            FURI_LOG_E(TAG, "Failed to access storage.");
            break;
        }

        file = storage_file_alloc(storage);
        if(!file) {
            FURI_LOG_E(TAG, "Failed to allocate file.");
            break;
        }

        if(!storage_file_exists(storage, buffer)) {
            FURI_LOG_D(TAG, "Creating file: %s", buffer);
            genie_create_file(storage, buffer, key_low);
        }

        if(storage_file_open(file, buffer, FSAM_READ_WRITE, FSOM_OPEN_EXISTING)) {
            if(!storage_file_seek(file, GENIE_VERSION, true)) {
                FURI_LOG_E(TAG, "Failed to seek to GENIE_VERSION @ %d", GENIE_VERSION);
                break;
            }

            uint16_t version = storage_file_read16(file);
            if((version >> 8) > GENIE_MAJOR_VERSION) {
                FURI_LOG_E(TAG, "Unsupported version: %04X", version);
                break;
            }

            if(!storage_file_seek(file, GENIE_SN, true)) {
                FURI_LOG_E(TAG, "Failed to seek to GENIE_SN @ %d", GENIE_SN);
                break;
            }

            uint32_t low = storage_file_read32(file);
            if(low != key_low) {
                FURI_LOG_E(TAG, "Key mismatch. Expected: %08lX, got: %08lX", key_low, low);
                break;
            }

            if(!storage_file_seek(file, GENIE_REC_COUNT, true)) {
                FURI_LOG_E(TAG, "Failed to seek to GENIE_REC_COUNT @ %d", GENIE_REC_COUNT);
                break;
            }
            uint16_t count = storage_file_read16(file);

            if(!storage_file_seek(file, GENIE_DATA + (count * 4), true)) {
                FURI_LOG_E(
                    TAG, "Failed to seek to GENIE_DATA+count*4 @ %d", GENIE_DATA + (count * 4));
                break;
            }

            uint32_t existing = storage_file_read32(file);
            if(existing != 0 && existing != key_high) {
                if(count != 0xFFFF) {
                    FURI_LOG_E(
                        TAG,
                        "Key mismatch at %04X. Old: %08lX, New: %08lX",
                        count,
                        existing,
                        key_high);
                    result = count;
                } else {
                    result = 0x10000;
                }
                break;
            }

            if(!storage_file_seek(file, GENIE_DATA + (count * 4), true)) {
                FURI_LOG_E(
                    TAG, "Failed to seek to GENIE_DATA+count*4 @ %d", GENIE_DATA + (count * 4));
                break;
            }

            if(!storage_file_write32(file, key_high)) {
                FURI_LOG_E(TAG, "Failed to write to key_high to file: %08lX", key_high);
                break;
            }

            if(!storage_file_seek(file, GENIE_REC_COUNT, true)) {
                FURI_LOG_E(TAG, "Failed to seek to GENIE_REC_COUNT @ %d", GENIE_REC_COUNT);
                break;
            }

            if(count != 0xFFFF) {
                count++;
                result = count;

                if(!storage_file_write16(file, count)) {
                    FURI_LOG_E(TAG, "Failed to write count to file: %04X", count);
                    break;
                }
            } else {
                result = 0x10000;
            }

        } else {
            FURI_LOG_E(TAG, "Failed to open file");
            break;
        }
    } while(false);

    if(file) {
        storage_file_close(file);
        storage_file_free(file);
    }

    furi_record_close(RECORD_STORAGE);
    return result;
}

bool genie_save(uint32_t count, FuriString* key) {
    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    char buffer[8 + 1 + 16 + 2 + 1] = {0};
    snprintf(buffer, 8 + 1 + 16 + 2 + 1, "%08lX,%s\r\n", count, furi_string_get_cstr(key));

    File* file = NULL;
    do {
        if(!storage) {
            FURI_LOG_E(TAG, "Failed to access storage");
            break;
        }

        file = storage_file_alloc(storage);
        if(file && storage_file_open(
                       file,
                       (GENIE_SAVE_FOLDER "/" GENIE_SAVE_NAME GENIE_SAVE_EXTENSION),
                       FSAM_WRITE,
                       FSOM_OPEN_APPEND)) {
            if(!storage_file_write(file, buffer, COUNT_OF(buffer) - 1)) {
                FURI_LOG_E(TAG, "Failed to write to file");
                break;
            }
            success = true;
        } else {
            FURI_LOG_E(TAG, "Failed to open file");
            break;
        }
    } while(false);

    if(file) {
        storage_file_close(file);
        storage_file_free(file);
    }

    furi_record_close(RECORD_STORAGE);
    return success;
}

uint32_t genie_load() {
    uint32_t count = 0;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriString* buffer = furi_string_alloc();

    File* file = NULL;
    do {
        if(!storage) {
            FURI_LOG_E(TAG, "Failed to access storage");
            break;
        }

        ensure_save_folder_exists(storage);

        file = storage_file_alloc(storage);
        if(storage_file_open(
               file,
               (GENIE_SAVE_FOLDER "/" GENIE_SAVE_NAME GENIE_SAVE_EXTENSION),
               FSAM_READ,
               FSOM_OPEN_EXISTING)) {
            FURI_LOG_D(TAG, "Reading file.");

            char data[8 + 1 + 16 + 2 + 1] = {0};

            while(storage_file_read(file, data, 8 + 1 + 16 + 2)) {
                sscanf(data, "%08lX", &count);
                FURI_LOG_D(TAG, "Read: %s, count: %ld", data, count);
            }
        }
    } while(false);

    if(file) {
        storage_file_close(file);
        storage_file_free(file);
    }

    furi_string_free(buffer);
    furi_record_close(RECORD_STORAGE);
    return count;
}