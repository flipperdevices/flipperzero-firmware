#include "view_delete.h"

/**
 * @brief      Delete Draw Callback.
 * @details    This function is called when the user selects delete for a saved UHF tag.
 * @param      canvas    The canvas - Canvas object for drawing the screen.
 * @param      model  The view model - model for the view with variables required for drawing.
*/
void uhf_reader_view_delete_draw_callback(Canvas* canvas, void* model) {
    UHFReaderDeleteModel* MyModel = (UHFReaderDeleteModel*)model;
    FuriString* XStr = furi_string_alloc();

    //Clearing the canvas, setting the color, font and content displayed.
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 11, " Confirm EPC Deletion!");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 22, "Name:");

    //Displaying the name of the saved UHF Tag to delete.
    canvas_draw_str(canvas, 32, 22, furi_string_get_cstr(MyModel->SelectedTagName));

    //Displaying the index of the saved tag as shown in the Index file.
    furi_string_printf(XStr, "%ld", MyModel->SelectedTagIndex);
    canvas_draw_str(canvas, 4, 33, "EPC Index:");
    canvas_draw_str(canvas, 53, 33, furi_string_get_cstr(XStr));

    //Displaying the EPC scrolling across the screen
    canvas_draw_str(canvas, 4, 44, "EPC:");
    MyModel->ScrollingText = (char*)furi_string_get_cstr(MyModel->SelectedTagEpc);

    //Setting the width of the screen for the sliding window
    uint32_t ScreenWidthChars = 24;

    // Calculate the start and end indices of the substring to draw
    uint32_t StartPos = MyModel->ScrollOffset;

    //Calculate the length of the scrolling text
    uint32_t Len = strlen(MyModel->ScrollingText);

    //I am sure there is a better way to do this that involves slightly safer memory management...
    char VisiblePart[ScreenWidthChars + 2];
    memset(VisiblePart, ' ', ScreenWidthChars);
    VisiblePart[ScreenWidthChars] = '\0';

    //Fill the array up with the epc values
    for(uint32_t i = 0; i < ScreenWidthChars; i++) {
        uint32_t CharIndex = StartPos + i;
        if(CharIndex <= Len) {
            VisiblePart[i] = MyModel->ScrollingText[CharIndex];
        }
    }

    //Draw the visible part of the epc
    canvas_draw_str(canvas, 28, 44, VisiblePart);

    //Cancel and Confirm buttons
    elements_button_left(canvas, "Cancel");
    elements_button_right(canvas, "Confirm");
    furi_string_free(XStr);
}

/**
 * @brief      Callback for delete input.
 * @details    This function is called when the user presses a button while on the delete screen.
 * @param      event    The event - InputEvent object.
 * @param      context  The context - UHFReaderApp object.
 * @return     true if the event was handled, false otherwise.
*/
bool uhf_reader_view_delete_input_callback(InputEvent* event, void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Switch to the tag action menu if the left button pressed or the delete success screen if the right button pressed
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewTagAction);
            return true;
        } else if(event->key == InputKeyRight) {
            view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewDeleteSuccess);
            return true;
        }
    }

    return false;
}

