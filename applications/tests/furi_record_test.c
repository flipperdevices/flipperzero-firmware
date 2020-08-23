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
    if(!furi_create("test/pipe", NULL)) {
        fprintf(debug_uart, "cannot create record\n");
        return false;
    }

    FuriRecordHandler pipe_record = furi_open(
        "test/pipe", false, false, pipe_record_cb, NULL
    );
    if(pipe_record.record == NULL) {
        fprintf(debug_uart, "cannot open record\n");
        return false;
    }

    const uint8_t WRITE_VALUE = 1;

    if(!furi_write(&pipe_record, &WRITE_VALUE, sizeof(uint8_t))) {
        fprintf(debug_uart, "cannot write to record\n");
        return false;
    }

    if(pipe_record_value != WRITE_VALUE) {
        fprintf(debug_uart, "wrong value (get %d, write %d)\n", pipe_record_value, WRITE_VALUE);
        return false;
    }

    uint8_t read_value = 0;
    if(furi_read(&pipe_record, &read_value, sizeof(uint8_t))) {
        fprintf(debug_uart, "reading from pipe record not allowed\n");
        return false;
    }

    furi_close(&pipe_record);

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
4. Read and check data, that subscriber get data
5. Try to write/read wrong size of data
6. Create another task, do concurrent access to data with take/give
*/
bool furi_holding_data(FILE* debug_uart) {

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