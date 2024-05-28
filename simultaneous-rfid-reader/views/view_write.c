#include "view_write.h"

/**
 * @brief      Callback for returning to write submenu screen.
 * @details    This function is called when user press back button.
 * @param      _context  The context - unused
 * @return     next view id
*/
uint32_t uhf_reader_navigation_write_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewWrite;
}

/**
 * @brief      Callback for the epc value text input screen.
 * @details    This function saves the current tag selected with all its info
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_epc_value_text_updated(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    bool redraw = true;
    with_view_model(
        app->view_write,
        
        //Keep track of the new epc value
        UHFReaderWriteModel * model,
        { furi_string_set(model->new_epc_value, app->temp_save_buffer); },
        redraw);

    view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewWrite);
}

/**
 * @brief      Callback for write timer elapsed.
 * @details    This function is called when the timer is elapsed for the write screen
 * @param      context  The context - The UHFReaderApp
*/
void uhf_reader_view_write_timer_callback(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    view_dispatcher_send_custom_event(app->view_dispatcher, UHFReaderEventIdRedrawScreen);
}


/**
 * @brief      Write enter callback function.
 * @details    This function is called when the view transitions to the write screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_write_enter_callback(void* context) {
    
    //Grab the period for the timer 
    uint32_t period = furi_ms_to_ticks(200);
    UHFReaderApp* app = (UHFReaderApp*)context;
    
    //Allocate space for the FuriStrings used 
    FuriString* temp_str = furi_string_alloc();
    FuriString* temp_tag = furi_string_alloc();
    
    //Open the saved epcs file to extract the uhf tag info
    if(!flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open Saved file");
        flipper_format_file_close(app->epc_file);
    } else {
        furi_string_printf(temp_str, "Tag%ld", app->selected_tag_index);
        if(!flipper_format_read_string(app->epc_file, furi_string_get_cstr(temp_str), temp_tag)) {
            FURI_LOG_D(TAG, "Could not read tag %ld data", app->selected_tag_index);
        } else {
            
            //Grab the saved uhf tag info from the saved epcs file
            const char* inputString = furi_string_get_cstr(temp_tag);
            furi_string_set(app->epc_to_write, extractEPC(inputString));
            furi_string_set(app->epc_name, extractName(inputString));
            
            //Set the write model uhf tag values accordingly 
            bool redraw = true;
            with_view_model(
                app->view_write,
                UHFReaderWriteModel * model,
                {
                    furi_string_set(model->epc_value, extractEPC(inputString));
                    furi_string_set(model->tid_value, extractTID(inputString));
                    furi_string_set(model->res_value, extractRES(inputString));
                    furi_string_set(model->mem_value, extractMEM(inputString));
                },
                redraw);
            //Close the file
            flipper_format_file_close(app->epc_file);
        }
    }

    //Start the timer 
    furi_assert(app->timer == NULL);
    app->timer =
        furi_timer_alloc(uhf_reader_view_write_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(app->timer, period);
    
    //Setting default reading states and freeing FuriStrings used
    app->is_writing = false;
    furi_string_free(temp_tag);
    furi_string_free(temp_str);
    
}

/**
 * @brief      Callback when the user exits the write screen.
 * @details    This function is called when the user exits the write screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_write_exit_callback(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);
    app->timer = NULL;
}

/**
 * @brief      Callback for custom write events.
 * @details    This function is called when a custom event is sent to the view dispatcher.
 * @param      event    The event id - UHFReaderAppEventId value.
 * @param      context  The context - UHFReaderApp object.
 * @return     true if the event was handled, false otherwise. 
*/
bool uhf_reader_view_write_custom_event_callback(uint32_t event, void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    switch(event) {
    
    // Redraw screen by passing true to last parameter of with_view_model.
    case UHFReaderEventIdRedrawScreen:
        {
            bool redraw = true;
            with_view_model(
                app->view_write, UHFReaderWriteModel * _model, { UNUSED(_model); }, redraw);
            return true;
        }
    
    //The ok button was pressed to trigger a write
    case UHFReaderEventIdOkPressed:
        {
            bool redraw = false;
            with_view_model(
                app->view_write,
                UHFReaderWriteModel * model,
                {
                    //TODO: Modify this to work for the YMR1001
                    //Send the write command 
                    //I'm sure there is a better way to do this but works for now...
                    if(furi_string_equal(model->write_function, WRITE_EPC)) {
                        uart_helper_send(app->uart_helper, "WRITE\n", 6);
                        uart_helper_send_string(app->uart_helper, model->epc_value);
                        uart_helper_send_string(app->uart_helper, model->new_epc_value);
                    } else if(furi_string_equal(model->write_function, WRITE_EPC_MEM)) {
                        uart_helper_send(app->uart_helper, "WRITERES\n", 9);
                        uart_helper_send_string(app->uart_helper, model->epc_value);
                        uart_helper_send_string(app->uart_helper, model->new_epc_value);
                    } else if(furi_string_equal(model->write_function, WRITE_USR_MEM)) {
                        uart_helper_send(app->uart_helper, "WRITEUSR\n", 9);
                        uart_helper_send_string(app->uart_helper, model->epc_value);
                        uart_helper_send_string(app->uart_helper, model->new_epc_value);
                    }
                    else if(furi_string_equal(model->write_function, WRITE_TID_MEM)) {
                        uart_helper_send(app->uart_helper, "WRITETID\n", 9);
                        uart_helper_send_string(app->uart_helper, model->epc_value);
                        uart_helper_send_string(app->uart_helper, model->new_epc_value);
                    }
                    
                },
                redraw);

            return true;
        }
    default:
        return false;
    }
}

