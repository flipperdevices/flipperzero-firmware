#include "../app.h"
/**
 * @brief      Callback for handling UART input
 * @details    This function is called there is content to read in the buffer
 * @param      line     The current line in the buffer to read
 * @param      context  The UHFReaderApp
*/
void uart_demo_process_line(FuriString* Line, void* Context) {
    UHFReaderApp* App = (UHFReaderApp*)Context;
    bool Redraw = true;

    switch(App->State) {
    // Handle the Idle State
    case UHFReaderStateIdle:

        // Handles the EPC values
        if(strcmp(furi_string_get_cstr(Line), "TR") == 0) {
            App->State = UHFReaderStateWaitForNumber;
        }

        // Handles the TID values
        else if(strcmp(furi_string_get_cstr(Line), "TID") == 0) {
            // The TID values are sent first, so initialize the index for each type of field
            App->CurEpcIndex = 26;
            App->CurTidIndex = 1;
            App->CurResIndex = 1;
            App->CurMemIndex = 1;

            bool Redraw = true;
            with_view_model(
                App->ViewRead, UHFReaderConfigModel * Model, { Model->CurEpcIndex = 1; }, Redraw);
            App->State = UHFReaderStateWaitForTID;
        }

        // Handles the Reserved Memory Values
        else if(strcmp(furi_string_get_cstr(Line), "RES") == 0) {
            App->State = UHFReaderStateWaitForRES;

        }

        // Handles the User Memory Values
        else if(strcmp(furi_string_get_cstr(Line), "MEM") == 0) {
            App->State = UHFReaderStateWaitForMEM;

        }

        // Handles modified EPC value
        else if(strcmp(furi_string_get_cstr(Line), "EVOK") == 0) {
            if(!flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
                FURI_LOG_E(TAG, "Failed to open file");
            }

            FuriString* NumEpcs = furi_string_alloc();
            FuriString* EpcAndName = furi_string_alloc();
            FuriString* TempTid = furi_string_alloc();
            FuriString* TempRes = furi_string_alloc();
            FuriString* TempMem = furi_string_alloc();

            with_view_model(
                App->ViewWrite,
                UHFReaderWriteModel * Model,
                {
                    furi_string_set(TempTid, Model->TidValue);
                    furi_string_set(TempRes, Model->ResValue);
                    furi_string_set(TempMem, Model->MemValue);
                    furi_string_set(Model->WriteFunction, WRITE_EPC_OK);
                },
                Redraw);

            furi_string_printf(NumEpcs, "Tag%ld", App->SelectedTagIndex);

            furi_string_printf(
                EpcAndName,
                "%s:%s:%s:%s:%s",
                furi_string_get_cstr(App->EpcName),
                App->TempSaveBuffer,
                furi_string_get_cstr(TempTid),
                furi_string_get_cstr(TempRes),
                furi_string_get_cstr(TempMem));

            if(!flipper_format_update_string_cstr(
                   App->EpcFile, furi_string_get_cstr(NumEpcs), furi_string_get_cstr(EpcAndName))) {
                FURI_LOG_E(TAG, "Failed to write to file");
            }

            flipper_format_file_close(App->EpcFile);
            furi_string_free(NumEpcs);
            furi_string_free(EpcAndName);
            furi_string_free(TempTid);
            furi_string_free(TempRes);
            furi_string_free(TempMem);

        } else if(strcmp(furi_string_get_cstr(Line), "EVBAD") == 0) {
            with_view_model(
                App->ViewWrite,
                UHFReaderWriteModel * Model,
                { furi_string_set(Model->WriteFunction, WRITE_EPC_FAIL); },
                Redraw);
        } else if(strcmp(furi_string_get_cstr(Line), "RVOK") == 0) {
            if(!flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
                FURI_LOG_E(TAG, "Failed to open file");
            }

            FuriString* NumEpcs = furi_string_alloc();
            FuriString* EpcAndName = furi_string_alloc();
            FuriString* TempTid = furi_string_alloc();
            FuriString* TempEpc = furi_string_alloc();
            FuriString* TempMem = furi_string_alloc();

            with_view_model(
                App->ViewWrite,
                UHFReaderWriteModel * Model,
                {
                    furi_string_set(TempTid, Model->TidValue);
                    furi_string_set(TempEpc, App->EpcToWrite);
                    furi_string_set(TempMem, Model->MemValue);
                    furi_string_set(Model->WriteFunction, WRITE_EPC_OK);
                },
                Redraw);

            furi_string_printf(NumEpcs, "Tag%ld", App->SelectedTagIndex);

            furi_string_printf(
                EpcAndName,
                "%s:%s:%s:%s:%s",
                furi_string_get_cstr(App->EpcName),
                furi_string_get_cstr(TempEpc),
                furi_string_get_cstr(TempTid),
                App->TempSaveBuffer,
                furi_string_get_cstr(TempMem));

            if(!flipper_format_update_string_cstr(
                   App->EpcFile, furi_string_get_cstr(NumEpcs), furi_string_get_cstr(EpcAndName))) {
                FURI_LOG_E(TAG, "Failed to write to file");
            }

            flipper_format_file_close(App->EpcFile);
            furi_string_free(NumEpcs);
            furi_string_free(EpcAndName);
            furi_string_free(TempTid);
            furi_string_free(TempEpc);
            furi_string_free(TempMem);

        }
        // Handles error if write failed for reserved memory
        else if(strcmp(furi_string_get_cstr(Line), "RVBAD") == 0) {
            with_view_model(
                App->ViewWrite,
                UHFReaderWriteModel * Model,
                { furi_string_set(Model->WriteFunction, WRITE_EPC_FAIL); },
                Redraw);

        }

        // Handles TID modified values
        else if(strcmp(furi_string_get_cstr(Line), "TVOK") == 0) {
            if(!flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
                FURI_LOG_E(TAG, "Failed to open file");
            }

            FuriString* NumEpcs = furi_string_alloc();
            FuriString* EpcAndName = furi_string_alloc();
            FuriString* TempRes = furi_string_alloc();
            FuriString* TempEpc = furi_string_alloc();
            FuriString* TempMem = furi_string_alloc();

            with_view_model(
                App->ViewWrite,
                UHFReaderWriteModel * Model,
                {
                    furi_string_set(TempRes, Model->ResValue);
                    furi_string_set(TempEpc, App->EpcToWrite);
                    furi_string_set(TempMem, Model->MemValue);
                    furi_string_set(Model->WriteFunction, WRITE_EPC_OK);
                },
                Redraw);

            furi_string_printf(NumEpcs, "Tag%ld", App->SelectedTagIndex);

            furi_string_printf(
                EpcAndName,
                "%s:%s:%s:%s:%s",
                furi_string_get_cstr(App->EpcName),
                furi_string_get_cstr(TempEpc),
                App->TempSaveBuffer,
                furi_string_get_cstr(TempRes),
                furi_string_get_cstr(TempMem));

            if(!flipper_format_update_string_cstr(
                   App->EpcFile, furi_string_get_cstr(NumEpcs), furi_string_get_cstr(EpcAndName))) {
                FURI_LOG_E(TAG, "Failed to write to file");
            }

            flipper_format_file_close(App->EpcFile);
            furi_string_free(NumEpcs);
            furi_string_free(EpcAndName);
            furi_string_free(TempRes);
            furi_string_free(TempEpc);
            furi_string_free(TempMem);

        }

        // Handles error if write failed for TID
        else if(strcmp(furi_string_get_cstr(Line), "TVBAD") == 0) {
            with_view_model(
                App->ViewWrite,
                UHFReaderWriteModel * Model,
                { furi_string_set(Model->WriteFunction, WRITE_EPC_FAIL); },
                Redraw);

        }

        // Handles modified user memory values
        else if(strcmp(furi_string_get_cstr(Line), "UVOK") == 0) {
            if(!flipper_format_file_open_existing(App->EpcFile, APP_DATA_PATH("Saved_EPCs.txt"))) {
                FURI_LOG_E(TAG, "Failed to open file");
            }

            FuriString* NumEpcs = furi_string_alloc();
            FuriString* EpcAndName = furi_string_alloc();
            FuriString* TempTid = furi_string_alloc();
            FuriString* TempEpc = furi_string_alloc();
            FuriString* TempRes = furi_string_alloc();

            with_view_model(
                App->ViewWrite,
                UHFReaderWriteModel * Model,
                {
                    furi_string_set(TempTid, Model->TidValue);
                    furi_string_set(TempEpc, App->EpcToWrite);
                    furi_string_set(TempRes, Model->ResValue);
                    furi_string_set(Model->WriteFunction, WRITE_EPC_OK);
                },
                Redraw);

            furi_string_printf(NumEpcs, "Tag%ld", App->SelectedTagIndex);

            furi_string_printf(
                EpcAndName,
                "%s:%s:%s:%s:%s",
                furi_string_get_cstr(App->EpcName),
                furi_string_get_cstr(TempEpc),
                furi_string_get_cstr(TempTid),
                furi_string_get_cstr(TempRes),
                App->TempSaveBuffer);

            if(!flipper_format_update_string_cstr(
                   App->EpcFile, furi_string_get_cstr(NumEpcs), furi_string_get_cstr(EpcAndName))) {
                FURI_LOG_E(TAG, "Failed to write to file");
            }

            flipper_format_file_close(App->EpcFile);
            furi_string_free(NumEpcs);
            furi_string_free(EpcAndName);
            furi_string_free(TempTid);
            furi_string_free(TempEpc);
            furi_string_free(TempRes);

        }

        // Handles failed User Memory writes
        else if(strcmp(furi_string_get_cstr(Line), "UVBAD") == 0) {
            with_view_model(
                App->ViewWrite,
                UHFReaderWriteModel * Model,
                { furi_string_set(Model->WriteFunction, WRITE_EPC_FAIL); },
                Redraw);
        }
        break;

    // Wait for the number of tags read
    case UHFReaderStateWaitForNumber:
        App->NumberOfEpcsToRead = atoi(furi_string_get_cstr(Line));
        memset(App->EpcValues, 0, 127 * 26);
        App->State = UHFReaderStateCollectEPCs;
        break;

    // Collect each EPC value
    case UHFReaderStateCollectEPCs:
        if(strcmp(furi_string_get_cstr(Line), "end") == 0) {
            uart_helper_send(App->UartHelper, "done\n", 5);
            App->State = UHFReaderStateDoneCollecting;

        } else {
            App->EpcValues[App->CurEpcIndex] = strdup(furi_string_get_cstr(Line));
            App->CurEpcIndex += 26;
        }
        break;

    // Wait for the number of TIDs read
    case UHFReaderStateWaitForTID:
        App->NumberOfTidsToRead = atoi(furi_string_get_cstr(Line));
        memset(App->TidValues, 0, 127 * 41);
        App->State = UHFReaderStateCollectTIDs;
        break;

    // Wait for number of reserved reads
    case UHFReaderStateWaitForRES:
        App->NumberOfResToRead = atoi(furi_string_get_cstr(Line));
        memset(App->ResValues, 0, 127 * 17);
        App->State = UHFReaderStateCollectRESs;
        break;

    // Wait for number of user reads
    case UHFReaderStateWaitForMEM:
        App->NumberOfMemToRead = atoi(furi_string_get_cstr(Line));
        memset(App->MemValues, 0, 127 * 33);
        App->State = UHFReaderStateCollectMEMs;
        break;

    // Collect the TIDs read
    case UHFReaderStateCollectTIDs:
        if(strcmp(furi_string_get_cstr(Line), "end") == 0) {
            uart_helper_send(App->UartHelper, "done\n", 5);
            App->State = UHFReaderStateDoneCollectingTIDs;
        } else {
            App->TidValues[App->CurTidIndex * 41] = strdup(furi_string_get_cstr(Line));
            App->CurTidIndex += 1;
        }
        break;

    // Collect the reserved memory blocks read
    case UHFReaderStateCollectRESs:
        if(strcmp(furi_string_get_cstr(Line), "end") == 0) {
            uart_helper_send(App->UartHelper, "done\n", 5);
            App->State = UHFReaderStateDoneCollectingRESs;

        } else {
            App->ResValues[App->CurResIndex * 17] = strdup(furi_string_get_cstr(Line));
            App->CurResIndex += 1;
        }
        break;

    // Collect the User memory blocks read
    case UHFReaderStateCollectMEMs:
        if(strcmp(furi_string_get_cstr(Line), "end") == 0) {
            uart_helper_send(App->UartHelper, "done\n", 5);
            App->State = UHFReaderStateDoneCollectingMEMs;
        } else {
            App->MemValues[App->CurMemIndex * 33] = strdup(furi_string_get_cstr(Line));
            App->CurMemIndex += 1;
        }
        break;

    // State after done collecting TIDs
    case UHFReaderStateDoneCollectingTIDs:
        App->CurTidIndex = 1;
        with_view_model(
            App->ViewEpc,
            UHFRFIDTagModel * Model,
            {
                if(App->NumberOfTidsToRead > 0) {
                    furi_string_set_str(Model->Tid, App->TidValues[App->CurTidIndex * 41]);
                }
            },
            Redraw);

        // Send the next command to read the EPCs from the RPi
        uart_helper_send(App->UartHelper, "EPCS\n", 5);
        App->State = UHFReaderStateIdle;
        break;

    // State after done collecting Reserved Mem Blocks
    case UHFReaderStateDoneCollectingRESs:
        App->CurResIndex = 1;
        with_view_model(
            App->ViewEpc,
            UHFRFIDTagModel * Model,
            {
                if(App->NumberOfResToRead > 0) {
                    furi_string_set_str(Model->Reserved, App->ResValues[App->CurResIndex * 17]);
                }
            },
            Redraw);
        uart_helper_send(App->UartHelper, "MEM\n", 4);
        App->State = UHFReaderStateIdle;
        break;

    // State after done collecting User Mem Blocks
    case UHFReaderStateDoneCollectingMEMs:
        App->CurMemIndex = 1;
        with_view_model(
            App->ViewEpc,
            UHFRFIDTagModel * Model,
            {
                if(App->NumberOfMemToRead > 0) {
                    furi_string_set_str(Model->User, App->MemValues[App->CurMemIndex * 33]);
                }
            },
            Redraw);
        App->IsReading = false;
        with_view_model(
            App->ViewRead, UHFReaderConfigModel * Model, { Model->IsReading = false; }, Redraw);
        App->State = UHFReaderStateIdle;
        break;
    // State after done collecting EPCs
    case UHFReaderStateDoneCollecting:
        App->CurEpcIndex = 26;
        bool Redraw = true;
        with_view_model(
            App->ViewRead,
            UHFReaderConfigModel * Model,
            {
                if(App->NumberOfEpcsToRead > 0) {
                    furi_string_set_str(Model->EpcValue, App->EpcValues[Model->CurEpcIndex * 26]);

                    Model->NumEpcsRead = App->NumberOfEpcsToRead;
                }
            },
            Redraw);
        with_view_model(
            App->ViewEpc,
            UHFRFIDTagModel * Model,
            {
                if(App->NumberOfEpcsToRead > 0) {
                    furi_string_set_str(Model->Epc, App->EpcValues[App->CurTidIndex * 26]);
                }
            },
            Redraw);

        uart_helper_send(App->UartHelper, "RES\n", 4);
        App->State = UHFReaderStateIdle;
        break;
    }
}