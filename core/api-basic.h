#pragma once

#include "flipper.h"

/*
== Flipper universal registry implementation (FURI) ==

## Requirements

* start daemon app
* kill app
* start child thread (kill when parent app was killed)
* switch between UI apps
*/

/*
Create record.
creates new record in registry and store pointer into it
*/
bool furi_create(const char* name, void* ptr);

/*
Open record.
get stored pointer by its name
*/
void* furi_open(const char* name);

// == Flipper Application control (flapp) ==

typedef FlappHandler uint32_t; // TODO

/*
simply starts application. It call `app` entrypoint with `param` passed as argument
Useful for daemon applications and pop-up.
*/
FlappHandler* flapp_start(void(app*)(void*), char* name, void* param);

/*
swtich to other application.
System **stop current app**, call `app` entrypoint with `param` passed
as argument and save current application entrypoint to `prev` field in
current application registry. Useful for UI or "active" application.
*/
FlappHandler* flapp_switch(void(app*)(void*), char* name, void* param);

/*
Exit application
stop current application (stop thread and clear application's stack),
start application from `prev` entry in current application registry,
cleanup current application registry.
*/
void flapp_exit(void* param);

/*
stop specified `app` without returning to `prev` application.
*/
bool flapp_kill(FlappHandler* app);

/*
If case one app depend on other, notify that app is ready.
*/
void flapp_ready();

/*
Register on-exit callback.
It called before app will be killed.
Not recommended to use in user scenario, only for system purpose
(unregister callbacks, release mutexes, etc.)
*/
bool flapp_on_exit(void(cb*)(void*), void* ctx);

/*
== ValueMutex ==

The most simple concept is ValueMutex.
It is wrapper around mutex and value pointer.
You can take and give mutex to work with value and read and write value.
*/

typedef struct {
    void* value;
    size_t size;
    osMutex mutex;
} ValueMutex;

/*
Creates ValueMutex.
*/
bool init_mutex(ValueMutex* valuemutex, void* value, size_t size);

/*
Call for work with data stored in mutex.
Returns pointer to data if success, NULL otherwise.
*/
void* acquire_mutex(ValueMutex* valuemutex, uint32_t timeout);

/*
Helper: infinitly wait for mutex
*/
static inline void* acquire_mutex_block(ValueMutex* valuemutex) {
    return acquire_mutex(valuemutex, OsWaitForever);
}

/*
Release mutex after end of work with data.
Call `release_mutex` and pass ValueData instance and pointer to data.
*/
bool release_mutex(ValueMutex* valuemutex, void* value);

/*
Instead of take-access-give sequence you can use `read_mutex` and `write_mutex` functions.
Both functions return true in case of success, false otherwise.
*/
bool read_mutex(ValueMutex* valuemutex, void* data, size_t len, uint32_t timeout);

bool write_mutex(ValueMutex* valuemutex, void* data, size_t len, uint32_t timeout);

inline static bool write_mutex_block(ValueMutex* valuemutex, void* data, size_t len) {
    return write_mutex(valuemutex, data, len, OsWaitForever);
}

inline static bool read_mutex_block(ValueMutex* valuemutex, void* data, size_t len) {
    return read_mutex(valuemutex, data, len, OsWaitForever);
}

/*

Usage example

```C
// MANIFEST
// name="example-provider-app"
// stack=128

void provider_app(void* _p) {
    // create record with mutex
    uint32_t example_value = 0;
    ValueMutex example_mutex;
    // call `init_mutex`.
    if(!init_mutex(&example_mutex, (void*)&example_value, sizeof(uint32_t))) {
        printf("critical error\n");
        flapp_exit(NULL);
    }

    if(furi_create("provider/example", (void*)&example_mutex)) {
        printf("critical error\n");
        flapp_exit(NULL);
    }

    // we are ready to provide record to other apps
    flapp_ready();

    // get value and increment it
    while(1) {
        uint32_t* value = acquire_mutex(&example_mutex, OsWaitForever);
        if(value != NULL) {
            value++;
        }
        release_mutex(&example_mutex, value);

        osDelay(100);
    }
}

// MANIFEST
// name="example-consumer-app"
// stack=128
// require="example-provider-app"
void consumer_app(void* _p) {
    // this app run after flapp_ready call in all requirements app

    // open mutex value
    ValueMutex* counter_mutex = furi_open("provider/example");
    if(counter_mutex == NULL) {
        printf("critical error\n");
        flapp_exit(NULL);
    }

    // continously read value every 1s
    uint32_t counter;
    while(1) {
        if(read_mutex(counter_mutex, &counter, sizeof(counter), OsWaitForever)) {
            printf("counter value: %d\n", counter);
        }

        osDelay(1000);
    }
}
```
*/


# PubSub

PubSub allows users to subscribe on notifies and notify subscribers. Notifier side can pass `void*` arg to subscriber callback, and also subscriber can set `void*` context pointer that pass into callback (you can see callback signature below).

