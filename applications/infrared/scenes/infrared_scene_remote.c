#include "../infrared_i.h"

typedef enum {
    ButtonIndexPlus = -2,
    ButtonIndexEdit = -1,
    ButtonIndexNA = 0,
} ButtonIndex;

#pragma pack(push, 1)
typedef union {
    uint32_t packed_value;
    struct {
        uint16_t type;
        int16_t index;
    } content;
} InfraredSceneRemoteEvent;
#pragma pack(pop)

static void
    infrared_scene_remote_button_menu_callback(void* context, int32_t index, InputType type) {
    Infrared* infrared = context;

    InfraredSceneRemoteEvent event;
    event.content.index = index;

    if(type == InputTypePress) {
        event.content.type = InfraredCustomEventTransmitStarted;
    } else if(type == InputTypeRelease) {
        event.content.type = InfraredCustomEventTransmitStopped;
    } else if(type == InputTypeShort) {
        event.content.type = InfraredCustomEventMenuSelected;
    } else {
        furi_crash("Unexpected input type");
    }

    view_dispatcher_send_custom_event(infrared->view_dispatcher, event.packed_value);
}

void infrared_scene_remote_on_enter(void* context) {
    Infrared* infrared = context;
    InfraredRemote* remote = infrared->remote;
    ButtonMenu* button_menu = infrared->button_menu;

    infrared_worker_tx_set_get_signal_callback(
        infrared->worker, infrared_worker_tx_get_signal_steady_callback, infrared);
    infrared_worker_tx_set_signal_sent_callback(
        infrared->worker, infrared_signal_sent_callback, infrared);

    size_t button_count = infrared_remote_get_button_count(remote);
    for(size_t i = 0; i < button_count; ++i) {
        InfraredRemoteButton* button = infrared_remote_get_button(remote, i);
        button_menu_add_item(
            button_menu,
            infrared_remote_button_get_name(button),
            i,
            infrared_scene_remote_button_menu_callback,
            ButtonMenuItemTypeCommon,
            context);
    }

    button_menu_add_item(
        button_menu,
        "+",
        ButtonIndexPlus,
        infrared_scene_remote_button_menu_callback,
        ButtonMenuItemTypeControl,
        context);
    button_menu_add_item(
        button_menu,
        "Edit",
        ButtonIndexEdit,
        infrared_scene_remote_button_menu_callback,
        ButtonMenuItemTypeControl,
        context);

    button_menu_set_header(button_menu, infrared_remote_get_name(remote));

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewButtonMenu);
}

bool infrared_scene_remote_on_event(void* context, SceneManagerEvent event) {
    Infrared* infrared = context;
    SceneManager* scene_manager = infrared->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        const uint32_t possible_scenes[] = {InfraredSceneStart, InfraredSceneRemoteList};
        infrared_switch_to_previous_scene_one_of(
            infrared, possible_scenes, sizeof(possible_scenes) / sizeof(uint32_t));
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        InfraredSceneRemoteEvent custom_event;
        custom_event.packed_value = event.event;

        const uint16_t custom_type = custom_event.content.type;
        const int16_t menu_index = custom_event.content.index;

        if(custom_type == InfraredCustomEventTransmitStarted) {
            furi_assert(menu_index >= 0);
            infrared_tx_start_button_index(infrared, menu_index);
            consumed = true;
        } else if(custom_type == InfraredCustomEventTransmitStopped) {
            infrared_tx_stop(infrared);
            consumed = true;
        } else if(custom_type == InfraredCustomEventMenuSelected) {
            furi_assert(menu_index < 0);
            if(menu_index == ButtonIndexPlus) {
                infrared->app_state.is_learning_new_remote = false;
                scene_manager_next_scene(scene_manager, InfraredSceneLearn);
                consumed = true;
            } else if(menu_index == ButtonIndexEdit) {
                scene_manager_next_scene(scene_manager, InfraredSceneEdit);
                consumed = true;
            }
        }
    }

    return consumed;
}

void infrared_scene_remote_on_exit(void* context) {
    Infrared* infrared = context;
    infrared_worker_tx_set_get_signal_callback(infrared->worker, NULL, NULL);
    infrared_worker_tx_set_signal_sent_callback(infrared->worker, NULL, NULL);
    button_menu_reset(infrared->button_menu);
}
