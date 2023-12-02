#include "../wiegand.h"

void wiegand_load_scene_on_enter(void* context) {
    App* app = context;
    bit_count = 0;
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, WIEGAND_SAVE_EXTENSION, NULL);
    browser_options.base_path = WIEGAND_SAVE_FOLDER;

    furi_string_set(app->file_path, browser_options.base_path);

    FuriString* buffer = furi_string_alloc(1024);

    if(dialog_file_browser_show(app->dialogs, app->file_path, app->file_path, &browser_options)) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        File* data_file = storage_file_alloc(storage);
        if(storage_file_open(
               data_file, furi_string_get_cstr(app->file_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            while(!storage_file_eof(data_file)) {
                char ch;
                furi_string_reset(buffer);
                while(storage_file_read(data_file, &ch, 1) && !storage_file_eof(data_file)) {
                    furi_string_push_back(buffer, ch);
                    if(ch == '\n') {
                        break;
                    }
                }

                if(furi_string_start_with(buffer, "RAW_Data: ")) {
                    bit_count = 0;
                    int length = furi_string_size(buffer);
                    uint32_t temp;
                    char ch;
                    for(int i = 8; i < length - 1; i++) {
                        if(furi_string_get_char(buffer, i) == 'D') {
                            i++;
                            data[bit_count] = furi_string_get_char(buffer, i) == '1';
                            i += 2; // Skip space
                            temp = 0;

                            while(i < length && (ch = furi_string_get_char(buffer, i)) != ' ') {
                                temp = temp * 10 + (ch - '0');
                                i++;
                            }
                            data_fall[bit_count] = temp;
                            i++; // Skip space

                            temp = 0;
                            while(i < length && (ch = furi_string_get_char(buffer, i)) != ' ' &&
                                  ch != '\n') {
                                temp = temp * 10 + (ch - '0');
                                i++;
                            }
                            data_rise[bit_count] = temp;
                            bit_count++;
                        }
                    }
                    break;
                }
            }

            storage_file_close(data_file);
        }
        storage_file_free(data_file);
        furi_record_close(RECORD_STORAGE);
    }

    if(bit_count == 0) {
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, WiegandMainMenuScene);
    } else {
        data_saved = true;
        scene_manager_next_scene(app->scene_manager, WiegandDataScene);
    }
}
