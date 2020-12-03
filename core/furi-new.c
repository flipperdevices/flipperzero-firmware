#include "furi-new.h"

osMutexId_t furi_record_mutex;
osSemaphoreId_t furi_record_available_semaphore;
list_furi_record_t furi_registry;

FuriRecordItem* find_record_by_name(const char* name) {
    // iterate over items
    list_furi_record_it_t it;
    for(list_furi_record_it(it, furi_registry); !list_furi_record_end_p(it);
        list_furi_record_next(it)) {
        FuriRecordItem* item = list_furi_record_ref(it);

        // if the iterator is equal to our element
        if(strcmp(item->name, name) == 0) {
            return item;
        }
    }

    return NULL;
}

bool remove_record_by_name(const char* name) {
    // iterate over items
    list_furi_record_it_t it;
    for(list_furi_record_it(it, furi_registry); !list_furi_record_end_p(it);
        list_furi_record_next(it)) {
        FuriRecordItem* item = list_furi_record_ref(it);

        // if the iterator is equal to our element
        if(strcmp(item->name, name) == 0) {
            list_furi_record_remove(furi_registry, it);
            return true;
        }
    }

    return false;
}

FuriRecordItem* find_record_by_app_id(osThreadId_t app_id) {
    // iterate over items
    list_furi_record_it_t it;
    for(list_furi_record_it(it, furi_registry); !list_furi_record_end_p(it);
        list_furi_record_next(it)) {
        FuriRecordItem* item = list_furi_record_ref(it);

        // if the iterator is equal to our element
        if(item->app_id == app_id) {
            return item;
        }
    }

    return NULL;
}

bool remove_record_by_app_id(osThreadId_t app_id) {
    // iterate over items
    list_furi_record_it_t it;
    for(list_furi_record_it(it, furi_registry); !list_furi_record_end_p(it);
        list_furi_record_next(it)) {
        FuriRecordItem* item = list_furi_record_ref(it);

        // if the iterator is equal to our element
        if(item->app_id == app_id) {
            list_furi_record_remove(furi_registry, it);
            return true;
        }
    }

    return false;
}

bool new_furi_init() {
    // create mutex
    const osMutexAttr_t furi_record_mutex_attr = {NULL, osMutexRecursive, NULL, 0U};
    furi_record_mutex = osMutexNew(furi_record_mutex_attr);
    if(furi_record_mutex == NULL) return false;

    furi_record_available_semaphore = osSemaphoreNew(1, 0, NULL);

    // construct registry list
    list_furi_record_init(furi_registry);
}

void* new_furi_open_internal(const char* name) {
    FuriRecordItem* item = NULL;
    if(osMutexAcquire(furi_record_mutex, osWaitForever) == osOK) {
        FuriRecordItem* item = find_record_by_name(name);
        if(item) {
            if(item->app_id != osThreadGetId()) {
                list_app_push_move(item->open_by_apps, osThreadGetId());
            }
        }
        osMutexRelease(furi_record_mutex);
    }
    return item;
}

void* new_furi_open(const char* name, uint32_t timeout) {
    // that fuction use FreeRTOS timeouts
    FuriRecordItem* item = NULL;
    TickType_t xTicksToWait = timeout;
    TimeOut_t xTimeOut;

    // record time
    vTaskSetTimeOutState(&xTimeOut);

    while(item == NULL) {
        osSemaphoreAcquire(furi_record_available_semaphore, timeout);
        item = new_furi_open_internal(name);
        osSemaphoreRelease(furi_record_available_semaphore);

        // check for timeout
        if(xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) != pdFALSE) {
            break;
        }
    }

    return item;
}

void* new_furi_open_block(const char* name) {
    FuriRecordItem* item = new_furi_open(name, osWaitForever);
    return item;
}

bool new_furi_create(const char* name, void* ptr) {
    if(osMutexAcquire(furi_record_mutex, osWaitForever) == osOK) {
        // put uninitialized item to the list
        FuriRecordItem* item = list_furi_record_push_raw(furi_registry);

        // initialize item
        item->app_id = osThreadGetId();
        item->payload = ptr;
        item->name = strdup(name);

        // construct list that hold apps, who open that record
        list_app_init(item->open_by_apps);

        osMutexRelease(furi_record_mutex);

        // signal that new record available
        // in case if anybody wait for record
        osSemaphoreRelease(furi_record_available_semaphore);

        return true;
    }
    return false;
}

bool furi_new_delete(const char* name) {
    if(osMutexAcquire(furi_record_mutex, osWaitForever) == osOK) {
        FuriRecordItem* item = find_record_by_name(name);

        if(item) {
            // close apps that use this record
            list_app_it_t apps_iterator;
            for(list_app_it(apps_iterator, item->open_by_apps); !list_app_end_p(apps_iterator);
                list_app_next(apps_iterator)) {
                osThreadId_t app_id = list_app_ref(apps_iterator);

                
                // TODO: close app
                // furiac_close(item->app_id);
            }

            // remove record
            remove_record_by_name(name);
            osMutexRelease(furi_record_mutex);
            return true;
        } else {
            return false;
        }
    }
    return false;
}

/* flappity-flapp */

