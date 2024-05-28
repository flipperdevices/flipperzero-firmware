#include "view_tag_actions.h"

/**
 * @brief      Callback when the user exits the epc info screen.
 * @details    This function is called when the user exits the epc info screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_epc_info_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewTagAction;
}

/**
 * @brief      Callback for the rename text input screen.
 * @details    This function handles the renaming logic for saved UHF tags.
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_rename_text_updated(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Allocating FuriStrings to store each of the values associated with each UHF Tag
    FuriString* TempTid = furi_string_alloc();
    FuriString* TempRes = furi_string_alloc();
    FuriString* TempMem = furi_string_alloc();
    FuriString* TempStr = furi_string_alloc();
    FuriString* TempTag = furi_string_alloc();
    FuriString* TempEpcStr = furi_string_alloc();

    //Open the saved EPCs file
    if(!flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open Saved file");
        flipper_format_file_close(App->EpcFile);

    } else {
        furi_string_printf(TempStr, "Tag%ld", App->SelectedTagIndex);

        //Get the current tag data for the tag being renamed
        if(!flipper_format_read_string(App->EpcFile, furi_string_get_cstr(TempStr), TempTag)) {
            FURI_LOG_D(TAG, "Could not read tag %ld data", App->SelectedTagIndex);
        } else {
            //Get all the fields for the current tag being renamed
            const char* InputString = furi_string_get_cstr(TempTag);
            char* ExtractedEpc = extract_epc(InputString);
            char* ExtractedTid = extract_tid(InputString);
            char* ExtractedRes = extract_res(InputString);
            char* ExtractedMem = extract_mem(InputString);
            furi_string_set_str(TempEpcStr, ExtractedEpc);
            furi_string_set_str(TempTid, ExtractedTid);
            furi_string_set_str(TempRes, ExtractedRes);
            furi_string_set_str(TempMem, ExtractedMem);
            flipper_format_file_close(App->EpcFile);
        }
    }

    //Open the file again
    if(!flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open file");
    }

    //Allocate new FuriStrings for storing the newly named UHF tag
    FuriString* NumEpcs = furi_string_alloc();
    FuriString* EpcAndName = furi_string_alloc();
    furi_string_printf(NumEpcs, "Tag%ld", App->SelectedTagIndex);
    furi_string_printf(
        EpcAndName,
        "%s:%s:%s:%s:%s",
        App->TempSaveBuffer,
        furi_string_get_cstr(TempEpcStr),
        furi_string_get_cstr(TempTid),
        furi_string_get_cstr(TempRes),
        furi_string_get_cstr(TempMem));

    //Write to the file and update the current string in its index
    if(!flipper_format_update_string_cstr(
           App->EpcFile, furi_string_get_cstr(NumEpcs), furi_string_get_cstr(EpcAndName))) {
        FURI_LOG_E(TAG, "Failed to write to file");
    }

    //Close the file
    flipper_format_file_close(App->EpcFile);

    //Free all FuriStrings used
    furi_string_free(EpcAndName);
    furi_string_free(NumEpcs);
    furi_string_free(TempEpcStr);
    furi_string_free(TempTid);
    furi_string_free(TempRes);
    furi_string_free(TempMem);
    furi_string_free(TempStr);
    furi_string_free(TempTag);

    //Remove the current saved submenu to reallocate and update it
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewSaved);
    submenu_free(App->SubmenuSaved);
    App->SubmenuSaved = submenu_alloc();
    submenu_set_header(App->SubmenuSaved, "Saved EPCs");

    //Update the index file or create a new one doesn't exist
    FuriString* ExtractedNumTagsStr = furi_string_alloc();
    if(!flipper_format_file_open_existing(App->EpcIndexFile, APP_DATA_PATH("Index_File.txt"))) {
        FURI_LOG_E(TAG, "Creating new index file");
        flipper_format_file_close(App->EpcIndexFile);
        if(!flipper_format_file_open_new(App->EpcIndexFile, APP_DATA_PATH("Index_File.txt"))) {
            FURI_LOG_E(TAG, "Failed to open file");
        } else {
            if(!flipper_format_write_string_cstr(App->EpcIndexFile, "Number of Tags", "0")) {
                FURI_LOG_E(TAG, "Failed to write to file");
            } else {
                App->NumberOfSavedTags = 0;
            }
        }
    } else {
        if(!flipper_format_read_string(App->EpcIndexFile, "Number of Tags", ExtractedNumTagsStr)) {
        } else {
            App->NumberOfSavedTags = (uint32_t)atoi(furi_string_get_cstr(ExtractedNumTagsStr));
        }
    }
    furi_string_free(ExtractedNumTagsStr);
    flipper_format_file_close(App->EpcIndexFile);

    //Open the saved epcs file to iterate through and create new submenu items
    if(!flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open Saved file");
        flipper_format_file_close(App->EpcFile);

    } else {
        for(uint32_t i = 0; i < (uint32_t)App->NumberOfSavedTags; i++) {
            FuriString* TempStr = furi_string_alloc();
            FuriString* TempTag = furi_string_alloc();
            furi_string_printf(TempStr, "Tag%ld", i + 1);
            if(!flipper_format_read_string(App->EpcFile, furi_string_get_cstr(TempStr), TempTag)) {
                FURI_LOG_D(TAG, "Could not read tag %ld data", i + 1);
            } else {
                const char* InputString = furi_string_get_cstr(TempTag);
                char* ExtractedName = extract_name(InputString);

                //Update the saved submenu
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
    view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewSaved);
}

/**
 * @brief      Tag Info Submenu Callback
 * @details    Handles the different submenu options for the tag actions menu.
 * @param      context    The UHFReaderApp - the app for working with variables
 * @param      index  The selected submenu index
*/
void uhf_reader_submenu_tag_info_callback(void* context, uint32_t index) {
    UHFReaderApp* App = (UHFReaderApp*)context;

    switch(index) {
    //Handles the Tag Info menu
    case UHFReaderSubmenuIndexTagInfo:
        view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewEpcInfo);
        break;

    //Handles the rename screen
    case UHFReaderSubmenuIndexTagRename:
        text_input_set_header_text(App->RenameInput, "Rename EPC");

        //Grab the text from the rename text input
        bool Redraw = false;
        with_view_model(
            App->ViewWrite,
            UHFReaderWriteModel * Model,
            {
                strncpy(
                    App->TempSaveBuffer,
                    furi_string_get_cstr(Model->EpcName),
                    App->TempBufferSaveSize);
            },
            Redraw);

        // Configure the text input.  When user enters text and clicks OK then the rename text updated callback will run
        bool ClearPreviousText = true;
        text_input_set_result_callback(
            App->RenameInput,
            uhf_reader_rename_text_updated,
            App,
            App->TempSaveBuffer,
            App->TempBufferSaveSize,
            ClearPreviousText);

        // Pressing the BACK button will reload the configure screen.
        view_set_previous_callback(
            text_input_get_view(App->RenameInput), uhf_reader_navigation_epc_info_callback);
        view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewRenameInput);
        break;

    //Handles the write screen
    case UHFReaderSubmenuIndexTagWrite:
        view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewWrite);
        break;

    //Handles the delete screen
    case UHFReaderSubmenuIndexTagDelete:
        view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewDelete);
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
uint32_t uhf_reader_navigation_tag_action_exit_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewSaved;
}

