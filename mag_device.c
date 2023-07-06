#include "mag_device.h"

#include <toolbox/path.h>
#include <flipper_format/flipper_format.h>

#define TAG "MagDevice"

static const char* mag_file_header = "Flipper Mag device";
static const uint32_t mag_file_version = 1;

MagDevice* mag_device_alloc() {
    MagDevice* mag_dev = malloc(sizeof(MagDevice));
    mag_dev->dev_data.track[0].str = furi_string_alloc();
    mag_dev->dev_data.track[1].str = furi_string_alloc();
    mag_dev->dev_data.track[2].str = furi_string_alloc();
    mag_dev->storage = furi_record_open(RECORD_STORAGE);
    mag_dev->dialogs = furi_record_open(RECORD_DIALOGS);
    mag_dev->load_path = furi_string_alloc();
    return mag_dev;
}

void mag_device_data_clear(MagDeviceData* dev_data) {
    furi_string_reset(dev_data->track[0].str);
    furi_string_reset(dev_data->track[1].str);
    furi_string_reset(dev_data->track[2].str);
}

void mag_device_clear(MagDevice* mag_dev) {
    furi_assert(mag_dev);

    mag_device_data_clear(&mag_dev->dev_data);
    memset(&mag_dev->dev_data, 0, sizeof(mag_dev->dev_data));
    furi_string_reset(mag_dev->load_path);
}

void mag_device_free(MagDevice* mag_dev) {
    furi_assert(mag_dev);

    mag_device_clear(mag_dev);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    furi_string_free(mag_dev->load_path);

    //furi_string_free(mag_dev->dev_data.track[0].str);
    //furi_string_free(mag_dev->dev_data.track[1].str);
    //furi_string_free(mag_dev->dev_data.track[2].str);

    free(mag_dev);
}

void mag_device_set_name(MagDevice* mag_dev, const char* name) {
    furi_assert(mag_dev);

    strlcpy(mag_dev->dev_name, name, MAG_DEV_NAME_MAX_LEN);
}

static bool mag_device_save_file(
    MagDevice* mag_dev,
    const char* dev_name,
    const char* folder,
    const char* extension,
    bool use_load_path) {
    furi_assert(mag_dev);

    bool saved = false;
    FlipperFormat* file = flipper_format_file_alloc(mag_dev->storage);
    FuriString* temp_str;
    temp_str = furi_string_alloc();

    do {
        if(use_load_path && !furi_string_empty(mag_dev->load_path)) {
            // Get dir name
            path_extract_dirname(furi_string_get_cstr(mag_dev->load_path), temp_str);
            // Create mag directory if necessary
            if(!storage_simply_mkdir((mag_dev->storage), furi_string_get_cstr(temp_str))) break;
            // Make path to file to be saved
            furi_string_cat_printf(temp_str, "/%s%s", dev_name, extension);
        } else {
            // Create mag directory if necessary
            if(!storage_simply_mkdir((mag_dev->storage), MAG_APP_FOLDER)) break;
            // First remove mag device file if it was saved
            furi_string_printf(temp_str, "%s/%s%s", folder, dev_name, extension);
        }
        // Open file
        if(!flipper_format_file_open_always(file, furi_string_get_cstr(temp_str))) break;

        // Write header
        if(!flipper_format_write_header_cstr(file, mag_file_header, mag_file_version)) break;

        // Write comment
        if(!flipper_format_write_comment_cstr(file, "Mag device track data")) break;

        // Write data
        for(uint8_t i = 0; i < MAG_DEV_TRACKS; i++) {
            furi_string_printf(temp_str, "Track %d", i + 1);
            if(!flipper_format_write_string_cstr(
                   file,
                   furi_string_get_cstr(temp_str),
                   furi_string_get_cstr(mag_dev->dev_data.track[i].str)))
                break;
        }

        saved = true;
    } while(0);

    if(!saved) {
        dialog_message_show_storage_error(mag_dev->dialogs, "Cannot save\nfile");
    }

    furi_string_free(temp_str);
    flipper_format_free(file);

    return saved;
}

bool mag_device_save(MagDevice* mag_dev, const char* dev_name) {
    // wrapping function in the event we have multiple formats
    return mag_device_save_file(mag_dev, dev_name, MAG_APP_FOLDER, MAG_APP_EXTENSION, true);
}

static bool mag_device_load_data(MagDevice* mag_dev, FuriString* path, bool show_dialog) {
    bool parsed = false;

    FlipperFormat* file = flipper_format_file_alloc(mag_dev->storage);
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    bool deprecated_version = false;
    bool data_read = true;

    if(mag_dev->loading_cb) {
        mag_dev->loading_cb(mag_dev->loading_cb_ctx, true);
    }

    do {
        if(!flipper_format_file_open_existing(file, furi_string_get_cstr(path))) break;

        // Read and verify header, check file version
        uint32_t version;
        if(!flipper_format_read_header(file, temp_str, &version)) break;
        if(furi_string_cmp_str(temp_str, mag_file_header) || (version != mag_file_version)) {
            deprecated_version = true;
            break;
        }

        // Parse data
        for(uint8_t i = 0; i < MAG_DEV_TRACKS; i++) {
            furi_string_printf(temp_str, "Track %d", i + 1);
            if(!flipper_format_read_string(
                   file, furi_string_get_cstr(temp_str), mag_dev->dev_data.track[i].str)) {
                FURI_LOG_D(TAG, "Could not read track %d data", i + 1);

                // TODO: smarter load handling now that it is acceptible for some tracks to be empty
                data_read = false;
            }
        }

        parsed = true;
    } while(false);

    if((!parsed) && (show_dialog)) {
        if(deprecated_version) {
            dialog_message_show_storage_error(mag_dev->dialogs, "File format\ndeprecated");
        } else if(!data_read) {
            dialog_message_show_storage_error(mag_dev->dialogs, "Cannot read\ndata");
        } else {
            dialog_message_show_storage_error(mag_dev->dialogs, "Cannot parse\nfile");
        }
    }

    furi_string_free(temp_str);
    flipper_format_free(file);

    return parsed;
}