/**
 * @brief      Callback when the user exits the delete screen.
 * @details    This function is called when the user exits the delete screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_delete_exit_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewTagAction;
}

/**
 * @brief      Callback when the user exits the delete screen.
 * @details    This function is called when the user exits the delete screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_delete_exit_callback(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    furi_timer_stop(App->Timer);
    furi_timer_free(App->Timer);
    App->Timer = NULL;
}

/**
 * @brief      Delete enter callback function.
 * @details    This function is called when the view transitions to the delete screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_delete_enter_callback(void* context) {
    //Grab the period for the timer
    int32_t Period = furi_ms_to_ticks(200);
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Create FuriStrings for storing saved UHF tag values
    FuriString* TempStr = furi_string_alloc();
    FuriString* TempTag = furi_string_alloc();
    FuriString* TempEpcName = furi_string_alloc();
    FuriString* TempEpcStr = furi_string_alloc();

    //Open the saved epcs text file
    if(!flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open Saved file");
        flipper_format_file_close(App->EpcFile);

    } else {
        //Read from the file selecting the current tag the user picked
        furi_string_printf(TempStr, "Tag%ld", App->SelectedTagIndex);
        if(!flipper_format_read_string(App->EpcFile, furi_string_get_cstr(TempStr), TempTag)) {
            FURI_LOG_D(TAG, "Could not read tag %ld data", App->SelectedTagIndex);
        } else {
            //Grabbing the extracted name and epc from the file to display to the user
            const char* InputString = furi_string_get_cstr(TempTag);
            char* ExtractedEpc = extract_epc(InputString);
            char* ExtractedName = extract_name(InputString);
            furi_string_set_str(TempEpcName, ExtractedName);
            furi_string_set_str(TempEpcStr, ExtractedEpc);
            App->EpcDelete = TempEpcStr;
            App->EpcNameDelete = TempEpcName;

            //Set the view models for both this view and the delete success view
            bool Redraw = true;
            with_view_model(
                App->ViewDelete,
                UHFReaderDeleteModel * model,
                {
                    furi_string_set(model->SelectedTagEpc, App->EpcDelete);
                    model->SelectedTagIndex = App->SelectedTagIndex;
                    furi_string_set(model->SelectedTagName, App->EpcNameDelete);
                },
                Redraw);
            with_view_model(
                App->ViewDeleteSuccess,
                UHFReaderDeleteModel * model,
                {
                    furi_string_set(model->SelectedTagEpc, App->EpcDelete);
                    model->SelectedTagIndex = App->SelectedTagIndex;
                    furi_string_set(model->SelectedTagName, App->EpcNameDelete);
                },
                Redraw);
            //Close the file
            flipper_format_file_close(App->EpcFile);
        }
    }

    //Freeing all FuriStrings used
    furi_string_free(TempTag);
    furi_string_free(TempStr);
    furi_string_free(TempEpcName);
    furi_string_free(TempEpcStr);

    //Start the timer
    furi_assert(App->Timer == NULL);
    App->Timer =
        furi_timer_alloc(uhf_reader_view_epc_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(App->Timer, Period);
}

/**
 * @brief      Allocates the delete view.
 * @details    This function allocates all variables for the delete view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_delete_alloc(UHFReaderApp* App) {
    //Allocating the view and setting all callback functions
    App->ViewDelete = view_alloc();
    view_set_draw_callback(App->ViewDelete, uhf_reader_view_delete_draw_callback);
    view_set_input_callback(App->ViewDelete, uhf_reader_view_delete_input_callback);
    view_set_previous_callback(App->ViewDelete, uhf_reader_navigation_delete_exit_callback);
    view_set_enter_callback(App->ViewDelete, uhf_reader_view_delete_enter_callback);
    view_set_exit_callback(App->ViewDelete, uhf_reader_view_delete_exit_callback);
    view_set_context(App->ViewDelete, App);

    //Allocating the view model
    view_allocate_model(App->ViewDelete, ViewModelTypeLockFree, sizeof(UHFReaderDeleteModel));
    FuriString* DefaultEpcName = furi_string_alloc();
    furi_string_set_str(DefaultEpcName, "Default Name");
    UHFReaderDeleteModel* ModelDelete = view_get_model(App->ViewDelete);

    //Setting default values for the view model
    ModelDelete->SelectedTagEpc = furi_string_alloc_set("ABCDEF12");
    ModelDelete->SelectedTagIndex = 1;
    ModelDelete->SelectedTagName = DefaultEpcName;
    ModelDelete->ScrollOffset = 0;
    ModelDelete->ScrollingText = "Press Delete";
    App->EpcDelete = furi_string_alloc_set("Enter Name");
    App->EpcNameDelete = furi_string_alloc_set("Enter Name");
    view_dispatcher_add_view(App->ViewDispatcher, UHFReaderViewDelete, App->ViewDelete);
}

/**
 * @brief      Frees the delete view.
 * @details    This function frees all variables for the delete view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_delete_free(UHFReaderApp* App) {
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewDelete);
    view_free(App->ViewDelete);
}
