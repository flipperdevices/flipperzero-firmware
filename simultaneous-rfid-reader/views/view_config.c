#include "view_config.h"


/**
 * @brief      Callback for returning to submenu.
 * @details    This function is called when user press back button.
 * @param      _context  The context - unused
 * @return     next view id
*/
uint32_t uhf_reader_navigation_config_submenu_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewSubmenu;
}

/**
 * @brief      Allocates the configuration menu
 * @details    This function allocates all views and variables related to the configuration menu.
 * @param      app  The UHFReaderApp - used to allocate app variables and views.
*/
void view_config_alloc(UHFReaderApp* app) {

    //Allocate the power input menu
    power_menu_alloc(app);
    
    //Creating the variable item list
    app->variable_item_list_config = variable_item_list_alloc();
    variable_item_list_reset(app->variable_item_list_config);
    
    //Initializing configuration setting variables
    app->setting_1_values[0] = 1;
    app->setting_1_values[1] = 2;
    app->setting_1_names[0] = "Disconnected";
    app->setting_1_names[1] = "Connected";
    app->reader_connected = false;
    app->setting_1_config_label = "Reader Status:";
    app->setting_2_config_label = "Power Level";
    app->setting_2_entry_text = "Enter Value In Range 0-2700";
    app->setting_2_default_value = "1500";
    app->setting_3_values[0] = 1;
    app->setting_3_values[1] = 2;
    app->setting_3_names[0] = "Internal";
    app->setting_3_names[1] = "External";
    app->setting_3_config_label = "Antenna";
    
    // Add setting 1 to variable item list
    VariableItem* item = variable_item_list_add(
        app->variable_item_list_config,
        app->setting_1_config_label,
        COUNT_OF(app->setting_1_values),
        uhf_reader_setting_1_change,
        app
    );

    //Creating the default index for setting one which is the connection status
    app->setting_1_index = 0;
    variable_item_set_current_value_index(item, app->setting_1_index);
    variable_item_set_current_value_text(item, app->setting_1_names[app->setting_1_index]);
    
    //Creating the default power value 
    app->setting_2_power_str = furi_string_alloc_set(app->setting_2_default_value);
    app->setting_2_item = variable_item_list_add(
        app->variable_item_list_config, app->setting_2_config_label, 1, NULL, NULL
    );
    variable_item_set_current_value_text(
        app->setting_2_item, furi_string_get_cstr(app->setting_2_power_str)
    );
    variable_item_list_set_enter_callback(
        app->variable_item_list_config, uhf_reader_setting_item_clicked, app
    );

    // Add setting 3 to variable item list
    VariableItem* antenna_selection = variable_item_list_add(
        app->variable_item_list_config,
        app->setting_3_config_label,
        COUNT_OF(app->setting_3_values),
        uhf_reader_setting_3_change,
        app
    );

    //Default index for the antenna selection option
    app->setting_3_index = 0;
    variable_item_set_current_value_index(antenna_selection, app->setting_3_index);
    variable_item_set_current_value_text(antenna_selection, app->setting_3_names[app->setting_3_index]);

    //Setting previous callback
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_config),
        uhf_reader_navigation_config_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        UHFReaderViewConfigure,
        variable_item_list_get_view(app->variable_item_list_config));
}

/**
 * @brief      Callback for returning to configure screen.
 * @details    This function is called when user press back button.
 * @param      _context  The context - unused
 * @return     next view id
*/
uint32_t uhf_reader_navigation_configure_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewConfigure;
}

/**
 * @brief      Handles the connection setting
 * @details    Attempts to connect/disconnect from the reader.
 * @param      item  VariableItem - the current selection for connect values.
*/
void uhf_reader_setting_1_change(VariableItem* item) {
    UHFReaderApp* app = variable_item_get_context(item);

    //Getting the index
    uint8_t index = variable_item_get_current_value_index(item);
    
    //Will eventually do some sort of check to confirm sucessful connection
    if(app->reader_connected == false) {
        uart_helper_send(app->uart_helper, "C\n", 2);
        app->reader_connected = true;
        //TODO add ACK check to make sure that the connection was sucessful
    } else {
        uart_helper_send(app->uart_helper, "D\n", 2);
        app->reader_connected = false;
    }

    //Setting the current setting value for both the read and write screens
    variable_item_set_current_value_text(item, app->setting_1_names[index]);
    UHFReaderConfigModel* model_read = view_get_model(app->view_read);
    model_read->setting_1_index = index;
    furi_string_set(model_read->setting_1_value, app->setting_1_names[index]);
    UHFReaderWriteModel* model_write = view_get_model(app->view_write);
    model_write->setting_1_index = index;
    furi_string_set(model_write->setting_1_value, app->setting_1_names[index]);
}