bool mag_file_select(MagDevice* mag_dev) {
    furi_assert(mag_dev);

    // Input events and views are managed by file_browser
    FuriString* mag_app_folder;
    mag_app_folder = furi_string_alloc_set(MAG_APP_FOLDER);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, MAG_APP_EXTENSION, &I_mag_file_10px);
    browser_options.base_path = MAG_APP_FOLDER;

    bool res = dialog_file_browser_show(
        mag_dev->dialogs, mag_dev->load_path, mag_app_folder, &browser_options);

    furi_string_free(mag_app_folder);
    if(res) {
        FuriString* filename;
        filename = furi_string_alloc();
        path_extract_filename(mag_dev->load_path, filename, true);
        strncpy(mag_dev->dev_name, furi_string_get_cstr(filename), MAG_DEV_NAME_MAX_LEN);
        res = mag_device_load_data(mag_dev, mag_dev->load_path, true);
        if(res) {
            mag_device_set_name(mag_dev, mag_dev->dev_name);
        }
        furi_string_free(filename);
    }

    return res;
}

bool mag_device_delete(MagDevice* mag_dev, bool use_load_path) {
    furi_assert(mag_dev);

    bool deleted = false;
    FuriString* file_path;
    file_path = furi_string_alloc();

    do {
        // Delete original file
        if(use_load_path && !furi_string_empty(mag_dev->load_path)) {
            furi_string_set(file_path, mag_dev->load_path);
        } else {
            furi_string_printf(
                file_path, "%s/%s%s", MAG_APP_FOLDER, mag_dev->dev_name, MAG_APP_EXTENSION);
        }
        if(!storage_simply_remove(mag_dev->storage, furi_string_get_cstr(file_path))) break;
        deleted = true;
    } while(false);

    if(!deleted) {
        dialog_message_show_storage_error(mag_dev->dialogs, "Cannot remove\nfile");
    }

    furi_string_free(file_path);
    return deleted;
}

bool mag_device_parse_card_string(MagDevice* mag_dev, FuriString* f_card_str) {
    furi_assert(mag_dev);
    FURI_LOG_D(TAG, "mag_device_parse_card_string");

    const char* card_str = furi_string_get_cstr(f_card_str);

    FURI_LOG_D(TAG, "Parsing card string: %s", card_str);

    // Track 1
    const char* track1_start = strchr(card_str, '%');
    if(!track1_start) {
        FURI_LOG_D(TAG, "Could not find track 1 start");
        return false;
    }
    track1_start++;
    const char* track1_end = strchr(track1_start, '?');
    if(!track1_end) {
        FURI_LOG_D(TAG, "Could not find track 1 end");
        return false;
    }
    size_t track1_len = track1_end - track1_start;
    
    FURI_LOG_D(TAG, "Track 1: %.*s", track1_len, track1_start);

    mag_dev->dev_data.track[0].len = track1_len;
    furi_string_printf(mag_dev->dev_data.track[0].str, "%%%.*s?", track1_len, track1_start);

    // Track 2
    const char* track2_start = strchr(track1_end, ';');
    if (!track2_start) {
        FURI_LOG_D(TAG, "Could not find track 2 start");
        return true;
    }

    track2_start++;
    const char* track2_end = strchr(track2_start, '?');
    if(!track2_end) {
        FURI_LOG_D(TAG, "Could not find track 2 end");
        return true;
    }
    size_t track2_len = track2_end - track2_start;

    FURI_LOG_D(TAG, "Track 2: %.*s", track2_len, track2_start);

    mag_dev->dev_data.track[1].len = track2_len;
    furi_string_printf(mag_dev->dev_data.track[1].str, "%%%.*s?", track2_len, track2_start);

    // Track 3
    const char* track3_start = strchr(track2_end, ';');
    if (!track3_start) {
        FURI_LOG_D(TAG, "Could not find track 3 start");
        return true;
    }

    track3_start++;
    const char* track3_end = strchr(track3_start, '?');
    if(!track3_end) {
        FURI_LOG_D(TAG, "Could not find track 3 end");
        return true;
    }
    size_t track3_len = track3_end - track3_start;

    FURI_LOG_D(TAG, "Track 3: %.*s", track3_len, track3_start);

    mag_dev->dev_data.track[2].len = track3_len;
    furi_string_printf(mag_dev->dev_data.track[2].str, "%%%.*s?", track3_len, track3_start);
    
    return true;
}


void mag_device_set_loading_callback(
    MagDevice* mag_dev,
    MagLoadingCallback callback,
    void* context) {
    furi_assert(mag_dev);

    mag_dev->loading_cb = callback;
    mag_dev->loading_cb_ctx = context;
}
