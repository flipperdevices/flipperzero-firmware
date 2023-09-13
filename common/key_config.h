#pragma once

#include <gui/view.h>
#include <gui/view_dispatcher.h>

typedef struct FlipboardModel FlipboardModel;
typedef struct KeyConfig KeyConfig;

KeyConfig* key_config_alloc(FlipboardModel* model, uint32_t config_view_id);
View* key_config_get_view(KeyConfig* key_config);
uint32_t key_config_get_view_id(KeyConfig* key_config);
void key_config_register_subview(
    KeyConfig* key_config,
    ViewDispatcher* view_dispatcher,
    uint32_t variable_item_list_view_id);
void key_config_free(KeyConfig* key_config);