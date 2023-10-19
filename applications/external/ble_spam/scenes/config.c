#include "../ble_spam.h"

#include "protocols/_protocols.h"

static void random_mac_changed(VariableItem* item) {
    Ctx* ctx = variable_item_get_context(item);
    ctx->attack->payload.random_mac = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, ctx->attack->payload.random_mac ? "ON" : "OFF");
}

void scene_config_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    VariableItem* item;
    VariableItemList* list = ctx->variable_item_list;
    variable_item_list_reset(list);

    variable_item_list_add(list, ctx->attack->title, 0, NULL, ctx);

    item = variable_item_list_add(list, "Random MAC", 2, random_mac_changed, ctx);
    variable_item_set_current_value_index(item, ctx->attack->payload.random_mac);
    variable_item_set_current_value_text(item, ctx->attack->payload.random_mac ? "ON" : "OFF");

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewVariableItemList);
}

bool scene_config_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}

void scene_config_on_exit(void* _ctx) {
    UNUSED(_ctx);
}
