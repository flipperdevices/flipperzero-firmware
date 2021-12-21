#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>

typedef enum {
    U2fNotifyRegister,
    U2fNotifyAuth,
    U2fNotifyWink,
} U2fNotifyEvent;

typedef struct U2fData U2fData;

typedef void (*U2fEvtCallback)(U2fNotifyEvent evt, void* context);

bool u2f_init(U2fData** U2F_inst);

void u2f_free(U2fData* U2F);

void u2f_set_event_callback(U2fData* U2F, U2fEvtCallback callback, void* context);

void u2f_confirm_user_present(U2fData* U2F);

uint16_t u2f_msg_parse(U2fData* U2F, uint8_t* buf, uint16_t len);

void u2f_wink(U2fData* U2F);

#ifdef __cplusplus
}
#endif
