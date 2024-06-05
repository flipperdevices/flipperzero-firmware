#include "view_delete_success.h"

/**
 * @brief      Delete Success Draw Callback.
 * @details    This function is called when the user confirmed deleting a saved UHF tag.
 * @param      canvas    The canvas - Canvas object for drawing the screen.
 * @param      model  The view model - model for the view with variables required for drawing.
*/
void uhf_reader_view_delete_success_draw_callback(Canvas* canvas, void* model) {
    UHFReaderDeleteModel* MyModel = (UHFReaderDeleteModel*)model;
    FuriString* XStr = furi_string_alloc();

    //Clearing the canvas, setting the color, font and content displayed.
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 11, " Successfully Deleted!");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 22, "Name:");

    //Displaying the name of the saved UHF Tag deleted.
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

    //Exit button
    elements_button_center(canvas, "Exit");
    furi_string_free(XStr);
}

/**
 * @brief      Callback for delete success input.
 * @details    This function is called when the user presses a button while on the delete success screen.
 * @param      event    The event - InputEvent object.
 * @param      context  The context - UHFReaderApp object.
 * @return     true if the event was handled, false otherwise.
*/
bool uhf_reader_view_delete_success_input_callback(InputEvent* event, void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Return to the saved menu after deleting the saved epc
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyOk) {
            view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewSaved);
            return true;
        }
    }

    return false;
}

/**
 * @brief      Delete Success enter callback function.
 * @details    This function is called when the view transitions to the delete success screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_delete_success_enter_callback(void* context) {
    //Grab the period for the timer
    int32_t Period = furi_ms_to_ticks(200);
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Call the helper functions below to update the saved UHF tag submenu
    delete_and_update_entry(context, App->SelectedTagIndex);
    update_dictionary_keys(context);

    //Start the timer for the delete success screen
    furi_assert(App->Timer == NULL);
    App->Timer =
        furi_timer_alloc(uhf_reader_view_epc_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(App->Timer, Period);
}

/**
 * @brief      Callback when the user exits the delete success screen.
 * @details    This function is called when the user exits the delete success screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_delete_success_exit_callback(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Stop and free the timer
    furi_timer_stop(App->Timer);
    furi_timer_free(App->Timer);
    App->Timer = NULL;
}

/**
 * @brief      Allocates the delete success view.
 * @details    This function allocates all variables for the delete success view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_delete_success_alloc(UHFReaderApp* App) {
    //Allocating the view and setting all callback functions
    App->ViewDeleteSuccess = view_alloc();
    view_set_draw_callback(App->ViewDeleteSuccess, uhf_reader_view_delete_success_draw_callback);
    view_set_input_callback(App->ViewDeleteSuccess, uhf_reader_view_delete_success_input_callback);
    view_set_previous_callback(App->ViewDeleteSuccess, uhf_reader_navigation_saved_exit_callback);
    view_set_enter_callback(App->ViewDeleteSuccess, uhf_reader_view_delete_success_enter_callback);
    view_set_exit_callback(App->ViewDeleteSuccess, uhf_reader_view_delete_success_exit_callback);
    view_set_context(App->ViewDeleteSuccess, App);

    //Allocating the view model
    view_allocate_model(
        App->ViewDeleteSuccess, ViewModelTypeLockFree, sizeof(UHFReaderDeleteModel));
    UHFReaderDeleteModel* ModelDeleteSuccess = view_get_model(App->ViewDeleteSuccess);

    //Setting default values for the view model
    ModelDeleteSuccess->SelectedTagEpc = furi_string_alloc_set("ABCDEF12");
    ModelDeleteSuccess->SelectedTagIndex = 1;
    ModelDeleteSuccess->SelectedTagName = furi_string_alloc_set("Default Name");
    ModelDeleteSuccess->ScrollOffset = 0;
    ModelDeleteSuccess->ScrollingText = "Press Delete";
    view_dispatcher_add_view(
        App->ViewDispatcher, UHFReaderViewDeleteSuccess, App->ViewDeleteSuccess);
}

/**
 * @brief      Frees the delete success view.
 * @details    This function frees all variables for the delete success view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_delete_success_free(UHFReaderApp* App) {
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewDeleteSuccess);
    view_free(App->ViewDeleteSuccess);
}
