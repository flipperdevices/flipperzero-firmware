/*
Listen to two devices chat and then allow options for saving either code

The dmcomm code just outputs a string of r codes. So we do some stuff to alternate
between saving to r_code and s_code so we can save each devices' codes separately.
Additionally, this method _always_ terminates via timeout... so we need additional handling for that.

e.g.
r:FC03 r:F30C r:ED12 r:EE11 t

will be interpreted as

s_code = V1-FC03-ED12
r_code = V2-F3OC-EE11

It's probably more complicated than it needs to be atm :/

TODO:
- For some reason... this hangs the UI while DMComm is doing stuff. I remember it happening similar before but don't remember why.
- Refactor the serial reader into a state machine

something like:

idle r:
 r -> pending r code
 t -> save validate (reset for send code)
 newline -> save validate
 anything else -> reset

idle s:
 r -> pending s code
 t -> save validate (reset for send code)
 newline -> save validate
 anything else -> reset

pending code:
 : -> read code
 anything else -> reset

read code:
 A-Z,0-9 -> populate code
 space -> end code -> idle <opposite code s/r>
 anything else -> reset

save validate:
 if code valid -> done
 if not valid -> reset

reset:
 clear code then -> idle s
*/
#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "dmcomm_lib/fcom.h"
#include "scene_read_code.h"
#include <furi_hal_cortex.h>

#define MAX_DISPLAY_BYTES 36

// These need to persist between processInput calls as the serial data comes in at random points
static char curcode = 0;
static bool first = false;
static bool assert_next_colon = false; // Necessary for managing state

// NOTE: listen mode only returns 'r' codes. but we want to split into "1st" and "2nd" device codes
// So use this to alternate logically between which code slot we save in between data blocks
static char altcode = 's';

/*
Callback from dmcomm thread with serial results
*/

void processReadInput(void* context) {
    furi_assert(context);
    App* app = context;

    char out[64];
    size_t recieved = 1;

    while(recieved > 0) {
        memset(out, 0, 64);
        recieved = furi_stream_buffer_receive(app->dmcomm_output_stream, &out, 63, 0);

        if(app->state->waitForCode && recieved > 0) {
            FURI_LOG_I(TAG, "DMComm Sent Data: %d <%s>", recieved, out);
            //FURI_LOG_I(TAG, "reading code");
            int l = strlen(out);
            for(int i = 0; i < l; i++) {
                if(out[i] == 't' ||
                   (assert_next_colon && out[i] != ':')) { // reset for timeout and continue :(

                    if((app->state->rpackets == 0 && app->state->spackets == 0) ||
                       (assert_next_colon && out[i] != ':')) {
                        //FURI_LOG_I(TAG, "reset codes");
                        curcode = 0;
                        first = true;
                        app->state->spackets = 0;
                        app->state->rpackets = 0;
                        furi_string_reset(app->state->r_code);
                        furi_string_reset(app->state->s_code);
                    } // Listen always times out
                }
                if(assert_next_colon) assert_next_colon = false;
                if(out[i] == 'r' && altcode == 's') { // Starts an s code block
                    curcode = 's';
                    assert_next_colon = true;
                    //FURI_LOG_I(TAG, "read s");
                    if(furi_string_empty(app->state->s_code)) {
                        furi_string_push_back(app->state->s_code, app->state->current_code[0]);
                        if(first) {
                            //FURI_LOG_I(TAG, "s first");
                            first = false;
                            furi_string_push_back(app->state->s_code, '1');
                        } else {
                            //FURI_LOG_I(TAG, "s second");
                            furi_string_push_back(app->state->s_code, '2');
                        }
                    }
                    furi_string_push_back(app->state->s_code, '-');
                } else if(out[i] == 'r' && altcode == 'r') { // Starts an r code block
                    curcode = 'r';
                    assert_next_colon = true;
                    //FURI_LOG_I(TAG, "read r");
                    if(furi_string_empty(app->state->r_code)) {
                        furi_string_push_back(app->state->r_code, app->state->current_code[0]);
                        if(first) {
                            //FURI_LOG_I(TAG, "r first");
                            first = false;
                            furi_string_push_back(app->state->r_code, '1');
                        } else {
                            //FURI_LOG_I(TAG, "r second");
                            furi_string_push_back(app->state->r_code, '2');
                        }
                    }
                    furi_string_push_back(app->state->r_code, '-');
                } else if(
                    curcode != 0 &&
                    (('A' <= out[i] && out[i] <= 'Z') ||
                     ('0' <= out[i] &&
                      out[i] <= '9'))) { // If we're reading a code, read alphanum into the code
                    //FURI_LOG_I(TAG, "read char %c", out[i]);
                    if(curcode == 's') furi_string_push_back(app->state->s_code, out[i]);
                    if(curcode == 'r') furi_string_push_back(app->state->r_code, out[i]);
                } else if(
                    curcode != 0 && (out[i] == ' ' ||
                                     out[i] == '\n')) { // If we're reading a code, a space ends it
                    //FURI_LOG_I(TAG, "code done");
                    if(curcode == 's') app->state->spackets++;
                    if(curcode == 'r') app->state->rpackets++;
                    if(altcode == 's')
                        altcode = 'r';
                    else
                        altcode = 's';
                    curcode = 0;
                }

                if(out[i] == '\n') {
                    //FURI_LOG_I(TAG, "data done");
                    // If we have a newline, then we've ended input. Since we're not driving
                    // input, we don't know codeLen ahead of time. So now set it to spackets
                    // so we can enter the final if and output the codes
                    app->state->codeLen = app->state->spackets;
                }

                // if spackets == rpackets and spackets = code packets, then present code for saving
                // and stop scanning for new codes. Perhaps shutdown dmcomm at this point?
                if(app->state->codeLen > 0 && app->state->rpackets == app->state->codeLen &&
                   app->state->spackets == app->state->codeLen) {
                    FURI_LOG_I(TAG, "s code %s", furi_string_get_cstr(app->state->s_code));
                    FURI_LOG_I(TAG, "r code %s", furi_string_get_cstr(app->state->r_code));

                    furi_string_set_strn(
                        app->dialog_header,
                        furi_string_get_cstr(app->state->s_code),
                        MAX_DISPLAY_BYTES);
                    if(furi_string_size(app->dialog_header) >= MAX_DISPLAY_BYTES)
                        furi_string_cat(app->dialog_header, "...");
                    dialog_ex_set_header(
                        app->dialog,
                        furi_string_get_cstr(app->dialog_header),
                        64,
                        12,
                        AlignCenter,
                        AlignTop);

                    furi_string_set_strn(
                        app->dialog_text,
                        furi_string_get_cstr(app->state->r_code),
                        MAX_DISPLAY_BYTES);
                    if(furi_string_size(app->dialog_text) >= MAX_DISPLAY_BYTES)
                        furi_string_cat(app->dialog_text, "...");
                    dialog_ex_set_text(
                        app->dialog,
                        furi_string_get_cstr(app->dialog_text),
                        10,
                        24,
                        AlignLeft,
                        AlignTop);

                    dialog_ex_set_left_button_text(app->dialog, "Save Top");
                    dialog_ex_set_right_button_text(app->dialog, "Save Bot");
                    app->state->waitForCode = false;
                    dmcomm_sendcommand(
                        app, "0\n"); // Stop the dmcomm, we don't want to overwrite codes
                    break; // stop reading data
                    FURI_LOG_I(TAG, "done");
                }
            }
        }
    }
}

