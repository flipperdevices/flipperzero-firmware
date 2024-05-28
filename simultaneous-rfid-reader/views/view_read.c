#include "view_read.h"


/**
 * @brief      Read Draw Callback.
 * @details    This function is called when the user selects read on the main submenu.
 * @param      canvas    The canvas - Canvas object for drawing the screen.
 * @param      model  The view model - model for the view with variables required for drawing.
*/
void uhf_reader_view_read_draw_callback(Canvas* canvas, void* model) {
    UHFReaderConfigModel* my_model = (UHFReaderConfigModel*)model;
    FuriString* xstr = furi_string_alloc();
    
    //Clearing the canvas, setting the color, font and content displayed.
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 11, "           Read Menu:");
    canvas_set_font(canvas, FontSecondary);

    //Dispalying the current number of UHF Tags read
    furi_string_printf(xstr, "%ld", my_model->num_epcs_read);
    canvas_draw_str(canvas, 4, 33, "# EPCs:");
    canvas_draw_str(canvas, 45, 33, furi_string_get_cstr(xstr));

    //Displaying the index of the current tag being viewed 
    furi_string_printf(xstr, "%ld", my_model->cur_epc_index);
    canvas_draw_str(canvas, 70, 33, "Cur Tag:");
    canvas_draw_str(canvas, 115, 33, furi_string_get_cstr(xstr));

    //Displaying the power level
    canvas_draw_str(canvas, 4, 22, "Pow: ");
    canvas_draw_str(canvas, 28, 22, furi_string_get_cstr(my_model->setting_2_power));

    //Displaying the antenna selected 
    canvas_draw_str(canvas, 70, 22, "Ant:");
    canvas_draw_str(canvas, 90, 22, furi_string_get_cstr(my_model->setting_3_value));

    //Displaying the EPC in a scrolling fashion
    my_model->scrolling_text = (char*)furi_string_get_cstr(my_model->epc_value);
    
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

    // Now draw the visible part of the string
    canvas_draw_str(canvas, 0, 44, visible_part);
    

    if(!my_model->is_reading) {
        
        //Dispaly the Prev and Next buttons if the app isn't reading
        elements_button_left(canvas, "Prev");
        elements_button_center(canvas, "Start");
        elements_button_right(canvas, "Next");
    } else {
        elements_button_center(canvas, "Stop");
    }
    furi_string_free(xstr);
}

