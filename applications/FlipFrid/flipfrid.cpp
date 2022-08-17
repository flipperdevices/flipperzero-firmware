#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>

#include "../lfrfid/helpers/rfid_timer_emulator.h"
#include "flipfrid.h"

#define EMIT_STEPS 10
#define TAG "FLIPFRID"

uint8_t id_list[12][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Default uid
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Only FF
    {0x11, 0x11, 0x11, 0x11, 0x11}, // Only 11
    {0x22, 0x22, 0x22, 0x22, 0x22}, // Only 22
    {0x33, 0x33, 0x33, 0x33, 0x33}, // Only 33
    {0x44, 0x44, 0x44, 0x44, 0x44}, // Only 44
    {0x55, 0x55, 0x55, 0x55, 0x55}, // Only 55
    {0x66, 0x66, 0x66, 0x66, 0x66}, // Only 66
    {0x77, 0x77, 0x77, 0x77, 0x77}, // Only 77
    {0x88, 0x88, 0x88, 0x88, 0x88}, // Only 88
    {0x99, 0x99, 0x99, 0x99, 0x99}, // Only 99
    {0x12, 0x34, 0x56, 0x78, 0x9A}, // Incremental UID
};

typedef struct {
    LfrfidKeyType type;
    char* name;
} Badges;

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType evt_type;
    InputKey key;
    InputType input_type;
} FlipFridEvent;

// STRUCTS
typedef struct {
    bool emitting;
    LfrfidKeyType current_badge_type;
    uint8_t current_uid;
    uint8_t current_uid_repeat;
} FlipFridState;

static void flipfrid_draw_callback(Canvas* const canvas, void* ctx) {
    const FlipFridState* flipfrid_state = (FlipFridState*)acquire_mutex((ValueMutex*)ctx, 100);

    if(flipfrid_state == NULL) {
        return;
    }

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    // Frame
    canvas_draw_frame(canvas, 0, 0, 128, 64);

    // Title
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignTop, "Flip/Frid");

    // Badge Type
    char uid[15];
    char badge_type[12];
    switch(flipfrid_state->current_badge_type) {
    case LfrfidKeyType::KeyEM4100:
        strcpy(badge_type, "  EM4100 >");
        snprintf(
            uid,
            sizeof(uid),
            "%X:%X:%X:%X:%X",
            id_list[flipfrid_state->current_uid][0],
            id_list[flipfrid_state->current_uid][1],
            id_list[flipfrid_state->current_uid][2],
            id_list[flipfrid_state->current_uid][3],
            id_list[flipfrid_state->current_uid][4]);
        break;
    case LfrfidKeyType::KeyH10301:
        strcpy(badge_type, "< HID26 >");
        snprintf(
            uid,
            sizeof(uid),
            "%X:%X:%X",
            id_list[flipfrid_state->current_uid][0],
            id_list[flipfrid_state->current_uid][1],
            id_list[flipfrid_state->current_uid][2]);
        break;
    case LfrfidKeyType::KeyI40134:
        strcpy(badge_type, "< Indala >");
        snprintf(
            uid,
            sizeof(uid),
            "%X:%X:%X",
            id_list[flipfrid_state->current_uid][0],
            id_list[flipfrid_state->current_uid][1],
            id_list[flipfrid_state->current_uid][2]);
        break;
    case LfrfidKeyType::KeyIoProxXSF:
        strcpy(badge_type, "< IoProxs  ");
        snprintf(
            uid,
            sizeof(uid),
            "%X:%X:%X:%X",
            id_list[flipfrid_state->current_uid][0],
            id_list[flipfrid_state->current_uid][1],
            id_list[flipfrid_state->current_uid][2],
            id_list[flipfrid_state->current_uid][3]);
        break;
    default:

        break;
    }

    // Badge infos
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 28, AlignCenter, AlignCenter, badge_type);

    if(flipfrid_state->emitting) {
        canvas_draw_str_aligned(canvas, 64, 42, AlignCenter, AlignCenter, uid);

        // Progress bar
        char progress[EMIT_STEPS + 2] = "";
        strcat(progress, "[");
        for(int i = 0; i < flipfrid_state->current_uid_repeat; i++) {
            strcat(progress, "=");
        }
        for(int i = 0; i < (EMIT_STEPS - flipfrid_state->current_uid_repeat); i++) {
            strcat(progress, "-");
        }
        strcat(progress, "]");
        canvas_draw_str_aligned(canvas, 64, 58, AlignCenter, AlignBottom, progress);
    } else {
        canvas_draw_str_aligned(
            canvas, 64, 42, AlignCenter, AlignCenter, "Press OK to start/stop");
    }

    release_mutex((ValueMutex*)ctx, flipfrid_state);
}

void flipfrid_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    FlipFridEvent event = {
        .evt_type = EventTypeKey, .key = input_event->key, .input_type = input_event->type};
    furi_message_queue_put(event_queue, &event, 25);
}

static void flipfrid_timer_callback(FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    FlipFridEvent event = {
        .evt_type = EventTypeTick, .key = InputKeyUp, .input_type = InputTypeRelease};
    furi_message_queue_put(event_queue, &event, 25);
}

