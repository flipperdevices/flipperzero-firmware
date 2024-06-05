#include "saved_epc_functions.h"

/**
 * @brief      Function to update the saved epcs file
 * @details    This function updates the dictionary that is being used to store all the saved epcs.
 * @param      context  - The UHFReaderApp
*/
void update_dictionary_keys(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;

    // Updating the saved epcs menu
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewSaved);
    submenu_free(App->SubmenuSaved);
    App->SubmenuSaved = submenu_alloc();
    submenu_set_header(App->SubmenuSaved, "Saved EPCs");
    uint32_t TotalTags = App->NumberOfSavedTags;

    // Open the saved epcs file and extract the tag name and create the submenu items
    if(flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
        for(uint32_t i = 0; i < TotalTags; i++) {
            FuriString* TempStr = furi_string_alloc();
            FuriString* TempTag = furi_string_alloc();
            furi_string_printf(TempStr, "Tag%ld", i + 1);

            if(!flipper_format_read_string(App->EpcFile, furi_string_get_cstr(TempStr), TempTag)) {
                FURI_LOG_D(TAG, "Could not read tag %ld data", i + 1);
            } else {
                // Extract the name of the saved UHF Tag
                const char* InputString = furi_string_get_cstr(TempTag);
                char* ExtractedName = extract_name(InputString);

                if(ExtractedName != NULL) {
                    submenu_add_item(
                        App->SubmenuSaved,
                        ExtractedName,
                        (i + 1),
                        uhf_reader_submenu_saved_callback,
                        App);
                    free(ExtractedName);
                }
            }
            furi_string_free(TempStr);
            furi_string_free(TempTag);
        }
    }
    view_set_previous_callback(
        submenu_get_view(App->SubmenuSaved), uhf_reader_navigation_saved_exit_callback);
    view_dispatcher_add_view(
        App->ViewDispatcher, UHFReaderViewSaved, submenu_get_view(App->SubmenuSaved));
    flipper_format_file_close(App->EpcFile);
}

/**
 * @brief      Function to delete and update a saved tag in the saved epcs file
 * @details    This function deletes the specified tag and updates the saved epcs file 
 * @param      context  - The UHFReaderApp
 * @param      key_to_delete  - The index of the saved UHF tag to delete
*/
void delete_and_update_entry(void* context, uint32_t KeyToDelete) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    uint32_t TotalTags = App->NumberOfSavedTags;
    FuriString* EpcToDelete = furi_string_alloc();

    // Open the saved epcs file
    if(flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
        // Update subsequent keys
        for(uint32_t i = 1; i <= TotalTags; i++) {
            FuriString* TempStrOld = furi_string_alloc();
            FuriString* TempStrNew = furi_string_alloc();
            furi_string_printf(TempStrOld, "Tag%ld", i);

            // Calculate the new key based on the deletion
            uint32_t NewKey = (i > KeyToDelete) ? i - 1 : i;

            // Skip the deleted key
            if(i != KeyToDelete) {
                furi_string_printf(TempStrNew, "Tag%ld", NewKey);
                FuriString* TempTag = furi_string_alloc();
                if(!flipper_format_read_string(
                       App->EpcFile, furi_string_get_cstr(TempStrOld), TempTag)) {
                    FURI_LOG_D(TAG, "Could not read tag %ld data", i);
                } else {
                    if(!flipper_format_update_string_cstr(
                           App->EpcFile,
                           furi_string_get_cstr(TempStrNew),
                           furi_string_get_cstr(TempTag))) {
                        FURI_LOG_D(TAG, "Could not update tag %ld data", i);
                        flipper_format_write_string_cstr(
                            App->EpcFile,
                            furi_string_get_cstr(TempStrNew),
                            furi_string_get_cstr(TempTag));
                    }
                }

                furi_string_free(TempTag);
            }
            furi_string_free(TempStrOld);
            furi_string_free(TempStrNew);
        }

        furi_string_printf(EpcToDelete, "Tag%ld", App->NumberOfSavedTags);
        if(!flipper_format_delete_key(App->EpcFile, furi_string_get_cstr(EpcToDelete))) {
            FURI_LOG_D(TAG, "Could not delete saved tag with index %ld", App->NumberOfSavedTags);
        }

        // Update the total number of saved tags
        App->NumberOfSavedTags--;
        flipper_format_file_close(App->EpcFile);
    }

    // Update the index file
    FuriString* NewNumEpcs = furi_string_alloc();
    furi_string_printf(NewNumEpcs, "%ld", App->NumberOfSavedTags);
    if(flipper_format_file_open_existing(App->EpcIndexFile, APP_DATA_PATH("Index_File.txt"))) {
        if(!flipper_format_write_string_cstr(
               App->EpcIndexFile, "Number of Tags", furi_string_get_cstr(NewNumEpcs))) {
            FURI_LOG_E(TAG, "Failed to write to file");
        } else {
            FURI_LOG_E(TAG, "Updated index file!");
        }
    }
    furi_string_free(EpcToDelete);
    flipper_format_file_close(App->EpcIndexFile);
    furi_string_free(NewNumEpcs);
}
