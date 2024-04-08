#include <furi.h>
#include <furi_hal.h>
#include <infrared.h>
#include <infrared_worker.h>
#include <furi_hal_infrared.h>
#include <gui/gui.h>
#include <bit_lib/bit_lib.h>

#define TAG "IR Decoder"

typedef struct {
    InfraredMessage* decoded_signal;
    FuriMutex* mutex;
    ViewPort* view_port;
} IRDecoderState;
