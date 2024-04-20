#include <input/input.h>
#include <gui/gui.h>
#include <gui/view_port.h>

#include "si4713/si4713.h"

typedef enum { DeviceOn, DeviceOff, DeviceDisconnected } DeviceState;

typedef enum {
    MainView,
    TransmittingView,
    ReceivingView,
} CurrentView;

typedef struct {
    uint16_t freq;
    uint8_t tx_power;
    DeviceState working;
    CurrentView view;

    char string_buffer[10];
} SI4713AppState;

typedef struct {
    SI4713Device* device;

    Gui* gui;
    ViewPort* viewport;
    FuriMutex* mutex;
    FuriMessageQueue* event_queue;

    SI4713AppState state;
} SI4713App;
