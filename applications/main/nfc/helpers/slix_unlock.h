#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SLIX_PASSWORD_LEN (4)

typedef enum {
    SlixUnlockMethodManual,
    SlixUnlockMethodTonieBox,
} SlixUnlockMethod;

typedef struct {
    SlixUnlockMethod method;
    uint8_t password[SLIX_PASSWORD_LEN];
} SlixUnlock;

SlixUnlock* slix_unlock_alloc();

void slix_unlock_free(SlixUnlock* instance);

void slix_unlock_reset(SlixUnlock* instance);

#ifdef __cplusplus
}
#endif
