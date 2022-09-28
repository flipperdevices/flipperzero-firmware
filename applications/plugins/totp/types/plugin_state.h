#ifndef _TOTP_PLUGIN_STATE_H_
#define _TOTP_PLUGIN_STATE_H_

#include <notification/notification.h>
#include <gui/gui.h>
#include "../lib/list/list.h"
#include "../scenes/totp_scenes_enum.h"

typedef struct {
    Scene current_scene;
    void* current_scene_state;
    bool changing_scene;
    NotificationApp* notification;
    Gui* gui;

    float timezone_offset;
    ListNode* tokens_list;
    bool token_list_loaded;
    uint8_t tokens_count;

    uint8_t* crypto_verify_data;
    uint8_t crypto_verify_data_length;
    uint8_t iv[16];
    uint8_t base_iv[16];
} PluginState;

#endif
