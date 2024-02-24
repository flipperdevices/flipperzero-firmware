/*
Sends whatever code is in current_code to the dmcomm
Then listens for the response code from the other device.

When it receives that code, it gives you the option to save it.

Currently will not persist the recieved code after saving.
If a code is captured, we still send the digirom repeatedly,
but don't capture followup codes... So in this way it's kind
of like a punching bag mode as well.
*/
#ifndef SCENE_SEND_CODE_HEADERS
#define SCENE_SEND_CODE_HEADERS

#include "flipper.h"

typedef enum {
    SerialInputAvailable,
} SendCodeEvent;

void fcom_send_code_scene_on_enter(void* context);
bool fcom_send_code_scene_on_event(void* context, SceneManagerEvent event);
void fcom_send_code_scene_on_exit(void* context);

#endif