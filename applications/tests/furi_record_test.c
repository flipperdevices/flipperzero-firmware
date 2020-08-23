#include <stdio.h>
#include <string.h>
#include "flipper.h"
#include "debug.h"

/*
TEST: pipe record

1. create pipe record
2. Open/subscribe to it 
3. write data
4. check that subscriber get data
5. try to read, get error
6. close record
7. try to write, get error
*/

static uint8_t pipe_record_value = 0;

void pipe_record_cb(const void* value, size_t size) {
    // hold value to static var
    pipe_record_value = *((uint8_t*)value);
}

bool furi_pipe_record(FILE* debug_uart) {
    // 1. create pipe record
    if(!furi_create("test/pipe", NULL, 0)) {
        fprintf(debug_uart, "cannot create record\n");
        return false;
    }

    // 2. Open/subscribe to it 
    FuriRecordHandler pipe_record = furi_open(
        "test/pipe", false, false, pipe_record_cb, NULL
    );
    if(pipe_record.record == NULL) {
        fprintf(debug_uart, "cannot open record\n");
        return false;
    }

    const uint8_t WRITE_VALUE = 1;
    // 3. write data
    if(!furi_write(&pipe_record, &WRITE_VALUE, sizeof(uint8_t))) {
        fprintf(debug_uart, "cannot write to record\n");
        return false;
    }

    // 4. check that subscriber get data
    if(pipe_record_value != WRITE_VALUE) {
        fprintf(debug_uart, "wrong value (get %d, write %d)\n", pipe_record_value, WRITE_VALUE);
        return false;
    }

    // 5. try to read, get error
    uint8_t read_value = 0;
    if(furi_read(&pipe_record, &read_value, sizeof(uint8_t))) {
        fprintf(debug_uart, "reading from pipe record not allowed\n");
        return false;
    }

    // 6. close record
    furi_close(&pipe_record);

    // 7. try to write, get error
    if(furi_write(&pipe_record, &WRITE_VALUE, sizeof(uint8_t))) {
        fprintf(debug_uart, "writing to closed record not allowed\n");
        return false;
    }

    return true;
}

/*
TEST: holding data

1. Create holding record
2. Open/Subscribe on it
3. Write data
4. Check that subscriber get data
5. Read and check data
6. Try to write/read wrong size of data
*/

static uint8_t holding_record_value = 0;

void holding_record_cb(const void* value, size_t size) {
    // hold value to static var
    holding_record_value = *((uint8_t*)value);
}

bool furi_holding_data(FILE* debug_uart) {
    // 1. Create holding record
    uint8_t holder = 0;
    if(!furi_create("test/holding", (void*)&holder, sizeof(holder))) {
        fprintf(debug_uart, "cannot create record\n");
        return false;
    }

    // 2. Open/Subscribe on it
    FuriRecordHandler holding_record = furi_open(
        "test/holding", false, false, holding_record_cb, NULL
    );
    if(holding_record.record == NULL) {
        fprintf(debug_uart, "cannot open record\n");
        return false;
    }

    const uint8_t WRITE_VALUE = 1;
    // 3. write data
    if(!furi_write(&holding_record, &WRITE_VALUE, sizeof(uint8_t))) {
        fprintf(debug_uart, "cannot write to record\n");
        return false;
    }

    // 4. check that subscriber get data
    if(holding_record_value != WRITE_VALUE) {
        fprintf(debug_uart, "wrong sub value (get %d, write %d)\n", holding_record_value, WRITE_VALUE);
        return false;
    }

    // 5. Read and check data
    uint8_t read_value = 0;
    if(!furi_read(&holding_record, &read_value, sizeof(uint8_t))) {
        fprintf(debug_uart, "cannot read from record\n");
        return false;
    }

    if(read_value != WRITE_VALUE) {
        fprintf(debug_uart, "wrong read value (get %d, write %d)\n", read_value, WRITE_VALUE);
        return false;
    }

    // 6. Try to write/read wrong size of data
    if(furi_write(&holding_record, &WRITE_VALUE, 100)) {
        fprintf(debug_uart, "overflowed write not allowed\n");
        return false;
    }

    if(furi_read(&holding_record, &read_value, 100)) {
        fprintf(debug_uart, "overflowed read not allowed\n");
        return false;
    }

    return true;
}

/*
TEST: concurrent access

1. Create holding record
2. Open it twice
3. Change value simultaneously in two app and check integrity
*/

