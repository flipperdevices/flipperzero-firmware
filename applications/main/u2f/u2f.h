#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>

typedef struct {
    uint8_t cla;
    uint8_t ins;
    uint8_t p1;
    uint8_t p2;
    uint8_t len[];
} __attribute__((packed)) U2fApduCommand;

typedef enum {
    U2fNotifyRegister,
    U2fNotifyAuth,
    U2fNotifyAuthSuccess,
    U2fNotifyWink,
    U2fNotifyConnect,
    U2fNotifyDisconnect,
    U2fNotifyError,
} U2fNotifyEvent;

typedef struct U2fData U2fData;

typedef void (*U2fEvtCallback)(U2fNotifyEvent evt, void* context);

U2fData* u2f_alloc();

bool u2f_init(U2fData* instance);

void u2f_free(U2fData* instance);

void u2f_set_event_callback(U2fData* instance, U2fEvtCallback callback, void* context);

void u2f_confirm_user_present(U2fData* instance);

uint16_t u2f_msg_parse(U2fData* instance, const uint8_t* in_buf, uint16_t len, uint8_t* out_buf);

void u2f_wink(U2fData* instance);

void u2f_set_state(U2fData* instance, uint8_t state);

#ifdef __cplusplus
}
#endif
