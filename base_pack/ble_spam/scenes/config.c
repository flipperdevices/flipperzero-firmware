#include "../ble_spam.h"

#include "protocols/_protocols.h"

static void _config_bool(VariableItem* item) {
    bool* value = variable_item_get_context(item);
    *value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, *value ? "ON" : "OFF");
}
static void config_bool(VariableItemList* list, const char* name, bool* value) {
    VariableItem* item = variable_item_list_add(list, name, 2, _config_bool, value);
    variable_item_set_current_value_index(item, *value);
    variable_item_set_current_value_text(item, *value ? "ON" : "OFF");
}

static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    if(!ctx->attack->protocol) {
        index--;
    } else if(ctx->attack->protocol->config_count) {
        uint8_t extra = ctx->attack->protocol->config_count(&ctx->attack->payload);
        if(index > extra) index -= extra;
    }

    switch(index) {
    case ConfigRandomMac:
        break;
    case ConfigLedIndicator:
        break;
    case ConfigLockKeyboard:
        ctx->lock_keyboard = true;
        view_dispatcher_send_custom_event(ctx->view_dispatcher, 0);
        notification_message_block(ctx->notification, &sequence_display_backlight_off);
        break;
    default:
        break;
    }
}
void scene_config_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    VariableItemList* list = ctx->variable_item_list;

    // Add item to be used as attack name header
    //variable_item_list_add(list, ctx->attack->title, 0, NULL, NULL);
    //variable_item_list_set_header(list, ctx->attack->title);

    config_bool(list, "Random MAC", &ctx->attack->payload.random_mac);

    variable_item_list_set_enter_callback(list, config_callback, ctx);
    if(!ctx->attack->protocol) {
        variable_item_list_add(list, "None shall escape the SINK", 0, NULL, NULL);
    } else if(ctx->attack->protocol->extra_config) {
        ctx->fallback_config_enter = config_callback;
        ctx->attack->protocol->extra_config(ctx);
    }

    config_bool(list, "LED Indicator", &ctx->led_indicator);

    variable_item_list_add(list, "Lock Keyboard", 0, NULL, NULL);

    variable_item_list_set_selected_item(
        list, scene_manager_get_scene_state(ctx->scene_manager, SceneConfig));

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewVariableItemList);
}

bool scene_config_on_event(void* _ctx, SceneManagerEvent event) {
    Ctx* ctx = _ctx;
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_previous_scene(ctx->scene_manager);
        return true;
    }
    return false;
}

void scene_config_on_exit(void* _ctx) {
    Ctx* ctx = _ctx;
    variable_item_list_reset(ctx->variable_item_list);
}
