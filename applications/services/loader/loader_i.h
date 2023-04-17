#pragma once
#include <furi.h>
#include <toolbox/api_lock.h>
#include "loader.h"
#include "loader_menu.h"

struct Loader {
    FuriPubSub* pubsub;
    FuriMessageQueue* queue;
    LoaderMenu* loader_menu;
    bool is_locked;
};

typedef enum {
    LoaderMessageTypeStartByName,
    LoaderMessageTypeShowMenu,
    LoaderMessageTypeMenuClosed,
    LoaderMessageTypeIsLocked,
    LoaderMessageTypeAppClosed,
} LoaderMessageType;

typedef struct {
    const char* name;
    const char* args;
} LoaderMessageStartByName;

typedef struct {
    LoaderStatus status_value;
} LoaderMessageStartResult;

typedef struct {
    bool bool_value;
} LoaderMessageStartIsLockedResult;

typedef struct {
    FuriApiLock api_lock;
    LoaderMessageType type;

    union {
        LoaderMessageStartByName start;
    };

    union {
        LoaderMessageStartResult* start_result;
        LoaderMessageStartIsLockedResult* is_locked_result;
    };
} LoaderMessage;
