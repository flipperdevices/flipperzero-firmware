#include "view_epc.h"

/**
 * @brief      Callback for the timer used for the epc info screen
 * @details    This function is called for timer events.
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_view_epc_timer_callback(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;

    //TODO: Make this function usable by Tag Info Screen
    //Update the offset for each value used in the draw callback
    with_view_model(
        app->view_epc,
        UHFRFIDTagModel * model,
        {
            uint32_t len = strlen(model->scrolling_text_epc);
            uint32_t len_tid = strlen(model->scrolling_text_tid);
            uint32_t len_res = strlen(model->scrolling_text_res);
            uint32_t len_mem = strlen(model->scrolling_text_mem);

            //Incrementing each offset
            model->scroll_offset_epc++;
            model->scroll_offset_tid++;
            model->scroll_offset_res++;
            model->scroll_offset_mem++;

            //Check the bounds of the offset and reset if necessary
            if(model->scroll_offset_epc >= len) {
                model->scroll_offset_epc = 0;
            }
            if(model->scroll_offset_tid >= len_tid) {
                model->scroll_offset_tid = 0;
            }
            if(model->scroll_offset_res >= len_res) {
                model->scroll_offset_res = 0;
            }
            if(model->scroll_offset_mem >= len_mem) {
                model->scroll_offset_mem = 0;
            }
        },
        true);

    //TODO: Make a timer function for delete screen and move this to it
    with_view_model(
        app->view_delete,
        UHFReaderDeleteModel * model,
        {
            uint32_t len_epc = strlen(model->scrolling_text);
            model->scroll_offset++;
            if(model->scroll_offset >= len_epc) {
                model->scroll_offset = 0;
            }
        },
        true);
    with_view_model(
        app->view_delete_success,
        UHFReaderDeleteModel * model,
        {
            uint32_t len_epc = strlen(model->scrolling_text);
            model->scroll_offset++;
            if(model->scroll_offset >= len_epc) {
                model->scroll_offset = 0;
            }
        },
        true);
    view_dispatcher_send_custom_event(app->view_dispatcher, UHFReaderEventIdRedrawScreen);
}

/**
 * @brief      View EPC Draw Callback.
 * @details    This function is called when the user presses the down button on the read screen to show the tag details.
 * @param      canvas    The canvas - Canvas object for drawing the screen.
 * @param      model  The view model - model for the view with variables required for drawing.
*/
void uhf_reader_view_epc_draw_callback(Canvas* canvas, void* model) {
    UHFRFIDTagModel* my_model = (UHFRFIDTagModel*)model;
    FuriString* xstr = furi_string_alloc();

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
    my_model->scrolling_text_epc = (char*)furi_string_get_cstr(my_model->EPC);
    my_model->scrolling_text_tid = (char*)furi_string_get_cstr(my_model->TID);
    my_model->scrolling_text_res = (char*)furi_string_get_cstr(my_model->Reserved);
    my_model->scrolling_text_mem = (char*)furi_string_get_cstr(my_model->User);

    //Setting the width of the screen for the sliding window
    uint32_t screen_width_chars = 24;

    // Calculate the start and end indices of the substring to draw
    uint32_t start_pos = my_model->scroll_offset_epc;

    //Calculate the length of the scrolling text
    uint32_t len = strlen(my_model->scrolling_text_epc);

    //I am sure there is a better way to do this that involves slightly safer memory management...
    char visible_part[screen_width_chars + 2];
    memset(visible_part, ' ', screen_width_chars);
    visible_part[screen_width_chars] = '\0';

    //Fill the array up with the epc values
    for(uint32_t i = 0; i < screen_width_chars; i++) {
        uint32_t char_index = start_pos + i;
        if(char_index <= len) {
            visible_part[i] = my_model->scrolling_text_epc[char_index];
        }
    }

    //This is just the same code for the TID, Reserved, and User memory
    //TODO: Make this into some sort of function to reduce code duplication
    canvas_draw_str(canvas, 28, 22, visible_part);
    uint32_t start_pos_tid = my_model->scroll_offset_tid;
    uint32_t len_tid = strlen(my_model->scrolling_text_tid);

    char visible_part_tid[screen_width_chars + 2];
    memset(visible_part_tid, ' ', screen_width_chars);
    visible_part_tid[screen_width_chars] = '\0';

    for(uint32_t i = 0; i < screen_width_chars; i++) {
        uint32_t char_index_tid = start_pos_tid + i;
        if(char_index_tid <= len_tid) {
            visible_part_tid[i] = my_model->scrolling_text_tid[char_index_tid];
        }
    }

    canvas_draw_str(canvas, 28, 33, visible_part_tid);
    uint32_t start_pos_res = my_model->scroll_offset_res;
    uint32_t len_res = strlen(my_model->scrolling_text_res);

    char visible_part_res[screen_width_chars + 2];
    memset(visible_part_res, ' ', screen_width_chars);
    visible_part_res[screen_width_chars] = '\0';

    for(uint32_t i = 0; i < screen_width_chars; i++) {
        uint32_t char_index_res = start_pos_res + i;
        if(char_index_res <= len_res) {
            visible_part_res[i] = my_model->scrolling_text_res[char_index_res];
        }
    }
    
    canvas_draw_str(canvas, 46, 44, visible_part_res);
    uint32_t start_pos_mem = my_model->scroll_offset_mem;
    uint32_t len_mem = strlen(my_model->scrolling_text_mem);

    char visible_part_mem[screen_width_chars + 2];
    memset(visible_part_mem, ' ', screen_width_chars);
    visible_part_mem[screen_width_chars] = '\0';

    for(uint32_t i = 0; i < screen_width_chars; i++) {
        uint32_t char_index_mem = start_pos_mem + i;
        if(char_index_mem <= len_mem) {
            visible_part_mem[i] = my_model->scrolling_text_mem[char_index_mem];
        }
    }
    
    canvas_draw_str(canvas, 48, 55, visible_part_mem);
    furi_string_free(xstr);
}