```C
typedef void(PubSubCallback*)(void*, void*);

typedef struct {
    PubSubCallback cb;
    void* ctx;
} PubSubItem;

typedef struct {
    PubSub* self;
    PubSubItem* item;
} PubSubId;

typedef struct {
    PubSubItem items[NUM_OF_CALLBACKS];
    PubSubId ids[NUM_OF_CALLBACKS]; ///< permanent links to item
    size_t count; ///< count of callbacks
} PubSub;
```

To create PubSub you should create PubSub instance and call `init_pubsub`.

```C
void init_pubsub(PubSub* pubsub) {
    pubsub->count = 0;

    for(size_t i = 0; i < NUM_OF_CALLBACKS; i++) {
        pubsub->items[i].
    }
}
```

Use `subscribe_pubsub` to register your callback.

```C
// TODO add mutex to reconfigurate PubSub
PubSubId* subscribe_pubsub(PubSub* pubsub, PubSubCallback cb, void* ctx) {
    if(pubsub->count >= NUM_OF_CALLBACKS) return NULL;

    pubsub->count++;
    PubSubItem* current = pubsub->items[pubsub->count];
    
    current->cb = cb;
    currrnt->ctx = ctx;

    pubsub->ids[pubsub->count].self = pubsub;
    pubsub->ids[pubsub->count].item = current;

    flapp_on_exit(unsubscribe_pubsub, &(pubsub->ids[pubsub->count]));
    
    return current;
}
```

Use `unsubscribe_pubsub` to unregister callback.

```C
void unsubscribe_pubsub(PubSubId* pubsub_id) {
    // TODO: add, and rearrange all items to keep subscribers item continuous
    // TODO: keep ids link actual
    // TODO: also add mutex on every pubsub changes

    // trivial implementation for NUM_OF_CALLBACKS = 1
    if(NUM_OF_CALLBACKS != 1) return;

    if(pubsub_id != NULL || pubsub_id->self != NULL || pubsub_id->item != NULL) return;

    pubsub_id->self->count = 0;
    pubsub_id->item = NULL;
}

```

Use `notify_pubsub` to notify subscribers.

```C
void notify_pubsub(PubSub* pubsub, void* arg) {
    // iterate over subscribers
    for(size_t i = 0; i < pubsub->count; i++) {
        pubsub->items[i]->cb(arg, pubsub->items[i]->ctx);
    }
}
```

## Usage example

```C
/*
MANIFEST
name="test"
stack=128
*/

void example_pubsub_handler(void* arg, void* ctx) {
    printf("get %d from %s\n", *(uint32_t*)arg, (const char*)ctx);
}

void pubsub_test() {
    const char* app_name = "test app";

    PubSub example_pubsub;
    init_pubsub(&example_pubsub);

    if(!subscribe_pubsub(&example_pubsub, example_pubsub_handler, (void*)app_name)) {
        printf("critical error\n");
        flapp_exit(NULL);
    }

    uint32_t counter = 0;
    while(1) {
        notify_pubsub(&example_pubsub, (void*)&counter);
        counter++;

        osDelay(100);
    }
}
```

# ValueComposer

```C
typedef void(ValueComposerCallback)(void* ctx, void* state);

void COPY_COMPOSE(void* ctx, void* state) {
    read_mutex((ValueMutex*)ctx, state, 0);
}

typedef enum {
    UiLayerBelowNotify
    UiLayerNotify,
    UiLayerAboveNotify
} UiLayer;
```

```C
ValueComposerHandle* add_compose_layer(
    ValueComposer* composer, ValueComposerCallback cb, void* ctx, uint32_t layer
);
```

```C
bool remove_compose_layer(ValueComposerHandle* handle);
```

```C
void request_compose(ValueComposerHandle* handle);
```

See [LED](LED-API) or [Display](Display-API) API for examples.

# ValueManager

More complicated concept is ValueManager. It is like ValueMutex, but user can subscribe to value updates.

```C
typedef struct {
    ValueMutex value;
    PubSub pubsub;
} ValueManager;
```

First of all you can use value and pubsub part as showing above: aquire/release mutex, read value, subscribe/unsubscribe pubsub. There are two specific methods for ValueManager:

`write_managed` acquire value, changes it and send notify with current value.

```C
bool write_managed(ValueManager* managed, void* data, size_t len, uint32_t timeout) {
    void* value = acquire_mutex(managed->mutex, timeout);
    if(value == NULL) return false;

    memcpy(value, data, len):

    notify_pubsub(&managed->pubsub, value);

    if(!release_mutex(managed->mutex, value)) return false;
    
    return true;
}
```

`commit_managed` works as `release_mutex` but send notify with current value.

```C
bool commit_managed(ValueManager* managed, void* value) {
    if(value != managed->mutex->value) return false;

    notify_pubsub(&managed->pubsub, value);
    
    if(!osMutexGive(managed->mutex)) return false;
    
    return true;
}
```
