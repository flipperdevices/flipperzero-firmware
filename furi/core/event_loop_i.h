#pragma once

#include "event_loop.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*FuriEventLoopBaseGetLevel)(FuriEventLoopBase* object, FuriEventLoopEvent event);

typedef struct {
    const FuriEventLoopBaseGetLevel get_level;
} FuriEventLoopInterface;

typedef struct FuriEventLoopItem FuriEventLoopItem;

struct FuriEventLoopBase {
    const FuriEventLoopInterface* vtable;
    FuriEventLoopItem* item_in;
    FuriEventLoopItem* item_out;
};

void furi_event_loop_base_notify(FuriEventLoopBase* base, FuriEventLoopEvent event);

#ifdef __cplusplus
}
#endif
