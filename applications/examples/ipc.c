#include "flipper.h"
#include <string.h>

#define FB_WIDTH 10
#define FB_HEIGHT 3
#define FB_SIZE (FB_WIDTH * FB_HEIGHT)

// context structure used for pass some object from app thread to callback
typedef struct {
    osSemaphoreId_t events; // queue to pass events from callback to app thread
    FuriRecordSubscriber* log; // app logger
} IpcCtx;

static void handle_fb_change(const void* fb, size_t fb_size, void* raw_ctx) {
    IpcCtx* ctx = (IpcCtx*)raw_ctx; // make right type

    fuprintf(ctx->log, "[cb] framebuffer updated\n");

    // send event to app thread
    osSemaphoreRelease(ctx->events);

    // Attention! Please, do not make blocking operation like IO and waits inside callback
    // Remember that callback execute in calling thread/context
}

static void print_fb(char* fb, FuriRecordSubscriber* log) {
    if(fb == NULL) return;
    
    /* draw framebuffer like this:
    +==========+
    |          |
    |          |
    |          |
    +==========+
    */

    char row_buffer[FB_WIDTH + 1];
    row_buffer[FB_WIDTH] = '\0';

    // FB layout is hardcoded here
    fuprintf(log, "+==========+\n");
    for(uint8_t i = 0; i < FB_HEIGHT; i++) {
        strncpy(row_buffer, &fb[FB_WIDTH * i], FB_WIDTH);
        fuprintf(log, "|%s|\n", row_buffer);
    }
    fuprintf(log, "+==========+\n");
}

void application_ipc_display(void* p) {
    // get logger
    FuriRecordSubscriber* log = get_default_log();

    // create ASCII "framebuffer"
    // FB_WIDTH x FB_HEIGHT char buffer
    char _framebuffer[FB_SIZE];

    // init framebuffer by spaces
    for(size_t i = 0; i < FB_SIZE; i++) {
        _framebuffer[i] = ' ';
    }

    // create record
    if(!furi_create("test_fb", (void*)_framebuffer, FB_SIZE)) {
        fuprintf(log, "[display] cannot create fb record\n");
        furiac_exit(NULL);
    }

    osSemaphoreAttr_t semaphore_attr = {
        .name = NULL,
        .attr_bits = 0,
        .cb_mem = NULL,
        .cb_size = 0
    };
    #ifdef configSUPPORT_STATIC_ALLOCATION
    StaticSemaphore_t event_descriptor;
    semaphore_attr.cb_mem = &event_descriptor;
    semaphore_attr.cb_size = sizeof(StaticSemaphore_t);
    #endif
    // create stack-based counting semaphore
    osSemaphoreId_t events = osSemaphoreNew(255, 0, &semaphore_attr);

    if(events == NULL) {
        fuprintf(log, "[display] cannot create event semaphore\n");
        furiac_exit(NULL);
    }

    // save log and event queue in context structure
    IpcCtx ctx = {.events = events, .log = log};

    // subscribe to record. ctx will be passed to handle_fb_change
    FuriRecordSubscriber* fb_record = furi_open(
        "test_fb", false, false, handle_fb_change, NULL, &ctx
    );

    if(fb_record == NULL) {
        fuprintf(log, "[display] cannot open fb record\n");
        furiac_exit(NULL);
    }

    #ifdef HW_DISPLAY
    // on Flipper target -- open screen

    // draw border

    #else
    // on Local target -- print "blank screen"
    {
        void* fb = furi_take(fb_record);
        print_fb((char*)fb, log);
        furi_give(fb_record);
    }
    #endif

    while(1) {
        // wait for event
        if(osSemaphoreAcquire(events, osWaitForever) == osOK) {
            fuprintf(log, "[display] get fb update\n\n");

            #ifdef HW_DISPLAY
            // on Flipper target draw the screen
            #else
            // on local target just print
            {
                void* fb = furi_take(fb_record);
                print_fb((char*)fb, log);
                furi_give(fb_record);
            }
            #endif
        }
    }
}


// Widget application
void application_ipc_widget(void* p) {
    FuriRecordSubscriber* log = get_default_log();

    // open record
    FuriRecordSubscriber* fb_record = furi_open(
        "test_fb", false, false, NULL, NULL, NULL
    );

    if(fb_record == NULL) {
        fuprintf(log, "[widget] cannot create fb record\n");
        furiac_exit(NULL);
    }

    uint8_t counter = 0;

    while(1) {
        delay(120);

        // write some ascii demo here: '#'' symbol run on overall screen
        char* fb = (char*)furi_take(fb_record);

        if(fb == NULL) furiac_exit(NULL);

        for(size_t i = 0; i < FB_SIZE; i++) {
            fb[i] = ' ';
        }

        fb[counter % FB_SIZE] = '#';

        furi_commit(fb_record);

        counter++;
    }
}
