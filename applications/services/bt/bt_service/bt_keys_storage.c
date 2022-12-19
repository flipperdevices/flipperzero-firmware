#include "bt_keys_storage.h"

#include <furi.h>
#include <furi_hal_bt.h>
#include <lib/toolbox/saved_struct.h>
#include <storage/storage.h>

#define BT_KEYS_STORAGE_PATH INT_PATH(BT_KEYS_STORAGE_FILE_NAME)
#define BT_KEYS_STORAGE_VERSION (1)
#define BT_KEYS_STORAGE_MAGIC (0x18)

#define TAG "BtKeyStorage"

typedef struct {
    uint16_t data_size;
    uint8_t* data;
} BtKeysStorageProfileKeys;

struct BtKeysStorage {
    uint8_t* nvm_sram_buff;
    uint16_t nvm_sram_buff_size;
    uint32_t nvm_flash_total_size;
    FuriString* file_path;
    uint8_t* raw_keys_buff;
    size_t raw_keys_buff_size;
    BtKeysStorageProfileKeys profile_keys[BtProfileNum];
};

static bool bt_keys_storage_parse_profile_keys(BtKeysStorage* instance) {
    size_t buff_i = 0;

    bool profile_keys_loaded = true;
    for(size_t i = 0; i < BtProfileNum; i++) {
        if(buff_i >= instance->raw_keys_buff_size) {
            profile_keys_loaded = false;
            break;
        }
        memcpy(
            &instance->profile_keys[i].data_size,
            &instance->raw_keys_buff[buff_i],
            sizeof(instance->profile_keys[i].data_size));
        buff_i += sizeof(instance->profile_keys[i].data_size);
        if(instance->profile_keys[i].data_size == 0) continue;
        if(buff_i >= instance->raw_keys_buff_size) {
            profile_keys_loaded = false;
            break;
        }
        instance->profile_keys[i].data = &instance->raw_keys_buff[buff_i];
        buff_i += instance->profile_keys[i].data_size;
    }

    return profile_keys_loaded;
}

bool bt_keys_storage_delete(Bt* bt) {
    furi_assert(bt);
    bool delete_succeed = false;
    bool bt_is_active = furi_hal_bt_is_active();

    furi_hal_bt_stop_advertising();
    delete_succeed = furi_hal_bt_clear_white_list();
    if(bt_is_active) {
        furi_hal_bt_start_advertising();
    }

    return delete_succeed;
}

BtKeysStorage* bt_keys_storage_alloc() {
    BtKeysStorage* instance = malloc(sizeof(BtKeysStorage));
    // Set nvm ram parameters
    furi_hal_bt_get_key_storage_buff(&instance->nvm_sram_buff, &instance->nvm_sram_buff_size);
    // Set key storage file
    instance->file_path = furi_string_alloc();
    furi_string_set_str(instance->file_path, BT_KEYS_STORAGE_PATH);

    return instance;
}

void bt_keys_storage_free(BtKeysStorage* instance) {
    furi_assert(instance);

    furi_string_free(instance->file_path);
    free(instance);
}

void bt_keys_storage_set_file_path(BtKeysStorage* instance, const char* path) {
    furi_assert(instance);
    furi_assert(path);

    furi_string_set_str(instance->file_path, path);
}

void bt_keys_storage_set_ram_params(BtKeysStorage* instance, uint8_t* buff, uint16_t size) {
    furi_assert(instance);
    furi_assert(buff);

    instance->nvm_sram_buff = buff;
    instance->nvm_sram_buff_size = size;
}

