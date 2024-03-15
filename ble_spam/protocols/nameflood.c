#include "nameflood.h"
#include "_protocols.h"

#include <storage/storage.h>
#include <toolbox/stream/file_stream.h>

// Hacked together by @Willy-JL

static char names[256][sizeof(((NamefloodCfg*)0)->name)];
static uint8_t names_count = 0;

static const char* get_name(const Payload* payload) {
    UNUSED(payload);
    return "NameFlood";
}

static const char* random_name() {
    if(names_count == 0) {
        // Fill random names
        Storage* storage = furi_record_open(RECORD_STORAGE);
        Stream* stream = file_stream_alloc(storage);
        FuriString* line = furi_string_alloc();
        if(!storage_file_exists(storage, APP_DATA_PATH("nameflood.txt"))) {
            // Copy default names
            storage_common_copy(
                storage, APP_ASSETS_PATH("nameflood.txt"), APP_DATA_PATH("nameflood.txt"));
        }
        if(file_stream_open(
               stream, APP_DATA_PATH("nameflood.txt"), FSAM_READ, FSOM_OPEN_EXISTING)) {
            while(stream_read_line(stream, line)) {
                furi_string_replace_all(line, "\r", "");
                furi_string_replace_all(line, "\n", "");
                if(furi_string_size(line)) {
                    strlcpy(names[names_count++], furi_string_get_cstr(line), sizeof(names[0]));
                }
            }
        }
        furi_string_free(line);
        file_stream_close(stream);
        stream_free(stream);
        furi_record_close(RECORD_STORAGE);

        if(names_count == 0) {
            // Add fallback if list is empty
            strlcpy(names[names_count++], "NameFlood", sizeof(names[0]));
        }
    }

    return names[rand() % names_count];
}

static void make_packet(uint8_t* _size, uint8_t** _packet, Payload* payload) {
    NamefloodCfg* cfg = payload ? &payload->cfg.nameflood : NULL;

    const char* name;
    switch(cfg ? payload->mode : PayloadModeRandom) {
    case PayloadModeRandom:
    default:
        name = random_name();
        break;
    case PayloadModeValue:
        name = cfg->name;
        break;
    }
    uint8_t name_len = strlen(name);

    uint8_t size = 12 + name_len;
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

    packet[i++] = 2; // Size
    packet[i++] = 0x01; // AD Type (Flags)
    packet[i++] = 0x06; // Flags

    packet[i++] = name_len + 1; // Size
    packet[i++] = 0x09; // AD Type (Complete Local Name)
    memcpy(&packet[i], name, name_len); // Device Name
    i += name_len;

    packet[i++] = 3; // Size
    packet[i++] = 0x02; // AD Type (Incomplete Service UUID List)
    packet[i++] = 0x12; // Service UUID (Human Interface Device)
    packet[i++] = 0x18; // ...

    packet[i++] = 2; // Size
    packet[i++] = 0x0A; // AD Type (Tx Power Level)
    packet[i++] = 0x00; // 0dBm

    *_size = size;
    *_packet = packet;
}

enum {
    _ConfigExtraStart = ConfigExtraStart,
    ConfigName,
    ConfigInfoSettings,
    ConfigCOUNT,
};
static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    switch(index) {
    case ConfigName:
        scene_manager_next_scene(ctx->scene_manager, SceneNamefloodName);
        break;
    case ConfigInfoSettings:
        break;
    default:
        ctx->fallback_config_enter(ctx, index);
        break;
    }
}
static void extra_config(Ctx* ctx) {
    Payload* payload = &ctx->attack->payload;
    NamefloodCfg* cfg = &payload->cfg.nameflood;
    VariableItemList* list = ctx->variable_item_list;
    VariableItem* item;

    item = variable_item_list_add(list, "Display Name", 0, NULL, NULL);
    variable_item_set_current_value_text(
        item, payload->mode == PayloadModeRandom ? "Random" : cfg->name);

    variable_item_list_add(list, "See in phone BT settings", 0, NULL, NULL);

    variable_item_list_set_enter_callback(list, config_callback, ctx);
}

static uint8_t config_count(const Payload* payload) {
    UNUSED(payload);
    return ConfigCOUNT - ConfigExtraStart - 1;
}

const Protocol protocol_nameflood = {
    .icon = &I_ble_spam,
    .get_name = get_name,
    .make_packet = make_packet,
    .extra_config = extra_config,
    .config_count = config_count,
};

static void name_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    payload->mode = PayloadModeValue;
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_nameflood_name_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    NamefloodCfg* cfg = &payload->cfg.nameflood;
    TextInput* text_input = ctx->text_input;

    text_input_set_header_text(text_input, "Press back for random");

    text_input_set_result_callback(
        text_input, name_callback, ctx, cfg->name, sizeof(cfg->name), true);

    text_input_set_minimum_length(text_input, 0);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewTextInput);
}
bool scene_nameflood_name_on_event(void* _ctx, SceneManagerEvent event) {
    Ctx* ctx = _ctx;
    Payload* payload = &ctx->attack->payload;
    if(event.type == SceneManagerEventTypeBack) {
        payload->mode = PayloadModeRandom;
    }
    return false;
}
void scene_nameflood_name_on_exit(void* _ctx) {
    Ctx* ctx = _ctx;
    text_input_reset(ctx->text_input);
}