/**
 * @brief      Callback when the user exits the read screen.
 * @details    This function is called when the user exits the read screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_read_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewRead;
}

/**
 * @brief      Callback for the timer used for the read screen
 * @details    This function is called for timer events.
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_view_read_timer_callback(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    
    //Update the offset for the epc in the read draw callback
    with_view_model(
        app->view_read,
        UHFReaderConfigModel * model,
        {
            uint32_t len = strlen(model->scrolling_text);

            //Incrementing each offset
            model->scroll_offset++;

            //Check the bounds of the offset and reset if necessary
            if(model->scroll_offset >= len) {
                model->scroll_offset = 0;
            }
        },
        true);
    view_dispatcher_send_custom_event(app->view_dispatcher, UHFReaderEventIdRedrawScreen);
}


/**
 * @brief      Callback for the saved text input screen.
 * @details    This function saves the current tag selected with all its info
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_save_text_updated(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    bool redraw = true;
    
    //Allocating FuriStrings to store each of the values associated with each UHF Tag
    FuriString* tid = furi_string_alloc();
    FuriString* mem = furi_string_alloc();
    FuriString* res = furi_string_alloc();
    
    //Set the current EPC to save for the app based on the model's value
    with_view_model(
        app->view_read,
        UHFReaderConfigModel * model,
        {
            furi_string_set(model->epc_name, app->temp_save_buffer);
            model->epc_name = app->epc_name; // was app->epc_name = model->epc_name
            app->epc_to_save = (char*)furi_string_get_cstr(model->epc_value);
        },
        redraw);

    //Set the tid, User and Reserved memory values if any tags were read
    with_view_model(
        app->view_epc,
        UHFRFIDTagModel * model,
        {
            if(app->number_of_epcs_to_read > 0) {
                furi_string_set(tid, model->TID);
                furi_string_set(mem, model->User);
                furi_string_set(res, model->Reserved);
            }
            else{
                furi_string_free(tid);
                furi_string_free(mem);
                furi_string_free(res);
                return;
            }
        },
        redraw);
    
    //Open the saved EPCS file if tags were read
    if(!flipper_format_file_open_append(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open file");
    }

    //Allocating some FuriStrings for use with the file
    FuriString* num_epcs = furi_string_alloc();
    FuriString* epc_and_name = furi_string_alloc();
    
    //Increment the total number of saved tags and write this new tag with all its values to the epc_and_name FuriString
    app->number_of_saved_tags++; 
    furi_string_printf(num_epcs, "Tag%ld", app->number_of_saved_tags);
    furi_string_printf(
        epc_and_name,
        "%s:%s:%s:%s:%s",
        app->temp_save_buffer,
        app->epc_to_save,
        furi_string_get_cstr(tid),
        furi_string_get_cstr(res),
        furi_string_get_cstr(mem));
    
    //Attempt to write the string using the given format
    if(!flipper_format_write_string_cstr(
           app->epc_file, furi_string_get_cstr(num_epcs), furi_string_get_cstr(epc_and_name))) {
        FURI_LOG_E(TAG, "Failed to write to file");
        flipper_format_file_close(app->epc_file);
    }
    else{
        //Add the new tag to the saved submenu
        submenu_add_item(
        app->submenu_saved,
        app->temp_save_buffer,
        app->number_of_saved_tags,
        uhf_reader_submenu_saved_callback,
        app);
        flipper_format_file_close(app->epc_file);

        //Update the Index_File to have the new total number of saved tags and write to the file
        FuriString* new_num_epcs = furi_string_alloc();
        furi_string_printf(new_num_epcs, "%ld", app->number_of_saved_tags);
        if(!flipper_format_file_open_existing(app->epc_index_file, APP_DATA_PATH("Index_File.txt"))) {
            FURI_LOG_E(TAG, "Failed to open index file");
        } else {
            if(!flipper_format_write_string_cstr(
                app->epc_index_file, "Number of Tags", furi_string_get_cstr(new_num_epcs))) {
                FURI_LOG_E(TAG, "Failed to write to file");
                flipper_format_file_close(app->epc_index_file);
            } else {
                flipper_format_file_close(app->epc_index_file);
            }
        }
        furi_string_free(new_num_epcs);
    }
    
    //Freeing all the FuriStrings used
    furi_string_free(epc_and_name);
    furi_string_free(num_epcs);
    furi_string_free(res);
    furi_string_free(tid);
    furi_string_free(mem);
    view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewRead);
}


/**
 * @brief      Callback for read screen input.
 * @details    This function is called when the user presses a button while on the read screen.
 * @param      event    The event - InputEvent object.
 * @param      context  The context - UHFReaderApp object.
 * @return     true if the event was handled, false otherwise.
*/
bool uhf_reader_view_read_input_callback(InputEvent* event, void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;

    //Handles all short input types 
    if(event->type == InputTypeShort) {

        //If the user presses the left button while the app is not reading
        if(event->key == InputKeyLeft && !app->is_reading) {
            bool redraw = true;

            with_view_model(
                app->view_read,
                UHFReaderConfigModel * model,
                {
                    //Check if there are any epcs that were read and decrement the current epc index to show the previous epc 
                    if(app->number_of_epcs_to_read > 0 && model->cur_epc_index > 1) {
                        
                        //TODO: Add checks for number of tids, res, and user mem read in case of short reads
                        model->cur_epc_index -= 1;
                        app->cur_tid_index -= 1;
                        app->cur_res_index -= 1;
                        app->cur_mem_index -= 1;
                        furi_string_set_str(
                            model->epc_value, app->epc_values[model->cur_epc_index * 26]);
                        model->num_epcs_read = app->number_of_epcs_to_read;
                    }
                },
                redraw);
            
            //Updating the current TID, EPC, Reserved and User memory values to display on the view epc screen
            with_view_model(
                app->view_epc,
                UHFRFIDTagModel * model,
                {
                    if(app->number_of_epcs_to_read > 0) {
                        furi_string_set_str(model->EPC, app->epc_values[app->cur_tid_index * 26]);
                    }
                    if(app->number_of_tids_to_read > 0) {
                        furi_string_set_str(model->TID, app->tid_values[app->cur_tid_index * 41]);
                    }
                    if(app->number_of_res_to_read > 0) {
                        furi_string_set_str(
                            model->Reserved, app->res_values[app->cur_res_index * 17]);
                    }
                    if(app->number_of_mem_to_read > 0) {
                        furi_string_set_str(model->User, app->mem_values[app->cur_mem_index * 33]);
                    }
                },
                redraw);

            return true;
        } 
        //Increment the current epc index if the app is not reading
        else if(event->key == InputKeyRight && !app->is_reading) {
            bool redraw = true;
            with_view_model(
                app->view_read,
                UHFReaderConfigModel * model,
                {
                    if(app->number_of_epcs_to_read > 0 &&
                       model->cur_epc_index < app->number_of_epcs_to_read) {
                        
                        //TODO: Add better bounds checking to ensure no crashses with short reads
                        model->cur_epc_index += 1;
                        app->cur_tid_index += 1;
                        app->cur_res_index += 1;
                        app->cur_mem_index += 1;
                        furi_string_set_str(
                            model->epc_value, app->epc_values[model->cur_epc_index * 26]);
                        model->num_epcs_read = app->number_of_epcs_to_read;
                    }
                },
                redraw);

            //Updating the current TID, EPC, Reserved and User memory values to display on the view epc screen
            with_view_model(
                app->view_epc,
                UHFRFIDTagModel * model,
                {
                    if(app->number_of_epcs_to_read > 0) {
                        furi_string_set_str(model->EPC, app->epc_values[app->cur_tid_index * 26]);
                    }
                    if(app->number_of_tids_to_read > 0) {
                        furi_string_set_str(model->TID, app->tid_values[app->cur_tid_index * 41]);
                    }
                    if(app->number_of_res_to_read > 0) {
                        furi_string_set_str(
                            model->Reserved, app->res_values[app->cur_res_index * 17]);
                    }
                    if(app->number_of_mem_to_read > 0) {
                        furi_string_set_str(model->User, app->mem_values[app->cur_mem_index * 33]);
                    }
                },
                redraw);

            return true;
        } 
        
        //Handles the up key press that allows the user to save the currently selected UHF Tag
        else if(event->key == InputKeyUp && !app->is_reading) {
            
            //Setting the text input header 
            text_input_set_header_text(app->save_input, "Save EPC");
            bool redraw = false;
            with_view_model(
                app->view_read,
                UHFReaderConfigModel * model,
                {
                    //Copy the name contents from the text input
                    strncpy(
                        app->temp_save_buffer,
                        furi_string_get_cstr(model->epc_name),
                        app->temp_buffer_save_size);
                },
                redraw);

            //Set the text input result callback function
            bool clear_previous_text = false;
            text_input_set_result_callback(
                app->save_input,
                uhf_reader_save_text_updated,
                app,
                app->temp_save_buffer,
                app->temp_buffer_save_size,
                clear_previous_text);
            view_set_previous_callback(
                text_input_get_view(app->save_input), uhf_reader_navigation_read_callback);
            view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewSaveInput);

            return true;
        } 
        //If the down button is pressed, then show the view epc screen
        else if(event->key == InputKeyDown && !app->is_reading) {
            view_set_previous_callback(app->view_epc, uhf_reader_navigation_read_callback);
            view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewEPCDump);
            return true;
        }
    } 
    //Handles the ok button being pressed 
    else if(event->type == InputTypePress) {
        if(event->key == InputKeyOk) {
            view_dispatcher_send_custom_event(app->view_dispatcher, UHFReaderEventIdOkPressed);

            return true;
        }
    }

    return false;
}

