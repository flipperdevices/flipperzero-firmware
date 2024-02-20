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

/*
Callback from dmcomm serial output. Similar to the callback in read code,
but output is single sided


206199692 [I][FCOM] DMComm Data: s:800E r:023E s:218E r:A02E s:483E r:B69E s:142E r:28CE s:010E 
206199696 [I][FCOM] reading code
206199698 [I][FCOM] found s
206199700 [I][FCOM] found r
206199701 [I][FCOM] found s
206199703 [I][FCOM] found r
206199705 [I][FCOM] found s
206199707 [I][FCOM] found r
206199708 [I][FCOM] found s
206199710 [I][FCOM] found r
206199712 [I][FCOM] found s
206199714 [I][FCOM] s code V1-800E-218E-483E-142E-010E
206199717 [I][FCOM] r code V2-023E-A02E-B69E-28CE-0C6E-11FE
206199726 [I][FCOM] done
206210730 [I][FCOM] DMComm Data: r:0C6E s:A00E r:11FE
*/
void scbs(void* context)
{
    furi_assert(context);
    App* app = context;

    char out[64];
    size_t recieved = 0;
    memset(out, 0, 64);

    recieved = furi_stream_buffer_receive(
        app->dmcomm_output_stream,
        &out,
        63,
        0);
    UNUSED(recieved);
    FURI_LOG_I(TAG, "DMComm Data: %s", out);
  
    if(app->state->waitForCode)
    {
        FURI_LOG_I(TAG, "reading code");
        int l = strlen(out);
        int first = true;
        for(int i = 0; i < l; i++)
        {
            if(out[i] == 't')
            { // reset for timeout and continue
                app->state->spackets = 0;
                app->state->rpackets = 0;
                furi_string_reset(app->state->r_code);
                furi_string_reset(app->state->s_code);
            }

            // Scode
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
                int limit = 4;
                for(int j = 0; j < limit; j++)
                {
                    if(out[i] == '@' || out[i] == '^')
                        limit++; // These are extra control characters
                    furi_string_push_back(app->state->s_code, out[i++]); // 4 hex
                }
                app->state->spackets++;
            }

            // Rcode
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
                int limit = 4;
                for(int j = 0; j < limit; j++)
                {
                    if(out[i] == '@' || out[i] == '^')
                        limit++; // These are extra control characters
                    furi_string_push_back(app->state->r_code, out[i++]); // 4 hex
                }
                app->state->rpackets++;
            }
        }


        //if spackets == rpackets and spackets = code packets, then present code for saving
        if(app->state->rpackets == app->state->codeLen && app->state->spackets == app->state->codeLen)
        {
            FURI_LOG_I(TAG, "s code %s", furi_string_get_cstr(app->state->s_code));
            FURI_LOG_I(TAG, "r code %s", furi_string_get_cstr(app->state->r_code));
            if(strlen(app->state->current_code) > 2 && app->state->current_code[1] == '1')
                dialog_ex_set_text(app->dialog, furi_string_get_cstr(app->state->r_code), 10, 24, AlignLeft, AlignTop);
            else
                dialog_ex_set_text(app->dialog, furi_string_get_cstr(app->state->s_code), 10, 24, AlignLeft, AlignTop);

            dialog_ex_set_right_button_text(app->dialog, "Save");
            app->state->waitForCode = false;
            FURI_LOG_I(TAG, "done");
        }
    }
}

void send_code_dialog_callback(DialogExResult result, void* context) {
    FURI_LOG_I(TAG, "send_code_dialog_callback");
    furi_assert(context);
    App* app = context;

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

    dialog_ex_set_header(app->dialog, app->state->current_code, 64, 2, AlignCenter, AlignTop);
    dialog_ex_set_text(app->dialog, "Response Code Goes Here", 10, 24, AlignLeft, AlignTop);
    dialog_ex_set_left_button_text(app->dialog, NULL);
    dialog_ex_set_right_button_text(app->dialog, NULL);
    dialog_ex_set_center_button_text(app->dialog, NULL); // This will eventually be a "resend" button
    dialog_ex_set_result_callback(app->dialog, send_code_dialog_callback);
    dialog_ex_set_context(app->dialog, app);

    // Setup dmcomm to send
    app->state->codeLen = 0;
    app->state->rpackets = 0;
    app->state->spackets = 0;
    for(size_t i = 0; i < strlen(app->state->current_code); i++)
    {
        if(app->state->current_code[i] == '-')
            app->state->codeLen++;
    }
    app->state->waitForCode = true;
    setSerialOutputCallback(scbs);
    furi_string_reset(app->state->r_code);
    furi_string_reset(app->state->s_code);
    dmcomm_sendcommand(app, app->state->current_code);
    dmcomm_sendcommand(app, "\n");

    view_dispatcher_switch_to_view(app->view_dispatcher, FcomSendCodeView);
}

bool fcom_send_code_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_send_code_scene_on_event");
    UNUSED(context);
    UNUSED(event);

    return false; //consumed event
}

void fcom_send_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_send_code_scene_on_exit");
    App* app = context;

    // Clear out dmcomm
    setSerialOutputCallback(NULL);
    dmcomm_sendcommand(app, "0\n");
    app->state->waitForCode = false;
}


