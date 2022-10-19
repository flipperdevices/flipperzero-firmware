#include <furi.h>
#include <gui/gui.h>
#include <stdlib.h>

#include "../include/pwn_zero.h"

#include "../include/constants.h"

PwnZero* pwn_zero_alloc() {
    PwnZero* pwn = malloc(sizeof(PwnZero));

    pwn->pwnagotchi = pwnagotchi_alloc();

    return pwn;
}

void pwn_zero_free(PwnZero* pwn) {

    pwnagotchi_free(pwn->pwnagotchi);
    free(pwn);
    pwn = NULL;

}

bool pwn_zero_screen_set(PwnZero* pwn, uint8_t i, uint8_t j, bool status) {
    if (i >= FLIPPER_SCREEN_HEIGHT || j >= FLIPPER_SCREEN_WIDTH) {
        return false;
    }

    pwn->pwnagotchi->screen[i][j] = status;
    return true;
}

void pwn_zero_screen_clear(const PwnZero* pwn) {
    memset(pwn->pwnagotchi->screen, 0, sizeof(pwn->pwnagotchi->screen));
}

void pwn_zero_screen_flush(Canvas* canvas, void* context) {
    const PwnZero* pwn = acquire_mutex((ValueMutex*) context, 25);
    if (pwn == NULL) {
        return;
    }

    canvas_clear(canvas);

    // Loop through and color each bit
    for (size_t ii = 0; ii < FLIPPER_SCREEN_HEIGHT; ii++) {
        for (size_t jj = 0; jj < FLIPPER_SCREEN_WIDTH; jj++) {
            if (pwn->pwnagotchi->screen[ii][jj]) {
                canvas_draw_dot(canvas, jj, ii);
            }
        }
    }

    release_mutex((ValueMutex*) context, pwn);
}

void input_callback(InputEvent* inputEvent, FuriMessageQueue* eventQueue) {
    furi_assert(eventQueue);

    furi_message_queue_put(eventQueue, &inputEvent, FuriWaitForever);
}

void pwn_uart_rx_cb(UartIrqEvent event, uint8_t data, void* context) {
    furi_assert(context);

    PwnZero* pwnZero = acquire_mutex_block((ValueMutex*) context);
    Pwnagotchi* pwn = pwnZero->pwnagotchi;

    if (event == UartIrqEventRXNE) {
        // Manage the queue
        if (pwn->writeIdx >= (pwn->messageQueue + pwn->queueSize)) {
            pwn->writeIdx = pwn->messageQueue;
        }

    //     // Now add it in
    //     *(pwn->writeIdx) = data;
    //     pwn->writeIdx += 1;

    }

    UNUSED(pwn);
    UNUSED(event);
    UNUSED(data);

    release_mutex(context, pwnZero);

}

int32_t pwn_zero_app(void* p) {
    UNUSED(p);
    
    // Setup input
    FuriMessageQueue* eventQueue = furi_message_queue_alloc(4, sizeof(InputEvent));

    PwnZero* pwnZero = pwn_zero_alloc();

    ValueMutex pwnMutex;
    if(!init_mutex(&pwnMutex, pwnZero, sizeof(PwnZero))) {
        FURI_LOG_E("PwnZero", "cannot create mutex\r\n");
        pwn_zero_free(pwnZero);
        return 255;
    }
    
    // Setup gui state and viewport
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, pwn_zero_screen_flush, &pwnMutex);
    view_port_input_callback_set(view_port, input_callback, eventQueue);  

    // Set rx callback
    furi_hal_uart_set_irq_cb(pwnZero->pwnagotchi->channel, pwn_uart_rx_cb, &pwnMutex); 

    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    /// Body will go here
    for (int ii = 0; ii < FLIPPER_SCREEN_HEIGHT; ii++) {
        // Loop until message
        uint8_t* messageQueue;
        uint8_t* writeIdx;
        do {
            PwnZero* tmp = (PwnZero*) acquire_mutex_block(&pwnMutex);
            messageQueue = tmp->pwnagotchi->messageQueue;
            writeIdx = tmp->pwnagotchi->writeIdx;

            release_mutex(&pwnMutex, tmp);
        } while (messageQueue == writeIdx);

        PwnZero* pwnZero = (PwnZero*) acquire_mutex_block(&pwnMutex);
        for (int jj = 0; jj < FLIPPER_SCREEN_WIDTH; jj++) {

            pwn_zero_screen_set(pwnZero, ii, jj, true);


        }
        view_port_update(view_port);
        release_mutex(&pwnMutex, pwnZero);

    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    delete_mutex(&pwnMutex);

    return 0;
}