// TODO this test broke because mutex in furi is not implemented

typedef struct {
    // a and b must be equal
    uint8_t a;
    uint8_t b;
} ConcurrentValue;

void furi_concurent_app(void* p) {
    FILE* debug_uart = (FILE*)p;

    FuriRecordHandler holding_record = furi_open(
        "test/concurrent", false, false, NULL, NULL
    );
    if(holding_record.record == NULL) {
        fprintf(debug_uart, "cannot open record\n");
        furiac_exit(NULL);
    }

    for(size_t i = 0; i < 10; i++) {
        ConcurrentValue* value = (ConcurrentValue*)furi_take(&holding_record);

        if(value == NULL) {
            fprintf(debug_uart, "cannot take record\n");
            furiac_exit(NULL);
        }
        // emulate read-modify-write
        uint8_t a = value->a;
        uint8_t b = value->b;
        a++;
        b++;
        delay(2); // this is only for test, do not add delay between take/give in prod!
        value->a = a;
        value->b = b;
        furi_give(&holding_record);
    }

    furiac_exit(NULL);
}

bool furi_concurrent_access(FILE* debug_uart) {
    // 1. Create holding record
    ConcurrentValue holder = {.a = 0, .b = 0};
    if(!furi_create("test/concurrent", (void*)&holder, sizeof(ConcurrentValue))) {
        fprintf(debug_uart, "cannot create record\n");
        return false;
    }

    // 2. Open it
    FuriRecordHandler holding_record = furi_open(
        "test/concurrent", false, false, NULL, NULL
    );
    if(holding_record.record == NULL) {
        fprintf(debug_uart, "cannot open record\n");
        return false;
    }

    // 3. Create second app for interact with it
    FuriApp* second_app = furiac_start(
        furi_concurent_app, "furi concurent app", (void*)debug_uart
    );

    // 4. multiply ConcurrentValue::a
    for(size_t i = 0; i < 4; i++) {
        ConcurrentValue* value = (ConcurrentValue*)furi_take(&holding_record);

        if(value == NULL) {
            fprintf(debug_uart, "cannot take record\n");
            return false;
        }
        // emulate read-modify-write
        uint8_t a = value->a;
        uint8_t b = value->b;
        a++;
        b++;
        value->a = a;
        delay(10); // this is only for test, do not add delay between take/give in prod!
        value->b = b;
        furi_give(&holding_record);
    }

    delay(20);

    if(second_app->handler != NULL) {
        fprintf(debug_uart, "second app still alive\n");
        return false;
    }

    if(holder.a != holder.b) {
        fprintf(debug_uart, "broken integrity: a=%d, b=%d\n", holder.a, holder.b);
        return false;
    }

    return true;
}

/*
TEST: non-existent data
1. Try to open non-existent record
2. Check for NULL handler
3. Try to write/read, get error
*/
bool furi_nonexistent_data(FILE* debug_uart) {

    return true;
}

/*
TEST: mute algorithm
1. Create "parent" application:
    1. Create pipe record
    2. Open watch handler: no_mute=false, solo=false, subscribe to data and state.

2. Open handler A: no_mute=false, solo=false, NULL subscriber. Subscribe to state.
Try to write data to A and check subscriber.

3. Open handler B: no_mute=true, solo=true, NULL subscriber.
Check A state cb get FlipperRecordStateMute.
Try to write data to A and check that subscriber get no data. (muted)
Try to write data to B and check that subscriber get data.

4. Open hadler C: no_mute=false, solo=true, NULL subscriber.
Try to write data to A and check that subscriber get no data. (muted)
Try to write data to B and check that subscriber get data. (not muted because open with no_mute)
Try to write data to C and check that subscriber get data.

5. Open handler D: no_mute=false, solo=false, NULL subscriber.
Try to write data to A and check that subscriber get no data. (muted)
Try to write data to B and check that subscriber get data. (not muted because open with no_mute)
Try to write data to C and check that subscriber get data. (not muted because D open without solo)
Try to write data to D and check that subscriber get data.

8. Close C, close B.
Check A state cb get FlipperRecordStateUnmute
Try to write data to A and check that subscriber get data. (unmuted)
Try to write data to D and check that subscriber get data.

9. Exit "parent application"
Check A state cb get FlipperRecordStateDeleted
*/
bool furi_mute_algorithm(FILE* debug_uart) {

    return true;
}