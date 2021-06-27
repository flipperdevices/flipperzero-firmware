#include <nfc/scenes/nfc_scene_start.h>
#include <furi.h>
#include <nfc/nfc_i.h>
#include <gui/modules/submenu.h>

#include <gui/view_dispatcher.h>

typedef enum {
    SubmenuIndexRead,
    SubmenuIndexRunScript,
    SubmenuIndexSaved,
    SubmenuIndexAddManualy,
} SubmenuIndex;

void nfc_scene_start_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    
    Nfc* nfc = (Nfc*)context;
    if(index == SubmenuIndexRead) {
        view_dispatcher_send_custom_event(nfc->nfc_common.view_dispatcher, 0);
    } else if(index == SubmenuIndexRunScript) {

    } else if(index == SubmenuIndexSaved) {

    } else if(index == SubmenuIndexAddManualy) {

    }
}

const void nfc_scene_start_on_enter(void* context) {
    furi_assert(context);

    Nfc* nfc = (Nfc*)context;
    submenu_add_item(nfc->submenu, "Read card", SubmenuIndexRead, nfc_scene_start_submenu_callback, nfc);
    submenu_add_item(nfc->submenu, "Run special action", SubmenuIndexRunScript, nfc_scene_start_submenu_callback, nfc);
    submenu_add_item(nfc->submenu, "Saved cards", SubmenuIndexSaved, nfc_scene_start_submenu_callback, nfc);
    submenu_add_item(nfc->submenu, "Add manualy", SubmenuIndexAddManualy, nfc_scene_start_submenu_callback, nfc);

    view_dispatcher_switch_to_view(nfc->nfc_common.view_dispatcher, NfcViewMenu);
}

const void nfc_scene_start_on_event(void* context, uint32_t event) {
    // return false;
}

const void nfc_scene_start_on_exit(void* context) {

}

NfcSceneStart* nfc_scence_start_alloc() {
    NfcSceneStart* scene = furi_alloc(sizeof(NfcSceneStart));
    scene->scene.on_start = nfc_scene_start_on_enter;
    scene->scene.on_event = nfc_scene_start_on_event;
    scene->scene.on_exit = nfc_scene_start_on_exit;

    return scene;
}