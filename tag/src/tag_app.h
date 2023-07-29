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

#define LOOP_MESSAGE_TIMEOUT_ms 500
#define ENERGY_MAX 50.0
#define ENERGY_RECOVERY_ms 10000

typedef enum {
    TagEventTypeInput,
    TagEventTypeSubGhzDataDetected,
    TagEventTypeInfraredMessage,
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
    float_t energy;
    uint32_t last_ir_address;
    uint32_t last_ir_command;
    FuriString* string_buffer;
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