/**
 * @brief      Write Draw Callback.
 * @details    This function is called when the user selects write on the main submenu.
 * @param      canvas    The canvas - Canvas object for drawing the screen.
 * @param      model  The view model - model for the view with variables required for drawing.
*/
void uhf_reader_view_write_draw_callback(Canvas* canvas, void* model) {
    UHFReaderWriteModel* my_model = (UHFReaderWriteModel*)model;
    FuriString* xstr = furi_string_alloc();
    
    //Clearing the canvas, setting the color, font and content displayed.
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 11, "           Write Menu:");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 33, "Write Mode:");

    //Displaying the current write mode selected
    canvas_draw_str(canvas, 51, 33, furi_string_get_cstr(my_model->write_function));

    //Display the current power level
    canvas_draw_str(canvas, 4, 22, "Pow: ");
    canvas_draw_str(canvas, 28, 22, furi_string_get_cstr(my_model->setting_2_power));

    //Display the antenna selected
    canvas_draw_str(canvas, 70, 22, "Ant:");
    canvas_draw_str(canvas, 90, 22, furi_string_get_cstr(my_model->setting_3_value));

    //Display the current write status
    canvas_draw_str(canvas, 0, 44, "Write Status: ");
    canvas_draw_str(canvas, 65, 44, furi_string_get_cstr(my_model->write_status));

    //Display the write button 
    if(!my_model->is_writing) {
        elements_button_center(canvas, "Write");

    } else {
        elements_button_center(canvas, "Done");
    }
    furi_string_free(xstr);
}

