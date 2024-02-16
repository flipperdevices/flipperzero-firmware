#ifndef DMCOMM_LINK_HEADERS
#define DMCOMM_LINK_HEADERS

#include "flipper.h"
#include "app_state.h"
#include "dmcomm/dmcomm.h"

int32_t dmcomm_reader(void* context);
void dmcomm_sendcommand(void* context, const char* cmd);
void dmcomm_senddata(void* context, uint8_t* data, size_t len);

#endif