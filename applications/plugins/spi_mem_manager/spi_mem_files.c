#include "spi_mem_app.h"

void spi_mem_file_create_folder(SPIMemApp* app) {
    if(!storage_simply_mkdir(app->storage, SPI_MEM_FILE_FOLDER)) {
        dialog_message_show_storage_error(app->dialogs, "Cannot create\napp folder");
    }
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

bool spi_mem_file_delete(SPIMemApp* app) {
    return storage_simply_remove(app->storage, furi_string_get_cstr(app->file_path));
}