/**
 * @brief      Handles the power menu.
 * @details    This function handles the power value that is set from the configuration screen and sends it to the RPi Zero via UART.
 * @param      context The UHFReaderApp app - used to allocate app variables and views.
*/
void uhf_reader_setting_2_text_updated(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    bool redraw = true;
    
    //Chaning the read screen's power value to the one set in the configuration menu
    with_view_model(
        app->view_read,
        UHFReaderConfigModel * model,
        {
            //Send the power command to the RPi Zero
            uart_helper_send(app->uart_helper, "POWER\n", 6);

            //Set the current power determined by user
            furi_string_set(model->setting_2_power, app->temp_buffer);

            //Send the power value to the RPi Zero
            uart_helper_send_string(app->uart_helper, model->setting_2_power);

            //Update the power value in the configuration screen
            variable_item_set_current_value_text(
                app->setting_2_item, furi_string_get_cstr(model->setting_2_power));
        },
        redraw);
    
    //Changes the power value for the write screen
    with_view_model(
        app->view_write,
        UHFReaderWriteModel * model,
        {
            furi_string_set(model->setting_2_power, app->temp_buffer);
            variable_item_set_current_value_text(
                app->setting_2_item, furi_string_get_cstr(model->setting_2_power));
        },
        redraw);

    //Switch back to the configuration view
    view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewConfigure);
}

/**
 * @brief      Handles the Antenna Selection
 * @details    This function is a place holder for future functionality.
 * @param      item  VariableItem - the current selection for antenna values.
*/
void uhf_reader_setting_3_change(VariableItem* item) {
    UHFReaderApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    

    if(index == 1) {
        uart_helper_send(app->uart_helper, "External\n", 9);
        //TODO: ADD SUPPORT FOR DIFFERENT ANTENNA TYPES AFTER HARDWARE DEVELOPED!
    } else {
        uart_helper_send(app->uart_helper, "Internal\n", 9);
    }

    //TODO: WAIT FOR ACK AND THEN SET TEXT VALUE
    variable_item_set_current_value_text(item, app->setting_3_names[index]);
    
    //Updating the antenna value for the read screen
    UHFReaderConfigModel* model_read = view_get_model(app->view_read);
    model_read->setting_3_index = index;
    furi_string_set(model_read->setting_3_value, app->setting_3_names[index]);
    
    //Updating the value of the antenna mode for the write screen
    UHFReaderWriteModel* model_write = view_get_model(app->view_write);
    model_write->setting_3_index = index;
    furi_string_set(model_write->setting_3_value, app->setting_3_names[index]);
}

/**
 * @brief      Allocates the power text screen
 * @details    Allocates the text input object for the power screen.
 * @param      app  The UHFReaderApp - used for allocating variables and text input.
*/
void power_menu_alloc(UHFReaderApp* app){

    //TODO: Add checks for reader connection and change power levels based on module used.
    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, UHFReaderViewSetPower, text_input_get_view(app->text_input));
    app->temp_buffer_size = 5; 
    app->temp_buffer = (char*)malloc(app->temp_buffer_size);
}

/**
 * @brief      Handles the setting items clicked
 * @details    Handles the power value input by the user.
 * @param      context, index - context used for UHFReaderApp, index used for state check.
*/
void uhf_reader_setting_item_clicked(void* context, uint32_t index) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    index++; 
    
    //Check if the power menu is being selected
    if(index == 2) {
        // Header to display on the power value input screen.
        text_input_set_header_text(app->text_input, app->setting_2_entry_text);

        //Modify the value of the power for the read and write models
        bool redraw = false;
        with_view_model(
            app->view_read,
            UHFReaderConfigModel * model,
            {
                strncpy(
                    app->temp_buffer,
                    furi_string_get_cstr(model->setting_2_power),
                    app->temp_buffer_size);
            },
            redraw);
        with_view_model(
            app->view_write,
            UHFReaderWriteModel * model,
            {
                strncpy(
                    app->temp_buffer,
                    furi_string_get_cstr(model->setting_2_power),
                    app->temp_buffer_size);
            },
            redraw);
        
        //Setting the power text input callback function 
        bool clear_previous_text = false;
        text_input_set_result_callback(
            app->text_input,
            uhf_reader_setting_2_text_updated,
            app,
            app->temp_buffer,
            app->temp_buffer_size,
            clear_previous_text);
        view_set_previous_callback(
            text_input_get_view(app->text_input), uhf_reader_navigation_configure_callback);
        view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewSetPower);
    }
}

/**
 * @brief      Frees the configure screen.
 * @details    Frees all variables and views for the configure screen.
 * @param      app  The UHFReaderApp - used for freeing variables and text input.
*/
void view_config_free(UHFReaderApp* app){
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewSetPower);
    text_input_free(app->text_input);
    free(app->temp_buffer);
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewConfigure);
    variable_item_list_free(app->variable_item_list_config);
}