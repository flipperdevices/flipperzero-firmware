#include "view_saved.h"

/**
 * @brief      Callback when the user exits the saved screen.
 * @details    This function is called when the user exits the saved screen.
 * @param      _context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_saved_exit_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewSubmenu;
}

/**
 * @brief      Callback for the saved tag selected
 * @details    This function is called when the user selects a saved tag from the submenu
 * @param      context  The context - UHFReaderApp object 
 * @param      index    The index of the selected saved tag
*/
void uhf_reader_submenu_saved_callback(void* context, uint32_t index) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    switch(index) {
    case UHFReaderSubmenuIndexTagAction:
        break;
    default:
        App->SelectedTagIndex = index;
        view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewTagAction);
        break;
    }
}

/**
 * @brief      Allocates the Saved menu menu
 * @details    This function allocates all variables for the saved menu
 * @param      context  The context - UHFReaderApp object.
*/
void view_saved_menu_alloc(UHFReaderApp* App){

    //Allocate the saved submenu and a FuriString to store the number of saved tags
    App->SubmenuSaved = submenu_alloc();
    submenu_set_header(App->SubmenuSaved, "Saved EPCs");
    FuriString* ExtractedNumTagsStr = furi_string_alloc();
    
    //Try to open the Index_File or create a new one if it doesn't exist
    if(!flipper_format_file_open_existing(App->EpcIndexFile, APP_DATA_PATH("Index_File.txt"))) {
        FURI_LOG_E(TAG, "Creating new index file");
        flipper_format_file_close(App->EpcIndexFile);
        if(!flipper_format_file_open_new(App->EpcIndexFile, APP_DATA_PATH("Index_File.txt"))) {
            FURI_LOG_E(TAG, "Failed to open file");
        } else {
            
            //Sets the default format of the index file if none is present
            if(!flipper_format_write_string_cstr(App->EpcIndexFile, "Number of Tags", "0")) {
                FURI_LOG_E(TAG, "Failed to write to file");
            } else {
                App->NumberOfSavedTags = 0;
            }
        }
    } else {
        //Extract the number of saved tags and set the app variable accordingly 
        if(!flipper_format_read_string(
               App->EpcIndexFile, "Number of Tags", ExtractedNumTagsStr)) {
        } else {
            App->NumberOfSavedTags =
                (uint32_t)atoi(furi_string_get_cstr(ExtractedNumTagsStr));
        }
    }

    //Close the index file
    flipper_format_file_close(App->EpcIndexFile);
    
    //Open the saved EPCs file
    if(!flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open Saved file");
        flipper_format_file_close(App->EpcFile);

    } else {

        //Look through each index and extract the tag name 
        for(uint32_t i = 0; i < (uint32_t)App->NumberOfSavedTags; i++) {
            FuriString* TempStr = furi_string_alloc();
            FuriString* TempTag = furi_string_alloc();
            furi_string_printf(TempStr, "Tag%ld", i + 1);
            if(!flipper_format_read_string(
                   App->EpcFile, furi_string_get_cstr(TempStr), TempTag)) {
                FURI_LOG_D(TAG, "Could not read tag %ld data", i + 1);
            } else {
                
                //Extract the name of the saved epc for this index
                const char* InputString = furi_string_get_cstr(TempTag);
                char* ExtractedName = extract_name(InputString);

                //Create a new submenu item if the name isn't NULL
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
        flipper_format_file_close(App->EpcFile);
    }
    view_set_previous_callback(
        submenu_get_view(App->SubmenuSaved), uhf_reader_navigation_saved_exit_callback);
    view_dispatcher_add_view(
        App->ViewDispatcher, UHFReaderViewSaved, submenu_get_view(App->SubmenuSaved));
}

/**
 * @brief      Frees the saved view.
 * @details    This function frees all variables for the saved view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_saved_free(UHFReaderApp* App){
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewSaved);
    submenu_free(App->SubmenuSaved);
}
