#include "furi-new.h"
#include "api-basic/check.h"

osMutexId_t furi_core_mutex;
osSemaphoreId_t furi_new_record_available;
list_furi_record_t furi_record_list;
list_furi_app_t furi_app_list;

FuriRecordItem* find_record_by_name(const char* name) {
    // iterate over items
    list_furi_record_it_t it;
    for(list_furi_record_it(it, furi_record_list); !list_furi_record_end_p(it);
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
    for(list_furi_record_it(it, furi_record_list); !list_furi_record_end_p(it);
        list_furi_record_next(it)) {
        FuriRecordItem* item = list_furi_record_ref(it);

        // if the iterator is equal to our element
        if(strcmp(item->name, name) == 0) {
            // destruct record
            free(item->name);
            list_app_clear(item->open_by_apps);

            // and remove from list
            list_furi_record_remove(furi_record_list, it);
            return true;
        }
    }

    return false;
}

FuriRecordItem* find_record_by_app_id(FuriAppId app_id) {
    // iterate over items
    list_furi_record_it_t it;
    for(list_furi_record_it(it, furi_record_list); !list_furi_record_end_p(it);
        list_furi_record_next(it)) {
        FuriRecordItem* item = list_furi_record_ref(it);

        // if the iterator is equal to our element
        if(item->app_id == app_id) {
            return item;
        }
    }

    return NULL;
}

bool remove_record_by_app_id(FuriAppId app_id) {
    // iterate over items
    list_furi_record_it_t it;
    for(list_furi_record_it(it, furi_record_list); !list_furi_record_end_p(it);
        list_furi_record_next(it)) {
        FuriRecordItem* item = list_furi_record_ref(it);

        // if the iterator is equal to our element
        if(item->app_id == app_id) {
            list_furi_record_remove(furi_record_list, it);
            return true;
        }
    }

    return false;
}

bool remove_app_from_list(list_app_t app_list, FuriAppId app_id) {
    list_app_it_t it;
    bool removed = false;

    // iterate over items
    for(list_app_it(it, app_list); !list_app_end_p(it); list_app_next(it)) {
        FuriAppId item = *list_app_ref(it);

        // if the iterator is equal to our element
        if(item == app_id) {
            list_app_remove(app_list, it);
            removed = true;
        }
    }

    return removed;
}

void* new_furi_open_internal(const char* name) {
    FuriRecordItem* item = NULL;
    if(osMutexAcquire(furi_core_mutex, osWaitForever) == osOK) {
        item = find_record_by_name(name);
        if(item) {
            FuriAppId app_id = flapp_current_app_id();

            // look, it is app?
            FuriAppItem* app = find_app_by_app_id(app_id);

            if(app == NULL) {
                // if it is not a app, it should be thread
                app = find_app_by_thread_id(app_id);
            }

            // cannot be, should be asserted
            furi_check(app);

            if(item->app_id != app->app_id) {
                list_app_push_back(item->open_by_apps, app_id);
            }
        }
        osMutexRelease(furi_core_mutex);
    }
    return item;
}

// init core
bool new_furi_init(void) {
    // create mutex
    const osMutexAttr_t furi_core_mutex_attr = {NULL, osMutexRecursive, NULL, 0U};
    furi_core_mutex = osMutexNew(&furi_core_mutex_attr);
    if(furi_core_mutex == NULL) return false;

    furi_new_record_available = osSemaphoreNew(1, 0, NULL);

    // construct registry list
    list_furi_record_init(furi_record_list);
    list_furi_app_init(furi_app_list);
}

// open record with timeout
void* new_furi_open(const char* name, uint32_t timeout) {
    FuriRecordItem* item = NULL;
    TickType_t xTicksToWait = timeout;
    TimeOut_t xTimeOut;

    // that function use FreeRTOS timeouts
    // record time
    vTaskSetTimeOutState(&xTimeOut);

    while(true) {
        item = new_furi_open_internal(name);
        if(item != NULL) break;

        // check for timeout
        if(xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) != pdFALSE) {
            break;
        }

        // TODO: check signal that new record available
        osDelay(10);
    }

    return item != NULL ? item->payload : NULL;
}

