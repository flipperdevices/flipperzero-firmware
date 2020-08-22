#include "furi.h"
#include "cmsis_os.h"
#include <string.h>

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

#define MAX_RECORD_COUNT 32

static FuriRecord records[MAX_RECORD_COUNT];
static size_t current_buffer_idx = 0;

// find record pointer by name
static FuriRecord* find_record(const char* name) {
    FuriRecord* res = NULL;
    for(size_t i = 0; i < MAX_RECORD_COUNT; i++) {
        if(strcmp(records[i].name, name) == 0) {
            res = &records[i];
        }
    }

    return res;
}

bool furi_create(const char* name, void* value) {
    if(current_buffer_idx >= MAX_TASK_COUNT) {
        // max record count exceed
        #ifdef DEBUG
            printf("[FURI] max record count exceed\n");
        #endif
        return NULL;
    }

    records[current_buffer_idx].mute_counter = 0;
    records[current_buffer_idx].mutex = xSemaphoreCreateMutexStatic; // TODO: create mutex
    records[current_buffer_idx].value = value;
    records[current_buffer_idx].name = name;

    for(size_t i = 0; i < MAX_RECORD_SUBSCRIBERS; i++) {
        records[current_buffer_idx].subscribers[i].allocated = false;
    }
}

FuriRecordHandler* furi_open(
    const char* name,
    bool solo,
    bool no_mute,
    FlipperRecordCallback value_callback,
    FlipperRecordStateCallback state_callback
) {
    // get furi record by name
    FuriRecord* record = find_record(name);

    if(record == NULL) {
        // cannot find record
        #ifdef DEBUG
            printf("[FURI] cannot find record %s\n", name);
        #endif

        return NULL;
    }

    // allocate subscriber
    FuriRecordSubscriber* subscriber = NULL;

    for(size_t i = 0; i < MAX_RECORD_SUBSCRIBERS; i++) {
        if(!records[current_buffer_idx].subscribers[i].allocated) {
            subscriber = &records[current_buffer_idx].subscribers[i];
        }
    }

    if(subscriber == NULL) {
        // cannot add subscriber (full)
        #ifdef DEBUG
            printf("[FURI] cannot add subscriber (full)\n");
        #endif

        return NULL;
    }

    // increase mute_counter
    if(solo) {
        record->mute_counter++;
    }

    // set all parameters
    subscriber->allocated = true;
    subscriber->mute_counter = record->mute_counter;
    subscriber->no_mute = no_mute;
    subscriber->cb = value_callback;
    subscriber->state_cb = state_cb;

    // register record in application
    FuriApp* current_task = find_task(xTaskGetCurrentTaskHandle());

    current_task->records[current_task->records_count] = record;
    current_task->records_count++;

    return {.record = record, .subscriber = subscriber};
}


void furi_close(FuriRecordHandler* handler) {
    // deallocate subscriber
    handler->subscriber->handler = false;

    // set mute counter to next max value
    uint8_t max_mute_counter = 0;
    for(size_t i = 0; i < MAX_RECORD_SUBSCRIBERS; i++) {
        if(handler->record->subscribers[i].allocated) {
            if(handler->record->subscribers[i].mute_counter > max_mute_counter) {
                max_mute_counter = handler->record->subscribers[i].mute_counter;
            }
        }
    }
    handler->record->mute_counter = max_mute_counter;
}

bool furi_read(FuriRecordHandler* record, void* data, size_t size) {

}

bool furi_write(FuriRecordHandler* record, const void* data, size_t size) {

}

void* furi_take(FuriRecordHandler* record) {

}

void furi_give(FuriRecordHandler* record) {

}