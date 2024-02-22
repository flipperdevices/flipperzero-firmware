#pragma once

#include "dmcomm/hal.h"
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
/*
C functions that we can call from C code to
hook into the CPP code.
*/
EXTERNC void set_serial_callback(DmcommCallback cb);
EXTERNC void setup_fcom(void* context);
EXTERNC void loop_fcom();
EXTERNC void destroy_fcom();

#undef EXTERNC