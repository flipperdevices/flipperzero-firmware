#include "spi_mem_app_i.h"

void spi_mem_file_create_folder(SPIMemApp* app) {
    if(!storage_simply_mkdir(app->storage, SPI_MEM_FILE_FOLDER)) {
        dialog_message_show_storage_error(app->dialogs, "Cannot create\napp folder");
    }
}

bool spi_mem_file_delete(SPIMemApp* app) {
    if(!storage_simply_remove(app->storage, furi_string_get_cstr(app->file_path))) {
        FuriString* file_name = furi_string_alloc();
        FuriString* message = furi_string_alloc();
        path_extract_filename(app->file_path, file_name, true);
        furi_string_printf(message, "Cannot detete\n%s", furi_string_get_cstr(file_name));
        dialog_message_show_storage_error(app->dialogs, furi_string_get_cstr(message));
        furi_string_free(file_name);
        furi_string_free(message);
        return false;
    }
    return true;
}

bool spi_mem_file_select(SPIMemApp* app) {
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, SPI_MEM_FILE_EXTENSION, &I_Dip8_10px);
    bool success =
        dialog_file_browser_show(app->dialogs, app->file_path, app->file_path, &browser_options);
    if(success) {
        // success = ibutton_load_key_data(ibutton, ibutton->file_path, true);
    }
    return success;
}

bool spi_mem_file_open(SPIMemApp* app) {
    bool success = false;
    app->flipper_file = flipper_format_file_alloc(app->storage);
    do {
        if(furi_string_end_with(app->file_path, SPI_MEM_FILE_EXTENSION)) {
            if(!spi_mem_file_delete(app)) break;
            size_t filename_start = furi_string_search_rchar(app->file_path, '/');
            furi_string_left(app->file_path, filename_start);
        }
        furi_string_cat_printf(app->file_path, "/%s%s", app->text_buffer, SPI_MEM_FILE_EXTENSION);
        FURI_LOG_E(TAG, furi_string_get_cstr(app->file_path));
        if(!flipper_format_file_open_always(
               app->flipper_file, furi_string_get_cstr(app->file_path)))
            break;
        if(!flipper_format_write_header_cstr(app->flipper_file, SPI_MEM_FILE_TYPE, 1)) break;
        success = true;
    } while(0);
    if(!success) {
        dialog_message_show_storage_error(app->dialogs, "Cannot save\nfile");
    }
    return success;
}

bool spi_mem_file_write_block(SPIMemApp* app, uint8_t* data, size_t size) {
    if(!flipper_format_write_hex(app->flipper_file, "Data", data, size)) return false;
    return true;
}

void spi_mem_file_close(SPIMemApp* app) {
    flipper_format_file_close(app->flipper_file);
    flipper_format_free(app->flipper_file);
}
