#include "swiftpair.h"
#include "_protocols.h"

// Hacked together by @Willy-JL and @Spooks4576
// Documentation at https://learn.microsoft.com/en-us/windows-hardware/design/component-guidelines/bluetooth-swift-pair

static const char* swiftpair_get_name(const ProtocolCfg* _cfg) {
    const SwiftpairCfg* cfg = &_cfg->swiftpair;
    UNUSED(cfg);
    return "SwiftPair";
}

static void swiftpair_make_packet(uint8_t* _size, uint8_t** _packet, const ProtocolCfg* _cfg) {
    const SwiftpairCfg* cfg = _cfg ? &_cfg->swiftpair : NULL;

    const char* display_name;
    if(cfg && cfg->display_name[0] != '\0') {
        display_name = cfg->display_name;
    } else {
        const char* names[] = {
            "Assquach💦",
            "Flipper 🐬",
            "iOS 17 🍎",
            "Kink💦",
            "👉👌",
            "🔵🦷",
        };
        display_name = names[rand() % COUNT_OF(names)];
    }
    uint8_t display_name_len = strlen(display_name);

    uint8_t size = 7 + display_name_len;
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

    packet[i++] = size - 1; // Size
    packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
    packet[i++] = 0x06; // Company ID (Microsoft)
    packet[i++] = 0x00; // ...
    packet[i++] = 0x03; // Microsoft Beacon ID
    packet[i++] = 0x00; // Microsoft Beacon Sub Scenario
    packet[i++] = 0x80; // Reserved RSSI Byte
    memcpy(&packet[i], display_name, display_name_len); // Display Name
    i += display_name_len;

    *_size = size;
    *_packet = packet;
}

enum {
    ConfigDisplayName,
};
static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    switch(index) {
    case ConfigDisplayName:
        scene_manager_next_scene(ctx->scene_manager, SceneSwiftpairDisplayName);
    default:
        break;
    }
}
static uint8_t swiftpair_config_list(Ctx* ctx) {
    SwiftpairCfg* cfg = &ctx->attack->payload.cfg.swiftpair;
    VariableItemList* list = ctx->variable_item_list;
    uint8_t item_count = 0;
    VariableItem* item;

    item_count++;
    item = variable_item_list_add(list, "Display Name", 0, NULL, NULL);
    variable_item_set_current_value_text(
        item, cfg->display_name[0] != '\0' ? cfg->display_name : "Random");

    variable_item_list_set_enter_callback(list, config_callback, ctx);

    return item_count;
}

const Protocol protocol_swiftpair = {
    .icon = &I_windows,
    .get_name = swiftpair_get_name,
    .make_packet = swiftpair_make_packet,
    .config_list = swiftpair_config_list,
};

static void display_name_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_swiftpair_display_name_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    SwiftpairCfg* cfg = &ctx->attack->payload.cfg.swiftpair;
    TextInput* text_input = ctx->text_input;
    text_input_reset(text_input);

    text_input_set_header_text(text_input, "Leave empty for random");

    text_input_set_result_callback(
        text_input, display_name_callback, ctx, cfg->display_name, sizeof(cfg->display_name), true);

    text_input_set_minimum_length(text_input, 0);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewTextInput);
}
bool scene_swiftpair_display_name_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_swiftpair_display_name_on_exit(void* _ctx) {
    UNUSED(_ctx);
}
