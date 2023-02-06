#include "../ibutton_i.h"
#include <dolphin/dolphin.h>

void ibutton_scene_read_success_on_enter(void* context) {
    iButton* ibutton = context;
    Widget* widget = ibutton->widget;

    widget_add_button_element(widget, GuiButtonTypeLeft, "Retry", ibutton_widget_callback, context);
    widget_add_button_element(widget, GuiButtonTypeRight, "More", ibutton_widget_callback, context);

    view_dispatcher_switch_to_view(ibutton->view_dispatcher, iButtonViewWidget);
    ibutton_notification_message(ibutton, iButtonNotificationMessageGreenOn);
}

bool ibutton_scene_read_success_on_event(void* context, SceneManagerEvent event) {
    iButton* ibutton = context;
    SceneManager* scene_manager = ibutton->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
        scene_manager_next_scene(scene_manager, iButtonSceneExitConfirm);
    } else if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(scene_manager, iButtonSceneReadKeyMenu);
        } else if(event.event == GuiButtonTypeLeft) {
            scene_manager_next_scene(scene_manager, iButtonSceneRetryConfirm);
        }
    }

    return consumed;
}

void ibutton_scene_read_success_on_exit(void* context) {
    iButton* ibutton = context;

    widget_reset(ibutton->widget);

    ibutton_notification_message(ibutton, iButtonNotificationMessageGreenOff);
}
