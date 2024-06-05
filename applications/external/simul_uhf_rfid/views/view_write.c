#include "view_write.h"

/**
 * @brief      Callback for returning to write submenu screen.
 * @details    This function is called when user press back button.
 * @param      context  The context - unused
 * @return     next view id
*/
uint32_t uhf_reader_navigation_write_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewWrite;
}

/**
 * @brief      Callback for the epc value text input screen.
 * @details    This function saves the current tag selected with all its info
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_epc_value_text_updated(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    bool redraw = true;
    with_view_model(
        App->ViewWrite,

        //Keep track of the new epc value
        UHFReaderWriteModel * Model,
        { furi_string_set(Model->NewEpcValue, App->TempSaveBuffer); },
        redraw);

    view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewWrite);
}

/**
 * @brief      Callback for write timer elapsed.
 * @details    This function is called when the timer is elapsed for the write screen
 * @param      context  The context - The UHFReaderApp
*/
void uhf_reader_view_write_timer_callback(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    view_dispatcher_send_custom_event(App->ViewDispatcher, UHFReaderEventIdRedrawScreen);
}

/**
 * @brief      Write enter callback function.
 * @details    This function is called when the view transitions to the write screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_write_enter_callback(void* context) {
    //Grab the period for the timer
    uint32_t Period = furi_ms_to_ticks(200);
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Allocate space for the FuriStrings used
    FuriString* TempStr = furi_string_alloc();
    FuriString* TempTag = furi_string_alloc();

    //Open the saved epcs file to extract the uhf tag info
    if(!flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open Saved file");
        flipper_format_file_close(App->EpcFile);
    } else {
        furi_string_printf(TempStr, "Tag%ld", App->SelectedTagIndex);
        if(!flipper_format_read_string(App->EpcFile, furi_string_get_cstr(TempStr), TempTag)) {
            FURI_LOG_D(TAG, "Could not read tag %ld data", App->SelectedTagIndex);
        } else {
            //Grab the saved uhf tag info from the saved epcs file
            const char* InputString = furi_string_get_cstr(TempTag);
            furi_string_set(App->EpcToWrite, extract_epc(InputString));
            furi_string_set(App->EpcName, extract_name(InputString));

            //Set the write model uhf tag values accordingly
            bool redraw = true;
            with_view_model(
                App->ViewWrite,
                UHFReaderWriteModel * Model,
                {
                    furi_string_set(Model->EpcValue, extract_epc(InputString));
                    furi_string_set(Model->TidValue, extract_tid(InputString));
                    furi_string_set(Model->ResValue, extract_res(InputString));
                    furi_string_set(Model->MemValue, extract_mem(InputString));
                },
                redraw);
            //Close the file
            flipper_format_file_close(App->EpcFile);
        }
    }

    //Start the timer
    furi_assert(App->Timer == NULL);
    App->Timer =
        furi_timer_alloc(uhf_reader_view_write_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(App->Timer, Period);

    //Setting default reading states and freeing FuriStrings used
    App->IsWriting = false;
    furi_string_free(TempTag);
    furi_string_free(TempStr);
}

/**
 * @brief      Callback when the user exits the write screen.
 * @details    This function is called when the user exits the write screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_write_exit_callback(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    furi_timer_stop(App->Timer);
    furi_timer_free(App->Timer);
    App->Timer = NULL;
}

/**
 * @brief      Callback for custom write events.
 * @details    This function is called when a custom event is sent to the view dispatcher.
 * @param      event    The event id - UHFReaderAppEventId value.
 * @param      context  The context - UHFReaderApp object.
 * @return     true if the event was handled, false otherwise. 
*/
bool uhf_reader_view_write_custom_event_callback(uint32_t event, void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    switch(event) {
    // Redraw screen by passing true to last parameter of with_view_model.
    case UHFReaderEventIdRedrawScreen: {
        bool redraw = true;
        with_view_model(
            App->ViewWrite, UHFReaderWriteModel * _Model, { UNUSED(_Model); }, redraw);
        return true;
    }

    //The ok button was pressed to trigger a write
    case UHFReaderEventIdOkPressed: {
        bool redraw = false;
        with_view_model(
            App->ViewWrite,
            UHFReaderWriteModel * Model,
            {
                //TODO: Modify this to work for the YMR1001
                //Send the write command
                //I'm sure there is a better way to do this but works for now...
                if(furi_string_equal(Model->WriteFunction, WRITE_EPC)) {
                    uart_helper_send(App->UartHelper, "WRITE\n", 6);
                    uart_helper_send_string(App->UartHelper, Model->EpcValue);
                    uart_helper_send_string(App->UartHelper, Model->NewEpcValue);
                } else if(furi_string_equal(Model->WriteFunction, WRITE_EPC_MEM)) {
                    uart_helper_send(App->UartHelper, "WRITERES\n", 9);
                    uart_helper_send_string(App->UartHelper, Model->EpcValue);
                    uart_helper_send_string(App->UartHelper, Model->NewEpcValue);
                } else if(furi_string_equal(Model->WriteFunction, WRITE_USR_MEM)) {
                    uart_helper_send(App->UartHelper, "WRITEUSR\n", 9);
                    uart_helper_send_string(App->UartHelper, Model->EpcValue);
                    uart_helper_send_string(App->UartHelper, Model->NewEpcValue);
                } else if(furi_string_equal(Model->WriteFunction, WRITE_TID_MEM)) {
                    uart_helper_send(App->UartHelper, "WRITETID\n", 9);
                    uart_helper_send_string(App->UartHelper, Model->EpcValue);
                    uart_helper_send_string(App->UartHelper, Model->NewEpcValue);
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
    UHFReaderWriteModel* MyModel = (UHFReaderWriteModel*)model;
    FuriString* xstr = furi_string_alloc();

    //Clearing the canvas, setting the color, font and content displayed.
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 11, "           Write Menu:");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 33, "Write Mode:");

    //Displaying the current write mode selected
    canvas_draw_str(canvas, 51, 33, furi_string_get_cstr(MyModel->WriteFunction));

    //Display the current power level
    canvas_draw_str(canvas, 4, 22, "Pow: ");
    canvas_draw_str(canvas, 28, 22, furi_string_get_cstr(MyModel->Setting2Power));

    //Display the antenna selected
    canvas_draw_str(canvas, 70, 22, "Ant:");
    canvas_draw_str(canvas, 90, 22, furi_string_get_cstr(MyModel->Setting3Value));

    //Display the current write status
    canvas_draw_str(canvas, 0, 44, "Write Status: ");
    canvas_draw_str(canvas, 65, 44, furi_string_get_cstr(MyModel->WriteStatus));

    //Display the write button
    if(!MyModel->IsWriting) {
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
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Handle the short input types
    if(event->type == InputTypeShort) {
        //If the left button is pressed, then pull up the EPC value and keyboard
        if(event->key == InputKeyLeft && !App->IsWriting) {
            text_input_set_header_text(App->EpcWrite, "EPC Value");
            bool redraw = false;
            with_view_model(
                App->ViewWrite,

                //Store the new epc value and mark the write function as the epc selection
                UHFReaderWriteModel * Model,
                {
                    strncpy(
                        App->TempSaveBuffer,
                        furi_string_get_cstr(Model->EpcValue),
                        App->TempBufferSaveSize);
                    furi_string_set_str(Model->WriteFunction, WRITE_EPC);
                },
                redraw);

            // Configure the text input
            bool clear_previous_text = false;
            text_input_set_result_callback(
                App->EpcWrite,
                uhf_reader_epc_value_text_updated,
                App,
                App->TempSaveBuffer,
                App->TempBufferSaveSize,
                clear_previous_text);
            view_set_previous_callback(
                text_input_get_view(App->EpcWrite), uhf_reader_navigation_write_callback);
            view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewEpcWriteInput);
            return true;
        }

        //If the right button is pressed, then display the reserved memory bank and display the keyboard
        else if(event->key == InputKeyRight && !App->IsWriting) {
            text_input_set_header_text(App->EpcWrite, "Reserved Memory Bank");
            bool redraw = false;
            with_view_model(
                App->ViewWrite,

                //Store the modified value for the reserved memory bank
                UHFReaderWriteModel * Model,
                {
                    strncpy(
                        App->TempSaveBuffer,
                        furi_string_get_cstr(Model->ResValue),
                        App->TempBufferSaveSize);
                    furi_string_set_str(Model->WriteFunction, WRITE_EPC_MEM);
                },
                redraw);

            // Configure the text input
            bool clear_previous_text = false;
            text_input_set_result_callback(
                App->EpcWrite,
                uhf_reader_epc_value_text_updated,
                App,
                App->TempSaveBuffer,
                App->TempBufferSaveSize,
                clear_previous_text);
            view_set_previous_callback(
                text_input_get_view(App->EpcWrite), uhf_reader_navigation_write_callback);
            view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewEpcWriteInput);
            return true;
        }

        //If the up button is pressed, then display the user memory bank and keyboard
        else if(event->key == InputKeyUp && !App->IsWriting) {
            text_input_set_header_text(App->EpcWrite, "User Memory Bank");
            bool redraw = false;
            with_view_model(
                App->ViewWrite,

                //Store the modified user memory value
                UHFReaderWriteModel * Model,
                {
                    strncpy(
                        App->TempSaveBuffer,
                        furi_string_get_cstr(Model->MemValue),
                        App->TempBufferSaveSize);
                    furi_string_set(Model->WriteFunction, WRITE_USR_MEM);
                },
                redraw);

            // Configure the text input
            bool clear_previous_text = false;
            text_input_set_result_callback(
                App->EpcWrite,
                uhf_reader_epc_value_text_updated,
                App,
                App->TempSaveBuffer,
                App->TempBufferSaveSize,
                clear_previous_text);
            view_set_previous_callback(
                text_input_get_view(App->EpcWrite), uhf_reader_navigation_write_callback);
            view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewEpcWriteInput);
            return true;
        }

        //If the down button is pressed, then display the TID memory bank and the keyboard
        else if(event->key == InputKeyDown && !App->IsWriting) {
            text_input_set_header_text(App->EpcWrite, "TID Memory Bank");
            bool redraw = false;
            with_view_model(
                App->ViewWrite,

                //Store the modified TID value
                UHFReaderWriteModel * Model,
                {
                    strncpy(
                        App->TempSaveBuffer,
                        furi_string_get_cstr(Model->TidValue),
                        App->TempBufferSaveSize);
                    furi_string_set_str(Model->WriteFunction, WRITE_TID_MEM);
                },
                redraw);

            // Configure the text input
            bool clear_previous_text = false;
            text_input_set_result_callback(
                App->EpcWrite,
                uhf_reader_epc_value_text_updated,
                App,
                App->TempSaveBuffer,
                App->TempBufferSaveSize,
                clear_previous_text);
            view_set_previous_callback(
                text_input_get_view(App->EpcWrite), uhf_reader_navigation_write_callback);
            view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewEpcWriteInput);
            return true;
        }
    } else if(event->type == InputTypePress) {
        if(event->key == InputKeyOk) {
            //Handle the OK button event
            view_dispatcher_send_custom_event(App->ViewDispatcher, UHFReaderEventIdOkPressed);
            return true;
        }
    }
    return false;
}

