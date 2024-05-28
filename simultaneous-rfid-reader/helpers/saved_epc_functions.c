#include "saved_epc_functions.h"

/**
 * @brief      Function to update the saved epcs file
 * @details    This function updates the dictionary that is being used to store all the saved epcs.
 * @param      context  - The UHFReaderApp
*/
void updateDictionaryKeys(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;

    //Updating the saved epcs menu
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewSaved);
    submenu_free(app->submenu_saved);
    app->submenu_saved = submenu_alloc();
    submenu_set_header(app->submenu_saved, "Saved EPCs");
    uint32_t totalTags = app->number_of_saved_tags;

    //Open the saved epcs file and extract the tag name and create the submenu items
    if(flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
        for(uint32_t i = 0; i < totalTags; i++) {
            
            FuriString* temp_str = furi_string_alloc();
            FuriString* temp_tag = furi_string_alloc();
            furi_string_printf(temp_str, "Tag%ld", i + 1);
            
            if(!flipper_format_read_string(
                   app->epc_file, furi_string_get_cstr(temp_str), temp_tag)) {
                FURI_LOG_D(TAG, "Could not read tag %ld data", i + 1);
            } else {
                
                //Extract the name of the saved UHF Tag
                const char* inputString = furi_string_get_cstr(temp_tag);
                char* extractedName = extractName(inputString);

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
    }
    view_set_previous_callback(
        submenu_get_view(app->submenu_saved), uhf_reader_navigation_saved_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, UHFReaderViewSaved, submenu_get_view(app->submenu_saved));
    flipper_format_file_close(app->epc_file);
}

/**
 * @brief      Function to delete and update a saved tag in the saved epcs file
 * @details    This function deletes the specified tag and updates the saved epcs file 
 * @param      context  - The UHFReaderApp
 * @param      keyToDelete  - The index of the saved UHF tag to delete
*/
void deleteAndUpdateEntry(void* context, uint32_t keyToDelete) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    uint32_t totalTags = app->number_of_saved_tags;
    FuriString* epc_to_delete = furi_string_alloc();

    //Open the saved epcs file
    if(flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
        
        //Update subsequent keys
        for(uint32_t i = 1; i <= totalTags; i++) {
            FuriString* temp_str_old = furi_string_alloc();
            FuriString* temp_str_new = furi_string_alloc();
            furi_string_printf(temp_str_old, "Tag%ld", i);

            //Calculate the new key based on the deletion
            uint32_t newKey = (i > keyToDelete) ? i - 1 : i;

            //Skip the deleted key
            if(i != keyToDelete) { 
                furi_string_printf(temp_str_new, "Tag%ld", newKey);
                FuriString* temp_tag = furi_string_alloc();
                if(!flipper_format_read_string(
                       app->epc_file, furi_string_get_cstr(temp_str_old), temp_tag)) {
                    FURI_LOG_D(TAG, "Could not read tag %ld data", i);
                } else {
                    if(!flipper_format_update_string_cstr(
                           app->epc_file,
                           furi_string_get_cstr(temp_str_new),
                           furi_string_get_cstr(temp_tag))) {
                        FURI_LOG_D(TAG, "Could not update tag %ld data", i);
                        flipper_format_write_string_cstr(
                            app->epc_file,
                            furi_string_get_cstr(temp_str_new),
                            furi_string_get_cstr(temp_tag));
                    }
                }

                furi_string_free(temp_tag);
            }
            furi_string_free(temp_str_old);
            furi_string_free(temp_str_new);
        }

        furi_string_printf(epc_to_delete, "Tag%ld", app->number_of_saved_tags);
        if(!flipper_format_delete_key(app->epc_file, furi_string_get_cstr(epc_to_delete))) {
            FURI_LOG_D(
                TAG, "Could not delete saved tag with index %ld", app->number_of_saved_tags);
        }
        
        //Update the total number of saved tags
        app->number_of_saved_tags--;
        flipper_format_file_close(app->epc_file);
    }

    //Update the index file
    FuriString* new_num_epcs = furi_string_alloc();
    furi_string_printf(new_num_epcs, "%ld", app->number_of_saved_tags);
    if(flipper_format_file_open_existing(app->epc_index_file, APP_DATA_PATH("Index_File.txt"))) {
        if(!flipper_format_write_string_cstr(
               app->epc_index_file, "Number of Tags", furi_string_get_cstr(new_num_epcs))) {
            FURI_LOG_E(TAG, "Failed to write to file");
        } else {
            FURI_LOG_E(TAG, "Updated index file!");
        }
    }
    furi_string_free(epc_to_delete);
    flipper_format_file_close(app->epc_index_file);
    furi_string_free(new_num_epcs);
}