bool bt_keys_storage_load(BtKeysStorage* instance, BtProfile profile) {
    furi_assert(instance);
    furi_assert(profile < BtProfileNum);

    bool loaded = false;
    do {
        // Get payload size
        instance->raw_keys_buff_size = 0;
        if(!saved_struct_get_payload_size(
               furi_string_get_cstr(instance->file_path),
               BT_KEYS_STORAGE_MAGIC,
               BT_KEYS_STORAGE_VERSION,
               &instance->raw_keys_buff_size)) {
            FURI_LOG_E(TAG, "Failed to read payload size");
            break;
        }
        FURI_LOG_W(TAG, "Payload size: %d", instance->raw_keys_buff_size);

        // Load raw payload
        instance->raw_keys_buff = malloc(instance->raw_keys_buff_size);
        if(!saved_struct_load(
               furi_string_get_cstr(instance->file_path),
               instance->raw_keys_buff,
               instance->raw_keys_buff_size,
               BT_KEYS_STORAGE_MAGIC,
               BT_KEYS_STORAGE_VERSION)) {
            FURI_LOG_E(TAG, "Failed to load struct");
            break;
        }

        // Parse profile keys
        if(!bt_keys_storage_parse_profile_keys(instance)) {
            FURI_LOG_E(TAG, "Failed to parse profiles keys");
            break;
        }

        // Load key data to ram
        if(instance->profile_keys[profile].data_size > instance->nvm_sram_buff_size) {
            FURI_LOG_E(TAG, "NVM RAM buffer overflow");
            break;
        }

        furi_hal_bt_nvm_sram_sem_acquire();
        memcpy(
            instance->nvm_sram_buff,
            instance->profile_keys[profile].data,
            instance->profile_keys[profile].data_size);
        furi_hal_bt_nvm_sram_sem_release();

        loaded = true;
    } while(false);

    // Free allocated memory
    if(instance->raw_keys_buff) {
        free(instance->raw_keys_buff);
        instance->raw_keys_buff = NULL;
    }
    instance->raw_keys_buff_size = 0;

    return loaded;
}

