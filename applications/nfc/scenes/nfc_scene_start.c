#include <nfc/scenes/nfc_scene_start.h>
#include <furi.h>
#include <nfc/nfc_i.h>
#include <nfc/view_modules/submenu_vm.h>

#include <gui/view_dispatcher.h>

void nfc_scene_start_on_enter(void* context) {
    furi_assert(context);

    Nfc* nfc = (Nfc*)context;
    SubmenuVM* submenu_vm = nfc_get_submenu_vm(nfc);
    string_t* items = furi_alloc(3 * sizeof(items));
    string_init_set_str(items[0], "First");
    string_init_set_str(items[1], "Second");
    string_init_set_str(items[2], "Third");
    SubmenuConfig config = {
        .items_num = 3,
        .item_text = (string_t*) items,
        // .header = NULL,
        .selected_item = 0,     
    };
    submenu_vm_config_apply(submenu_vm, &config);
    view_dispatcher_switch_to_view(nfc->nfc_common.view_dispatcher, NfcViewMenuVM);
}

NfcSceneStart* nfc_scence_start_alloc() {
    NfcSceneStart* scene = furi_alloc(sizeof(NfcSceneStart));
    scene->scene.on_start = nfc_scene_start_on_enter;

    return scene;
}