#include "view_read.h"

/**
 * @brief      Read Draw Callback.
 * @details    This function is called when the user selects read on the main submenu.
 * @param      canvas    The canvas - Canvas object for drawing the screen.
 * @param      model  The view model - model for the view with variables required for drawing.
*/
void uhf_reader_view_read_draw_callback(Canvas* canvas, void* model) {
    UHFReaderConfigModel* MyModel = (UHFReaderConfigModel*)model;
    FuriString* XStr = furi_string_alloc();

    //Clearing the canvas, setting the color, font and content displayed.
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 11, "           Read Menu:");
    canvas_set_font(canvas, FontSecondary);

    //Displaying the current number of UHF Tags read
    furi_string_printf(XStr, "%ld", MyModel->NumEpcsRead);
    canvas_draw_str(canvas, 4, 33, "# EPCs:");
    canvas_draw_str(canvas, 45, 33, furi_string_get_cstr(XStr));

    //Displaying the index of the current tag being viewed
    furi_string_printf(XStr, "%ld", MyModel->CurEpcIndex);
    canvas_draw_str(canvas, 70, 33, "Cur Tag:");
    canvas_draw_str(canvas, 115, 33, furi_string_get_cstr(XStr));

    //Displaying the power level
    canvas_draw_str(canvas, 4, 22, "Pow: ");
    canvas_draw_str(canvas, 28, 22, furi_string_get_cstr(MyModel->Setting2Power));

    //Displaying the antenna selected
    canvas_draw_str(canvas, 70, 22, "Ant:");
    canvas_draw_str(canvas, 90, 22, furi_string_get_cstr(MyModel->Setting3Value));

    //Displaying the EPC in a scrolling fashion
    MyModel->ScrollingText = (char*)furi_string_get_cstr(MyModel->EpcValue);

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

    // Now draw the visible part of the string
    canvas_draw_str(canvas, 0, 44, VisiblePart);

    if(!MyModel->IsReading) {
        //Display the Prev and Next buttons if the app isn't reading
        elements_button_left(canvas, "Prev");
        elements_button_center(canvas, "Start");
        elements_button_right(canvas, "Next");
    } else {
        elements_button_center(canvas, "Stop");
    }
    furi_string_free(XStr);
}

