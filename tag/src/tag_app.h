#ifndef TAG_APP_HEADERS
#define TAG_APP_HEADERS

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <time.h>
#include <gui/gui.h>
#include <lib/infrared/worker/infrared_transmit.h>
#include <lib/infrared/worker/infrared_worker.h>

#define TAG "tag"

typedef enum {
    TagEventTypeInput,
    TagEventTypeSubGhzDataDetected,
    TagEventTypeInfraredMessage,
    // app specific events go here
} TagEventType;

typedef struct {
    TagEventType type;
    InfraredMessage* ir_message;
    InputEvent input;
} TagEvent;

typedef enum {
    TagAppModeUninitialised,
    TagAppModeReady,
    TagAppModePlaying,
    TagAppModeFinished,
    TagAppModeError,
    TagAppModeQuit
} TagAppMode;

typedef struct {
    uint32_t start;
    uint8_t total_damage;
    uint8_t total_hits;
} GameData;

typedef struct {
    TagAppMode mode;
    FuriMessageQueue* queue;
    FuriMutex* data_mutex;
    ViewPort* view_port;
    Gui* gui;
    bool running;
    GameData* data;
} TagAppState;

TagAppState* tag_app_state_get();

#endif