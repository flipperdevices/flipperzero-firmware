#include "../ublox_i.h"

void ublox_scene_wiring_on_enter(void* context) {
    furi_assert(context);

    Ublox* ublox = context;
    widget_add_icon_element(ublox->widget, 0, 0, &I_ublox_wiring);
    view_dispatcher_switch_to_view(ublox->view_dispatcher, UbloxViewWidget);
}

bool ublox_scene_wiring_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void ublox_scene_wiring_on_exit(void* context) {
    furi_assert(context);

    Ublox* ublox = context;
    widget_reset(ublox->widget);
}
