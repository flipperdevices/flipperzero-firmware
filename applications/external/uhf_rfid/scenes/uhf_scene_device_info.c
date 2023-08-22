#include "../uhf_app_i.h"
#include <dolphin/dolphin.h>

typedef enum { EPC_INFO, TID_INFO, USER_INFO } UHFTagInfo;

static UHFTagInfo current_info;

char* get_current_bank_info_str() {
    switch(current_info) {
    case EPC_INFO:
        return "EPC Bank";
    case TID_INFO:
        return "TID Bank";
    case USER_INFO:
        return "User Bank";
    }
    return "";
}

char* get_next_bank_info_str() {
    switch(current_info) {
    case EPC_INFO:
        current_info = TID_INFO;
        return "TID";
    case TID_INFO:
        current_info = USER_INFO;
        return "USER";
    case USER_INFO:
        current_info = EPC_INFO;
        return "EPC";
    }
    return "";
}

void uhf_scene_device_info_widget_callback(GuiButtonType result, InputType type, void* context) {
    UHFApp* uhf_app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(uhf_app->view_dispatcher, result);
    }
}

void change_view_on_event(UHFApp* uhf_app) {
    UHFTag* uhf_tag = uhf_app->uhf_device->uhf_tag_wrapper->uhf_tag;
    FuriString* furi_temp_str;
    furi_temp_str = furi_string_alloc();
    char* temp_str;
    size_t length;

    widget_reset(uhf_app->widget);
    widget_add_string_element(
        uhf_app->widget, 64, 5, AlignCenter, AlignCenter, FontPrimary, get_current_bank_info_str());

    switch(current_info) {
    case EPC_INFO:
        temp_str = convertToHexString(uhf_tag->epc->data, uhf_tag->epc->size);
        length = uhf_tag->epc->size;
        break;
    case TID_INFO:
        temp_str = convertToHexString(uhf_tag->tid->data, uhf_tag->tid->size);
        length = uhf_tag->tid->size;
        break;
    case USER_INFO:
        temp_str = convertToHexString(uhf_tag->user->data, uhf_tag->user->size);
        length = uhf_tag->user->size;
        break;
    default:
        temp_str = NULL;
        length = 0;
        break;
    }

    furi_string_cat_printf(furi_temp_str, "Length: %d bytes", length);

    widget_add_string_element(
        uhf_app->widget,
        3,
        12,
        AlignLeft,
        AlignTop,
        FontKeyboard,
        furi_string_get_cstr(furi_temp_str));

    widget_add_string_multiline_element(
        uhf_app->widget, 3, 24, AlignLeft, AlignTop, FontKeyboard, temp_str);

    widget_add_button_element(
        uhf_app->widget,
        GuiButtonTypeRight,
        get_next_bank_info_str(),
        uhf_scene_device_info_widget_callback,
        uhf_app);

    widget_add_button_element(
        uhf_app->widget, GuiButtonTypeLeft, "Back", uhf_scene_device_info_widget_callback, uhf_app);

    furi_string_free(furi_temp_str);
    free(temp_str);
}

void uhf_scene_device_info_on_enter(void* context) {
    UHFApp* uhf_app = context;
    current_info = EPC_INFO;
    dolphin_deed(DolphinDeedNfcReadSuccess);
    change_view_on_event(uhf_app);
    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewWidget);
}

bool uhf_scene_device_info_on_event(void* context, SceneManagerEvent event) {
    UHFApp* uhf_app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeTick) return false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = scene_manager_previous_scene(uhf_app->scene_manager);
        } else if(event.event == GuiButtonTypeRight) {
            change_view_on_event(uhf_app);
        } else if(event.event == UHFCustomEventViewExit) {
            view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewWidget);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(uhf_app->scene_manager);
        consumed = true;
    }
    return consumed;
}

void uhf_scene_device_info_on_exit(void* context) {
    UHFApp* uhf_app = context;

    // Clear views
    widget_reset(uhf_app->widget);
}
