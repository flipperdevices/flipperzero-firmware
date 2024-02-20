#include "wifi_marauder_ep.h"

// returns success (if true, then caller needs to free(the_html))
bool wifi_marauder_ep_read_html_file(WifiMarauderApp* app, uint8_t** the_html, size_t* html_size) {
    // browse for files
    FuriString* predefined_filepath = furi_string_alloc_set_str(MARAUDER_APP_FOLDER_HTML);
    FuriString* selected_filepath = furi_string_alloc();
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, ".html", &I_Text_10x10);
    if(!dialog_file_browser_show(
           app->dialogs, selected_filepath, predefined_filepath, &browser_options)) {
        return false;
    }

    File* index_html = storage_file_alloc(app->storage);
    if(!storage_file_open(
           index_html, furi_string_get_cstr(selected_filepath), FSAM_READ, FSOM_OPEN_EXISTING)) {
        dialog_message_show_storage_error(app->dialogs, "Cannot open file");
        return false;
    }

    uint64_t size = storage_file_size(index_html);

    *the_html = malloc(size); // to be freed by caller
    uint8_t* buf_ptr = *the_html;
    size_t read = 0;
    while(read < size) {
        size_t to_read = size - read;
        if(to_read > UINT16_MAX) to_read = UINT16_MAX;
        uint16_t now_read = storage_file_read(index_html, buf_ptr, (uint16_t)to_read);
        read += now_read;
        buf_ptr += now_read;
    }

    *html_size = read;

    storage_file_close(index_html);
    storage_file_free(index_html);

    furi_string_free(selected_filepath);
    furi_string_free(predefined_filepath);

    return true;
}