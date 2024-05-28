#include "view_tag_actions.h"

/**
 * @brief      Callback when the user exits the epc info screen.
 * @details    This function is called when the user exits the epc info screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_epc_info_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewTagAction;
}


/**
 * @brief      Callback for the rename text input screen.
 * @details    This function handles the renaming logic for saved UHF tags.
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_rename_text_updated(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;

    //Allocating FuriStrings to store each of the values associated with each UHF Tag
    FuriString* temp_tid = furi_string_alloc();
    FuriString* temp_res = furi_string_alloc();
    FuriString* temp_mem = furi_string_alloc();
    FuriString* temp_str = furi_string_alloc();
    FuriString* temp_tag = furi_string_alloc();
    FuriString* temp_epc_str = furi_string_alloc();
    
    //Open the saved EPCs file
    if(!flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open Saved file");
        flipper_format_file_close(app->epc_file);

    } else {
        furi_string_printf(temp_str, "Tag%ld", app->selected_tag_index);
        
        //Get the current tag data for the tag being renamed
        if(!flipper_format_read_string(app->epc_file, furi_string_get_cstr(temp_str), temp_tag)) {
            FURI_LOG_D(TAG, "Could not read tag %ld data", app->selected_tag_index);
        } else {
            
            //Get all the fields for the current tag being renamed
            const char* inputString = furi_string_get_cstr(temp_tag);
            char* extractedEPC = extractEPC(inputString);
            char* extractedTID = extractTID(inputString);
            char* extractedRES = extractRES(inputString);
            char* extractedMEM = extractMEM(inputString);
            furi_string_set_str(temp_epc_str, extractedEPC);
            furi_string_set_str(temp_tid, extractedTID);
            furi_string_set_str(temp_res, extractedRES);
            furi_string_set_str(temp_mem, extractedMEM);
            flipper_format_file_close(app->epc_file);
        }
    }

    //Open the file again
    if(!flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open file");
    }

    //Allocate new FuriStrings for storing the newly named UHF tag
    FuriString* num_epcs = furi_string_alloc();
    FuriString* epc_and_name = furi_string_alloc();
    furi_string_printf(num_epcs, "Tag%ld", app->selected_tag_index);
    furi_string_printf(
        epc_and_name,
        "%s:%s:%s:%s:%s",
        app->temp_save_buffer,
        furi_string_get_cstr(temp_epc_str),
        furi_string_get_cstr(temp_tid),
        furi_string_get_cstr(temp_res),
        furi_string_get_cstr(temp_mem));
    
    //Write to the file and update the current string in its index
    if(!flipper_format_update_string_cstr(
           app->epc_file, furi_string_get_cstr(num_epcs), furi_string_get_cstr(epc_and_name))) {
        FURI_LOG_E(TAG, "Failed to write to file");
    }

    //Close the file
    flipper_format_file_close(app->epc_file);

    //Free all FuriStrings used
    furi_string_free(epc_and_name);
    furi_string_free(num_epcs);
    furi_string_free(temp_epc_str);
    furi_string_free(temp_tid);
    furi_string_free(temp_res);
    furi_string_free(temp_mem);
    furi_string_free(temp_str);
    furi_string_free(temp_tag);
    
    //Remove the current saved submenu to reallocate and update it 
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewSaved);
    submenu_free(app->submenu_saved);
    app->submenu_saved = submenu_alloc();
    submenu_set_header(app->submenu_saved, "Saved EPCs");

    //Update the index file or create a new one doesn't exist
    FuriString* extracted_num_tags_str = furi_string_alloc();
    if(!flipper_format_file_open_existing(app->epc_index_file, APP_DATA_PATH("Index_File.txt"))) {
        FURI_LOG_E(TAG, "Creating new index file");
        flipper_format_file_close(app->epc_index_file);
        if(!flipper_format_file_open_new(app->epc_index_file, APP_DATA_PATH("Index_File.txt"))) {
            FURI_LOG_E(TAG, "Failed to open file");
        } else {
            if(!flipper_format_write_string_cstr(app->epc_index_file, "Number of Tags", "0")) {
                FURI_LOG_E(TAG, "Failed to write to file");
            } else {
                app->number_of_saved_tags = 0;
            }
        }
    } else {
        if(!flipper_format_read_string(
               app->epc_index_file, "Number of Tags", extracted_num_tags_str)) {
        } else {
            app->number_of_saved_tags =
                (uint32_t)atoi(furi_string_get_cstr(extracted_num_tags_str));
        }
    }
    furi_string_free(extracted_num_tags_str);
    flipper_format_file_close(app->epc_index_file);
    
    //Open the saved epcs file to iterate through and create new submenu items 
    if(!flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open Saved file");
        flipper_format_file_close(app->epc_file);

    } else {
        for(uint32_t i = 0; i < (uint32_t)app->number_of_saved_tags; i++) {
            FuriString* temp_str = furi_string_alloc();
            FuriString* temp_tag = furi_string_alloc();
            furi_string_printf(temp_str, "Tag%ld", i + 1);
            if(!flipper_format_read_string(
                   app->epc_file, furi_string_get_cstr(temp_str), temp_tag)) {
                FURI_LOG_D(TAG, "Could not read tag %ld data", i + 1);
            } else {
                const char* inputString = furi_string_get_cstr(temp_tag);
                char* extractedName = extractName(inputString);
                
                //Update the saved submenu
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
    view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewSaved);
}

/**
 * @brief      Tag Info Submenu Callback
 * @details    Handles the different submenu options for the tag actions menu.
 * @param      context    The UHFReaderApp - the app for working with variables
 * @param      index  The selected submenu index
*/
void uhf_reader_submenu_tag_info_callback(void* context, uint32_t index) {
    UHFReaderApp* app = (UHFReaderApp*)context;

    switch(index) {
    
    //Handles the Tag Info menu
    case UHFReaderSubmenuIndexTagInfo:
        view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewEPCInfo);
        break;

    //Handles the rename screen
    case UHFReaderSubmenuIndexTagRename:
        text_input_set_header_text(app->rename_input, "Rename EPC");

        //Grab the text from the rename text input
        bool redraw = false;
        with_view_model(
            app->view_write,
            UHFReaderWriteModel * model,
            {
                strncpy(
                    app->temp_save_buffer,
                    furi_string_get_cstr(model->epc_name),
                    app->temp_buffer_save_size);
            },
            redraw);

        // Configure the text input.  When user enters text and clicks OK then the rename text updated callback will run
        bool clear_previous_text = true;
        text_input_set_result_callback(
            app->rename_input,
            uhf_reader_rename_text_updated,
            app,
            app->temp_save_buffer,
            app->temp_buffer_save_size,
            clear_previous_text);

        // Pressing the BACK button will reload the configure screen.
        view_set_previous_callback(
            text_input_get_view(app->rename_input), uhf_reader_navigation_epc_info_callback);
        view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewRenameInput);
        break;

    //Handles the write screen
    case UHFReaderSubmenuIndexTagWrite:
        view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewWrite);
        break;
    
    //Handles the delete screen
    case UHFReaderSubmenuIndexTagDelete:
        view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewDelete);
        break;
    default:
        break;
    }
}

