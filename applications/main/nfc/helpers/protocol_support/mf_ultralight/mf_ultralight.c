#include "mf_ultralight.h"
#include "mf_ultralight_render.h"

#include <nfc/protocols/mf_ultralight/mf_ultralight_poller.h>

#include "../nfc_protocol_support_gui_common.h"
#include "../../../nfc_app_i.h"

enum {
    SubmenuIndexUnlock = SubmenuIndexCommonMax,
    SubmenuIndexUnlockByReader,
    SubmenuIndexUnlockByPassword,
};

static void nfc_scene_info_on_enter_mf_ultralight(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const MfUltralightData* data = nfc_device_get_data(device, NfcProtocolMfUltralight);

    FuriString* temp_str = furi_string_alloc();
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_mf_ultralight_info(data, NfcProtocolFormatTypeFull, temp_str);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));

    widget_add_button_element(
        instance->widget,
        GuiButtonTypeRight,
        "More",
        nfc_protocol_support_common_widget_callback,
        instance);

    furi_string_free(temp_str);
}

static NfcCommand
    nfc_scene_read_poller_callback_mf_ultralight(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolMfUltralight);

    NfcApp* instance = context;
    const MfUltralightPollerEvent* mf_ultralight_event = event.data;

    if(mf_ultralight_event->type == MfUltralightPollerEventTypeReadSuccess) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolMfUltralight, nfc_poller_get_data(instance->poller));
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcCustomEventReadHandlerSuccess);
        return NfcCommandStop;
    } else if(mf_ultralight_event->type == MfUltralightPollerEventTypeAuthRequest) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolMfUltralight, nfc_poller_get_data(instance->poller));
        const MfUltralightData* data =
            nfc_device_get_data(instance->nfc_device, NfcProtocolMfUltralight);
        if(instance->mf_ul_auth->type == MfUltralightAuthTypeXiaomii) {
            if(mf_ultralight_generate_xiaomi_pass(
                   instance->mf_ul_auth,
                   data->iso14443_3a_data->uid,
                   data->iso14443_3a_data->uid_len)) {
                mf_ultralight_event->data->auth_context.skip_auth = false;
            }
        } else if(instance->mf_ul_auth->type == MfUltralightAuthTypeAmiibo) {
            if(mf_ultralight_generate_amiibo_pass(
                   instance->mf_ul_auth,
                   data->iso14443_3a_data->uid,
                   data->iso14443_3a_data->uid_len)) {
                mf_ultralight_event->data->auth_context.skip_auth = false;
            }
        } else if(instance->mf_ul_auth->type == MfUltralightAuthTypeManual) {
            mf_ultralight_event->data->auth_context.skip_auth = false;
        } else {
            mf_ultralight_event->data->auth_context.skip_auth = true;
        }
        if(!mf_ultralight_event->data->auth_context.skip_auth) {
            mf_ultralight_event->data->auth_context.password = instance->mf_ul_auth->password;
        }
    } else if(mf_ultralight_event->type == MfUltralightPollerEventTypeAuthSuccess) {
        instance->mf_ul_auth->pack = mf_ultralight_event->data->auth_context.pack;
    }

    return NfcCommandContinue;
}

static void nfc_scene_read_on_enter_mf_ultralight(NfcApp* instance) {
    nfc_poller_start(instance->poller, nfc_scene_read_poller_callback_mf_ultralight, instance);
}

static void nfc_scene_read_menu_on_enter_mf_ultralight(NfcApp* instance) {
    Submenu* submenu = instance->submenu;

    const MfUltralightData* data =
        nfc_device_get_data(instance->nfc_device, NfcProtocolMfUltralight);

    if(!mf_ultralight_is_all_data_read(data)) {
        submenu_add_item(
            submenu,
            "Unlock",
            SubmenuIndexUnlock,
            nfc_protocol_support_common_submenu_callback,
            instance);
    }
}

static void nfc_scene_read_success_on_enter_mf_ultralight(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const MfUltralightData* data = nfc_device_get_data(device, NfcProtocolMfUltralight);

    FuriString* temp_str = furi_string_alloc();
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_mf_ultralight_info(data, NfcProtocolFormatTypeShort, temp_str);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));

    furi_string_free(temp_str);
}

static void nfc_scene_saved_menu_on_enter_mf_ultralight(NfcApp* instance) {
    Submenu* submenu = instance->submenu;
    const MfUltralightData* data =
        nfc_device_get_data(instance->nfc_device, NfcProtocolMfUltralight);

    if(!mf_ultralight_is_all_data_read(data)) {
        submenu_add_item(
            submenu,
            "Unlock with Reader",
            SubmenuIndexUnlockByReader,
            nfc_protocol_support_common_submenu_callback,
            instance);

        submenu_add_item(
            submenu,
            "Unlock with Password",
            SubmenuIndexUnlockByPassword,
            nfc_protocol_support_common_submenu_callback,
            instance);
    }
}

static bool nfc_scene_info_on_event_mf_ultralight(NfcApp* instance, uint32_t event) {
    if(event == GuiButtonTypeRight) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        return true;
    }

    return false;
}

static bool nfc_scene_read_menu_on_event_mf_ultralight(NfcApp* instance, uint32_t event) {
    switch(event) {
    case SubmenuIndexCommonEmulate:
        scene_manager_next_scene(instance->scene_manager, NfcSceneMfUltralightEmulate);
        return true;
    case SubmenuIndexUnlock:
        scene_manager_next_scene(instance->scene_manager, NfcSceneMfUltralightUnlockMenu);
        return true;
    default:
        return false;
    }
}

static bool nfc_scene_saved_menu_on_event_mf_ultralight(NfcApp* instance, uint32_t event) {
    switch(event) {
    case SubmenuIndexCommonEmulate:
        scene_manager_next_scene(instance->scene_manager, NfcSceneMfUltralightEmulate);
        return true;
    case SubmenuIndexUnlockByReader:
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        return true;
    case SubmenuIndexUnlockByPassword:
        scene_manager_next_scene(instance->scene_manager, NfcSceneMfUltralightUnlockMenu);
        return true;
    default:
        return false;
    }
}

const NfcProtocolSupportBase nfc_protocol_support_mf_ultralight = {
    .features = NfcProtocolFeatureEmulateFull,

    .scene_info =
        {
            .on_enter = nfc_scene_info_on_enter_mf_ultralight,
            .on_event = nfc_scene_info_on_event_mf_ultralight,
        },
    .scene_read =
        {
            .on_enter = nfc_scene_read_on_enter_mf_ultralight,
            .on_event = NULL,
        },
    .scene_read_menu =
        {
            .on_enter = nfc_scene_read_menu_on_enter_mf_ultralight,
            .on_event = nfc_scene_read_menu_on_event_mf_ultralight,
        },
    .scene_read_success =
        {
            .on_enter = nfc_scene_read_success_on_enter_mf_ultralight,
            .on_event = NULL,
        },
    .scene_saved_menu =
        {
            .on_enter = nfc_scene_saved_menu_on_enter_mf_ultralight,
            .on_event = nfc_scene_saved_menu_on_event_mf_ultralight,
        },
};