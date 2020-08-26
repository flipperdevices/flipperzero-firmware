#define _GNU_SOURCE

#include <stdio.h>
#include "log.h"
#include "furi.h"

ssize_t cookie_write(void* cookie, const char * buffer, size_t size) {
    if (buffer == 0) {
        /*
         * This means that we should flush internal buffers.  Since we
         * don't we just return.  (Remember, "handle" == -1 means that all
         * handles should be flushed.)
         */
        return 0;
    }
    
    if(furi_write((FuriRecordSubscriber*)cookie, buffer, size)) {
        return size;
    }

    return 0;
}

FILE* get_log(FuriRecordSubscriber* record) {
    FILE* fp = fopencookie((void*)record, "w+", (cookie_io_functions_t){
        .read  = NULL,
        .write = cookie_write,
        .seek  = NULL,
        .close = NULL
    });

    setvbuf(fp, NULL, _IONBF, 0);

    return fp;
}

FILE* get_default_log() {
    return get_log(furi_open("tty", false, false, NULL, NULL));
}