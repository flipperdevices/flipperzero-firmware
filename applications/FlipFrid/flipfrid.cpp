#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>

#include "../lfrfid/helpers/rfid_timer_emulator.h"
#include "flipfrid.h"

#define NUMBER_OF_ATTACKS 2
#define TIME_BETWEEN_CARDS \
    5 // Emulate 2 cards per second : (5 * (configTICK_RATE_HZ_RAW/10)) == (5*(1000/10)) == (5*100) == (500)ms
#define TAG "FLIPFRID"

uint8_t id_list[12][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Null bytes
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

typedef enum {
    DefaultKeys,
    BruteForceCustomerId,
} AttackType;

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
    AttackType current_attack_type;
    uint8_t* current_uid;
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
    char badge_type[19];
    switch(flipfrid_state->current_attack_type) {
    case AttackType::DefaultKeys:
        strcpy(badge_type, "  Default Values >");
        break;
    case AttackType::BruteForceCustomerId:
        strcpy(badge_type, "< BF Customer ID  ");
        break;
    default:
        break;
    }

    snprintf(
        uid,
        sizeof(uid),
        "%2X:%2X:%2X:%2X:%2X",
        flipfrid_state->current_uid[0],
        flipfrid_state->current_uid[1],
        flipfrid_state->current_uid[2],
        flipfrid_state->current_uid[3],
        flipfrid_state->current_uid[4]);

    // Badge infos
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 28, AlignCenter, AlignCenter, badge_type);

    if(flipfrid_state->emitting) {
        canvas_draw_str_aligned(canvas, 64, 42, AlignCenter, AlignCenter, uid);
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
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 10); // configTICK_RATE_HZ_RAW 1000

    // Register view port in GUI
    FURI_LOG_I(TAG, "Initializing gui");
    Gui* gui = (Gui*)furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Init values
    FlipFridEvent event;
    flipfrid_state->emitting = false;
    flipfrid_state->current_uid = id_list[0];
    flipfrid_state->current_attack_type = AttackType::DefaultKeys;
    RfidTimerEmulator* emulator;
    emulator = new RfidTimerEmulator();
    RfidTimerEmulator em = *emulator;

    int menu_selected_item_index = 0;
    uint8_t counter = 0;
    uint8_t attack_state = 0;
    bool running = true;
    while(running) {
        // Get next event
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 25);
        if(event_status == FuriStatusOk) {
            if(event.evt_type == EventTypeKey) {
                if(event.input_type == InputTypeShort) {
                    counter = 0;
                    switch(event.key) {
                    case InputKeyUp:
                    case InputKeyDown:
                        // OSEF
                        break;
                    case InputKeyRight:
                        // Next badge type
                        flipfrid_state->emitting = false;
                        if(menu_selected_item_index < (NUMBER_OF_ATTACKS - 1)) {
                            menu_selected_item_index++;
                            flipfrid_state->current_attack_type =
                                (AttackType)menu_selected_item_index;
                        }
                        break;
                    case InputKeyLeft:
                        // Previous badge type
                        flipfrid_state->emitting = false;
                        if(menu_selected_item_index > 0) {
                            menu_selected_item_index--;
                            flipfrid_state->current_attack_type =
                                (AttackType)menu_selected_item_index;
                        }
                        break;
                    case InputKeyOk:
                        if(flipfrid_state->emitting) {
                            flipfrid_state->emitting = false;
                        } else {
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
                    if(1 == counter) {
                        FURI_LOG_D(TAG, "Starting emulation ");
                        em.start(
                            LfrfidKeyType::KeyEM4100,
                            flipfrid_state->current_uid,
                            lfrfid_key_get_type_data_count(LfrfidKeyType::KeyEM4100));
                    } else if(0 == counter) {
                        em.stop();
                        // set next value
                        switch(flipfrid_state->current_attack_type) {
                        case AttackType::DefaultKeys:
                            flipfrid_state->current_uid = id_list[attack_state];
                            FURI_LOG_D(
                                TAG,
                                "DefaultKeys %X:%X:%X:%X:%X",
                                flipfrid_state->current_uid[0],
                                flipfrid_state->current_uid[1],
                                flipfrid_state->current_uid[2],
                                flipfrid_state->current_uid[3],
                                flipfrid_state->current_uid[4]);
                            attack_state = attack_state + 1;
                            if(attack_state >= sizeof(id_list) / sizeof(id_list[0])) {
                                attack_state = 0;
                            }
                            break;
                        case AttackType::BruteForceCustomerId:
                            uint8_t candidate[] = {attack_state, 0x00, 0x00, 0x00, 0x00};
                            FURI_LOG_D(
                                TAG,
                                "BruteForceCustomerId %X:%X:%X:%X:%X",
                                candidate[0],
                                candidate[1],
                                candidate[2],
                                candidate[3],
                                candidate[4]);
                            flipfrid_state->current_uid = candidate;
                            attack_state = attack_state + 1;
                            if(attack_state == 256) {
                                attack_state = 0;
                            }
                            break;
                        }
                    }
                    FURI_LOG_D(TAG, "Counter %d", counter);
                    if (counter > TIME_BETWEEN_CARDS) {
                        counter = 0;
                    } else {
                        counter++;
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