#include "view_epc.h"

/**
 * @brief      Callback for the timer used for the epc info screen
 * @details    This function is called for timer events.
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_view_epc_timer_callback(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;

    //TODO: Make this function usable by Tag Info Screen
    //Update the offset for each value used in the draw callback
    with_view_model(
        App->ViewEpc,
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

    //TODO: Make a timer function for delete screen and move this to it
    with_view_model(
        App->ViewDelete,
        UHFReaderDeleteModel * model,
        {
            uint32_t LenEpc = strlen(model->ScrollingText);
            model->ScrollOffset++;
            if(model->ScrollOffset >= LenEpc) {
                model->ScrollOffset = 0;
            }
        },
        true);
    with_view_model(
        App->ViewDeleteSuccess,
        UHFReaderDeleteModel * model,
        {
            uint32_t LenEpc = strlen(model->ScrollingText);
            model->ScrollOffset++;
            if(model->ScrollOffset >= LenEpc) {
                model->ScrollOffset = 0;
            }
        },
        true);
    view_dispatcher_send_custom_event(App->ViewDispatcher, UHFReaderEventIdRedrawScreen);
}

/**
 * @brief      View EPC Draw Callback.
 * @details    This function is called when the user presses the down button on the read screen to show the tag details.
 * @param      canvas    The canvas - Canvas object for drawing the screen.
 * @param      model  The view model - model for the view with variables required for drawing.
*/
void uhf_reader_view_epc_draw_callback(Canvas* canvas, void* model) {
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
 * @brief      Callback when the user exits the view epc info screen.
 * @details    This function is called when the user exits the epc info screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_exit_view_epc_callback(void* context) {
    UNUSED(context);
    return UHFReaderViewRead;
}

/**
 * @brief      Tag Info enter callback function.
 * @details    This function is called when the view transitions to the Tag Info screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_epc_enter_callback(void* context) {
    //Grab the period for the timer
    uint32_t Period = furi_ms_to_ticks(300);
    UHFReaderApp* App = (UHFReaderApp*)context;

    //Grab the current index for each memory bank
    uint32_t CurTidIndex = App->CurTidIndex;
    uint32_t CurResIndex = App->CurResIndex;
    uint32_t CurMemIndex = App->CurMemIndex;

    //Display the read values for each memory bank
    bool Redraw = true;
    with_view_model(
        App->ViewEpc,
        UHFRFIDTagModel * model,
        {
            if(App->NumberOfTidsToRead > 0 && CurTidIndex < App->NumberOfTidsToRead) {
                furi_string_set_str(model->Tid, App->TidValues[CurTidIndex * 41]);
            }
            if(App->NumberOfResToRead > 0 && CurResIndex < App->NumberOfResToRead) {
                furi_string_set_str(model->Reserved, App->ResValues[CurResIndex * 17]);
            }
            if(App->NumberOfMemToRead > 0 && CurMemIndex < App->NumberOfMemToRead) {
                furi_string_set_str(model->User, App->MemValues[CurMemIndex * 33]);
            }
        },
        Redraw);

    //Start the timer
    furi_assert(App->Timer == NULL);
    App->Timer =
        furi_timer_alloc(uhf_reader_view_epc_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(App->Timer, Period);
}

/**
 * @brief      Callback when the user exits the read epc info screen.
 * @details    This function is called when the user exits the read epc info screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_epc_exit_callback(void* context) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    furi_timer_stop(App->Timer);
    furi_timer_free(App->Timer);
    App->Timer = NULL;
}

/**
 * @brief      Allocates the epc info view on the read screen.
 * @details    This function allocates all variables for the epc info view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_epc_alloc(UHFReaderApp* App) {
    //Allocating the view and setting all callback functions
    App->ViewEpc = view_alloc();
    view_set_draw_callback(App->ViewEpc, uhf_reader_view_epc_draw_callback);
    view_set_previous_callback(App->ViewEpc, uhf_reader_navigation_exit_view_epc_callback);
    view_set_enter_callback(App->ViewEpc, uhf_reader_view_epc_enter_callback);
    view_set_exit_callback(App->ViewEpc, uhf_reader_view_epc_exit_callback);
    view_set_context(App->ViewEpc, App);

    //Allocating the view model
    view_allocate_model(App->ViewEpc, ViewModelTypeLockFree, sizeof(UHFRFIDTagModel));
    UHFRFIDTagModel* ModelEpc = view_get_model(App->ViewEpc);
    FuriString* UserMem = furi_string_alloc();
    furi_string_set_str(UserMem, "Press Read!");
    FuriString* TidMem = furi_string_alloc();
    furi_string_set_str(TidMem, "Press Read!");
    FuriString* Epc = furi_string_alloc();
    furi_string_set_str(Epc, "Press Read!");
    FuriString* ReservedMem = furi_string_alloc();
    furi_string_set_str(ReservedMem, "Press Read!");

    //Setting default values for the view model
    ModelEpc->User = UserMem;
    ModelEpc->Epc = Epc;
    ModelEpc->Tid = TidMem;
    ModelEpc->Reserved = ReservedMem;
    ModelEpc->ScrollOffsetEpc = 0;
    ModelEpc->ScrollingTextEpc = "EPC VALUE HERE";
    ModelEpc->ScrollOffsetTid = 0;
    ModelEpc->ScrollingTextTid = "TID VALUE HERE";
    ModelEpc->ScrollOffsetRes = 0;
    ModelEpc->ScrollingTextRes = "RES VALUE HERE";
    ModelEpc->ScrollOffsetMem = 0;
    ModelEpc->ScrollingTextMem = "MEM VALUE HERE";
    view_dispatcher_add_view(App->ViewDispatcher, UHFReaderViewEpcDump, App->ViewEpc);
}

/**
 * @brief      Frees the epc info view.
 * @details    This function frees all variables for the epc info view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_epc_free(UHFReaderApp* App) {
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewEpcDump);
    view_free(App->ViewEpc);
}
