#ifndef DMCOMM_LINK_HEADERS
#define DMCOMM_LINK_HEADERS

#include "flipper.h"
#include "app_state.h"
#include "dmcomm/dmcomm.h"

int32_t dmcomm_reader(void* context);
void dmcomm_sendcommand(void* context, const char* cmd);

#endif