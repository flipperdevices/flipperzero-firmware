#include "../nfc_crocodile_i.h"
#include "nfc_crocodile_scene.h"

#define MAX_LEN 255
#define LOOKUP_LEN 20
#define TAG "NfcCrocodileSceneDictionary"

char* files[MAX_LEN];
uint32_t files_len;

void nfc_crocodile_scene_dictionary_choice_callback(void* context, uint32_t index) {
    NfcCrocodile* nfc_crocodile = context;
    scene_manager_handle_custom_event(nfc_crocodile->scene_manager, index);
}

void nfc_crocodile_scene_dictionary_on_enter(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    submenu_reset(nfc_crocodile->submenu);
    submenu_set_header(nfc_crocodile->submenu, "Select dictionary");

    File* dir = storage_file_alloc(nfc_crocodile->storage);
    files_len = 0;
    if(storage_dir_open(dir, STORAGE_APP_DATA_PATH_PREFIX)) {
        FileInfo file_info;
        char* buf = malloc(MAX_LEN * sizeof(char));
        uint16_t buf_len = 0;
        while(true) {
            if(!storage_dir_read(dir, &file_info, buf, MAX_LEN)) {
                break;
            }

            if(file_info_is_dir(&file_info)) {
                continue;
            }

            buf_len = strlen(buf) + 1;
            files[files_len] = malloc(buf_len * sizeof(char));
            memcpy(files[files_len], buf, buf_len);

            submenu_add_item(
                nfc_crocodile->submenu,
                files[files_len],
                files_len,
                nfc_crocodile_scene_dictionary_choice_callback,
                context);

            FURI_LOG_D(TAG, "Found %s (size %llu)", buf, file_info.size);
            files_len++;
        }

        submenu_set_selected_item(
            nfc_crocodile->submenu,
            scene_manager_get_scene_state(
                nfc_crocodile->scene_manager, NfcCrocodileSceneDictionary));

        free(buf);
    } else {
        FURI_LOG_E(TAG, "Unable to open app data dir");
    }

    storage_file_free(dir);

    view_dispatcher_switch_to_view(nfc_crocodile->view_dispatcher, NfcCrocodileViewMenu);
}

bool nfc_crocodile_scene_dictionary_on_event(void* context, SceneManagerEvent event) {
    NfcCrocodile* nfc_crocodile = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        const char* filename = files[event.event];
        FuriString* full_path_f = furi_string_alloc();
        path_concat(STORAGE_APP_DATA_PATH_PREFIX, filename, full_path_f);
        const char* full_path = furi_string_get_cstr(full_path_f);
        File* f = storage_file_alloc(nfc_crocodile->storage);

        FURI_LOG_I(TAG, "Requested %s", full_path);

        storage_file_open(f, full_path, FSAM_READ, FSOM_OPEN_EXISTING);
        char buf[LOOKUP_LEN * 2];
        uint64_t file_size = storage_file_size(f);

        // Read separator
        char separator;
        storage_file_read(f, &separator, 1);

        // Reduce file_size by last word length
        storage_file_seek(f, file_size - LOOKUP_LEN, true);
        uint16_t read_count = storage_file_read(f, buf, LOOKUP_LEN);
        for(uint16_t i = 0; i < read_count; i++) {
            if(buf[read_count - i] == separator) {
                FURI_LOG_D(TAG, "Last comma found at index -%d", i);
                file_size -= i + 1;
                break;
            }
        }

        // Get random seek
        uint32_t rnd = furi_hal_random_get() % file_size;
        FURI_LOG_I(TAG, "total file size = %llu bytes. Rand seek = %lu", file_size, rnd);

        // Read until next separator
        storage_file_seek(f, rnd, true);
        read_count = storage_file_read(f, buf, 2 * LOOKUP_LEN);
        buf[read_count + 1] = '\0';
        FURI_LOG_I(TAG, "buf = %s", buf);

        // Extract next word
        char* ptr = NULL;
        for(uint16_t i = 0; i < read_count; i++) {
            if(buf[i] != separator) continue;
            if(!ptr) {
                ptr = buf + i + 1;
            }
            buf[i] = '\0';
        }
        FURI_LOG_I(TAG, "Found word: %s", ptr);
        uint16_t word_len = strlen(ptr) + 1;

        if(nfc_crocodile->card_content) {
            free(nfc_crocodile->card_content);
        }

        nfc_crocodile->card_content = malloc(word_len * sizeof(char));
        memcpy(nfc_crocodile->card_content, ptr, word_len);

        storage_file_close(f);
        storage_file_free(f);
        furi_string_free(full_path_f);

        scene_manager_set_scene_state(
            nfc_crocodile->scene_manager, NfcCrocodileSceneDictionary, event.event);
        scene_manager_next_scene(nfc_crocodile->scene_manager, NfcCrocodileSceneTransfer);
    }

    return consumed;
}

void nfc_crocodile_scene_dictionary_on_exit(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    for(uint32_t i = 0; i < files_len; i++) {
        free(files[i]);
    }
    submenu_reset(nfc_crocodile->submenu);
}