/**
 * @brief      Allocates the tag actions view.
 * @details    This function allocates all variables for the tag actions view.
 * @param      app  The UHFReaderApp object.
*/
void view_tag_actions_alloc(UHFReaderApp* App) {
    //Allocate the tag actions submenu
    App->SubmenuTagActions = submenu_alloc();
    submenu_set_header(App->SubmenuTagActions, "EPC Actions");
    submenu_add_item(
        App->SubmenuTagActions,
        "Tag Data",
        UHFReaderSubmenuIndexTagInfo,
        uhf_reader_submenu_tag_info_callback,
        App);
    submenu_add_item(
        App->SubmenuTagActions,
        "Rename",
        UHFReaderSubmenuIndexTagRename,
        uhf_reader_submenu_tag_info_callback,
        App);
    submenu_add_item(
        App->SubmenuTagActions,
        "Write",
        UHFReaderSubmenuIndexTagWrite,
        uhf_reader_submenu_tag_info_callback,
        App);
    submenu_add_item(
        App->SubmenuTagActions,
        "Delete",
        UHFReaderSubmenuIndexTagDelete,
        uhf_reader_submenu_tag_info_callback,
        App);
    view_set_previous_callback(
        submenu_get_view(App->SubmenuTagActions), uhf_reader_navigation_tag_action_exit_callback);
    view_dispatcher_add_view(
        App->ViewDispatcher, UHFReaderViewTagAction, submenu_get_view(App->SubmenuTagActions));

    //Allocate the rename text input
    App->RenameInput = text_input_alloc();
    view_dispatcher_add_view(
        App->ViewDispatcher, UHFReaderViewRenameInput, text_input_get_view(App->RenameInput));
}

/**
 * @brief      Frees the tag action view.
 * @details    This function frees all variables for the tag actions view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_tag_actions_free(UHFReaderApp* App) {
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewRenameInput);
    text_input_free(App->RenameInput);
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewTagAction);
    submenu_free(App->SubmenuTagActions);
}
