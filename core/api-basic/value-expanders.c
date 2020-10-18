#include "value-expanders.h"

bool write_managed(ValueManager* managed, void* data, size_t len, uint32_t timeout) {
    void* value = acquire_mutex(&managed->value, timeout);
    if(value == NULL) return false;

    memcpy(value, data, len);

    notify_pubsub(&managed->pubsub, value);

    if(!release_mutex(&managed->value, value)) return false;

    return true;
}

bool commit_managed(ValueManager* managed, void* value) {
    if(value != managed->value.value) return false;

    notify_pubsub(&managed->pubsub, value);

    if(!release_mutex(&managed->value, value)) return false;

    return true;
}
