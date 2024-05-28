#include "../app.h"

/**
 * @brief      Callback for handling UART input
 * @details    This function is called there is content to read in the buffer
 * @param      line     The current line in the buffer to read
 * @param      context  The UHFReaderApp
*/
void uart_demo_process_line(FuriString* line, void* context) {
    UHFReaderApp* app = context;
    bool redraw = true;

    switch(app->state) {
    
    //Handle the Idle State
    case UHFReaderState_Idle:

        //Handles the EPC values
        if(strcmp(furi_string_get_cstr(line), "TR") == 0) {
            app->state = UHFReaderState_WaitForNumber;
        } 
        
        //Handles the TID values
        else if(strcmp(furi_string_get_cstr(line), "TID") == 0) {

            //The TID values are sent first, so I initialize the index for each type of field
            app->cur_epc_index = 26;
            app->cur_tid_index = 1;
            app->cur_res_index = 1;
            app->cur_mem_index = 1;
            
            bool redraw = true;
            with_view_model(
                app->view_read,
                UHFReaderConfigModel * model,
                { model->cur_epc_index = 1; },
                redraw);
            app->state = UHFReaderState_WaitForTID;
        } 
        
        //Handles the Reserved Memory Values
        else if(strcmp(furi_string_get_cstr(line), "RES") == 0) {
            app->state = UHFReaderState_WaitForRES;

        } 

        //Handles the User Memory Values
        else if(strcmp(furi_string_get_cstr(line), "MEM") == 0) {
            app->state = UHFReaderState_WaitForMEM;

        } 
        
        //Handles modified epc value
        else if(strcmp(furi_string_get_cstr(line), "EVOK") == 0) { //For handling modified epc value

            if(!flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
                FURI_LOG_E(TAG, "Failed to open file");
            }

            FuriString* num_epcs = furi_string_alloc();
            FuriString* epc_and_name = furi_string_alloc();
            FuriString* temp_tid = furi_string_alloc();
            FuriString* temp_res = furi_string_alloc();
            FuriString* temp_mem = furi_string_alloc();

            with_view_model(
                app->view_write,
                UHFReaderWriteModel * model,
                {
                    furi_string_set(temp_tid, model->tid_value);
                    furi_string_set(temp_res, model->res_value);
                    furi_string_set(temp_mem, model->mem_value);
                    furi_string_set(model->write_function, WRITE_EPC_OK);
                },
                redraw);

            furi_string_printf(num_epcs, "Tag%ld", app->selected_tag_index);

            furi_string_printf(
                epc_and_name,
                "%s:%s:%s:%s:%s",
                furi_string_get_cstr(app->epc_name),
                app->temp_save_buffer,
                furi_string_get_cstr(temp_tid),
                furi_string_get_cstr(temp_res),
                furi_string_get_cstr(temp_mem));

            if(!flipper_format_update_string_cstr(
                   app->epc_file,
                   furi_string_get_cstr(num_epcs),
                   furi_string_get_cstr(epc_and_name))) {
                FURI_LOG_E(TAG, "Failed to write to file");
            }

            flipper_format_file_close(app->epc_file);
            furi_string_free(num_epcs);
            furi_string_free(epc_and_name);
            furi_string_free(temp_tid);
            furi_string_free(temp_res);
            furi_string_free(temp_mem);

        } else if(strcmp(furi_string_get_cstr(line), "EVBAD") == 0) {
            with_view_model(
                app->view_write,
                UHFReaderWriteModel * model,
                { furi_string_set(model->write_function, WRITE_EPC_FAIL); },
                redraw);
        } else if(strcmp(furi_string_get_cstr(line), "RVOK") == 0) {
            if(!flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
                FURI_LOG_E(TAG, "Failed to open file");
            }

            FuriString* num_epcs = furi_string_alloc();
            FuriString* epc_and_name = furi_string_alloc();
            FuriString* temp_tid = furi_string_alloc();
            FuriString* temp_epc = furi_string_alloc();
            FuriString* temp_mem = furi_string_alloc();

            with_view_model(
                app->view_write,
                UHFReaderWriteModel * model,
                {
                    furi_string_set(temp_tid, model->tid_value);
                    furi_string_set(temp_epc, app->epc_to_write);
                    furi_string_set(temp_mem, model->mem_value);
                    furi_string_set(model->write_function, WRITE_EPC_OK);
                },
                redraw);

            furi_string_printf(num_epcs, "Tag%ld", app->selected_tag_index);

            furi_string_printf(
                epc_and_name,
                "%s:%s:%s:%s:%s",
                furi_string_get_cstr(app->epc_name),
                furi_string_get_cstr(temp_epc),
                furi_string_get_cstr(temp_tid),
                app->temp_save_buffer,
                furi_string_get_cstr(temp_mem));

            if(!flipper_format_update_string_cstr(
                   app->epc_file,
                   furi_string_get_cstr(num_epcs),
                   furi_string_get_cstr(epc_and_name))) {
                FURI_LOG_E(TAG, "Failed to write to file");
            }

            flipper_format_file_close(app->epc_file);
            furi_string_free(num_epcs);
            furi_string_free(epc_and_name);
            furi_string_free(temp_tid);
            furi_string_free(temp_epc);
            furi_string_free(temp_mem);

        } 
        //Handles error if write failed for reservd memory
        else if(strcmp(furi_string_get_cstr(line), "RVBAD") == 0) {
            with_view_model(
                app->view_write,
                UHFReaderWriteModel * model,
                { furi_string_set(model->write_function, WRITE_EPC_FAIL); },
                redraw);

        } 

        //Handles TID modified values
        else if(strcmp(furi_string_get_cstr(line), "TVOK") == 0) {
            if(!flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
                FURI_LOG_E(TAG, "Failed to open file");
            }

            FuriString* num_epcs = furi_string_alloc();
            FuriString* epc_and_name = furi_string_alloc();
            FuriString* temp_res = furi_string_alloc();
            FuriString* temp_epc = furi_string_alloc();
            FuriString* temp_mem = furi_string_alloc();

            with_view_model(
                app->view_write,
                UHFReaderWriteModel * model,
                {
                    furi_string_set(temp_res, model->res_value);
                    furi_string_set(temp_epc, app->epc_to_write);
                    furi_string_set(temp_mem, model->mem_value);
                    furi_string_set(model->write_function, WRITE_EPC_OK);
                },
                redraw);

            furi_string_printf(num_epcs, "Tag%ld", app->selected_tag_index);

            furi_string_printf(
                epc_and_name,
                "%s:%s:%s:%s:%s",
                furi_string_get_cstr(app->epc_name),
                furi_string_get_cstr(temp_epc),
                app->temp_save_buffer,
                furi_string_get_cstr(temp_res),
                furi_string_get_cstr(temp_mem));

            if(!flipper_format_update_string_cstr(
                   app->epc_file,
                   furi_string_get_cstr(num_epcs),
                   furi_string_get_cstr(epc_and_name))) {
                FURI_LOG_E(TAG, "Failed to write to file");
            }

            flipper_format_file_close(app->epc_file);
            furi_string_free(num_epcs);
            furi_string_free(epc_and_name);
            furi_string_free(temp_res);
            furi_string_free(temp_epc);
            furi_string_free(temp_mem);

        } 
        
        //Handles error if write failed for TID
        else if(strcmp(furi_string_get_cstr(line), "TVBAD") == 0) {
            with_view_model(
                app->view_write,
                UHFReaderWriteModel * model,
                { furi_string_set(model->write_function, WRITE_EPC_FAIL); },
                redraw);

        } 
        
        //Handles modified user memory values
        else if(strcmp(furi_string_get_cstr(line), "UVOK") == 0) {
            if(!flipper_format_file_open_existing(app->epc_file, APP_DATA_PATH("Saved_EPCs.txt"))) {
                FURI_LOG_E(TAG, "Failed to open file");
            }

            FuriString* num_epcs = furi_string_alloc();
            FuriString* epc_and_name = furi_string_alloc();
            FuriString* temp_tid = furi_string_alloc();
            FuriString* temp_epc = furi_string_alloc();
            FuriString* temp_res = furi_string_alloc();

            with_view_model(
                app->view_write,
                UHFReaderWriteModel * model,
                {
                    furi_string_set(temp_tid, model->tid_value);
                    furi_string_set(temp_epc, app->epc_to_write);
                    furi_string_set(temp_res, model->res_value);
                    furi_string_set(model->write_function, WRITE_EPC_OK);
                },
                redraw);

            furi_string_printf(num_epcs, "Tag%ld", app->selected_tag_index);

            furi_string_printf(
                epc_and_name,
                "%s:%s:%s:%s:%s",
                furi_string_get_cstr(app->epc_name),
                furi_string_get_cstr(temp_epc),
                furi_string_get_cstr(temp_tid),
                furi_string_get_cstr(temp_res),
                app->temp_save_buffer);

            if(!flipper_format_update_string_cstr(
                   app->epc_file,
                   furi_string_get_cstr(num_epcs),
                   furi_string_get_cstr(epc_and_name))) {
                FURI_LOG_E(TAG, "Failed to write to file");
            }

            flipper_format_file_close(app->epc_file);
            furi_string_free(num_epcs);
            furi_string_free(epc_and_name);
            furi_string_free(temp_tid);
            furi_string_free(temp_epc);
            furi_string_free(temp_res);

        } 
        
        //Handles failed User Memory writes
        else if(strcmp(furi_string_get_cstr(line), "UVBAD") == 0) {
            with_view_model(
                app->view_write,
                UHFReaderWriteModel * model,
                { furi_string_set(model->write_function, WRITE_EPC_FAIL); },
                redraw);
        }
        break;
    
    //Wait for the number of tags read
    case UHFReaderState_WaitForNumber:
        app->number_of_epcs_to_read = atoi(furi_string_get_cstr(line));
        memset(app->epc_values, 0, 127 * 26);
        app->state = UHFReaderState_CollectEPCs;
        break;

    //Collect each epc value
    case UHFReaderState_CollectEPCs:
        if(strcmp(furi_string_get_cstr(line), "end") == 0) {
            uart_helper_send(app->uart_helper, "done\n", 5);
            app->state = UHFReaderState_DoneCollecting;

        } else {
            app->epc_values[app->cur_epc_index] = strdup(furi_string_get_cstr(line));
            app->cur_epc_index += 26;
        }
        break;
    
    //Wait for the number of TIDs read
    case UHFReaderState_WaitForTID:
        app->number_of_tids_to_read = atoi(furi_string_get_cstr(line));
        memset(app->tid_values, 0, 127 * 41);
        app->state = UHFReaderState_CollectTIDs;
        break;
    
    //Wait for number of reserved reads 
    case UHFReaderState_WaitForRES:
        app->number_of_res_to_read = atoi(furi_string_get_cstr(line));
        memset(app->res_values, 0, 127 * 17);
        app->state = UHFReaderState_CollectRESs;
        break;
    
    //Wait for number of user reads 
    case UHFReaderState_WaitForMEM:
        app->number_of_mem_to_read = atoi(furi_string_get_cstr(line));
        memset(app->mem_values, 0, 127 * 33);
        app->state = UHFReaderState_CollectMEMs;
        break;
    
    //Collect the TIDs read
    case UHFReaderState_CollectTIDs:
        if(strcmp(furi_string_get_cstr(line), "end") == 0) {
            uart_helper_send(app->uart_helper, "done\n", 5);
            app->state = UHFReaderState_DoneCollectingTIDs;
        } else {
            app->tid_values[app->cur_tid_index * 41] = strdup(furi_string_get_cstr(line));
            app->cur_tid_index += 1;
        }
        break;
    
    //Collect the reserved memory blocks read
    case UHFReaderState_CollectRESs:
        if(strcmp(furi_string_get_cstr(line), "end") == 0) {
            uart_helper_send(app->uart_helper, "done\n", 5);
            app->state = UHFReaderState_DoneCollectingRESs;

        } else {
            app->res_values[app->cur_res_index * 17] = strdup(furi_string_get_cstr(line));
            app->cur_res_index += 1;
        }
        break;
    
    //Collect the User memory blocks read
    case UHFReaderState_CollectMEMs:
        if(strcmp(furi_string_get_cstr(line), "end") == 0) {
            uart_helper_send(app->uart_helper, "done\n", 5);
            app->state = UHFReaderState_DoneCollectingMEMs;
        } else {
            app->mem_values[app->cur_mem_index * 33] = strdup(furi_string_get_cstr(line));
            app->cur_mem_index += 1;
        }
        break;

    //State after done collecting TIDs
    case UHFReaderState_DoneCollectingTIDs:
        app->cur_tid_index = 1;
        with_view_model(
            app->view_epc,
            UHFRFIDTagModel * model,
            {
                if(app->number_of_tids_to_read > 0) {
                    furi_string_set_str(model->TID, app->tid_values[app->cur_tid_index * 41]);
                }
            },
            redraw);

        //Send the next command to read the EPCS from the RPi
        uart_helper_send(app->uart_helper, "EPCS\n", 5);
        app->state = UHFReaderState_Idle;
        break;
    
    //State after done collecting Reserved Mem Blocks
    case UHFReaderState_DoneCollectingRESs:
        app->cur_res_index = 1;
        with_view_model(
            app->view_epc,
            UHFRFIDTagModel * model,
            {
                if(app->number_of_res_to_read > 0) {
                    furi_string_set_str(model->Reserved, app->res_values[app->cur_res_index * 17]);
                }
            },
            redraw);
        uart_helper_send(app->uart_helper, "MEM\n", 4);
        app->state = UHFReaderState_Idle;
        break;
    
    //State after done collecting User Mem Blocks
    case UHFReaderState_DoneCollectingMEMs:
        app->cur_mem_index = 1;
        with_view_model(
            app->view_epc,
            UHFRFIDTagModel * model,
            {
                if(app->number_of_mem_to_read > 0) {
                    furi_string_set_str(model->User, app->mem_values[app->cur_mem_index * 33]);
                }
            },
            redraw);
        app->is_reading = false;
        with_view_model(
            app->view_read, UHFReaderConfigModel * model, { model->is_reading = false; }, redraw);
        app->state = UHFReaderState_Idle;
        break;
    //State after done collecting EPCS
    case UHFReaderState_DoneCollecting:
        app->cur_epc_index = 26;
        bool redraw = true;
        with_view_model(
            app->view_read,
            UHFReaderConfigModel * model,
            {
                if(app->number_of_epcs_to_read > 0) {
                    furi_string_set_str(
                        model->epc_value, app->epc_values[model->cur_epc_index * 26]);

                    model->num_epcs_read = app->number_of_epcs_to_read;
                }
            },
            redraw);
        with_view_model(
            app->view_epc,
            UHFRFIDTagModel * model,
            {
                if(app->number_of_epcs_to_read > 0) {
                    furi_string_set_str(model->EPC, app->epc_values[app->cur_tid_index * 26]);
                }
            },
            redraw);

        uart_helper_send(app->uart_helper, "RES\n", 4);
        app->state = UHFReaderState_Idle;
        break;
    }
}