/**
 * @brief      Callback for write screen input.
 * @details    This function is called when the user presses a button while on the write screen.
 * @param      event    The event - InputEvent object.
 * @param      context  The context - UHFReaderApp object.
 * @return     true if the event was handled, false otherwise.
*/
bool uhf_reader_view_write_input_callback(InputEvent* event, void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    
    //Handle the short input types
    if(event->type == InputTypeShort) {
        
        //If the left button is pressed, then pull up the EPC value and keyboard
        if(event->key == InputKeyLeft && !app->is_writing) {
            text_input_set_header_text(app->epc_write, "EPC Value");
            bool redraw = false;
            with_view_model(
                app->view_write,
                
                //Store the new epc value and mark the write function as the epc selection
                UHFReaderWriteModel * model,
                {
                    strncpy(
                        app->temp_save_buffer,
                        furi_string_get_cstr(model->epc_value),
                        app->temp_buffer_save_size);
                    furi_string_set_str(model->write_function, WRITE_EPC);
                },
                redraw);

            // Configure the text input
            bool clear_previous_text = true;
            text_input_set_result_callback(
                app->epc_write,
                uhf_reader_epc_value_text_updated,
                app,
                app->temp_save_buffer,
                app->temp_buffer_save_size,
                clear_previous_text);
            view_set_previous_callback(
                text_input_get_view(app->epc_write), uhf_reader_navigation_write_callback);
            view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewEPCWriteInput);
            return true;
        } 
        
        //If the right button is pressed, then display the reserved memory bank and display the keyboard
        else if(event->key == InputKeyRight && !app->is_writing) {
            text_input_set_header_text(app->epc_write, "Reserved Memory Bank");
            bool redraw = false;
            with_view_model(
                app->view_write,
                
                //Store the modified value for the reserved memory bank
                UHFReaderWriteModel * model,
                {
                    strncpy(
                        app->temp_save_buffer,
                        furi_string_get_cstr(model->res_value),
                        app->temp_buffer_save_size);
                    furi_string_set_str(model->write_function, WRITE_EPC_MEM);
                },
                redraw);

            // Configure the text input
            bool clear_previous_text = true;
            text_input_set_result_callback(
                app->epc_write,
                uhf_reader_epc_value_text_updated,
                app,
                app->temp_save_buffer,
                app->temp_buffer_save_size,
                clear_previous_text);
            view_set_previous_callback(
                text_input_get_view(app->epc_write), uhf_reader_navigation_write_callback);
            view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewEPCWriteInput);
            return true;
        } 
        
        //If the up button is pressed, then display the user memory bank and keyboard
        else if(event->key == InputKeyUp && !app->is_writing) {
            text_input_set_header_text(app->epc_write, "User Memory Bank");
            bool redraw = false;
            with_view_model(
                app->view_write,
                
                //Store the modified user memory value
                UHFReaderWriteModel * model,
                {
                    strncpy(
                        app->temp_save_buffer,
                        furi_string_get_cstr(model->mem_value),
                        app->temp_buffer_save_size);
                    furi_string_set(model->write_function, WRITE_USR_MEM);
                },
                redraw);

            // Configure the text input
            bool clear_previous_text = true;
            text_input_set_result_callback(
                app->epc_write,
                uhf_reader_epc_value_text_updated,
                app,
                app->temp_save_buffer,
                app->temp_buffer_save_size,
                clear_previous_text);
            view_set_previous_callback(
                text_input_get_view(app->epc_write), uhf_reader_navigation_write_callback);
            view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewEPCWriteInput);
            return true;
        } 
        
        //If the down button is pressed, then display the TID memory bank and the keyboard
        else if(event->key == InputKeyDown && !app->is_writing) {
            text_input_set_header_text(app->epc_write, "TID Memory Bank");
            bool redraw = false;
            with_view_model(
                app->view_write,
                
                //Store the modified TID value
                UHFReaderWriteModel * model,
                {
                    strncpy(
                        app->temp_save_buffer,
                        furi_string_get_cstr(model->tid_value),
                        app->temp_buffer_save_size);
                    furi_string_set_str(model->write_function, WRITE_TID_MEM);
                },
                redraw);

            // Configure the text input
            bool clear_previous_text = true;
            text_input_set_result_callback(
                app->epc_write,
                uhf_reader_epc_value_text_updated,
                app,
                app->temp_save_buffer,
                app->temp_buffer_save_size,
                clear_previous_text);
            view_set_previous_callback(
                text_input_get_view(app->epc_write), uhf_reader_navigation_write_callback);
            view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewEPCWriteInput);
            return true;
        }
    } else if(event->type == InputTypePress) {
        if(event->key == InputKeyOk) {

            //Handle the OK button event
            view_dispatcher_send_custom_event(app->view_dispatcher, UHFReaderEventIdOkPressed);
            return true;
        }
    }
    return false;
}

