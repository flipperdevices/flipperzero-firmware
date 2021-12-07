#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>

typedef struct U2FHid U2FHid;

U2FHid* u2f_hid_start();

void u2f_hid_stop(U2FHid* u2f_hid);

#ifdef __cplusplus
}
#endif
