/*
fcom_thread initializes the dmcomm library, and then executes the loop
waiting on the app to set the dmcomm_run var to false. After which it
deallocates everything and shuts down. We run this basically in the
background of the application the entire time, whether in use or not.

Flipper scenes that send codes typically will send the code to
enter send mode when the scene is entered, and then send 0 when
the user leaves the scene to shut the dmcomm code back off.

send command is typically used for in-app commands and
send data is used for forwarding USB Serial data
*/

#ifndef DMCOMM_LINK_HEADERS
#define DMCOMM_LINK_HEADERS

#include "flipper.h"
#include "app_state.h"

int32_t fcom_thread(void* context);
void dmcomm_sendcommand(void* context, const char* cmd);
void dmcomm_senddata(void* context, uint8_t* data, size_t len);

#endif