#pragma once

#include "arduino.h"
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
/*
C functions that we can call from C code to
hook into the CPP code. dmcomm_link has functions
to send data into the dmcomm serial.
set_serial_callback can be called from any thread
to register to recieve the dmcomm's serial output.

Setting it to NULL will disable callbacks.

We still write to the stream buffer, but that is
capped at 128 bytes, after which we will just
drop data.
*/
EXTERNC void set_serial_callback(DmcommCallback cb);
EXTERNC void setup_fcom(void* context);
EXTERNC void loop_fcom();
EXTERNC void destroy_fcom();

#undef EXTERNC