/**
 * @brief      Callback when the user exits the read screen.
 * @details    This function is called when the user exits the read screen.
 * @param      _context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_read_submenu_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewSubmenu;
}


/**
 * @brief      Callback when the user exits the read screen.
 * @details    This function is called when the user exits the read screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_read_exit_callback(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);
    app->timer = NULL;
}

/**
 * @brief      Read enter callback function.
 * @details    This function is called when the view transitions to the read screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_read_enter_callback(void* context) {
   
    //Grab the period for the timer 
    uint32_t period = furi_ms_to_ticks(350); 
    UHFReaderApp* app = (UHFReaderApp*)context;
    
    //Start the timer 
    furi_assert(app->timer == NULL);
    app->timer =
        furi_timer_alloc(uhf_reader_view_read_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(app->timer, period);
    
    //Setting default reading states
    app->is_reading = false;
    with_view_model(
        app->view_read, UHFReaderConfigModel * model, { model->is_reading = false; }, true);
}

/**
 * @brief      Callback for custom read events.
 * @details    This function is called when a custom event is sent to the view dispatcher.
 * @param      event    The event id - UHFReaderAppEventId value.
 * @param      context  The context - UHFReaderApp object.
 * @return     true if the event was handled, false otherwise. 
*/
bool uhf_reader_view_read_custom_event_callback(uint32_t event, void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    
    switch(event) {
    
    //Redraw the screen
    case UHFReaderEventIdRedrawScreen:
        {
            bool redraw = true;
            with_view_model(
                app->view_read, UHFReaderConfigModel * _model, { UNUSED(_model); }, redraw);
            return true;
        }
    
    //The ok button was pressed
    case UHFReaderEventIdOkPressed:

        //Check if the app is reading 
        if(app->is_reading) {
            
            //Stop reading 
            app->is_reading = false;
            with_view_model(
                app->view_read,
                UHFReaderConfigModel * model,
                { model->is_reading = app->is_reading; },
                true);
        } else {

            //Check if the reader is connected before sending a read command
            if(app->reader_connected) {
                app->is_reading = true;
                
                //Send the read command to the RPi Zero via UART
                uart_helper_send(app->uart_helper, "READ\n", 5);
                with_view_model(
                    app->view_read,
                    UHFReaderConfigModel * model,
                    { model->is_reading = app->is_reading; },
                    true);
            }
        }
        return true;
    default:
        return false;
    }
}

