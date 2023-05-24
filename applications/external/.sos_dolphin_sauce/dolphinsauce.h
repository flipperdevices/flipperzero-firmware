#ifndef arha_dolphinsauce
#define arha_dolphinsauce

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>

#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/subghz_file_encoder_worker.h>

#define MARKER_HZ 220 * 8 * 2

typedef struct {
    FuriMutex* mutex;
    FuriMessageQueue* event_queue;
    ViewPort* view_port;
    Gui* gui;
    bool cw_on;
    bool txing;
    FuriTimer* timer;
    uint32_t* active_msg;

    uint8_t sos_mode_code;
} AppFSM;

void rf_start_cw();
void rf_stop_cw();
static void update_timer_callback(FuriMessageQueue* event_queue);

AppFSM* app_global;

/*                    > S  <>    X     ><  S <         */
// uint32_t sos_msg = 0b10101011101110111010101000000000;
uint32_t sms_msg = 0b10101000111011100010101000000000;
uint32_t sos_msg = 0b10101011101110111010101000000000;
//uint32_t test_msg = 0b10101010111111110011001100001111;
uint8_t test_msg_pos = 0;

#endif
