#include "../ibutton_i.h"

typedef enum {
    SubmenuIndexRead,
    SubmenuIndexSaved,
    SubmenuIndexAdd,
} SubmenuIndex;

void ibutton_scene_start_submenu_callback(void* context, uint32_t index) {
    iButton* ibutton = context;
    view_dispatcher_send_custom_event(ibutton->view_dispatcher, index);
}

void ibutton_scene_start_on_enter(void *context) {
    iButton* ibutton = context;
    Submenu* submenu = ibutton->submenu;

    submenu_add_item(submenu, "Read", SubmenuIndexRead, ibutton_scene_start_submenu_callback, ibutton);
    submenu_add_item(submenu, "Saved", SubmenuIndexSaved, ibutton_scene_start_submenu_callback, ibutton);
    submenu_add_item(submenu, "Add Manually", SubmenuIndexAdd, ibutton_scene_start_submenu_callback, ibutton);

    submenu_set_selected_item(submenu, SubmenuIndexRead);

    view_dispatcher_switch_to_view(ibutton->view_dispatcher, iButtonViewSubmenu);
}

bool ibutton_scene_start_on_event(void *context, SceneManagerEvent event) {
//     iButton* ibutton = context;
    return false;
}

void ibutton_scene_start_on_exit(void *context) {
    iButton* ibutton = context;
    submenu_reset(ibutton->submenu);
}