/**
 * @brief      Callback when the user exits the read screen.
 * @details    This function is called when the user exits the read screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_read_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewRead;
}

/**
 * @brief      Callback for the timer used for the read screen
 * @details    This function is called for timer events.
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_view_read_timer_callback(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Update the offset for the epc in the read draw callback
    with_view_model(
        App->ViewRead,
        UHFReaderConfigModel * model,
        {
            uint32_t Len = strlen(model->ScrollingText);

            //Incrementing each offset
            model->ScrollOffset++;

            //Check the bounds of the offset and reset if necessary
            if(model->ScrollOffset >= Len) {
                model->ScrollOffset = 0;
            }
        },
        true);
    view_dispatcher_send_custom_event(App->ViewDispatcher, UHFReaderEventIdRedrawScreen);
}

/**
 * @brief      Callback for the saved text input screen.
 * @details    This function saves the current tag selected with all its info
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_save_text_updated(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    bool Redraw = true;

    //Allocating FuriStrings to store each of the values associated with each UHF Tag
    FuriString* Tid = furi_string_alloc();
    FuriString* Mem = furi_string_alloc();
    FuriString* Res = furi_string_alloc();

    //Set the current EPC to save for the app based on the model's value
    with_view_model(
        App->ViewRead,
        UHFReaderConfigModel * model,
        {
            furi_string_set(model->EpcName, App->TempSaveBuffer);
            model->EpcName = App->EpcName;
            App->EpcToSave = (char*)furi_string_get_cstr(model->EpcValue);
        },
        Redraw);

    //Set the tid, User and Reserved memory values if any tags were read
    with_view_model(
        App->ViewEpc,
        UHFRFIDTagModel * model,
        {
            if(App->NumberOfEpcsToRead > 0) {
                furi_string_set(Tid, model->Tid);
                furi_string_set(Mem, model->User);
                furi_string_set(Res, model->Reserved);
            } else {
                furi_string_free(Tid);
                furi_string_free(Mem);
                furi_string_free(Res);
                return;
            }
        },
        Redraw);

    //Open the saved EPCS file if tags were read
    if(!flipper_format_file_open_append(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
        FURI_LOG_E(TAG, "Failed to open file");
    }

    //Allocating some FuriStrings for use with the file
    FuriString* NumEpcs = furi_string_alloc();
    FuriString* EpcAndName = furi_string_alloc();

    //Increment the total number of saved tags and write this new tag with all its values to the epc_and_name FuriString
    App->NumberOfSavedTags++;
    furi_string_printf(NumEpcs, "Tag%ld", App->NumberOfSavedTags);
    furi_string_printf(
        EpcAndName,
        "%s:%s:%s:%s:%s",
        App->TempSaveBuffer,
        App->EpcToSave,
        furi_string_get_cstr(Tid),
        furi_string_get_cstr(Res),
        furi_string_get_cstr(Mem));

    //Attempt to write the string using the given format
    if(!flipper_format_write_string_cstr(
           App->EpcFile, furi_string_get_cstr(NumEpcs), furi_string_get_cstr(EpcAndName))) {
        FURI_LOG_E(TAG, "Failed to write to file");
        flipper_format_file_close(App->EpcFile);
    } else {
        //Add the new tag to the saved submenu
        submenu_add_item(
            App->SubmenuSaved,
            App->TempSaveBuffer,
            App->NumberOfSavedTags,
            uhf_reader_submenu_saved_callback,
            App);
        flipper_format_file_close(App->EpcFile);

        //Update the Index_File to have the new total number of saved tags and write to the file
        FuriString* NewNumEpcs = furi_string_alloc();
        furi_string_printf(NewNumEpcs, "%ld", App->NumberOfSavedTags);
        if(!flipper_format_file_open_existing(App->EpcIndexFile, APP_DATA_PATH("Index_File.txt"))) {
            FURI_LOG_E(TAG, "Failed to open index file");
        } else {
            if(!flipper_format_write_string_cstr(
                   App->EpcIndexFile, "Number of Tags", furi_string_get_cstr(NewNumEpcs))) {
                FURI_LOG_E(TAG, "Failed to write to file");
                flipper_format_file_close(App->EpcIndexFile);
            } else {
                flipper_format_file_close(App->EpcIndexFile);
            }
        }
        furi_string_free(NewNumEpcs);
    }

    //Freeing all the FuriStrings used
    furi_string_free(EpcAndName);
    furi_string_free(NumEpcs);
    furi_string_free(Res);
    furi_string_free(Tid);
    furi_string_free(Mem);
    view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewRead);
}

/**
 * @brief      Callback for read screen input.
 * @details    This function is called when the user presses a button while on the read screen.
 * @param      event    The event - InputEvent object.
 * @param      context  The context - UHFReaderApp object.
 * @return     true if the event was handled, false otherwise.
*/
bool uhf_reader_view_read_input_callback(InputEvent* event, void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Handles all short input types
    if(event->type == InputTypeShort) {
        //If the user presses the left button while the app is not reading
        if(event->key == InputKeyLeft && !App->IsReading) {
            bool Redraw = true;

            with_view_model(
                App->ViewRead,
                UHFReaderConfigModel * model,
                {
                    //Check if there are any epcs that were read and decrement the current epc index to show the previous epc
                    if(App->NumberOfEpcsToRead > 0 && model->CurEpcIndex > 1) {
                        //TODO: Add checks for number of tids, res, and user mem read in case of short reads
                        model->CurEpcIndex -= 1;
                        App->CurTidIndex -= 1;
                        App->CurResIndex -= 1;
                        App->CurMemIndex -= 1;
                        furi_string_set_str(
                            model->EpcValue, App->EpcValues[model->CurEpcIndex * 26]);
                        model->NumEpcsRead = App->NumberOfEpcsToRead;
                    }
                },
                Redraw);

            //Updating the current TID, EPC, Reserved and User memory values to display on the view epc screen
            with_view_model(
                App->ViewEpc,
                UHFRFIDTagModel * model,
                {
                    if(App->NumberOfEpcsToRead > 0) {
                        furi_string_set_str(model->Epc, App->EpcValues[App->CurTidIndex * 26]);
                    }
                    if(App->NumberOfTidsToRead > 0) {
                        furi_string_set_str(model->Tid, App->TidValues[App->CurTidIndex * 41]);
                    }
                    if(App->NumberOfResToRead > 0) {
                        furi_string_set_str(
                            model->Reserved, App->ResValues[App->CurResIndex * 17]);
                    }
                    if(App->NumberOfMemToRead > 0) {
                        furi_string_set_str(model->User, App->MemValues[App->CurMemIndex * 33]);
                    }
                },
                Redraw);

            return true;
        }
        //Increment the current epc index if the app is not reading
        else if(event->key == InputKeyRight && !App->IsReading) {
            bool Redraw = true;
            with_view_model(
                App->ViewRead,
                UHFReaderConfigModel * model,
                {
                    if(App->NumberOfEpcsToRead > 0 &&
                       model->CurEpcIndex < App->NumberOfEpcsToRead) {
                        //TODO: Add better bounds checking to ensure no crashes with short reads
                        model->CurEpcIndex += 1;
                        App->CurTidIndex += 1;
                        App->CurResIndex += 1;
                        App->CurMemIndex += 1;
                        furi_string_set_str(
                            model->EpcValue, App->EpcValues[model->CurEpcIndex * 26]);
                        model->NumEpcsRead = App->NumberOfEpcsToRead;
                    }
                },
                Redraw);

            //Updating the current TID, EPC, Reserved and User memory values to display on the view epc screen
            with_view_model(
                App->ViewEpc,
                UHFRFIDTagModel * model,
                {
                    if(App->NumberOfEpcsToRead > 0) {
                        furi_string_set_str(model->Epc, App->EpcValues[App->CurTidIndex * 26]);
                    }
                    if(App->NumberOfTidsToRead > 0) {
                        furi_string_set_str(model->Tid, App->TidValues[App->CurTidIndex * 41]);
                    }
                    if(App->NumberOfResToRead > 0) {
                        furi_string_set_str(
                            model->Reserved, App->ResValues[App->CurResIndex * 17]);
                    }
                    if(App->NumberOfMemToRead > 0) {
                        furi_string_set_str(model->User, App->MemValues[App->CurMemIndex * 33]);
                    }
                },
                Redraw);

            return true;
        }

        //Handles the up key press that allows the user to save the currently selected UHF Tag
        else if(event->key == InputKeyUp && !App->IsReading) {
            //Setting the text input header
            text_input_set_header_text(App->SaveInput, "Save EPC");
            bool Redraw = false;
            with_view_model(
                App->ViewRead,
                UHFReaderConfigModel * model,
                {
                    //Copy the name contents from the text input
                    strncpy(
                        App->TempSaveBuffer,
                        furi_string_get_cstr(model->EpcName),
                        App->TempBufferSaveSize);
                },
                Redraw);

            //Set the text input result callback function
            bool ClearPreviousText = false;
            text_input_set_result_callback(
                App->SaveInput,
                uhf_reader_save_text_updated,
                App,
                App->TempSaveBuffer,
                App->TempBufferSaveSize,
                ClearPreviousText);
            view_set_previous_callback(
                text_input_get_view(App->SaveInput), uhf_reader_navigation_read_callback);
            view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewSaveInput);

            return true;
        }
        //If the down button is pressed, then show the view epc screen
        else if(event->key == InputKeyDown && !App->IsReading) {
            view_set_previous_callback(App->ViewEpc, uhf_reader_navigation_read_callback);
            view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewEpcDump);
            return true;
        }
    }
    //Handles the ok button being pressed
    else if(event->type == InputTypePress) {
        if(event->key == InputKeyOk) {
            view_dispatcher_send_custom_event(App->ViewDispatcher, UHFReaderEventIdOkPressed);

            return true;
        }
    }

    return false;
}

