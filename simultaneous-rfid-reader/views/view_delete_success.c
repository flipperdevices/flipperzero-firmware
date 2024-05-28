#include "view_delete_success.h"

/**
 * @brief      Delete Success Draw Callback.
 * @details    This function is called when the user confirmed deleting a saved UHF tag.
 * @param      canvas    The canvas - Canvas object for drawing the screen.
 * @param      model  The view model - model for the view with variables required for drawing.
*/
void uhf_reader_view_delete_success_draw_callback(Canvas* canvas, void* model) {
    UHFReaderDeleteModel* my_model = (UHFReaderDeleteModel*)model;
    FuriString* xstr = furi_string_alloc();

    //Clearing the canvas, setting the color, font and content displayed.
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 11, " Successfully Deleted!");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 22, "Name:");
    
    //Displaying the name of the saved UHF Tag deleted.
    canvas_draw_str(canvas, 32, 22, furi_string_get_cstr(my_model->selected_tag_name));
    
    //Displaying the index of the saved tag as shown in the Index file.
    furi_string_printf(xstr, "%ld", my_model->selected_tag_index);
    canvas_draw_str(canvas, 4, 33, "EPC Index:");
    canvas_draw_str(canvas, 53, 33, furi_string_get_cstr(xstr));

    //Displaying the EPC scrolling across the screen
    canvas_draw_str(canvas, 4, 44, "EPC:");
    my_model->scrolling_text = (char*)furi_string_get_cstr(my_model->selected_tag_epc);

    //Setting the width of the screen for the sliding window
    uint32_t screen_width_chars = 24;

    // Calculate the start and end indices of the substring to draw
    uint32_t start_pos = my_model->scroll_offset;

    //Calculate the length of the scrolling text
    uint32_t len = strlen(my_model->scrolling_text);

    //I am sure there is a better way to do this that involves slightly safer memory management...
    char visible_part[screen_width_chars + 2];
    memset(visible_part, ' ', screen_width_chars);
    visible_part[screen_width_chars] = '\0';

    //Fill the array up with the epc values
    for(uint32_t i = 0; i < screen_width_chars; i++) {
        uint32_t char_index = start_pos + i;
        if(char_index <= len) {
            visible_part[i] = my_model->scrolling_text[char_index];
        }
    }

    //Draw the visible part of the epc
    canvas_draw_str(canvas, 28, 44, visible_part);

    //Exit button
    elements_button_center(canvas, "Exit");
    furi_string_free(xstr);
}

/**
 * @brief      Callback for delete success input.
 * @details    This function is called when the user presses a button while on the delete success screen.
 * @param      event    The event - InputEvent object.
 * @param      context  The context - UHFReaderApp object.
 * @return     true if the event was handled, false otherwise.
*/
bool uhf_reader_view_delete_success_input_callback(InputEvent* event, void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    
    //Return to the saved menu after deleting the saved epc
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyOk) {
            view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewSaved);
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
    int32_t period = furi_ms_to_ticks(200);
    UHFReaderApp* app = (UHFReaderApp*)context;

    //Call the helper functions below to update the saved UHF tag submenu
    deleteAndUpdateEntry(context, app->selected_tag_index);
    updateDictionaryKeys(context);
    
    //Start the timer for the delete success screen
    furi_assert(app->timer == NULL);
    app->timer =
        furi_timer_alloc(uhf_reader_view_epc_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(app->timer, period);
}

/**
 * @brief      Callback when the user exits the delete success screen.
 * @details    This function is called when the user exits the delete success screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_delete_success_exit_callback(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    
    //Stop and free the timer
    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);
    app->timer = NULL;
}

/**
 * @brief      Allocates the delete success view.
 * @details    This function allocates all variables for the delete success view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_delete_success_alloc(UHFReaderApp* app){

    //Allocating the view and setting all callback functions
    app->view_delete_success = view_alloc();
    view_set_draw_callback(app->view_delete_success, uhf_reader_view_delete_success_draw_callback);
    view_set_input_callback(
        app->view_delete_success, uhf_reader_view_delete_success_input_callback);
    view_set_previous_callback(
        app->view_delete_success, uhf_reader_navigation_saved_exit_callback);
    view_set_enter_callback(
        app->view_delete_success, uhf_reader_view_delete_success_enter_callback);
    view_set_exit_callback(app->view_delete_success, uhf_reader_view_delete_success_exit_callback);
    view_set_context(app->view_delete_success, app);
    
    //Allocating the view model 
    view_allocate_model(
        app->view_delete_success, ViewModelTypeLockFree, sizeof(UHFReaderDeleteModel));
    UHFReaderDeleteModel* model_delete_success = view_get_model(app->view_delete_success);
    
    //Setting default values for the view model
    model_delete_success->selected_tag_epc = furi_string_alloc_set("ABCDEF12");
    model_delete_success->selected_tag_index = 1;
    model_delete_success->selected_tag_name = furi_string_alloc_set("Default Name");
    model_delete_success->scroll_offset = 0;
    model_delete_success->scrolling_text = "Press Delete";
    view_dispatcher_add_view(
        app->view_dispatcher, UHFReaderViewDeleteSuccess, app->view_delete_success);
}

/**
 * @brief      Frees the delete success view.
 * @details    This function frees all variables for the delete success view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_delete_success_free(UHFReaderApp* app){
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewDeleteSuccess);
    view_free(app->view_delete_success);
}