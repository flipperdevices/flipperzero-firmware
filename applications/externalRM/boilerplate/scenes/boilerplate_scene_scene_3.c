#include "../boilerplate.h"
#include "../helpers/boilerplate_custom_event.h"
#include "../helpers/boilerplate_haptic.h"
#include "../helpers/boilerplate_led.h"

typedef enum {
    ButtonIndexControl3 = -3,
    ButtonIndexControl2 = -2,
    ButtonIndexControl1 = -1,
    ButtonIndexButton1 = 0,
    ButtonIndexButton2 = 1,
    ButtonIndexButton3 = 2,
} ButtonIndex;

static void boilerplate_scene_3_callback(void* context, int32_t index, InputType type) {
    Boilerplate* app = context;

    uint16_t custom_type;
    if(type == InputTypePress) {
        custom_type = BoilerplateCustomEventMenuSelected;
    } else if(type == InputTypeRelease) {
        custom_type = BoilerplateCustomEventMenuVoid;
    } else if(type == InputTypeShort) {
        //somehow ButtonMenuItemTypeCommon uses InputTypeShort
        custom_type = BoilerplateCustomEventMenuSelected;
    } else {
        furi_crash("Unexpected Input Type");
    }
    view_dispatcher_send_custom_event(
        app->view_dispatcher, boilerplate_custom_menu_event_pack(custom_type, index));
}

void boilerplate_scene_scene_3_on_enter(void* context) {
    furi_assert(context);
    Boilerplate* app = context;
    ButtonMenu* button_menu = app->button_menu;
    SceneManager* scene_manager = app->scene_manager;

    button_menu_add_item(
        button_menu,
        "Common",
        ButtonIndexButton1,
        boilerplate_scene_3_callback,
        ButtonMenuItemTypeCommon,
        context);
    button_menu_add_item(
        button_menu,
        "Button",
        ButtonIndexButton2,
        boilerplate_scene_3_callback,
        ButtonMenuItemTypeCommon,
        context);
    button_menu_add_item(
        button_menu,
        "Examples",
        ButtonIndexButton1,
        boilerplate_scene_3_callback,
        ButtonMenuItemTypeCommon,
        context);

    button_menu_add_item(
        button_menu,
        "Control",
        ButtonIndexControl1,
        boilerplate_scene_3_callback,
        ButtonMenuItemTypeControl,
        context);

    button_menu_add_item(
        button_menu,
        "Button",
        ButtonIndexControl2,
        boilerplate_scene_3_callback,
        ButtonMenuItemTypeControl,
        context);

    button_menu_add_item(
        button_menu,
        "Examples",
        ButtonIndexControl3,
        boilerplate_scene_3_callback,
        ButtonMenuItemTypeControl,
        context);

    button_menu_set_header(button_menu, "Button Menu");
    const int16_t button_index =
        (signed)scene_manager_get_scene_state(app->scene_manager, BoilerplateViewIdScene3);
    button_menu_set_selected_item(button_menu, button_index);
    scene_manager_set_scene_state(scene_manager, BoilerplateSceneScene_3, ButtonIndexButton1);

    view_dispatcher_switch_to_view(app->view_dispatcher, BoilerplateViewIdScene3);
}

bool boilerplate_scene_scene_3_on_event(void* context, SceneManagerEvent event) {
    Boilerplate* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        const uint16_t custom_type = boilerplate_custom_menu_event_get_type(event.event);
        const int16_t button_index = boilerplate_custom_menu_event_get_value(event.event);
        if(custom_type == BoilerplateCustomEventMenuSelected) {
            switch(button_index) {
            case ButtonIndexButton1:
                boilerplate_play_happy_bump(app);
                boilerplate_led_set_rgb(app, 255, 0, 0);
                break;
            case ButtonIndexButton2:
                boilerplate_play_happy_bump(app);
                boilerplate_led_set_rgb(app, 0, 255, 0);
                break;
            case ButtonIndexButton3:
                boilerplate_play_happy_bump(app);
                boilerplate_led_set_rgb(app, 0, 0, 255);
                break;
            case ButtonIndexControl1:
                boilerplate_play_bad_bump(app);
                boilerplate_led_set_rgb(app, 255, 0, 255);
                break;
            case ButtonIndexControl2:
                boilerplate_play_bad_bump(app);
                boilerplate_led_set_rgb(app, 255, 255, 0);
                break;
            case ButtonIndexControl3:
                boilerplate_play_bad_bump(app);
                boilerplate_led_set_rgb(app, 0, 255, 255);
                break;
            }
            consumed = true;
        }
    }

    return consumed;
}

void boilerplate_scene_scene_3_on_exit(void* context) {
    Boilerplate* app = context;
    button_menu_reset(app->button_menu);
    notification_message(app->notification, &sequence_reset_red);
    notification_message(app->notification, &sequence_reset_green);
    notification_message(app->notification, &sequence_reset_blue);
}
