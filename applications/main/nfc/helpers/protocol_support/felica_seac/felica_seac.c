#include "felica_seac.h"
#include "felica_seac_render.h"

#include <nfc/protocols/felica_seac/felica_seac_poller.h>

#include "nfc/nfc_app_i.h"

#include "../nfc_protocol_support_common.h"
#include "../nfc_protocol_support_gui_common.h"

static void nfc_scene_info_on_enter_felica_seac(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const FelicaSeacData* data = nfc_device_get_data(device, NfcProtocolFelicaSeac);

    FuriString* temp_str = furi_string_alloc();
    nfc_append_filename_string_when_present(instance, temp_str);
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_felica_seac_info(data, NfcProtocolFormatTypeFull, temp_str);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 64, furi_string_get_cstr(temp_str));

    furi_string_free(temp_str);
}

static NfcCommand
    nfc_scene_read_poller_callback_felica_seac(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolFelicaSeac);

    NfcApp* instance = context;
    const FelicaSeacPollerEvent* felica_seac_event = event.event_data;

    if(felica_seac_event->type == FelicaSeacPollerEventTypeReady) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolFelicaSeac, nfc_poller_get_data(instance->poller));
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventPollerSuccess);
        return NfcCommandStop;
    }

    return NfcCommandContinue;
}

static void nfc_scene_read_on_enter_felica_seac(NfcApp* instance) {
    nfc_poller_start(instance->poller, nfc_scene_read_poller_callback_felica_seac, instance);
}

static void nfc_scene_read_success_on_enter_felica_seac(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const FelicaSeacData* data = nfc_device_get_data(device, NfcProtocolFelicaSeac);

    FuriString* temp_str = furi_string_alloc();
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_felica_seac_info(data, NfcProtocolFormatTypeShort, temp_str);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));

    furi_string_free(temp_str);
}

const NfcProtocolSupportBase nfc_protocol_support_felica_seac = {
    .features = NfcProtocolFeatureNone,

    .scene_info =
        {
            .on_enter = nfc_scene_info_on_enter_felica_seac,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read =
        {
            .on_enter = nfc_scene_read_on_enter_felica_seac,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read_menu =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read_success =
        {
            .on_enter = nfc_scene_read_success_on_enter_felica_seac,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_saved_menu =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_save_name =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_write =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_emulate =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
};