// block wait until open
void* new_furi_open_block(const char* name) {
    return new_furi_open(name, osWaitForever);
}

// close record
bool new_furi_close(const char* name) {
    FuriRecordItem* item = NULL;
    bool result = false;

    if(osMutexAcquire(furi_core_mutex, osWaitForever) == osOK) {
        item = find_record_by_name(name);
        if(item) {
            result = remove_app_from_list(item->open_by_apps, flapp_current_app_id());
        }
        osMutexRelease(furi_core_mutex);
    }

    return result;
}

// create record
bool new_furi_create(const char* name, void* ptr) {
    if(osMutexAcquire(furi_core_mutex, osWaitForever) == osOK) {
        // put uninitialized item to the list
        FuriRecordItem* item = list_furi_record_push_raw(furi_record_list);
        FuriAppId app_id = flapp_current_app_id();

        // look, it is app?
        FuriAppItem* app = find_app_by_app_id(app_id);

        if(app == NULL) {
            // if it is not a app, it should be thread
            app = find_app_by_thread_id(app_id);
        }

        // cannot be, should be asserted
        furi_check(app);

        // initialize item
        item->app_id = app->app_id;
        item->payload = ptr;
        item->name = strdup(name);

        // construct list that hold apps, who open that record
        list_app_init(item->open_by_apps);

        osMutexRelease(furi_core_mutex);

        // TODO: signal that new record available
        // in case if anybody wait for record

        return true;
    }
    return false;
}

// delete record
bool furi_new_delete(const char* name) {
    if(osMutexAcquire(furi_core_mutex, osWaitForever) == osOK) {
        FuriRecordItem* item = find_record_by_name(name);

        if(item) {
            // close apps that use this record
            list_app_it_t it;
            for(list_app_it(it, item->open_by_apps); !list_app_end_p(it); list_app_next(it)) {
                FuriAppId app_id = list_app_ref(it);

                // TODO: close app
                new_flapp_app_stop(item->app_id);
            }

            // remove record
            remove_record_by_name(name);
            osMutexRelease(furi_core_mutex);
            return true;
        } else {
            return false;
        }
    }
    return false;
}

/* flappity-flapp */
FuriAppItem* find_app_by_app_id(FuriAppId app_id) {
    // iterate over items
    list_furi_app_it_t it;
    for(list_furi_app_it(it, furi_app_list); !list_furi_app_end_p(it); list_furi_app_next(it)) {
        FuriAppItem* item = list_furi_app_ref(it);

        // if the iterator is equal to our element
        if(item->app_id == app_id) {
            return item;
        }
    }

    return NULL;
}

bool remove_app_by_app_id(FuriAppId app_id) {
    // iterate over items
    list_furi_app_it_t it;
    bool result = false;

    for(list_furi_app_it(it, furi_app_list); !list_furi_app_end_p(it); list_furi_app_next(it)) {
        FuriAppItem* application = list_furi_app_ref(it);

        // if the iterator is equal to our element
        if(application->app_id == app_id) {
            // destruct application data
            free(application->name);
            list_app_clear(application->threads);
            list_furi_exit_callback_clear(application->exit_callbacks);

            // remove app
            list_furi_app_remove(furi_app_list, it);

            result = true;
        }
    }

    return result;
}

FuriAppItem* find_app_by_thread_id(FuriAppId thread_id) {
    list_furi_app_it_t it;
    for(list_furi_app_it(it, furi_app_list); !list_furi_app_end_p(it); list_furi_app_next(it)) {
        FuriAppItem* application = list_furi_app_ref(it);

        list_app_it_t it;
        for(list_app_it(it, application->threads); !list_app_end_p(it); list_app_next(it)) {
            FuriAppId app_thread_id = *list_app_ref(it);

            if(app_thread_id == thread_id) {
                return application->app_id;
            }
        }
    }

    return NULL;
}

FuriAppId flapp_current_app_id(void) {
    return osThreadGetId();
}

