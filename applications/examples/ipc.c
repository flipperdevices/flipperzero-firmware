#include "flipper.h"

#define FB_WIDTH 10
#define FB_HEIGHT 3
#define FB_SIZE ((FB_WIDTH + 1) * FB_HEIGHT)

// context structure used for pass some object for app thread to callback
typedef struct {
    QueueHandle_t event_queue; // queue to pass events from callback to app thread
    FuriRecordSubscriber* log; // app logger
} IpcCtx;

static void handle_fb_change(void* fb, size_t fb_size, void* raw_ctx) {
    IpcCtx* ctx = (IpcCtx*)raw_ctx; // make right type

    fuprintf(ctx->log, "[cb] framebuffer updated");

    // send event to app thread
    uint8_t token = 0;
    xQueueSend(ctx->event_queue, &token, 0);

    // Attention! Please, do not make blocking operation like IO and waits inside callback
    // Remember that callback execute in calling thread/context
}

static void print_fb(char* fb, FuriRecordSubscriber* log) {
    /* draw framebuffer like this:
    +==========+
    |          |
    |          |
    |          |
    +==========+
    */

    // FB layout is hardcoded here
    fuprintf(
        log, "+==========+\n|%s|\n|%s|\n|%s|\n+==========+",
        &fb[(FB_WIDTH + 1) * 0],
        &fb[(FB_WIDTH + 1) * 1],
        &fb[(FB_WIDTH + 1) * 2]
    );
}

void application_ipc_display(void* p) {
    // get logger
    FuriRecordSubscriber* log = get_default_log();

    // create ASCII "framebuffer"
    // FB_WIDTH x FB_HEIGHT char buffer
    // rows splitted by '\0'
    char _framebuffer[FB_SIZE];

    // init framebuffer
    for(size_t i = 0; i < FB_SIZE; i++) {
        if(i % (FB_WIDTH + 1) == FB_WIDTH) {
            // place null-terminator at end of every row
            _framebuffer[i] = '\0';
        } else {
            // fill row with spaces
            _framebuffer[i] = ' ';
        }
    }

    // create record
    if(!furi_create("test_fb", (void*)_framebuffer, FB_SIZE)) {
        fuprintf(log, "cannot create fb record\n");
        furiac_exit(NULL);
    }

    // create stack-based queue
    StaticQueue_t queue_descriptor;
    uint8_t queue_buffer[1 * sizeof(uint8_t)];
    QueueHandle_t event_queue = xQueueCreateStatic(
        1, sizeof(uint8_t),
        queue_buffer, &queue_descriptor
    );

    // save log and event queue in context structure
    IpcCtx ctx = {.event_queue = event_queue, .log = log};

    // subscribe to record. ctx will be passed to handle_fb_change
    FuriRecordSubscriber* fb_record = furi_open(
        "test_fb", false, false, handle_fb_change, NULL, &ctx
    );

    if(fb_record == NULL) {
        fuprintf(log, "cannot open fb record\n");
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

    uint8_t token = 0;

    while(1) {
        // wait for event
        if(xQueueReceive(event_queue, &token, portMAX_DELAY) == pdTRUE) {
            fuprintf(log, "[display] get fb update");

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
    // open record
    FuriRecordSubscriber* fb_record = furi_open(
        "test_fb", false, false, handle_fb_change, NULL, NULL
    );

    // write "ASCII picture"
    void* fb = furi_take(fb_record);
    // fb[] = 
    furi_give(fb_record);

    while(1) {
        delay(100); 
    }
}