/**
 * @brief      Allocates the saved input view.
 * @details    This function allocates all variables for the saved input view.
 * @param      context  The context - UHFReaderApp object.
*/
void saved_input_alloc(UHFReaderApp* app){

    //Allocate a new text input component
    app->save_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, UHFReaderViewSaveInput, text_input_get_view(app->save_input));

    //Setting the max size of the buffer
    app->temp_buffer_save_size = 42; 
    app->temp_save_buffer = (char*)malloc(app->temp_buffer_save_size);
    
}

/**
 * @brief      Allocates the read view.
 * @details    This function allocates all variables for the read view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_read_alloc(UHFReaderApp* app){

    //Allocating the view and setting all callback functions
    saved_input_alloc(app);
    app->view_read = view_alloc();
    view_set_draw_callback(app->view_read, uhf_reader_view_read_draw_callback);
    view_set_input_callback(app->view_read, uhf_reader_view_read_input_callback);
    view_set_previous_callback(app->view_read, uhf_reader_navigation_read_submenu_callback);
    view_set_enter_callback(app->view_read, uhf_reader_view_read_enter_callback);
    view_set_exit_callback(app->view_read, uhf_reader_view_read_exit_callback);
    view_set_context(app->view_read, app);
    view_set_custom_callback(app->view_read, uhf_reader_view_read_custom_event_callback);
    
    //Allocating the view model
    view_allocate_model(app->view_read, ViewModelTypeLockFree, sizeof(UHFReaderConfigModel));
    UHFReaderConfigModel* model = view_get_model(app->view_read);
    FuriString* epc_value_default = furi_string_alloc();
    furi_string_set_str(epc_value_default, "Press Read");
    
    //Setting default values for the view model
    model->setting_1_index = app->setting_1_index;
    model->setting_2_power = app->setting_2_power_str;
    model->setting_3_index = app->setting_3_index;
    model->setting_1_value = furi_string_alloc_set(app->setting_1_names[app->setting_1_index]);
    model->setting_3_value = furi_string_alloc_set(app->setting_3_names[app->setting_3_index]);
    model->epc_name = furi_string_alloc_set("Enter name");
    model->scroll_offset = 0;
    model->scrolling_text = "Press Read";
    model->epc_value = epc_value_default;
    model->cur_epc_index = 1;
    model->num_epcs_read = 0;
    view_dispatcher_add_view(app->view_dispatcher, UHFReaderViewRead, app->view_read);
}

/**
 * @brief      Frees the read view.
 * @details    This function frees all variables for the read view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_read_free(UHFReaderApp* app){
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewSaveInput);
    text_input_free(app->save_input);
    free(app->temp_save_buffer);
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewRead);
    view_free(app->view_read);
}