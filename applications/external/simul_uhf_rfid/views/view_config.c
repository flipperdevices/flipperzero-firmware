#include "view_config.h"

/**
 * @brief      Callback for returning to submenu.
 * @details    This function is called when user press back button.
 * @param      context  The context - unused
 * @return     next view id
*/
uint32_t uhf_reader_navigation_config_submenu_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewSubmenu;
}

/**
 * @brief      Allocates the configuration menu
 * @details    This function allocates all views and variables related to the configuration menu.
 * @param      app  The UHFReaderApp - used to allocate app variables and views.
*/
void view_config_alloc(UHFReaderApp* App) {
    //Allocate the power input menu
    power_menu_alloc(App);

    //Creating the variable item list
    App->VariableItemListConfig = variable_item_list_alloc();
    variable_item_list_reset(App->VariableItemListConfig);

    //Initializing configuration setting variables
    App->Setting1Values[0] = 1;
    App->Setting1Values[1] = 2;
    App->Setting1Names[0] = "Disconnected";
    App->Setting1Names[1] = "Connected";
    App->ReaderConnected = false;
    App->Setting1ConfigLabel = "Reader Status:";
    App->Setting2ConfigLabel = "Power Level";
    App->Setting2EntryText = "Enter Value In Range 0-2700";
    App->Setting2DefaultValue = "1500";
    App->Setting3Values[0] = 1;
    App->Setting3Values[1] = 2;
    App->Setting3Names[0] = "Internal";
    App->Setting3Names[1] = "External";
    App->Setting3ConfigLabel = "Antenna";

    // Add setting 1 to variable item list
    VariableItem* Item = variable_item_list_add(
        App->VariableItemListConfig,
        App->Setting1ConfigLabel,
        COUNT_OF(App->Setting1Values),
        uhf_reader_setting_1_change,
        App);

    //Creating the default index for setting one which is the connection status
    App->Setting1Index = 0;
    variable_item_set_current_value_index(Item, App->Setting1Index);
    variable_item_set_current_value_text(Item, App->Setting1Names[App->Setting1Index]);

    //Creating the default power value
    App->Setting2PowerStr = furi_string_alloc_set(App->Setting2DefaultValue);
    App->Setting2Item = variable_item_list_add(
        App->VariableItemListConfig, App->Setting2ConfigLabel, 1, NULL, NULL);
    variable_item_set_current_value_text(
        App->Setting2Item, furi_string_get_cstr(App->Setting2PowerStr));
    variable_item_list_set_enter_callback(
        App->VariableItemListConfig, uhf_reader_setting_item_clicked, App);

    // Add setting 3 to variable item list
    VariableItem* AntennaSelection = variable_item_list_add(
        App->VariableItemListConfig,
        App->Setting3ConfigLabel,
        COUNT_OF(App->Setting3Values),
        uhf_reader_setting_3_change,
        App);

    //Default index for the antenna selection option
    App->Setting3Index = 0;
    variable_item_set_current_value_index(AntennaSelection, App->Setting3Index);
    variable_item_set_current_value_text(AntennaSelection, App->Setting3Names[App->Setting3Index]);

    //Setting previous callback
    view_set_previous_callback(
        variable_item_list_get_view(App->VariableItemListConfig),
        uhf_reader_navigation_config_submenu_callback);
    view_dispatcher_add_view(
        App->ViewDispatcher,
        UHFReaderViewConfigure,
        variable_item_list_get_view(App->VariableItemListConfig));
}

/**
 * @brief      Callback for returning to configure screen.
 * @details    This function is called when user press back button.
 * @param      context  The context - unused
 * @return     next view id
*/
uint32_t uhf_reader_navigation_configure_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewConfigure;
}

/**
 * @brief      Handles the connection setting
 * @details    Attempts to connect/disconnect from the reader.
 * @param      item  VariableItem - the current selection for connect values.
*/
void uhf_reader_setting_1_change(VariableItem* Item) {
    UHFReaderApp* App = variable_item_get_context(Item);

    //Getting the index
    uint8_t Index = variable_item_get_current_value_index(Item);

    //Will eventually do some sort of check to confirm successful connection
    if(App->ReaderConnected == false) {
        uart_helper_send(App->UartHelper, "C\n", 2);
        App->ReaderConnected = true;
        //TODO add ACK check to make sure that the connection was successful
    } else {
        uart_helper_send(App->UartHelper, "D\n", 2);
        App->ReaderConnected = false;
    }

    //Setting the current setting value for both the read and write screens
    variable_item_set_current_value_text(Item, App->Setting1Names[Index]);
    UHFReaderConfigModel* ModelRead = view_get_model(App->ViewRead);
    ModelRead->Setting1Index = Index;
    furi_string_set(ModelRead->Setting1Value, App->Setting1Names[Index]);
    UHFReaderWriteModel* ModelWrite = view_get_model(App->ViewWrite);
    ModelWrite->Setting1Index = Index;
    furi_string_set(ModelWrite->Setting1Value, App->Setting1Names[Index]);
}