bool bt_keys_storage_update(
    BtKeysStorage* instance,
    BtProfile profile,
    uint8_t* start_addr,
    uint32_t size) {
    furi_assert(instance);
    furi_assert(profile < BtProfileNum);

    bool updated = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    do {
        uint32_t offset = start_addr - instance->nvm_sram_buff;
        if(offset + size >= instance->nvm_sram_buff_size) {
            FURI_LOG_E(TAG, "NVM RAM buffer overflow");
            break;
        }

        FURI_LOG_I(
            TAG,
            "Base address: %p. Start update address: %p. Size changed: %ld",
            (void*)instance->nvm_sram_buff,
            start_addr,
            size);

        if(storage_common_stat(storage, furi_string_get_cstr(instance->file_path), NULL) ==
           FSE_OK) {
            // Load all profiles keys and modify
            instance->raw_keys_buff_size = 0;
            if(!saved_struct_get_payload_size(
                   furi_string_get_cstr(instance->file_path),
                   BT_KEYS_STORAGE_MAGIC,
                   BT_KEYS_STORAGE_VERSION,
                   &instance->raw_keys_buff_size)) {
                FURI_LOG_E(TAG, "Failed to read saved file");
                break;
            }
            instance->raw_keys_buff = malloc(instance->raw_keys_buff_size);
            if(!saved_struct_load(
                   furi_string_get_cstr(instance->file_path),
                   instance->raw_keys_buff,
                   instance->raw_keys_buff_size,
                   BT_KEYS_STORAGE_MAGIC,
                   BT_KEYS_STORAGE_VERSION)) {
                FURI_LOG_E(TAG, "Failed to load payload");
                break;
            }
            // Parse saved profile keys
            if(!bt_keys_storage_parse_profile_keys(instance)) {
                FURI_LOG_E(TAG, "Failed to parse profiles keys");
                break;
            }
            // Calculate new keys storage size
            size_t new_raw_key_buff_size = 0;
            for(size_t i = 0; i < COUNT_OF(instance->profile_keys); i++) {
                new_raw_key_buff_size += sizeof(instance->profile_keys[i].data_size);
                if(i == profile) {
                    instance->profile_keys[i].data_size =
                        start_addr - instance->nvm_sram_buff + size;
                    instance->profile_keys[i].data = instance->nvm_sram_buff;
                }
                new_raw_key_buff_size += instance->profile_keys[i].data_size;
            }
            // Fill new keys storage
            uint8_t* new_raw_key_buff = malloc(new_raw_key_buff_size);
            uint16_t buff_i = 0;
            for(size_t i = 0; i < COUNT_OF(instance->profile_keys); i++) {
                memcpy(
                    &new_raw_key_buff[buff_i],
                    &instance->profile_keys[i].data_size,
                    sizeof(instance->profile_keys[i].data_size));
                buff_i += sizeof(instance->profile_keys[i].data_size);
                if(instance->profile_keys[i].data_size) {
                    memcpy(
                        &new_raw_key_buff[buff_i],
                        instance->profile_keys[i].data,
                        instance->profile_keys[i].data_size);
                    buff_i += instance->profile_keys[i].data_size;
                }
            }
            // Save struct
            if(!saved_struct_save(
                   furi_string_get_cstr(instance->file_path),
                   new_raw_key_buff,
                   new_raw_key_buff_size,
                   BT_KEYS_STORAGE_MAGIC,
                   BT_KEYS_STORAGE_VERSION)) {
                FURI_LOG_E(TAG, "Failed to save keys to file");
                free(new_raw_key_buff);
                break;
            }
            FURI_LOG_I(TAG, "Updated payload sizeeeeee: %d", new_raw_key_buff_size);
            free(new_raw_key_buff);
            updated = true;
        } else {
            instance->raw_keys_buff_size = 0;
            for(size_t i = 0; i < COUNT_OF(instance->profile_keys); i++) {
                instance->raw_keys_buff_size += sizeof(instance->profile_keys[i].data_size);
                if(i == profile) {
                    instance->raw_keys_buff_size += offset + size;
                    instance->profile_keys[i].data_size = offset + size;
                    instance->profile_keys[i].data = &instance->nvm_sram_buff[offset];
                } else {
                    instance->profile_keys[i].data_size = 0;
                }
            }
            // Prepare raw data to save
            instance->raw_keys_buff = malloc(instance->raw_keys_buff_size);
            uint16_t buff_i = 0;
            for(size_t i = 0; i < COUNT_OF(instance->profile_keys); i++) {
                memcpy(
                    &instance->raw_keys_buff[buff_i],
                    &instance->profile_keys[i].data_size,
                    sizeof(instance->profile_keys[i].data_size));
                buff_i += sizeof(instance->profile_keys[i].data_size);
                if(instance->profile_keys[i].data_size) {
                    memcpy(
                        &instance->raw_keys_buff[buff_i],
                        instance->profile_keys[i].data,
                        instance->profile_keys[i].data_size);
                    buff_i += instance->profile_keys[i].data_size;
                }
            }
            // Save raw data to file
            furi_hal_bt_nvm_sram_sem_acquire();
            bool saved = saved_struct_save(
                furi_string_get_cstr(instance->file_path),
                instance->raw_keys_buff,
                instance->raw_keys_buff_size,
                BT_KEYS_STORAGE_MAGIC,
                BT_KEYS_STORAGE_VERSION);
            furi_hal_bt_nvm_sram_sem_release();
            FURI_LOG_I(TAG, "Updated payload size: %d", instance->raw_keys_buff_size);

            if(!saved) {
                FURI_LOG_E(TAG, "Failed to save new file");
                break;
            }

            updated = true;
        }
    } while(false);
    // Free allocated memory
    if(instance->raw_keys_buff) {
        free(instance->raw_keys_buff);
        instance->raw_keys_buff = NULL;
    }
    instance->raw_keys_buff_size = 0;

    furi_record_close(RECORD_STORAGE);
    return updated;
}
