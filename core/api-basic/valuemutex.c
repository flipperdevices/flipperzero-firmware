#include "valuemutex.h"

bool init_mutex(ValueMutex* valuemutex, void* value, size_t size) {
    valuemutex->mutex = osMutexCreate();
    if(valuemutex->mutex == NULL) return false;
    
    valuemutex->value = value;
    valuemutex->size = size;
    
    return true;
}

void* acquire_mutex(ValueMutex* valuemutex, uint32_t timeout) {
    if(osMutexTake(valuemutex->mutex, timeout) == osOk) {
        return valuemutex->value;
    } else {
        return NULL;
    }
}

bool release_mutex(ValueMutex* valuemutex, void* value) {
    if(value != valuemutex->value) return false;
    
    if(!osMutexGive(valuemutex->mutex)) return false;
    
    return true;
}

bool read_mutex(ValueMutex* valuemutex, void* data, size_t len, uint32_t timeout) {
    void* value = acquire_mutex(valuemutex, timeout);
    if(value == NULL || len > valuemutex->size) return false;
    memcpy(data, value, len > 0 ? len : valuemutex->size):
    if(!release_mutex(valuemutex, value)) return false;
    
    return true;
}

bool write_mutex(ValueMutex* valuemutex, void* data, size_t len, uint32_t timeout) {
    void* value = acquire_mutex(valuemutex, timeout);
    if(value == NULL || len > valuemutex->size) return false;
    memcpy(value, data, len > 0 ? len : valuemutex->size):
    if(!release_mutex(valuemutex, value)) return false;
    
    return true;
}