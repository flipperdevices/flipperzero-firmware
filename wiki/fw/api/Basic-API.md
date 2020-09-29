# ValueMutex

The most simple concept is ValueMutex. It is wrapper around mutex and value pointer. You can take and give mutex to work with value and read and write value.

```C
typedef struct {
    void* value;
    osMutex mutex;
    
    osMutexDescriptor __static // some internals;
} ValueMutex;
```

Create ValueMutex. Create instance of ValueMutex and call `init_mutex`.

```C
bool init_mutex(ValueMutex* valuemutex, void* value) {
    valuemutex->mutex = osMutexCreateStatic(valuemutex->__static);
    
    valuemutex->value = value;
    
    return true;
}
```

For work with data stored in mutex you should call `take_mutex`. It return pointer to data if success, NULL otherwise.

You must release mutex after end of work with data. Call `give_mutex` and pass ValueData instance and pointer to data.

```C
void* take_mutex(ValueMutex* valuemutex, uint32_t timeout) {
    if(osMutexTake(valuemutex->mutex, timeout) == osOk) {
        return valuemutex->value;
    } else {
        return NULL;
    }
}

bool give_mutex(ValueMutex* valuemutex, void* value) {
    if(value != valuemutex->value) return false;
    
    if(!osMutexGive(valuemutex->mutex)) return false;
    
    return true;
}
```
Instead of take-access-give sequence you can use `read_mutex` and `write_mutex` functions. Both functions return true in case of success, false otherwise.

```C
bool read_mutex(ValueMutex* valuemutex, void* data, size_t len, uint32_t timeout) {
    void* value = take_mutex(valuemutex, timeout);
    if(value == NULL) return false;
    memcpy(data, value, len):
    if(!give_mutex(valuemutex, value)) return false;
    
    return true;
}

bool write_mutex(ValueMutex* valuemutex, void* data, size_t len, uint32_t timeout) {
    void* value = take_mutex(valuemutex, timeout);
    if(value == NULL) return false;
    memcpy(value, data, len):
    if(!give_mutex(valuemutex, value)) return false;
    
    return true;
}
```

# PubSub

PubSub allows users to subscribe on notifies and notify subscribers. Notifier side can pass `void*` arg to subscriber callback, and also subscriber can set `void*` context pointer that pass into callback (you can see callback signature below).

```C
typedef void(PubSubCallback*)(void*, void*);

typedef struct {
    PubSubCallback cb;
    void* ctx;
} PubSubItem;

typedef struct {
    PubSubItem items[NUM_OF_CALLBACKS];
    size_t count; ///< count of callbacks
} PubSub;
```

To create PubSub you should create PubSub instance and call `init_pubsub`.

```C
void init_pubsub(PubSub* pubsub) {
    pubsub->count = 0;
}
```

Use `subscribe_pubsub` to register your callback.

```C
// TODO add mutex to reconfigurate PubSub
PubSubItem* subscribe_pubsub(PubSub* pubsub, PubSubCallback cb, void* ctx) {
    if(pubsub->count >= NUM_OF_CALLBACKS) return NULL;

    pubsub->count++;
    PubSubItem* current = pubsub->items[pubsub->count];
    
    current->cb = cb;
    currrnt->ctx = ctx;
    
    return current;
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

TODO: add `unsubscribe_pubsub`, and rearrange all items to keep subscribers item continuous
TODO: after implementing "app_exit_handler" unsubscribe on exit.

# ValueManager

More complicated concept is ValueManager. It is like ValueMutex, but user can subscribe to update value.

```C
typedef struct {
    ValueMutex value;
    PubSub pubsub;
}ValueManager;
```


LayerReducer