/**
 * @brief      Callback when the user exits the view epc info screen.
 * @details    This function is called when the user exits the epc info screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_exit_view_epc_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewRead;
}

/**
 * @brief      Tag Info enter callback function.
 * @details    This function is called when the view transitions to the Tag Info screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_epc_enter_callback(void* context) {
    //Grab the period for the timer 
    uint32_t period = furi_ms_to_ticks(300);
    UHFReaderApp* app = (UHFReaderApp*)context;

    //Grab the current index for each memory bank 
    uint32_t cur_tid_index = app->cur_tid_index;
    uint32_t cur_res_index = app->cur_res_index;
    uint32_t cur_mem_index = app->cur_mem_index;

    //Display the read values for each memory bank
    bool redraw = true;
    with_view_model(
        app->view_epc,
        UHFRFIDTagModel * model,
        {
            if(app->number_of_tids_to_read > 0 && cur_tid_index < app->number_of_tids_to_read) {
                furi_string_set_str(model->TID, app->tid_values[cur_tid_index * 41]);
            }
            if(app->number_of_res_to_read > 0 && cur_res_index < app->number_of_res_to_read) {
                furi_string_set_str(model->Reserved, app->res_values[cur_tid_index * 17]);
            }
            if(app->number_of_mem_to_read > 0 && cur_mem_index < app->number_of_mem_to_read) {
                furi_string_set_str(model->User, app->mem_values[cur_mem_index * 33]);
            }
        },
        redraw);

    //Start the timer 
    furi_assert(app->timer == NULL);
    app->timer =
        furi_timer_alloc(uhf_reader_view_epc_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(app->timer, period);
}

/**
 * @brief      Callback when the user exits the read epc info screen.
 * @details    This function is called when the user exits the read epc info screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_epc_exit_callback(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);
    app->timer = NULL;
}

/**
 * @brief      Allocates the epc info view on the read screen.
 * @details    This function allocates all variables for the epc info view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_epc_alloc(UHFReaderApp* app){

    //Allocating the view and setting all callback functions
    app->view_epc = view_alloc();
    view_set_draw_callback(app->view_epc, uhf_reader_view_epc_draw_callback);
    view_set_previous_callback(app->view_epc, uhf_reader_navigation_exit_view_epc_callback);
    view_set_enter_callback(app->view_epc, uhf_reader_view_epc_enter_callback);
    view_set_exit_callback(app->view_epc, uhf_reader_view_epc_exit_callback);
    view_set_context(app->view_epc, app);
    
    //Allocating the view model
    view_allocate_model(app->view_epc, ViewModelTypeLockFree, sizeof(UHFRFIDTagModel));
    UHFRFIDTagModel* model_epc = view_get_model(app->view_epc);
    FuriString* user_mem = furi_string_alloc();
    furi_string_set_str(user_mem, "Press Read!");
    FuriString* tid_mem = furi_string_alloc();
    furi_string_set_str(tid_mem, "Press Read!");
    FuriString* epc = furi_string_alloc();
    furi_string_set_str(epc, "Press Read!");
    FuriString* reserved_mem = furi_string_alloc();
    furi_string_set_str(reserved_mem, "Press Read!");
    
    //Setting default values for the view model
    model_epc->User = user_mem;
    model_epc->EPC = epc;
    model_epc->TID = tid_mem;
    model_epc->Reserved = reserved_mem;
    model_epc->scroll_offset_epc = 0;
    model_epc->scrolling_text_epc = "EPC VALUE HERE";
    model_epc->scroll_offset_tid = 0;
    model_epc->scrolling_text_tid = "TID VALUE HERE";
    model_epc->scroll_offset_res = 0;
    model_epc->scrolling_text_res = "RES VALUE HERE";
    model_epc->scroll_offset_mem = 0;
    model_epc->scrolling_text_mem = "MEM VALUE HERE";
    view_dispatcher_add_view(app->view_dispatcher, UHFReaderViewEPCDump, app->view_epc);
}

/**
 * @brief      Frees the epc info view.
 * @details    This function frees all variables for the epc info view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_epc_free(UHFReaderApp* app){
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewEPCDump);
    view_free(app->view_epc);
}