/**
 * @brief      Callback when the user exits the write screen.
 * @details    This function is called when the user exits the write screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_write_exit_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewTagAction;
}

/**
 * @brief      Allocates the write view.
 * @details    This function allocates all variables for the write view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_write_alloc(UHFReaderApp* App) {
    //Allocating the view and setting all callback functions
    App->ViewWrite = view_alloc();
    view_set_draw_callback(App->ViewWrite, uhf_reader_view_write_draw_callback);
    view_set_input_callback(App->ViewWrite, uhf_reader_view_write_input_callback);
    view_set_previous_callback(App->ViewWrite, uhf_reader_navigation_write_exit_callback);
    view_set_enter_callback(App->ViewWrite, uhf_reader_view_write_enter_callback);
    view_set_exit_callback(App->ViewWrite, uhf_reader_view_write_exit_callback);
    view_set_context(App->ViewWrite, App);
    view_set_custom_callback(App->ViewWrite, uhf_reader_view_write_custom_event_callback);

    //Allocating the view model
    view_allocate_model(App->ViewWrite, ViewModelTypeLockFree, sizeof(UHFReaderWriteModel));
    UHFReaderWriteModel* ModelWrite = view_get_model(App->ViewWrite);
    FuriString* EpcNameWriteDefault = furi_string_alloc();

    //Setting default values for the view model
    ModelWrite->Setting1Index = App->Setting1Index;
    ModelWrite->Setting2Power = App->Setting2PowerStr;
    ModelWrite->Setting3Index = App->Setting3Index;
    ModelWrite->EpcName = EpcNameWriteDefault;
    ModelWrite->Setting1Value = furi_string_alloc_set(App->Setting1Names[App->Setting1Index]);
    ModelWrite->Setting3Value = furi_string_alloc_set(App->Setting3Names[App->Setting3Index]);
    FuriString* EpcWriteDefault = furi_string_alloc();
    furi_string_set_str(EpcWriteDefault, "Press Write");
    FuriString* EpcValueWriteDefault = furi_string_alloc();
    furi_string_set_str(EpcValueWriteDefault, "Press Write");
    ModelWrite->EpcValue = EpcValueWriteDefault;
    FuriString* EpcValueWriteStatus = furi_string_alloc();
    furi_string_set_str(EpcValueWriteStatus, "Press Write");
    ModelWrite->WriteStatus = EpcValueWriteStatus;
    FuriString* WriteDefaultEpc = furi_string_alloc();
    ModelWrite->NewEpcValue = WriteDefaultEpc;
    FuriString* DefaultWriteFunction = furi_string_alloc();
    furi_string_set_str(DefaultWriteFunction, "Press Arrow Keys");
    ModelWrite->WriteFunction = DefaultWriteFunction;
    FuriString* DefaultWriteTid = furi_string_alloc();
    furi_string_set_str(DefaultWriteTid, "TID HERE");
    ModelWrite->TidValue = DefaultWriteTid;
    FuriString* DefaultWriteTidNew = furi_string_alloc();
    furi_string_set_str(DefaultWriteTidNew, "NEW TID HERE");
    ModelWrite->NewTidValue = DefaultWriteTidNew;
    FuriString* DefaultWriteRes = furi_string_alloc();
    furi_string_set_str(DefaultWriteRes, "RES HERE");
    ModelWrite->ResValue = DefaultWriteRes;
    FuriString* DefaultWriteResNew = furi_string_alloc();
    furi_string_set_str(DefaultWriteResNew, "NEW RES HERE");
    ModelWrite->NewResValue = DefaultWriteResNew;
    FuriString* DefaultWriteMem = furi_string_alloc();
    furi_string_set_str(DefaultWriteMem, "MEM HERE");
    ModelWrite->MemValue = DefaultWriteMem;
    FuriString* DefaultWriteMemNew = furi_string_alloc();
    furi_string_set_str(DefaultWriteMemNew, "NEW MEM HERE");
    ModelWrite->NewMemValue = DefaultWriteMemNew;
    App->EpcName = furi_string_alloc_set("Enter Name");
    App->EpcToWrite = furi_string_alloc_set("Enter Name");
    App->EpcWrite = text_input_alloc();
    view_dispatcher_add_view(
        App->ViewDispatcher, UHFReaderViewEpcWriteInput, text_input_get_view(App->EpcWrite));

    view_dispatcher_add_view(App->ViewDispatcher, UHFReaderViewWrite, App->ViewWrite);
}

/**
 * @brief      Frees the write view.
 * @details    This function frees all variables for the write view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_write_free(UHFReaderApp* App) {
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewEpcWriteInput);
    text_input_free(App->EpcWrite);
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewWrite);
    view_free(App->ViewWrite);
}