bool new_flapp_on_exit(FuriOnExitCallback callback, void* context) {
    bool result = false;

    if(osMutexAcquire(furi_core_mutex, osWaitForever) == osOK) {
        FuriAppItem* application = find_app_by_app_id(flapp_current_app_id());

        if(application != NULL) {
            FuriExitCbData* item = list_furi_exit_callback_push_raw(application->exit_callbacks);

            item->callback = callback;
            item->context = context;

            result = true;
        }

        osMutexRelease(furi_core_mutex);
    }

    return result;
}

FuriAppId new_flapp_app_start(FuriAppFn app, char* name, uint32_t stack_size, void* param) {
    osThreadId_t app_id = NULL;

    if(osMutexAcquire(furi_core_mutex, osWaitForever) == osOK) {
        osThreadAttr_t app_attr = {.name = name, .stack_size = stack_size};
        app_id = osThreadNew(app, param, &app_attr);

        if(app_id != NULL) {
            // put uninitialized item to the list
            FuriAppItem* item = list_furi_app_push_raw(furi_app_list);

            // initialize item
            item->app_id = app_id;
            item->name = strdup(name);

            // construct threads list
            list_app_init(item->threads);

            // construct callbacks list
            list_furi_exit_callback_init(item->exit_callbacks);
        }

        osMutexRelease(furi_core_mutex);
    }
    return app_id;
}

bool new_flapp_app_stop(FuriAppId app_id) {
    bool result = false;
    FuriAppItem* application = NULL;

    if(osMutexAcquire(furi_core_mutex, osWaitForever) == osOK) {
        application = find_app_by_app_id(app_id);

        if(application != NULL) {
            // do callbacks
            {
                list_furi_exit_callback_it_t it;
                for(list_furi_exit_callback_it(it, application->exit_callbacks);
                    !list_furi_exit_callback_end_p(it);
                    list_furi_exit_callback_next(it)) {
                    FuriExitCbData* cb = list_furi_exit_callback_ref(it);
                    cb->callback(cb->context);

                    // remove callback
                    list_furi_exit_callback_remove(application->exit_callbacks, it);
                }
            }

            // delete all app records
            FuriRecordItem* record = find_record_by_app_id(application->app_id);

            while(record != NULL) {
                furi_new_delete(record->name);
                record = find_record_by_app_id(application->app_id);
            }

            // stop threads
            {
                list_app_it_t it;
                for(list_app_it(it, application->threads); !list_app_end_p(it);
                    list_app_next(it)) {
                    FuriAppId thread_id = *list_app_ref(it);

                    new_flapp_thread_stop(thread_id);
                }
            }

            // remove application from applications list
            remove_app_by_app_id(application->app_id);

            // we end critical core context
            osMutexRelease(furi_core_mutex);

            // stop app
            osThreadTerminate(app_id);

            result = true;
        }
    }

    return result;
}

bool new_flapp_app_exit(void) {
    return new_flapp_app_stop(flapp_current_app_id());
}

FuriAppId new_flapp_thread_start(FuriAppFn app, uint32_t stack_size, void* param) {
    osThreadId_t app_id = flapp_current_app_id();
    osThreadId_t thread_app_id = NULL;

    if(osMutexAcquire(furi_core_mutex, osWaitForever) == osOK) {
        osThreadAttr_t app_attr = {.stack_size = stack_size};
        thread_app_id = osThreadNew(app, param, &app_attr);

        if(thread_app_id) {
            FuriAppItem* item = find_app_by_app_id(app_id);
            list_app_push_back(item->threads, thread_app_id);
        }

        osMutexRelease(furi_core_mutex);
    }

    return thread_app_id;
}

bool new_flapp_thread_stop(FuriAppId thread_id) {
    bool result = false;

    if(osMutexAcquire(furi_core_mutex, osWaitForever) == osOK) {
        FuriAppItem* application = find_app_by_thread_id(thread_id);

        if(application != NULL) {
            result = remove_app_from_list(application->threads, thread_id);
        }

        // we end critical core context
        osMutexRelease(furi_core_mutex);

        if(result) {
            osThreadTerminate(thread_id);
        }
    }
    return false;
}

bool new_flapp_thread_exit(void) {
    return new_flapp_thread_stop(flapp_current_app_id());
}