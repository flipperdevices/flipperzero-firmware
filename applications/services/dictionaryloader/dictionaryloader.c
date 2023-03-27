#include "dictionaryloader.h"
#include "m-string.h"
#include <toolbox/path.h>
#include <flipper_format/flipper_format.h>

int32_t dictionary_loader(void* p) {
    UNUSED(p);
    if(furi_hal_rtc_get_boot_mode() != FuriHalRtcBootModeNormal) {
        FURI_LOG_W(TAG, "Special startup detected, skipping");
    } else {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        FlipperFormat* file = flipper_format_file_alloc(storage);

        FuriString* NAMEHEADER;
        NAMEHEADER = furi_string_alloc_set("Dictionary path file");

        FuriString* filepath;
        filepath = furi_string_alloc_set("/ext/dolphin/dictionary.txt");

        bool result = false;

        FuriString* data;
        data = furi_string_alloc();

        do {
            if(!flipper_format_file_open_existing(file, furi_string_get_cstr(filepath))) {
                break;
            }

            // header
            uint32_t version;

            if(!flipper_format_read_header(file, data, &version)) {
                break;
            }

            if(furi_string_cmp_str(data, furi_string_get_cstr(NAMEHEADER)) != 0) {
                break;
            }

            if(version != 1) {
                break;
            }

            // get path
            if(!flipper_format_read_string(file, "Path", data)) {
                break;
            }

            result = true;
        } while(false);

        flipper_format_free(file);

        if(!result) {
            //file not good - write new one
            FlipperFormat* file = flipper_format_file_alloc(storage);

            bool res = false;

            do {
                // Open file for write
                if(!flipper_format_file_open_always(file, furi_string_get_cstr(filepath))) {
                    break;
                }

                // Write header
                if(!flipper_format_write_header_cstr(file, furi_string_get_cstr(NAMEHEADER), 1)) {
                    break;
                }

                // Write comments
                if(!flipper_format_write_comment_cstr(
                       file,
                       "This will change the file used as usr dictionary.")) {
                    break;
                }

                if(!flipper_format_write_comment_cstr(
                       file,
                       "Note: This is limited to the following charset : a-z, A-Z, 0-9, and _")) {
                    break;
                }


                //Write name
                if(!flipper_format_write_string_cstr(
                       file, "Path", furi_hal_version_get_name_ptr())) {
                    break;
                }

                res = true;
            } while(false);

            flipper_format_free(file);

            if(!res) {
                FURI_LOG_E(TAG, "Save failed.");
            }
        } else {
            if(!furi_string_size(data)) {
                //Empty file - get default path and write to file.
                FlipperFormat* file = flipper_format_file_alloc(storage);

                bool res = false;

                do {
                    // Open file for write
                    if(!flipper_format_file_open_always(file, furi_string_get_cstr(filepath))) {
                        break;
                    }

                    // Write header
                    if(!flipper_format_write_header_cstr(
                           file, furi_string_get_cstr(NAMEHEADER), 1)) {
                        break;
                    }

                    // Write comments
                    if(!flipper_format_write_comment_cstr(
                           file,
                           "Changing the value below will change the default user dictionary.")) {
                        break;
                    }

                    if(!flipper_format_write_comment_cstr(
                           file,
                           "Note: This is limited to the following charset: a-z, A-Z, 0-9, and _")) {
                        break;
                    }

                    if(!flipper_format_write_comment_cstr(
                           file, "It cannot contain any other characters.")) {
                        break;
                    }

                    //Write name
                    if(!flipper_format_write_string_cstr(
                           file, "Path", furi_hal_version_get_name_ptr())) {
                        break;
                    }

                    res = true;
                } while(false);

                flipper_format_free(file);

                if(!res) {
                    FURI_LOG_E(TAG, "Save failed.");
                }
            } else {
                char newdata[9];
                snprintf(newdata, 9, "%s", furi_string_get_cstr(data));
                //set name from file
                furi_hal_version_set_custom_path(newdata);
            }
        }

        furi_string_free(data);

        furi_string_free(filepath);
        furi_record_close(RECORD_STORAGE);
    }

    return 0;
}