/**
 * @brief      Callback when the user exits the read screen.
 * @details    This function is called when the user exits the read screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_read_submenu_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewSubmenu;
}

/**
 * @brief      Callback when the user exits the read screen.
 * @details    This function is called when the user exits the read screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_read_exit_callback(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    furi_timer_stop(App->Timer);
    furi_timer_free(App->Timer);
    App->Timer = NULL;
}

/**
 * @brief      Read enter callback function.
 * @details    This function is called when the view transitions to the read screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_read_enter_callback(void* context) {
    //Grab the period for the timer
    uint32_t Period = furi_ms_to_ticks(350);
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Start the timer
    furi_assert(App->Timer == NULL);
    App->Timer =
        furi_timer_alloc(uhf_reader_view_read_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(App->Timer, Period);

    //Setting default reading states
    App->IsReading = false;
    with_view_model(
        App->ViewRead, UHFReaderConfigModel * model, { model->IsReading = false; }, true);
}

/**
 * @brief      Callback for custom read events.
 * @details    This function is called when a custom event is sent to the view dispatcher.
 * @param      event    The event id - UHFReaderAppEventId value.
 * @param      context  The context - UHFReaderApp object.
 * @return     true if the event was handled, false otherwise. 
*/
bool uhf_reader_view_read_custom_event_callback(uint32_t event, void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;

    switch(event) {
    //Redraw the screen
    case UHFReaderEventIdRedrawScreen: {
        bool Redraw = true;
        with_view_model(
            App->ViewRead, UHFReaderConfigModel * _model, { UNUSED(_model); }, Redraw);
        return true;
    }

    //The ok button was pressed
    case UHFReaderEventIdOkPressed:

        //Check if the app is reading
        if(App->IsReading) {
            //Stop reading
            App->IsReading = false;
            with_view_model(
                App->ViewRead,
                UHFReaderConfigModel * model,
                { model->IsReading = App->IsReading; },
                true);
        } else {
            //Check if the reader is connected before sending a read command
            if(App->ReaderConnected) {
                App->IsReading = true;

                //Send the read command to the RPi Zero via UART
                uart_helper_send(App->UartHelper, "READ\n", 5);
                with_view_model(
                    App->ViewRead,
                    UHFReaderConfigModel * model,
                    { model->IsReading = App->IsReading; },
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
void saved_input_alloc(UHFReaderApp* App) {
    //Allocate a new text input component
    App->SaveInput = text_input_alloc();
    view_dispatcher_add_view(
        App->ViewDispatcher, UHFReaderViewSaveInput, text_input_get_view(App->SaveInput));

    //Setting the max size of the buffer
    App->TempBufferSaveSize = 42;
    App->TempSaveBuffer = (char*)malloc(App->TempBufferSaveSize);
}

/**
 * @brief      Allocates the read view.
 * @details    This function allocates all variables for the read view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_read_alloc(UHFReaderApp* App) {
    //Allocating the view and setting all callback functions
    saved_input_alloc(App);
    App->ViewRead = view_alloc();
    view_set_draw_callback(App->ViewRead, uhf_reader_view_read_draw_callback);
    view_set_input_callback(App->ViewRead, uhf_reader_view_read_input_callback);
    view_set_previous_callback(App->ViewRead, uhf_reader_navigation_read_submenu_callback);
    view_set_enter_callback(App->ViewRead, uhf_reader_view_read_enter_callback);
    view_set_exit_callback(App->ViewRead, uhf_reader_view_read_exit_callback);
    view_set_context(App->ViewRead, App);
    view_set_custom_callback(App->ViewRead, uhf_reader_view_read_custom_event_callback);

    //Allocating the view model
    view_allocate_model(App->ViewRead, ViewModelTypeLockFree, sizeof(UHFReaderConfigModel));
    UHFReaderConfigModel* Model = view_get_model(App->ViewRead);
    FuriString* EpcValueDefault = furi_string_alloc();
    furi_string_set_str(EpcValueDefault, "Press Read");

    //Setting default values for the view model
    Model->Setting1Index = App->Setting1Index;
    Model->Setting2Power = App->Setting2PowerStr;
    Model->Setting3Index = App->Setting3Index;
    Model->Setting1Value = furi_string_alloc_set(App->Setting1Names[App->Setting1Index]);
    Model->Setting3Value = furi_string_alloc_set(App->Setting3Names[App->Setting3Index]);
    Model->EpcName = furi_string_alloc_set("Enter name");
    Model->ScrollOffset = 0;
    Model->ScrollingText = "Press Read";
    Model->EpcValue = EpcValueDefault;
    Model->CurEpcIndex = 1;
    Model->NumEpcsRead = 0;
    view_dispatcher_add_view(App->ViewDispatcher, UHFReaderViewRead, App->ViewRead);
}

/**
 * @brief      Frees the read view.
 * @details    This function frees all variables for the read view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_read_free(UHFReaderApp* App) {
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewSaveInput);
    text_input_free(App->SaveInput);
    free(App->TempSaveBuffer);
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewRead);
    view_free(App->ViewRead);
}