void read_code_cb(void* context) {
    // This needs to be pretty short or it will delay comms
    furi_assert(context);
    App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, SerialInputAvailable);
}

void read_code_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    App* app = context;

    app->state->save_code_return_scene = FcomReadCodeScene;
    if(result == DialogExResultRight) {
        FURI_LOG_I(TAG, "DialogExResultRight");
        // copy r_code
        strncpy(
            app->state->result_code, furi_string_get_cstr(app->state->r_code), MAX_DIGIROM_LEN);
        scene_manager_next_scene(app->scene_manager, FcomSaveCodeScene);
    }
    if(result == DialogExResultLeft) {
        FURI_LOG_I(TAG, "DialogExResultLeft");
        // copy s_code
        strncpy(
            app->state->result_code, furi_string_get_cstr(app->state->s_code), MAX_DIGIROM_LEN);
        scene_manager_next_scene(app->scene_manager, FcomSaveCodeScene);
    }
}

void fcom_read_code_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_read_code_scene_on_enter");
    App* app = context;

    bool hasData = !furi_string_empty(app->state->r_code) &&
                   !furi_string_empty(app->state->s_code);

    if(hasData) {
        furi_string_set_strn(
            app->dialog_header, furi_string_get_cstr(app->state->s_code), MAX_DISPLAY_BYTES);
        if(furi_string_size(app->dialog_header) >= MAX_DISPLAY_BYTES)
            furi_string_cat(app->dialog_header, "...");
        dialog_ex_set_header(
            app->dialog, furi_string_get_cstr(app->dialog_header), 64, 12, AlignCenter, AlignTop);

        furi_string_set_strn(
            app->dialog_text, furi_string_get_cstr(app->state->r_code), MAX_DISPLAY_BYTES);
        if(furi_string_size(app->dialog_text) >= MAX_DISPLAY_BYTES)
            furi_string_cat(app->dialog_text, "...");
        dialog_ex_set_text(
            app->dialog, furi_string_get_cstr(app->dialog_text), 10, 24, AlignLeft, AlignTop);

        dialog_ex_set_left_button_text(app->dialog, "Save Top");
        dialog_ex_set_right_button_text(app->dialog, "Save Bot");
    } else {
        dialog_ex_set_header(app->dialog, "Waiting For Data", 64, 12, AlignCenter, AlignTop);
        dialog_ex_set_text(
            app->dialog, "Connect to device bus and initiate comms", 10, 24, AlignLeft, AlignTop);
        dialog_ex_set_left_button_text(app->dialog, NULL);
        dialog_ex_set_right_button_text(app->dialog, NULL);
    }

    dialog_ex_set_center_button_text(
        app->dialog, NULL); // This will eventually be a "resend" button
    dialog_ex_set_result_callback(app->dialog, read_code_dialog_callback);
    dialog_ex_set_context(app->dialog, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, FcomReadCodeView);

    // Initialize everything for capture
    curcode = 0;
    altcode = 's';
    first = true;
    app->state->codeLen = -1; // Set to negative until we know how long the codes are
    app->state->rpackets = 0;
    app->state->spackets = 0;
    app->state->waitForCode = !hasData;
    set_serial_callback(read_code_cb);

    if(hasData)
        dmcomm_sendcommand(app, "0\n");
    else {
        dmcomm_sendcommand(app, app->state->current_code);
        //dmcomm_sendcommand(app, "\n");  \n is included from listen menu
    }
}

bool fcom_read_code_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_read_code_scene_on_event");
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case SerialInputAvailable:
            processReadInput(app);
            consumed = true;
            break;
        }
        break;
    default: // eg. SceneManagerEventTypeBack, SceneManagerEventTypeTick
        consumed = false;
        break;
    }
    return consumed;
}

void fcom_read_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_read_code_scene_on_exit");
    UNUSED(context);
    App* app = context;
    UNUSED(app);

    dialog_ex_reset(app->dialog);
    set_serial_callback(NULL);
    dmcomm_sendcommand(app, "0\n");
    app->state->waitForCode = false;
}