/**
 * @brief      Callback when the user exits the write screen.
 * @details    This function is called when the user exits the write screen.
 * @param      _context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_write_exit_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewTagAction;
}


/**
 * @brief      Allocates the write view.
 * @details    This function allocates all variables for the write view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_write_alloc(UHFReaderApp* app){

    //Allocating the view and setting all callback functions
    app->view_write = view_alloc();
    view_set_draw_callback(app->view_write, uhf_reader_view_write_draw_callback);
    view_set_input_callback(app->view_write, uhf_reader_view_write_input_callback);
    view_set_previous_callback(app->view_write, uhf_reader_navigation_write_exit_callback);
    view_set_enter_callback(app->view_write, uhf_reader_view_write_enter_callback);
    view_set_exit_callback(app->view_write, uhf_reader_view_write_exit_callback);
    view_set_context(app->view_write, app);
    view_set_custom_callback(app->view_write, uhf_reader_view_write_custom_event_callback);
    
    //Allocating the view model
    view_allocate_model(app->view_write, ViewModelTypeLockFree, sizeof(UHFReaderWriteModel));
    UHFReaderWriteModel* model_write = view_get_model(app->view_write);
    FuriString* epc_name_write_default = furi_string_alloc();
    
    //Setting default values for the view model
    model_write->setting_1_index = app->setting_1_index;
    model_write->setting_2_power = app->setting_2_power_str;
    model_write->setting_3_index = app->setting_3_index;
    model_write->epc_name = epc_name_write_default;
    model_write->setting_1_value = furi_string_alloc_set(app->setting_1_names[app->setting_1_index]);
    model_write->setting_3_value = furi_string_alloc_set(app->setting_3_names[app->setting_3_index]);
    FuriString* epc_write_default = furi_string_alloc();
    furi_string_set_str(epc_write_default, "Press Write");
    FuriString* epc_value_write_default = furi_string_alloc();
    furi_string_set_str(epc_value_write_default, "Press Write");
    model_write->epc_value = epc_value_write_default;
    FuriString* epc_value_write_status = furi_string_alloc();
    furi_string_set_str(epc_value_write_status, "Press Write");
    model_write->write_status = epc_value_write_status;
    FuriString* write_default_epc = furi_string_alloc();
    model_write->new_epc_value = write_default_epc;
    FuriString* default_write_function = furi_string_alloc();
    furi_string_set_str(default_write_function, "Press Arrow Keys");
    model_write->write_function = default_write_function;
    FuriString* default_write_tid = furi_string_alloc();
    furi_string_set_str(default_write_tid, "TID HERE");
    model_write->tid_value = default_write_tid;
    FuriString* default_write_tid_new = furi_string_alloc();
    furi_string_set_str(default_write_tid_new, "NEW TID HERE");
    model_write->new_tid_value = default_write_tid_new;
    FuriString* default_write_res = furi_string_alloc();
    furi_string_set_str(default_write_res, "RES HERE");
    model_write->res_value = default_write_res;
    FuriString* default_write_res_new = furi_string_alloc();
    furi_string_set_str(default_write_res_new, "NEW RES HERE");
    model_write->new_res_value = default_write_res_new;
    FuriString* default_write_mem = furi_string_alloc();
    furi_string_set_str(default_write_mem, "MEM HERE");
    model_write->mem_value = default_write_mem;
    FuriString* default_write_mem_new = furi_string_alloc();
    furi_string_set_str(default_write_mem_new, "NEW MEM HERE");
    model_write->new_mem_value = default_write_mem_new;
    app->epc_name = furi_string_alloc_set("Enter Name");
    app->epc_to_write = furi_string_alloc_set("Enter Name");
    app->epc_write = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, UHFReaderViewEPCWriteInput, text_input_get_view(app->epc_write));

    view_dispatcher_add_view(app->view_dispatcher, UHFReaderViewWrite, app->view_write);
}

/**
 * @brief      Frees the write view.
 * @details    This function frees all variables for the write view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_write_free(UHFReaderApp* app){
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewEPCWriteInput);
    text_input_free(app->epc_write);
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewWrite);
    view_free(app->view_write);
}