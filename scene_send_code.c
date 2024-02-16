/*
Sends the code to the DMComm and initiates sending

Displays current code we are sending in bold
Waits for and displays last recieved code in thin text
Right will go to the save code dialog for the new code (if there is one)
*/

#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_send_code.h"
#include <furi_hal_cortex.h>

void scbs(void* context)
{
    furi_assert(context);
    App* app = context;

    furi_check(furi_mutex_acquire(app->dmcomm_output_mutex, FuriWaitForever) == FuriStatusOk);  
    const char *out = furi_string_get_cstr(app->dmcomm_output_buffer);
    FURI_LOG_I(TAG, "DMComm Data: %s", out);
  
    if(app->state->waitForCode)
    {
        FURI_LOG_I(TAG, "reading code");
        furi_string_reset(app->state->r_code);
        furi_string_reset(app->state->s_code);
        int rpackets = 0;
        int spackets = 0;
        int l = strlen(out);
        int first = true;
        for(int i = 0; i < l; i++)
        {
            if(out[i] == 's' && i + 5 < l)
            {
                FURI_LOG_I(TAG, "found s");
                if(furi_string_empty(app->state->s_code))
                {
                    if(first)
                    {
                        furi_string_cat_printf(app->state->s_code, "V1-");
                        first = false;
                    }
                    else
                        furi_string_cat_printf(app->state->s_code, "V2-");
                }
                else
                    furi_string_cat_printf(app->state->s_code, "-");

                i += 2; // :
                for(int j = 0; j < 4; j++)
                    furi_string_push_back(app->state->s_code, out[i++]); // 4 hex
                spackets++;
            }

            if(out[i] == 'r' && i + 5 < l)
            {
                FURI_LOG_I(TAG, "found r");
                if(furi_string_empty(app->state->r_code))
                {
                    if(first)
                    {
                        furi_string_cat_printf(app->state->r_code, "V1-");
                        first = false;
                    }
                    else
                        furi_string_cat_printf(app->state->r_code, "V2-");
                }
                else
                    furi_string_cat_printf(app->state->r_code, "-");

                i += 2; // :
                for(int j = 0; j < 4; j++)
                    furi_string_push_back(app->state->r_code, out[i++]); // 4 hex
                rpackets++;
            }
        }


        FURI_LOG_I(TAG, "s code %s", furi_string_get_cstr(app->state->s_code));
        FURI_LOG_I(TAG, "r code %s", furi_string_get_cstr(app->state->r_code));
        if(strlen(app->state->current_code) > 2 && app->state->current_code[1] == '1')
            dialog_ex_set_text(app->dialog, furi_string_get_cstr(app->state->r_code), 10, 24, AlignLeft, AlignTop);
        else
            dialog_ex_set_text(app->dialog, furi_string_get_cstr(app->state->s_code), 10, 24, AlignLeft, AlignTop);

        //if spackets == rpackets and spackets = code packets, then present code for saving
        if(rpackets > 0 && spackets > 0 && rpackets == spackets)
        {
            app->state->waitForCode = false;
            dialog_ex_set_right_button_text(app->dialog, "Save");
        }
    }

    furi_string_reset(app->dmcomm_output_buffer);
    furi_check(furi_mutex_release(app->dmcomm_output_mutex) == FuriStatusOk);

    FURI_LOG_I(TAG, "done");
}

void send_code_dialog_callback(DialogExResult result, void* context) {
    FURI_LOG_I(TAG, "send_code_dialog_callback");
    furi_assert(context);
    App* app = context;

    if(result == DialogExResultCenter) {
        FURI_LOG_I(TAG, "DialogExResultCenter");
        if(app->state->waitForCode)
            return; // dont allow qeueing up multiple...

        app->state->waitForCode = true;
        setSerialOutputCallback(scbs);
        furi_string_reset(app->state->r_code);
        furi_string_reset(app->state->s_code);
        dmcomm_sendcommand(app, app->state->current_code);
        dmcomm_sendcommand(app, "\n");
        FURI_LOG_I(TAG, "dmcomm_sendcommand");
    }

    if(result == DialogExResultLeft) {
        // ignore
    }
    if(result == DialogExResultRight) {
        if(furi_string_empty(app->state->r_code))
            return;
        if(furi_string_empty(app->state->s_code))
            return;
        // save code
        if(strlen(app->state->current_code) > 2 && app->state->current_code[1] == '1')
            strncpy(app->state->result_code, furi_string_get_cstr(app->state->r_code), MAX_FILENAME_LEN);
        else
            strncpy(app->state->result_code, furi_string_get_cstr(app->state->s_code), MAX_FILENAME_LEN);
        app->state->save_code_return_scene = FcomSendCodeScene;
        scene_manager_next_scene(app->scene_manager, FcomSaveCodeScene);
    }
}

void fcom_send_code_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_send_code_scene_on_enter");
    App* app = context;
    // initialize dcomm
    // start dcomm thread in read mode

    dialog_ex_set_header(app->dialog, app->state->current_code, 64, 2, AlignCenter, AlignTop);
    dialog_ex_set_text(app->dialog, "Response Code Goes Here", 10, 24, AlignLeft, AlignTop);
    dialog_ex_set_left_button_text(app->dialog, NULL);
    dialog_ex_set_right_button_text(app->dialog, NULL);
    dialog_ex_set_center_button_text(app->dialog, "Send");
    dialog_ex_set_result_callback(app->dialog, send_code_dialog_callback);
    dialog_ex_set_context(app->dialog, app);


    view_dispatcher_switch_to_view(app->view_dispatcher, FcomSendCodeView);
}

bool fcom_send_code_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_send_code_scene_on_event");
    UNUSED(context);
    UNUSED(event);

    // wait for event, then transfer to
    // Display code and Retry/More
    // More goes to submenu "Save" / "Emulate" 
    // Save goes to text input ("Name the card")
    // Emulate goes to Send screen "Send" press OK sends the code
    //

    return false; //consumed event
}

void fcom_send_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_send_code_scene_on_exit");
    UNUSED(context);
    App* app = context;
    UNUSED(app);

    setSerialOutputCallback(NULL);

    dmcomm_sendcommand(app, "0\n");
    app->state->waitForCode = false;
}


