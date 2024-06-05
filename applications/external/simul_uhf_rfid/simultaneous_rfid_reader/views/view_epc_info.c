#include "view_epc_info.h"

/**
 * @brief      EPC Info Draw Callback.
 * @details    This function is called when the user selects tag info on the tag actions submenu.
 * @param      canvas    The canvas - Canvas object for drawing the screen.
 * @param      model  The view model - model for the view with variables required for drawing.
*/
void uhf_reader_view_epc_info_draw_callback(Canvas* canvas, void* model) {
    UHFRFIDTagModel* MyModel = (UHFRFIDTagModel*)model;
    FuriString* XStr = furi_string_alloc();

    //Clearing the canvas, setting the color, font and content displayed.
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 11, "            EPC Info:");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 22, "EPC: ");
    canvas_draw_str(canvas, 4, 44, "Reserved: ");
    canvas_draw_str(canvas, 4, 33, "TID: ");
    canvas_draw_str(canvas, 4, 55, "User Mem: ");

    //Displaying the EPC, TID, Reserved, and User memory in a scrolling fashion
    MyModel->ScrollingTextEpc = (char*)furi_string_get_cstr(MyModel->Epc);
    MyModel->ScrollingTextTid = (char*)furi_string_get_cstr(MyModel->Tid);
    MyModel->ScrollingTextRes = (char*)furi_string_get_cstr(MyModel->Reserved);
    MyModel->ScrollingTextMem = (char*)furi_string_get_cstr(MyModel->User);

    //Setting the width of the screen for the sliding window
    uint32_t ScreenWidthChars = 24;

    // Calculate the start and end indices of the substring to draw
    uint32_t StartPos = MyModel->ScrollOffsetEpc;

    //Calculate the length of the scrolling text
    uint32_t Len = strlen(MyModel->ScrollingTextEpc);

    //I am sure there is a better way to do this that involves slightly safer memory management...
    char VisiblePart[ScreenWidthChars + 2];
    memset(VisiblePart, ' ', ScreenWidthChars);
    VisiblePart[ScreenWidthChars] = '\0';

    //Fill the array up with the epc values
    for(uint32_t i = 0; i < ScreenWidthChars; i++) {
        uint32_t CharIndex = StartPos + i;
        if(CharIndex <= Len) {
            VisiblePart[i] = MyModel->ScrollingTextEpc[CharIndex];
        }
    }

    //This is just the same code for the TID, Reserved, and User memory
    //TODO: Make this into some sort of function to reduce code duplication
    canvas_draw_str(canvas, 28, 22, VisiblePart);
    uint32_t StartPosTid = MyModel->ScrollOffsetTid;
    uint32_t LenTid = strlen(MyModel->ScrollingTextTid);

    char VisiblePartTid[ScreenWidthChars + 2];
    memset(VisiblePartTid, ' ', ScreenWidthChars);
    VisiblePartTid[ScreenWidthChars] = '\0';

    for(uint32_t i = 0; i < ScreenWidthChars; i++) {
        uint32_t CharIndexTid = StartPosTid + i;
        if(CharIndexTid <= LenTid) {
            VisiblePartTid[i] = MyModel->ScrollingTextTid[CharIndexTid];
        }
    }

    canvas_draw_str(canvas, 28, 33, VisiblePartTid);
    uint32_t StartPosRes = MyModel->ScrollOffsetRes;
    uint32_t LenRes = strlen(MyModel->ScrollingTextRes);

    char VisiblePartRes[ScreenWidthChars + 2];
    memset(VisiblePartRes, ' ', ScreenWidthChars);
    VisiblePartRes[ScreenWidthChars] = '\0';

    for(uint32_t i = 0; i < ScreenWidthChars; i++) {
        uint32_t CharIndexRes = StartPosRes + i;
        if(CharIndexRes <= LenRes) {
            VisiblePartRes[i] = MyModel->ScrollingTextRes[CharIndexRes];
        }
    }

    canvas_draw_str(canvas, 46, 44, VisiblePartRes);
    uint32_t StartPosMem = MyModel->ScrollOffsetMem;
    uint32_t LenMem = strlen(MyModel->ScrollingTextMem);

    char VisiblePartMem[ScreenWidthChars + 2];
    memset(VisiblePartMem, ' ', ScreenWidthChars);
    VisiblePartMem[ScreenWidthChars] = '\0';

    for(uint32_t i = 0; i < ScreenWidthChars; i++) {
        uint32_t CharIndexMem = StartPosMem + i;
        if(CharIndexMem <= LenMem) {
            VisiblePartMem[i] = MyModel->ScrollingTextMem[CharIndexMem];
        }
    }

    canvas_draw_str(canvas, 48, 55, VisiblePartMem);
    furi_string_free(XStr);
}