/**
 * @brief      Handles the power menu.
 * @details    This function handles the power value that is set from the configuration screen and sends it to the RPi Zero via UART.
 * @param      context The UHFReaderApp app - used to allocate app variables and views.
*/
void uhf_reader_setting_2_text_updated(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    bool Redraw = true;

    //Changing the read screen's power value to the one set in the configuration menu
    with_view_model(
        App->ViewRead,
        UHFReaderConfigModel * Model,
        {
            //Send the power command to the RPi Zero
            uart_helper_send(App->UartHelper, "POWER\n", 6);

            //Set the current power determined by user
            furi_string_set(Model->Setting2Power, App->TempBuffer);

            //Send the power value to the RPi Zero
            uart_helper_send_string(App->UartHelper, Model->Setting2Power);

            //Update the power value in the configuration screen
            variable_item_set_current_value_text(
                App->Setting2Item, furi_string_get_cstr(Model->Setting2Power));
        },
        Redraw);

    //Changes the power value for the write screen
    with_view_model(
        App->ViewWrite,
        UHFReaderWriteModel * Model,
        {
            furi_string_set(Model->Setting2Power, App->TempBuffer);
            variable_item_set_current_value_text(
                App->Setting2Item, furi_string_get_cstr(Model->Setting2Power));
        },
        Redraw);

    //Switch back to the configuration view
    view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewConfigure);
}

/**
 * @brief      Handles the Antenna Selection
 * @details    This function is a place holder for future functionality.
 * @param      item  VariableItem - the current selection for antenna values.
*/
void uhf_reader_setting_3_change(VariableItem* Item) {
    UHFReaderApp* App = variable_item_get_context(Item);
    uint8_t Index = variable_item_get_current_value_index(Item);

    if(Index == 1) {
        uart_helper_send(App->UartHelper, "External\n", 9);
        //TODO: ADD SUPPORT FOR DIFFERENT ANTENNA TYPES AFTER HARDWARE DEVELOPED!
    } else {
        uart_helper_send(App->UartHelper, "Internal\n", 9);
    }

    //TODO: WAIT FOR ACK AND THEN SET TEXT VALUE
    variable_item_set_current_value_text(Item, App->Setting3Names[Index]);

    //Updating the antenna value for the read screen
    UHFReaderConfigModel* ModelRead = view_get_model(App->ViewRead);
    ModelRead->Setting3Index = Index;
    furi_string_set(ModelRead->Setting3Value, App->Setting3Names[Index]);

    //Updating the value of the antenna mode for the write screen
    UHFReaderWriteModel* ModelWrite = view_get_model(App->ViewWrite);
    ModelWrite->Setting3Index = Index;
    furi_string_set(ModelWrite->Setting3Value, App->Setting3Names[Index]);
}

/**
 * @brief      Allocates the power text screen
 * @details    Allocates the text input object for the power screen.
 * @param      app  The UHFReaderApp - used for allocating variables and text input.
*/
void power_menu_alloc(UHFReaderApp* App) {
    //TODO: Add checks for reader connection and change power levels based on module used.
    App->TextInput = text_input_alloc();
    view_dispatcher_add_view(
        App->ViewDispatcher, UHFReaderViewSetPower, text_input_get_view(App->TextInput));
    App->TempBufferSize = 5;
    App->TempBuffer = (char*)malloc(App->TempBufferSize);
}

/**
 * @brief      Handles the setting items clicked
 * @details    Handles the power value input by the user.
 * @param      context, index - context used for UHFReaderApp, index used for state check.
*/
void uhf_reader_setting_item_clicked(void* context, uint32_t index) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    index++;

    //Check if the power menu is being selected
    if(index == 2) {
        // Header to display on the power value input screen.
        text_input_set_header_text(App->TextInput, App->Setting2EntryText);

        //Modify the value of the power for the read and write models
        bool Redraw = false;
        with_view_model(
            App->ViewRead,
            UHFReaderConfigModel * Model,
            {
                strncpy(
                    App->TempBuffer,
                    furi_string_get_cstr(Model->Setting2Power),
                    App->TempBufferSize);
            },
            Redraw);
        with_view_model(
            App->ViewWrite,
            UHFReaderWriteModel * Model,
            {
                strncpy(
                    App->TempBuffer,
                    furi_string_get_cstr(Model->Setting2Power),
                    App->TempBufferSize);
            },
            Redraw);

        //Setting the power text input callback function
        bool ClearPreviousText = false;
        text_input_set_result_callback(
            App->TextInput,
            uhf_reader_setting_2_text_updated,
            App,
            App->TempBuffer,
            App->TempBufferSize,
            ClearPreviousText);
        view_set_previous_callback(
            text_input_get_view(App->TextInput), uhf_reader_navigation_configure_callback);
        view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewSetPower);
    }
}

/**
 * @brief      Frees the configure screen.
 * @details    Frees all variables and views for the configure screen.
 * @param      app  The UHFReaderApp - used for freeing variables and text input.
*/
void view_config_free(UHFReaderApp* App) {
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewSetPower);
    text_input_free(App->TextInput);
    free(App->TempBuffer);
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewConfigure);
    variable_item_list_free(App->VariableItemListConfig);
}
