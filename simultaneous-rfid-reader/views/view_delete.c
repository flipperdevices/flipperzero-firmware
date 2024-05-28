#include "view_delete.h"

/**
 * @brief      Delete Draw Callback.
 * @details    This function is called when the user selects delete for a saved UHF tag.
 * @param      canvas    The canvas - Canvas object for drawing the screen.
 * @param      model  The view model - model for the view with variables required for drawing.
*/
void uhf_reader_view_delete_draw_callback(Canvas* canvas, void* model) {
    UHFReaderDeleteModel* my_model = (UHFReaderDeleteModel*)model;
    FuriString* xstr = furi_string_alloc();

    //Clearing the canvas, setting the color, font and content displayed.
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 11, " Confirm EPC Deletion!");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 22, "Name:");

    //Displaying the name of the saved UHF Tag to delete.
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

    //Cancel and Confirm buttons
    elements_button_left(canvas, "Cancel");
    elements_button_right(canvas, "Confirm");
    furi_string_free(xstr);
}

/**
 * @brief      Callback for delete input.
 * @details    This function is called when the user presses a button while on the delete screen.
 * @param      event    The event - InputEvent object.
 * @param      context  The context - UHFReaderApp object.
 * @return     true if the event was handled, false otherwise.
*/
bool uhf_reader_view_delete_input_callback(InputEvent* event, void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    
    //Switch to the tag action menu if the left button pressed or the delete success screen if the right button pressed
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewTagAction);
            return true;
        } else if(event->key == InputKeyRight) {
            view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewDeleteSuccess);
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
uint32_t uhf_reader_navigation_delete_exit_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewTagAction;
}

/**
 * @brief      Callback when the user exits the delete screen.
 * @details    This function is called when the user exits the delete screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_delete_exit_callback(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);
    app->timer = NULL;
}

/**
 * @brief      Delete enter callback function.
 * @details    This function is called when the view transitions to the delete screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_delete_enter_callback(void* context) {
    //Grab the period for the timer 
    int32_t period = furi_ms_to_ticks(200);
    UHFReaderApp* app = (UHFReaderApp*)context;
    
    //Create FuriStrings for storing saved UHF tag values 
    FuriString* temp_str = furi_string_alloc();
    FuriString* temp_tag = furi_string_alloc();
    FuriString* temp_epc_name = furi_string_alloc();
    FuriString* temp_epc_str = furi_string_alloc();

    //Open the saved epcs text file 
    if(!flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open Saved file");
        flipper_format_file_close(app->epc_file);

    } else {
        //Read from the file selecting the current tag the user picked
        furi_string_printf(temp_str, "Tag%ld", app->selected_tag_index);
        if(!flipper_format_read_string(app->epc_file, furi_string_get_cstr(temp_str), temp_tag)) {
            FURI_LOG_D(TAG, "Could not read tag %ld data", app->selected_tag_index);
        } else {

            //Grabbing the extracted name and epc from the file to display to the user
            const char* inputString = furi_string_get_cstr(temp_tag);
            char* extractedEPC = extractEPC(inputString);
            char* extractedName = extractName(inputString);
            furi_string_set_str(temp_epc_name, extractedName);
            furi_string_set_str(temp_epc_str, extractedEPC);
            app->epc_delete = temp_epc_str;
            app->epc_name_delete = temp_epc_name;
            
            //Set the view models for both this view and the delete success view
            bool redraw = true;
            with_view_model(
                app->view_delete,
                UHFReaderDeleteModel * model,
                {
                    furi_string_set(model->selected_tag_epc, app->epc_delete);
                    model->selected_tag_index = app->selected_tag_index;
                    furi_string_set(model->selected_tag_name, app->epc_name_delete);
                },
                redraw);
            with_view_model(
                app->view_delete_success,
                UHFReaderDeleteModel * model,
                {
                    furi_string_set(model->selected_tag_epc, app->epc_delete);
                    model->selected_tag_index = app->selected_tag_index;
                    furi_string_set(model->selected_tag_name, app->epc_name_delete);
                },
                redraw);
            //Close the file
            flipper_format_file_close(app->epc_file);
        }
    }

    //Freeing all FuriStrings used
    furi_string_free(temp_tag);
    furi_string_free(temp_str);
    furi_string_free(temp_epc_name);
    furi_string_free(temp_epc_str);
    
    //Start the timer 
    furi_assert(app->timer == NULL);
    app->timer =
        furi_timer_alloc(uhf_reader_view_epc_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(app->timer, period);
}

/**
 * @brief      Allocates the delete view.
 * @details    This function allocates all variables for the delete view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_delete_alloc(UHFReaderApp* app){

    //Allocating the view and setting all callback functions
    app->view_delete = view_alloc();
    view_set_draw_callback(app->view_delete, uhf_reader_view_delete_draw_callback);
    view_set_input_callback(app->view_delete, uhf_reader_view_delete_input_callback);
    view_set_previous_callback(app->view_delete, uhf_reader_navigation_delete_exit_callback);
    view_set_enter_callback(app->view_delete, uhf_reader_view_delete_enter_callback);
    view_set_exit_callback(app->view_delete, uhf_reader_view_delete_exit_callback);
    view_set_context(app->view_delete, app);

    //Allocating the view model
    view_allocate_model(app->view_delete, ViewModelTypeLockFree, sizeof(UHFReaderDeleteModel));
    FuriString* default_epc_name = furi_string_alloc();
    furi_string_set_str(default_epc_name, "Default Name");
    UHFReaderDeleteModel* model_delete = view_get_model(app->view_delete);
    
    //Setting default values for the view model
    model_delete->selected_tag_epc = furi_string_alloc_set("ABCDEF12");
    model_delete->selected_tag_index = 1;
    model_delete->selected_tag_name = default_epc_name;
    model_delete->scroll_offset = 0;
    model_delete->scrolling_text = "Press Delete";
    app->epc_delete = furi_string_alloc_set("Enter Name");
    app->epc_name_delete = furi_string_alloc_set("Enter Name");
    view_dispatcher_add_view(app->view_dispatcher, UHFReaderViewDelete, app->view_delete);
}

/**
 * @brief      Frees the delete view.
 * @details    This function frees all variables for the delete view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_delete_free(UHFReaderApp* app){
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewDelete);
    view_free(app->view_delete);
}