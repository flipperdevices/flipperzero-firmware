#pragma once

#include "flipper.h"
#include "valuemutex.h"
#include "pubsub.h"
#include "m-list.h"

/*
== Value composer ==
*/

typedef struct ValueComposer ValueComposer;

typedef void(*ValueComposerCallback)(void* ctx, void* state);

typedef enum { UiLayerBelowNotify, UiLayerNotify, UiLayerAboveNotify } UiLayer;

typedef struct {
    ValueComposerCallback cb;
    void* ctx;
    UiLayer layer;
    ValueComposer* composer;
} ValueComposerHandle;

LIST_DEF(list_composer_cb, ValueComposerHandle, M_POD_OPLIST);

struct ValueComposer {
    ValueMutex value;
    list_composer_cb_t layers[3];
    osMutexId_t mutex;
};

void COPY_COMPOSE(void* ctx, void* state) {
    read_mutex((ValueMutex*)ctx, state, 0, osWaitForever);
}

bool init_composer(ValueComposer* composer, void* value);

ValueComposerHandle*
add_compose_layer(ValueComposer* composer, ValueComposerCallback cb, void* ctx, UiLayer layer);

bool remove_compose_layer(ValueComposerHandle* handle);

void request_compose(ValueComposerHandle* handle);

// See [LED](LED-API) or [Display](Display-API) API for examples.

/*
== ValueManager ==

More complicated concept is ValueManager.
It is like ValueMutex, but user can subscribe to value updates.

First of all you can use value and pubsub part as showing above:
aquire/release mutex, read value, subscribe/unsubscribe pubsub.
There are two specific methods for ValueManager: write_managed, commit_managed
*/

typedef struct {
    ValueMutex value;
    PubSub pubsub;
} ValueManager;

/*
acquire value, changes it and send notify with current value.
*/
bool write_managed(ValueManager* managed, void* data, size_t len, uint32_t timeout);

/*
commit_managed works as `release_mutex` but send notify with current value.
*/
bool commit_managed(ValueManager* managed, void* value);
