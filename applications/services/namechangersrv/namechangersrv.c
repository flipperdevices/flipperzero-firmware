#include "namechangersrv.h"
#include "m-string.h"
#include <toolbox/path.h>
#include <flipper_format/flipper_format.h>

void namechanger_on_system_start() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    string_t NAMEHEADER;
    string_init_set_str(NAMEHEADER, "Flipper Name File");

    string_t folderpath;
    string_init_set_str(folderpath, "/ext/dolphin");

    string_t filepath;
    string_init_set_str(filepath, "/ext/dolphin/name.txt");

    //Make dir if doesn't exist
    if(storage_simply_mkdir(storage, string_get_cstr(folderpath))) {
        bool result = false;

        string_t data;
        string_init(data);

        do {
            if(!flipper_format_file_open_existing(file, string_get_cstr(filepath))) {
                break;
            }

            // header
            uint32_t version;

            if(!flipper_format_read_header(file, data, &version)) {
                break;
            }

            if(string_cmp_str(data, string_get_cstr(NAMEHEADER)) != 0) {
                break;
            }

            if(version != 1) {
                break;
            }

            // get Name
            if(!flipper_format_read_string(file, "Name", data)) {
                break;
            }

            result = true;
        } while(false);

        flipper_format_free(file);

        if(!result) {
            //file not good - write new one
            FlipperFormat* file = flipper_format_file_alloc(storage);

            bool res = false;

            string_t name;
            string_init_set_str(name, furi_hal_version_get_name_ptr());

            do {
                // Open file for write
                if(!flipper_format_file_open_always(file, string_get_cstr(filepath))) {
                    break;
                }

                // Write header
                if(!flipper_format_write_header_cstr(file, string_get_cstr(NAMEHEADER), 1)) {
                    break;
                }

                // Write comments
                if(!flipper_format_write_comment_cstr(
                       file,
                       "Changing the value below will change your FlipperZero device name.")) {
                    break;
                }

                if(!flipper_format_write_comment_cstr(
                       file,
                       "Note: This is limited to 8 characters using the following: a-z, A-Z, 0-9, and _")) {
                    break;
                }

                if(!flipper_format_write_comment_cstr(
                       file, "It can contain other characters but use at your own risk.")) {
                    break;
                }

                //Write name
                if(!flipper_format_write_string_cstr(file, "Name", string_get_cstr(name))) {
                    break;
                }

                res = true;
            } while(false);

            flipper_format_free(file);

            if(!res) {
                FURI_LOG_E(TAG, "Save failed.");
            }

            string_clear(name);
        } else {
            string_strim(data);
            FURI_LOG_I(TAG, "data: %s", data);

            if(!string_size(data)) {
                //Empty file - get default name and write to file.
                FlipperFormat* file = flipper_format_file_alloc(storage);

                bool res = false;

                string_t name;
                string_init_set_str(name, furi_hal_version_get_name_ptr());

                do {
                    // Open file for write
                    if(!flipper_format_file_open_always(file, string_get_cstr(filepath))) {
                        break;
                    }

                    // Write header
                    if(!flipper_format_write_header_cstr(file, string_get_cstr(NAMEHEADER), 1)) {
                        break;
                    }

                    // Write comments
                    if(!flipper_format_write_comment_cstr(
                           file,
                           "Changing the value below will change your FlipperZero device name.")) {
                        break;
                    }

                    if(!flipper_format_write_comment_cstr(
                           file,
                           "Note: This is limited to 8 characters using the following: a-z, A-Z, 0-9, and _")) {
                        break;
                    }

                    if(!flipper_format_write_comment_cstr(
                           file, "It cannot contain any other characters.")) {
                        break;
                    }

                    //Write name
                    if(!flipper_format_write_string_cstr(file, "Name", string_get_cstr(name))) {
                        break;
                    }

                    res = true;
                } while(false);

                flipper_format_free(file);

                if(!res) {
                    FURI_LOG_E(TAG, "Save failed.");
                }

                string_clear(name);
            } else {
                //set name from file
                furi_hal_version_set_custom_name(string_get_cstr(data));
            }
        }

        string_clear(data);
    }

    string_clear(filepath);
    string_clear(folderpath);
    furi_record_close(RECORD_STORAGE);
}