/**
 * @brief      Callback when the user exits the tag info screen.
 * @details    This function is called when the user exits the tag info screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_exit_epc_info_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewTagAction;
}

/**
 * @brief      Callback for the timer used for the tag info screen
 * @details    This function is called for timer events.
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_view_epc_info_timer_callback(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Update the offset for each value used in the draw callback
    with_view_model(
        App->ViewEpcInfo,
        UHFRFIDTagModel * model,
        {
            uint32_t Len = strlen(model->ScrollingTextEpc);
            uint32_t LenTid = strlen(model->ScrollingTextTid);
            uint32_t LenRes = strlen(model->ScrollingTextRes);
            uint32_t LenMem = strlen(model->ScrollingTextMem);

            //Incrementing each offset
            model->ScrollOffsetEpc++;
            model->ScrollOffsetTid++;
            model->ScrollOffsetRes++;
            model->ScrollOffsetMem++;

            //Check the bounds of the offset and reset if necessary
            if(model->ScrollOffsetEpc >= Len) {
                model->ScrollOffsetEpc = 0;
            }
            if(model->ScrollOffsetTid >= LenTid) {
                model->ScrollOffsetTid = 0;
            }
            if(model->ScrollOffsetRes >= LenRes) {
                model->ScrollOffsetRes = 0;
            }
            if(model->ScrollOffsetMem >= LenMem) {
                model->ScrollOffsetMem = 0;
            }
        },
        true);
    view_dispatcher_send_custom_event(App->ViewDispatcher, UHFReaderEventIdRedrawScreen);
}

/**
 * @brief      Tag Info enter callback function.
 * @details    This function is called when the view transitions to the Tag Info screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_epc_info_enter_callback(void* context) {
    //Grab the period for the timer
    uint32_t Period = furi_ms_to_ticks(200);
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Create FuriStrings for storing saved UHF tag values
    FuriString* TempStr = furi_string_alloc();
    FuriString* TempTag = furi_string_alloc();

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
            //Grabbing the extracted tid, reserved memory, user memory, and epc from the file to display to the user
            const char* InputString = furi_string_get_cstr(TempTag);
            bool Redraw = true;
            with_view_model(
                App->ViewEpcInfo,
                UHFRFIDTagModel * model,
                {
                    furi_string_set(model->Epc, extract_epc(InputString));
                    furi_string_set(model->Tid, extract_tid(InputString));
                    furi_string_set(model->Reserved, extract_res(InputString));
                    furi_string_set(model->User, extract_mem(InputString));
                },
                Redraw);
            //Close the file
            flipper_format_file_close(App->EpcFile);
        }
    }

    //Freeing all FuriStrings used
    furi_string_free(TempTag);
    furi_string_free(TempStr);

    //Start the timer
    furi_assert(App->Timer == NULL);
    App->Timer =
        furi_timer_alloc(uhf_reader_view_epc_info_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(App->Timer, Period);
}

/**
 * @brief      Callback when the user exits the tag info screen.
 * @details    This function is called when the user exits the tag info screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_epc_info_exit_callback(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    furi_timer_stop(App->Timer);
    furi_timer_free(App->Timer);
    App->Timer = NULL;
}

/**
 * @brief      Allocates the tag info view.
 * @details    This function allocates all variables for the tag info view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_epc_info_alloc(UHFReaderApp* App) {
    //Allocating the view and setting all callback functions
    App->ViewEpcInfo = view_alloc();
    view_set_draw_callback(App->ViewEpcInfo, uhf_reader_view_epc_info_draw_callback);
    view_set_previous_callback(App->ViewEpcInfo, uhf_reader_navigation_exit_epc_info_callback);
    view_set_enter_callback(App->ViewEpcInfo, uhf_reader_view_epc_info_enter_callback);
    view_set_exit_callback(App->ViewEpcInfo, uhf_reader_view_epc_info_exit_callback);
    view_set_context(App->ViewEpcInfo, App);

    //Allocating the view model
    view_allocate_model(App->ViewEpcInfo, ViewModelTypeLockFree, sizeof(UHFRFIDTagModel));
    UHFRFIDTagModel* ModelEpcInfo = view_get_model(App->ViewEpcInfo);
    FuriString* UserMemEpcInfo = furi_string_alloc();
    furi_string_set_str(UserMemEpcInfo, "000000...");
    FuriString* TidMemEpcInfo = furi_string_alloc();
    furi_string_set_str(TidMemEpcInfo, "E2801105200....");
    FuriString* EpcInfo = furi_string_alloc();
    furi_string_set_str(EpcInfo, "ABCDEF12");
    FuriString* ReservedMemEpcInfo = furi_string_alloc();
    furi_string_set_str(ReservedMemEpcInfo, "131313...");

    //Setting default values for the view model
    ModelEpcInfo->User = UserMemEpcInfo;
    ModelEpcInfo->Epc = EpcInfo;
    ModelEpcInfo->Tid = TidMemEpcInfo;
    ModelEpcInfo->Reserved = ReservedMemEpcInfo;
    ModelEpcInfo->ScrollOffsetEpc = 0;
    ModelEpcInfo->ScrollingTextEpc = "EPC VALUE HERE";
    ModelEpcInfo->ScrollOffsetTid = 0;
    ModelEpcInfo->ScrollingTextTid = "TID VALUE HERE";
    ModelEpcInfo->ScrollOffsetRes = 0;
    ModelEpcInfo->ScrollingTextRes = "RES VALUE HERE";
    ModelEpcInfo->ScrollOffsetMem = 0;
    ModelEpcInfo->ScrollingTextMem = "MEM VALUE HERE";
    view_dispatcher_add_view(App->ViewDispatcher, UHFReaderViewEpcInfo, App->ViewEpcInfo);
}

/**
 * @brief      Frees the tag info view.
 * @details    This function frees all variables for the tag info view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_epc_info_free(UHFReaderApp* App) {
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewEpcInfo);
    view_free(App->ViewEpcInfo);
}
