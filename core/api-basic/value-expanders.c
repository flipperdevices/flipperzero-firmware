#include "value-expanders.h"

bool init_composer(ValueComposer* composer, void* value) {
    if (!init_mutex(&composer->value, value, 0)) return false;

    for (size_t i = 0; i < sizeof(composer->layers)/sizeof(composer->layers[0]); i++) {
        list_composer_cb_init(composer->layers[i]);
    }

    // mutex without name,
    // no attributes (unfortunatly robust mutex is not supported by FreeRTOS),
    // with dynamic memory allocation
    const osMutexAttr_t value_mutex_attr = {
        .name = NULL, .attr_bits = 0, .cb_mem = NULL, .cb_size = 0U};

    composer->mutex = osMutexNew(&value_mutex_attr);
    if(composer->mutex == NULL) return false;

    return true;
}

ValueComposerHandle*
add_compose_layer(ValueComposer* composer, ValueComposerCallback cb, void* ctx, UiLayer layer) {
    if(osMutexAcquire(composer->mutex, osWaitForever) == osOK) {
        // put uninitialized item to the list
        ValueComposerHandle* handle = list_composer_cb_push_raw(composer->layers[layer]);

        handle->cb = cb;
        handle->ctx = ctx;
        handle->layer = layer;
        handle->composer = composer;

        // TODO unregister handle on app exit
        //flapp_on_exit(remove_compose_layer, handle);

        osMutexRelease(composer->mutex);

        return handle;
    } else {
        return NULL;
    }
}

bool remove_compose_layer(ValueComposerHandle* handle) {
    ValueComposer* composer = handle->composer;

    if(osMutexAcquire(composer->mutex, osWaitForever) == osOK) {
        bool result = false;

        // iterate over items
        list_composer_cb_it_t it;
        for(list_composer_cb_it(it, composer->layers[handle->layer]); !list_composer_cb_end_p(it);
            list_composer_cb_next(it)) {
            const ValueComposerHandle* item = list_composer_cb_cref(it);

            // if the iterator is equal to our element
            if(item == handle) {
                list_composer_cb_remove(composer->layers[handle->layer], it);
                result = true;
                break;
            }
        }

        osMutexRelease(composer->mutex);

        return result;
    } else {
        return false;
    }
}

void request_compose(ValueComposerHandle* handle) {
    ValueComposer* composer = handle->composer;

    if(osMutexAcquire(composer->mutex, osWaitForever) == osOK) {
        void* state = acquire_mutex(&composer->value, 0);
        if (state == NULL) {
            // This should not happen
            osMutexRelease(composer->mutex);
            return;
        }

        // Compose all levels for now
        for (size_t i = 0; i < sizeof(composer->layers)/sizeof(composer->layers[0]); i++) {
            // iterate over items
            list_composer_cb_it_t it;
            for(list_composer_cb_it(it, composer->layers[handle->layer]); !list_composer_cb_end_p(it);
            list_composer_cb_next(it)) {
                const ValueComposerHandle* h = list_composer_cb_cref(it);
                h->cb(h->ctx, state);
            }
        }

        release_mutex(&composer->value, state);
        osMutexRelease(composer->mutex);
    }
}


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
