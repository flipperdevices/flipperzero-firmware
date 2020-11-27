#include "furi-new.h"

osMutexId_t furi_record_mutex;
list_fury_record_t furi_registry;

FuriRecordItem* find_record_by_name(const char* name) {
    // iterate over items
    list_fury_record_it_t it;
    for(list_fury_record_it(it, furi_registry); !list_fury_record_end_p(it);
        list_fury_record_next(it)) {
        FuriRecordItem* item = list_fury_record_ref(it);

        // if the iterator is equal to our element
        if(strcmp(item->name, name) == 0) {
            return item;
        }
    }

    return NULL;
}

FuriRecordItem* find_record_by_app(FlipperApplication app) {
    // iterate over items
    list_fury_record_it_t it;
    for(list_fury_record_it(it, furi_registry); !list_fury_record_end_p(it);
        list_fury_record_next(it)) {
        FuriRecordItem* item = list_fury_record_ref(it);

        // if the iterator is equal to our element
        if(item->app == app) {
            return item;
        }
    }

    return NULL;
}

bool new_furi_init() {
    // create mutex
    furi_record_mutex = osMutexNew(NULL);
    if(furi_record_mutex == NULL) return false;

    // construct registry list
    list_fury_record_init(furi_registry);
}

void* new_furi_open_internal(const char* name) {
    FuriRecordItem* item = NULL;
    if(osMutexAcquire(furi_record_mutex, osWaitForever) == osOK) {
        FuriRecordItem* item = find_record_by_name(name);
        if(item) {
            if(item->app != osThreadGetId()) {
                list_app_push_move(item->open_by_apps, osThreadGetId());
            }
        }
        osMutexRelease(furi_record_mutex);
    }
    return item;
}

void* new_furi_open(const char* name, uint32_t timeout) 
{
    FuriRecordItem* item = NULL;
}

void* new_furi_open_block(const char* name) 
{
    FuriRecordItem* item = NULL;

    while(item == NULL){
        item = new_furi_open_internal(name);
        delay(100);
    }
    return item;
}

bool new_furi_create(const char* name, void* ptr) {
    if(osMutexAcquire(furi_record_mutex, osWaitForever) == osOK) {
        // put uninitialized item to the list
        FuriRecordItem* item = list_fury_record_push_raw(furi_registry);

        // initialize item
        item->app = osThreadGetId();
        item->payload = ptr;
        item->name = strdup(name);

        // construct list that hold who open that record
        list_app_init(item->open_by_apps);

        osMutexRelease(furi_record_mutex);

        return true;
    }
    return false;
}

bool furi_new_delete(const char* name) {
}