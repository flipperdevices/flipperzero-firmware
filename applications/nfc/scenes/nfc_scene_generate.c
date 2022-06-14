#include "../nfc_i.h"
#include "../helpers/nfc_generators.h"

void nfc_scene_generate_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_generate_on_enter(void* context) {
    Nfc* nfc = context;
    Submenu* submenu = nfc->submenu;

    int i = 0;
    for(const NfcGenerator* const* generator = nfc_generators; *generator != NULL;
        ++generator, ++i) {
        submenu_add_item(submenu, (*generator)->name, i, nfc_scene_generate_submenu_callback, nfc);
    }

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneGenerate));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_generate_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        nfc_device_clear(nfc->dev);
        const NfcGenerator* generator = nfc_generators[event.event];
        generator->generator_func(&nfc->dev->dev_data);

        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneGenerate, event.event);
        scene_manager_next_scene(nfc->scene_manager, generator->next_scene);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed =
            scene_manager_search_and_switch_to_previous_scene(nfc->scene_manager, NfcSceneStart);
    }

    return consumed;
}

void nfc_scene_generate_on_exit(void* context) {
    Nfc* nfc = context;

    submenu_reset(nfc->submenu);
}
