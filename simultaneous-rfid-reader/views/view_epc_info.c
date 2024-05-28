#include "view_epc_info.h"

/**
 * @brief      EPC Info Draw Callback.
 * @details    This function is called when the user selects tag info on the tag actions submenu.
 * @param      canvas    The canvas - Canvas object for drawing the screen.
 * @param      model  The view model - model for the view with variables required for drawing.
*/
void uhf_reader_view_epc_info_draw_callback(Canvas* canvas, void* model) {
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
 * @brief      Callback when the user exits the tag info screen.
 * @details    This function is called when the user exits the tag info screen.
 * @param      context  The context - not used
 * @return     the view id of the next view.
*/
uint32_t uhf_reader_navigation_exit_epc_info_callback(void* _context) {
    UNUSED(_context);
    return UHFReaderViewTagAction;
}

/**
 * @brief      Callback for the timer used for the tag info screen
 * @details    This function is called for timer events.
 * @param      context  The UHFReaderApp - Used to change app variables.
*/
void uhf_reader_view_epc_info_timer_callback(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;

    //Update the offset for each value used in the draw callback
    with_view_model(
        app->view_epc_info,
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
    view_dispatcher_send_custom_event(app->view_dispatcher, UHFReaderEventIdRedrawScreen);
}

/**
 * @brief      Tag Info enter callback function.
 * @details    This function is called when the view transitions to the Tag Info screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_epc_info_enter_callback(void* context) {
    //Grab the period for the timer 
    uint32_t period = furi_ms_to_ticks(200);
    UHFReaderApp* app = (UHFReaderApp*)context;

    //Create FuriStrings for storing saved UHF tag values 
    FuriString* temp_str = furi_string_alloc();
    FuriString* temp_tag = furi_string_alloc();

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
            //Grabbing the extracted tid, reserved memory, user memory, and epc from the file to display to the user
            const char* inputString = furi_string_get_cstr(temp_tag);
            bool redraw = true;
            with_view_model(
                app->view_epc_info,
                UHFRFIDTagModel * model,
                {
                    furi_string_set(model->EPC, extractEPC(inputString));
                    furi_string_set(model->TID, extractTID(inputString));
                    furi_string_set(model->Reserved, extractRES(inputString));
                    furi_string_set(model->User, extractMEM(inputString));
                },
                redraw);
            //Close the file
            flipper_format_file_close(app->epc_file);
        }
    }

    //Freeing all FuriStrings used
    furi_string_free(temp_tag);
    furi_string_free(temp_str);

    //Start the timer 
    furi_assert(app->timer == NULL);
    app->timer =
        furi_timer_alloc(uhf_reader_view_epc_info_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(app->timer, period);
}

/**
 * @brief      Callback when the user exits the tag info screen.
 * @details    This function is called when the user exits the tag info screen.
 * @param      context  The context - UHFReaderApp object.
*/
void uhf_reader_view_epc_info_exit_callback(void* context) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);
    app->timer = NULL;
}

/**
 * @brief      Allocates the tag info view.
 * @details    This function allocates all variables for the tag info view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_epc_info_alloc(UHFReaderApp* app){

    //Allocating the view and setting all callback functions
    app->view_epc_info = view_alloc();
    view_set_draw_callback(app->view_epc_info, uhf_reader_view_epc_info_draw_callback);
    view_set_previous_callback(app->view_epc_info, uhf_reader_navigation_exit_epc_info_callback);
    view_set_enter_callback(app->view_epc_info, uhf_reader_view_epc_info_enter_callback);
    view_set_exit_callback(app->view_epc_info, uhf_reader_view_epc_info_exit_callback);
    view_set_context(app->view_epc_info, app);
    
    //Allocating the view model
    view_allocate_model(app->view_epc_info, ViewModelTypeLockFree, sizeof(UHFRFIDTagModel));
    UHFRFIDTagModel* model_epc_info = view_get_model(app->view_epc_info);
    FuriString* user_mem_epc_info = furi_string_alloc();
    furi_string_set_str(user_mem_epc_info, "000000...");
    FuriString* tid_mem_epc_info = furi_string_alloc();
    furi_string_set_str(tid_mem_epc_info, "E2801105200....");
    FuriString* epc_info = furi_string_alloc();
    furi_string_set_str(epc_info, "ABCDEF12");
    FuriString* reserved_mem_epc_info = furi_string_alloc();
    furi_string_set_str(reserved_mem_epc_info, "131313...");
    
    //Setting default values for the view model
    model_epc_info->User = user_mem_epc_info;
    model_epc_info->EPC = epc_info;
    model_epc_info->TID = tid_mem_epc_info;
    model_epc_info->Reserved = reserved_mem_epc_info;
    model_epc_info->scroll_offset_epc = 0;
    model_epc_info->scrolling_text_epc = "EPC VALUE HERE";
    model_epc_info->scroll_offset_tid = 0;
    model_epc_info->scrolling_text_tid = "TID VALUE HERE";
    model_epc_info->scroll_offset_res = 0;
    model_epc_info->scrolling_text_res = "RES VALUE HERE";
    model_epc_info->scroll_offset_mem = 0;
    model_epc_info->scrolling_text_mem = "MEM VALUE HERE";
    view_dispatcher_add_view(app->view_dispatcher, UHFReaderViewEPCInfo, app->view_epc_info);
}

/**
 * @brief      Frees the tag info view.
 * @details    This function frees all variables for the tag info view.
 * @param      context  The context - UHFReaderApp object.
*/
void view_epc_info_free(UHFReaderApp* app){
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewEPCInfo);
    view_free(app->view_epc_info);
}