/**
 * @brief      Callback when the user exits the tag action screen.
 * @details    This function is called when the user exits the tag action screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_tag_action_exit_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewSaved;
}

/**
 * @brief      Allocates the tag actions view.
 * @details    This function allocates all variables for the tag actions view.
 * @param      app  The UHFReaderApp object.
*/
void view_tag_actions_alloc(UHFReaderApp* app){

    //Allocate the tag actions submenu
    app->submenu_tag_actions = submenu_alloc();
    submenu_set_header(app->submenu_tag_actions, "EPC Actions");
    submenu_add_item(
        app->submenu_tag_actions,
        "Tag Data",
        UHFReaderSubmenuIndexTagInfo,
        uhf_reader_submenu_tag_info_callback,
        app);
    submenu_add_item(
        app->submenu_tag_actions,
        "Rename",
        UHFReaderSubmenuIndexTagRename,
        uhf_reader_submenu_tag_info_callback,
        app);
    submenu_add_item(
        app->submenu_tag_actions,
        "Write",
        UHFReaderSubmenuIndexTagWrite,
        uhf_reader_submenu_tag_info_callback,
        app);
    submenu_add_item(
        app->submenu_tag_actions,
        "Delete",
        UHFReaderSubmenuIndexTagDelete,
        uhf_reader_submenu_tag_info_callback,
        app);
    view_set_previous_callback(
        submenu_get_view(app->submenu_tag_actions),
        uhf_reader_navigation_tag_action_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, UHFReaderViewTagAction, submenu_get_view(app->submenu_tag_actions));

    //Allocate the rename text input
    app->rename_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, UHFReaderViewRenameInput, text_input_get_view(app->rename_input));
    
}

/**
 * @brief      Frees the tag action view.
 * @details    This function frees all variables for the tag actions view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_tag_actions_free(UHFReaderApp* app){
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewRenameInput);
    text_input_free(app->rename_input);
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewTagAction);
    submenu_free(app->submenu_tag_actions);
}