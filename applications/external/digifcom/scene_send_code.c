/*
Sends the code to the DMComm and initiates sending

Displays current code we are sending in bold
Waits for and displays last recieved code in thin text
Right will go to the save code dialog for the new code (if there is one)

TODO:

Switch to a state machine like:

idle:
 r -> pending r code
 s -> pending s code
 t -> reset
 newline -> save validate
 anything else -> reset

pending code:
 : -> read code
 anything else -> reset

read code:
 A-Z,0-9 -> populate code
 space -> end code -> idle
 anything else -> reset

save validate:
 if code valid -> done
 if not valid -> reset

reset:
 clear code then -> idle
*/

#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "dmcomm_lib/fcom.h"
#include "scene_send_code.h"
#include <furi_hal_cortex.h>

#define MAX_DISPLAY_BYTES 36

// These need to persist between processInput calls as the serial data comes in at random points
static char curcode = 0;
static bool first = false;
static bool assert_next_colon = false; // Necessary for managing state

void processInput(void* context) {
    furi_assert(context);
    App* app = context;

    char out[64];
    size_t recieved = 1;

    while(recieved > 0) {
        memset(out, 0, 64);
        recieved = furi_stream_buffer_receive(app->dmcomm_output_stream, &out, 63, 5);

        if(app->state->waitForCode && recieved > 0) {
            FURI_LOG_I(TAG, "DMComm Sent Data: %d <%s>", recieved, out);
            //FURI_LOG_I(TAG, "reading code");
            int l = strlen(out);
            for(int i = 0; i < l; i++) {
                if(out[i] == 't' ||
                   (assert_next_colon && out[i] != ':')) { // reset for timeout and continue :(
                    //FURI_LOG_I(TAG, "reset codes");
                    curcode = 0;
                    first = true;
                    app->state->spackets = 0;
                    app->state->rpackets = 0;
                    furi_string_reset(app->state->r_code);
                    furi_string_reset(app->state->s_code);
                }
                if(assert_next_colon) assert_next_colon = false;
                if(out[i] == 's') { // Starts an s code block
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
                } else if(out[i] == 'r') { // Starts an r code block
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
                    curcode = 0;
                }

                // if spackets == rpackets and spackets = code packets, then present code for saving
                // and stop scanning for new codes. Perhaps shutdown dmcomm at this point?
                if(app->state->rpackets == app->state->codeLen &&
                   app->state->spackets == app->state->codeLen) {
                    FURI_LOG_I(TAG, "s code %s", furi_string_get_cstr(app->state->s_code));
                    FURI_LOG_I(TAG, "r code %s", furi_string_get_cstr(app->state->r_code));

                    if(strlen(app->state->current_code) > 2 && app->state->current_code[1] == '1')
                        furi_string_set_strn(
                            app->dialog_text,
                            furi_string_get_cstr(app->state->r_code),
                            MAX_DISPLAY_BYTES);
                    else
                        furi_string_set_strn(
                            app->dialog_text,
                            furi_string_get_cstr(app->state->s_code),
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

                    dialog_ex_set_right_button_text(app->dialog, "Save");
                    app->state->waitForCode = false;
                    break; // stop reading data
                    FURI_LOG_I(TAG, "done");
                }
            }
        }
    }
}

void scbs(void* context) {
    // This needs to be pretty short or it will delay comms
    furi_assert(context);
    App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, SerialInputAvailable);
}

void send_code_dialog_callback(DialogExResult result, void* context) {
    FURI_LOG_I(TAG, "send_code_dialog_callback");
    furi_assert(context);
    App* app = context;

    if(result == DialogExResultRight) {
        // If we are still waiting for a code, don't do anything
        if(app->state->waitForCode) return;
        // If either code is empty, then don't do anything
        if(furi_string_empty(app->state->r_code)) return;
        if(furi_string_empty(app->state->s_code)) return;

        // save code
        if(strlen(app->state->current_code) > 2 && app->state->current_code[1] == '1')
            strncpy(
                app->state->result_code,
                furi_string_get_cstr(app->state->r_code),
                MAX_DIGIROM_LEN);
        else
            strncpy(
                app->state->result_code,
                furi_string_get_cstr(app->state->s_code),
                MAX_DIGIROM_LEN);

        // Come back to this scene after code save
        app->state->save_code_return_scene = FcomSendCodeScene;
        scene_manager_next_scene(app->scene_manager, FcomSaveCodeScene);
    }
}

void fcom_send_code_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_send_code_scene_on_enter");
    App* app = context;

    // Initialize our GUI
    furi_string_set_strn(app->dialog_header, app->state->current_code, MAX_DISPLAY_BYTES);
    if(furi_string_size(app->dialog_header) >= MAX_DISPLAY_BYTES)
        furi_string_cat(app->dialog_header, "...");
    dialog_ex_set_header(
        app->dialog, furi_string_get_cstr(app->dialog_header), 64, 2, AlignCenter, AlignTop);
    dialog_ex_set_text(app->dialog, "Response Code Goes Here", 10, 24, AlignLeft, AlignTop);
    dialog_ex_set_left_button_text(app->dialog, NULL);
    dialog_ex_set_right_button_text(app->dialog, NULL);
    dialog_ex_set_center_button_text(
        app->dialog, NULL); // This will eventually be a "resend" button
    dialog_ex_set_result_callback(app->dialog, send_code_dialog_callback);
    dialog_ex_set_context(app->dialog, app);

    // Setup dmcomm to send
    curcode = 0;
    first = true;
    app->state->codeLen = 0;
    app->state->rpackets = 0;
    app->state->spackets = 0;
    for(size_t i = 0; i < strlen(app->state->current_code); i++) {
        if(app->state->current_code[i] == '-') app->state->codeLen++;
    }
    app->state->waitForCode = true;
    set_serial_callback(scbs);
    furi_string_reset(app->state->r_code);
    furi_string_reset(app->state->s_code);

    // Trigger send
    dmcomm_sendcommand(app, app->state->current_code);
    dmcomm_sendcommand(app, "\n");

    // start UI
    view_dispatcher_switch_to_view(app->view_dispatcher, FcomSendCodeView);
}

bool fcom_send_code_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_send_code_scene_on_event");
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case SerialInputAvailable:
            // DMComm sent us some data, so process it
            processInput(app);
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

void fcom_send_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_send_code_scene_on_exit");
    App* app = context;

    // Clear out dmcomm
    set_serial_callback(NULL);

    // pause dmcomm
    dmcomm_sendcommand(app, "0\n");
    app->state->waitForCode = false;
}
