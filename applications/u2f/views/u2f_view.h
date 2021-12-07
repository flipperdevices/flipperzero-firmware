#pragma once

#include <gui/view.h>

typedef struct U2F U2F;
typedef void (*U2FOkCallback)(InputType type, void* context);

U2F* u2f_alloc();

void u2f_free(U2F* u2f);

View* u2f_get_view(U2F* u2f);

void u2f_set_ok_callback(U2F* u2f, U2FOkCallback callback, void* context);

void u2f_set_state(U2F* u2f); //, U2FState* st);
