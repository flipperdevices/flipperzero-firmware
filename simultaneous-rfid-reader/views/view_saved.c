#include "view_saved.h"

/**
 * @brief      Callback when the user exits the saved screen.
 * @details    This function is called when the user exits the saved screen.
 * @param      _context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_saved_exit_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewSubmenu;
}


/**
 * @brief      Callback for the saved tag selected
 * @details    This function is called when the user selects a saved tag from the submenu
 * @param      context  The context - UHFReaderApp object 
 * @param      index    The index of the selected saved tag
*/
void uhf_reader_submenu_saved_callback(void* context, uint32_t index) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    switch(index) {
    case UHFReaderSubmenuIndexTagAction:
        break;
    default:
        app->selected_tag_index = index;
        view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewTagAction);
        break;
    }
}

/**
 * @brief      Allocates the Saved menu menu
 * @details    This function allocates all variables for the saved menu
 * @param      context  The context - UHFReaderApp object.
*/
void view_saved_menu_alloc(UHFReaderApp* app){

    //Allocate the saved submenu and a FuriString to store the number of saved tags
    app->submenu_saved = submenu_alloc();
    submenu_set_header(app->submenu_saved, "Saved EPCs");
    FuriString* extracted_num_tags_str = furi_string_alloc();
    
    //Try to open the Index_File or create a new one if it doesn't exist
    if(!flipper_format_file_open_existing(app->epc_index_file, APP_DATA_PATH("Index_File.txt"))) {
        FURI_LOG_E(TAG, "Creating new index file");
        flipper_format_file_close(app->epc_index_file);
        if(!flipper_format_file_open_new(app->epc_index_file, APP_DATA_PATH("Index_File.txt"))) {
            FURI_LOG_E(TAG, "Failed to open file");
        } else {
            
            //Sets the default format of the index file if none is present
            if(!flipper_format_write_string_cstr(app->epc_index_file, "Number of Tags", "0")) {
                FURI_LOG_E(TAG, "Failed to write to file");
            } else {
                app->number_of_saved_tags = 0;
            }
        }
    } else {
        //Extract the number of saved tags and set the app variable accordingly 
        if(!flipper_format_read_string(
               app->epc_index_file, "Number of Tags", extracted_num_tags_str)) {
        } else {
            app->number_of_saved_tags =
                (uint32_t)atoi(furi_string_get_cstr(extracted_num_tags_str));
        }
    }

    //Close the index file
    flipper_format_file_close(app->epc_index_file);
    
    //Open the saved EPCs file
    if(!flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open Saved file");
        flipper_format_file_close(app->epc_file);

    } else {

        //Look through each index and extract the tag name 
        for(uint32_t i = 0; i < (uint32_t)app->number_of_saved_tags; i++) {
            FuriString* temp_str = furi_string_alloc();
            FuriString* temp_tag = furi_string_alloc();
            furi_string_printf(temp_str, "Tag%ld", i + 1);
            if(!flipper_format_read_string(
                   app->epc_file, furi_string_get_cstr(temp_str), temp_tag)) {
                FURI_LOG_D(TAG, "Could not read tag %ld data", i + 1);
            } else {
                
                //Extract the name of the saved epc for this index
                const char* inputString = furi_string_get_cstr(temp_tag);
                char* extractedName = extractName(inputString);

                //Create a new submenu item if the name isn't NULL
                if(extractedName != NULL) {
                    submenu_add_item(
                        app->submenu_saved,
                        extractedName,
                        (i + 1),
                        uhf_reader_submenu_saved_callback,
                        app); 
                    free(extractedName);
                } 
            }
            furi_string_free(temp_str);
            furi_string_free(temp_tag);
        }
        flipper_format_file_close(app->epc_file);
    }
    view_set_previous_callback(
        submenu_get_view(app->submenu_saved), uhf_reader_navigation_saved_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, UHFReaderViewSaved, submenu_get_view(app->submenu_saved));
}

/**
 * @brief      Frees the saved view.
 * @details    This function frees all variables for the saved view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_saved_free(UHFReaderApp* app){
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewSaved);
    submenu_free(app->submenu_saved);
}