FlipFridApp::FlipFridApp() {
}

FlipFridApp::~FlipFridApp() {
}

// ENTRYPOINT
void FlipFridApp::run() {
    // Input
    FURI_LOG_I(TAG, "Initializing input");
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(FlipFridEvent));
    FlipFridState* flipfrid_state = (FlipFridState*)malloc(sizeof(FlipFridState));
    ValueMutex flipfrid_state_mutex;

    // Mutex
    FURI_LOG_I(TAG, "Initializing flipfrid mutex");
    if(!init_mutex(&flipfrid_state_mutex, flipfrid_state, sizeof(FlipFridState))) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        furi_message_queue_free(event_queue);
        free(flipfrid_state);
    }

    // Configure view port
    FURI_LOG_I(TAG, "Initializing viewport");
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, flipfrid_draw_callback, &flipfrid_state_mutex);
    view_port_input_callback_set(view_port, flipfrid_input_callback, event_queue);

    // Configure timer
    FURI_LOG_I(TAG, "Initializing timer");
    FuriTimer* timer =
        furi_timer_alloc(flipfrid_timer_callback, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 6); // configTICK_RATE_HZ_RAW 1000

    // Register view port in GUI
    FURI_LOG_I(TAG, "Initializing gui");
    Gui* gui = (Gui*)furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Init values
    FlipFridEvent event;
    flipfrid_state->emitting = false;
    flipfrid_state->current_uid = 0;
    flipfrid_state->current_uid_repeat = 0;
    flipfrid_state->current_badge_type = LfrfidKeyType::KeyEM4100;
    RfidTimerEmulator* emulator;
    emulator = new RfidTimerEmulator();
    RfidTimerEmulator em = *emulator;

    uint8_t badge_type_index = 0;
    LfrfidKeyType badges_types[] = {
        LfrfidKeyType::KeyEM4100,
        LfrfidKeyType::KeyH10301,
        LfrfidKeyType::KeyI40134,
        LfrfidKeyType::KeyIoProxXSF,
    };

    bool running = true;
    while(running) {
        // Get next event
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 25);
        if(event_status == FuriStatusOk) {
            if(event.evt_type == EventTypeKey) {
                if(event.input_type == InputTypeShort) {
                    switch(event.key) {
                    case InputKeyUp:
                    case InputKeyDown:
                        // OSEF
                        break;
                    case InputKeyRight:
                        // Next badge type
                        flipfrid_state->emitting = false;
                        if(badge_type_index <
                           (sizeof(badges_types) / sizeof(badges_types[0]) - 1)) {
                            badge_type_index++;
                            flipfrid_state->current_badge_type = badges_types[badge_type_index];
                        }
                        break;
                    case InputKeyLeft:
                        // Previous badge type
                        flipfrid_state->emitting = false;
                        if(badge_type_index > 0) {
                            badge_type_index--;
                            flipfrid_state->current_badge_type = badges_types[badge_type_index];
                        }
                        break;
                    case InputKeyOk:
                        if(flipfrid_state->emitting) {
                            flipfrid_state->emitting = false;
                        } else {
                            flipfrid_state->current_uid_repeat = 0;
                            flipfrid_state->current_uid = 0;
                            flipfrid_state->current_badge_type =
                                (LfrfidKeyType)((flipfrid_state->current_badge_type));
                            flipfrid_state->emitting = true;
                        }
                        break;
                    case InputKeyBack:
                        flipfrid_state->emitting = false;
                        running = false;
                        break;
                    }
                }
            } else if(event.evt_type == EventTypeTick) {
                // Emulate card

                if(flipfrid_state->emitting) {
                    
                    if(flipfrid_state->current_uid_repeat == 0) {
                        FURI_LOG_D(TAG, "Starting emulation %d", flipfrid_state->current_uid);
                        em.start(
                            flipfrid_state->current_badge_type,
                            id_list[flipfrid_state->current_uid],
                            lfrfid_key_get_type_data_count(flipfrid_state->current_badge_type));
                        flipfrid_state->current_uid_repeat++;
                    } else if(flipfrid_state->current_uid_repeat == EMIT_STEPS) {
                        FURI_LOG_D(TAG, "Stop emulation %d", flipfrid_state->current_uid);
                        flipfrid_state->current_uid_repeat = 0;
                        em.stop();

                        // Next uid
                        flipfrid_state->current_uid++;
                        if(flipfrid_state->current_uid == sizeof(id_list) / 5) {
                            flipfrid_state->current_uid = 0;
                        }
                    } else {
                        furi_delay_ms(100);
                        flipfrid_state->current_uid_repeat++;
                        FURI_LOG_D(
                            TAG,
                            "Starting emulation %d/%d",
                            flipfrid_state->current_uid_repeat,
                            EMIT_STEPS);
                    }
                }
                view_port_update(view_port);
            }
        }
    }

    // Cleanup
    furi_timer_stop(timer);
    furi_timer_free(timer);
    em.stop();
    free(emulator);
    FURI_LOG_I(TAG, "Cleaning up");
    free(flipfrid_state);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_record_close(RECORD_GUI);
}