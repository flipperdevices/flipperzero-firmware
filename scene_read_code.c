/*
Listen to two devices chat and then allow options for saving either code

Left save s code
Right save r code
Back return

Post save should go back to us
*/
#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_read_code.h"
#include <furi_hal_cortex.h>

void scb(void* context)
{
  furi_assert(context);
  App* app = context;

  furi_check(furi_mutex_acquire(app->dmcomm_output_mutex, FuriWaitForever) == FuriStatusOk);  
  const char *out = furi_string_get_cstr(app->dmcomm_output_buffer);
  FURI_LOG_I(TAG, "DMComm Data: %s", out);
  
    if(app->state->waitForCode)
    {
        furi_string_reset(app->state->r_code);
        furi_string_reset(app->state->s_code);
        furi_string_cat_printf(app->state->s_code, "V2-");
        int rpackets = 0;
        int spackets = 0;
        int l = strlen(out);
        int first = true;
        for(int i = 0; i < l; i++)
        {
            //if(code char 2 is 1/2, then read r, if it's 0 then read both)
            if(out[i] == 's' && i + 5 < l)
            {
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

                i++; // :
                for(int j = 0; j < 4; j++)
                    furi_string_push_back(app->state->s_code, out[i++]); // 4 hex
                spackets++;
            }

            if(out[i] == 'r' && i + 5 < l)
            {
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

                i++; // :
                for(int j = 0; j < 4; j++)
                    furi_string_push_back(app->state->r_code, out[i++]); // 4 hex
                rpackets++;
            }
        }

        //if spackets == rpackets and spackets = code packets, then present code for saving
        app->state->waitForCode = false;
    }

  furi_string_reset(app->dmcomm_output_buffer);
  furi_check(furi_mutex_release(app->dmcomm_output_mutex) == FuriStatusOk);
}
/*
got 12 bytes: V1-FC03-FD02 -> V1-[2 packets]
s:FC03 t 
s:FC03 t 
s:FC03 t 
s:FC03 t 
s:FC03 r:FC03 s:FD02 r:EE11 
s:FC03 t 
s:FC03 t
*/

/*
40228 [I][FCOM] DMComm Data: s:FC03 r:FC03 s:FD02 r:EE11

<code first char>1-FC03-FD02 # odd packets
<code first char>2-FC03-EE11 # even packets
*/

void read_code_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    App* app = context;
    UNUSED(app);
    if(result == DialogExResultRight) {
        FURI_LOG_I(TAG, "DialogExResultRight");
        scene_manager_next_scene(app->scene_manager, FcomSaveCodeScene);
    }
}

void fcom_read_code_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_enter");
    App* app = context;
    dialog_ex_set_header(app->dialog, "Read Digimon Code", 64, 12, AlignCenter, AlignTop);
    dialog_ex_set_left_button_text(app->dialog, NULL);
    dialog_ex_set_right_button_text(app->dialog, "Save");
    dialog_ex_set_center_button_text(app->dialog, NULL);
    dialog_ex_set_result_callback(app->dialog, read_code_dialog_callback);
    dialog_ex_set_context(app->dialog, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, FcomReadCodeView);

    setSerialOutputCallback(scb);

    // Code for DM20 copymon get
    //dmcomm_sendcommand(app, "V1-0C02-1207-810E-03AE-000E-000E-000E-000E-000E-@000E\n");
    
    furi_string_reset(app->state->r_code);
    furi_string_reset(app->state->s_code);

    app->state->waitForCode = true;

    // Other battle, agumon, device wins
    dmcomm_sendcommand(app, "V1-FC03-FD02\n");
    
    // start dcomm thread in read mode and flash LED.
    // when code is read, stop LED flashing and set right button to "Save"
}

bool fcom_read_code_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_event");
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

void fcom_read_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_exit");
    UNUSED(context);
    App* app = context;
    UNUSED(app);
    // shut down dcomm
    // clean up
}


