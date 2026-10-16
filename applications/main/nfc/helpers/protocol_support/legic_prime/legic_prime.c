#include "legic_prime.h"
#include "legic_prime_render.h"

#include <nfc/protocols/legic_prime/legic_prime_poller.h>

#include "nfc/nfc_app_i.h"

#include "../nfc_protocol_support_common.h"
#include "../nfc_protocol_support_gui_common.h"

static void nfc_scene_info_on_enter_legic_prime(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const LegicPrimeData* data = nfc_device_get_data(device, NfcProtocolLegicPrime);

    FuriString* temp_str = furi_string_alloc();
    nfc_append_filename_string_when_present(instance, temp_str);
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_legic_prime_info(data, NfcProtocolFormatTypeFull, temp_str);
    // Keep the last line clear of the "More" button when scrolled down.
    furi_string_cat_str(temp_str, "\n");

    widget_reset(instance->widget);
    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 64, furi_string_get_cstr(temp_str));

    furi_string_free(temp_str);
}

static void nfc_scene_more_info_on_enter_legic_prime(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const LegicPrimeData* data = nfc_device_get_data(device, NfcProtocolLegicPrime);

    furi_string_reset(instance->text_box_store);
    nfc_render_legic_prime_dump(data, instance->text_box_store);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 64, furi_string_get_cstr(instance->text_box_store));
}

static NfcCommand
    nfc_scene_read_poller_callback_legic_prime(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolLegicPrime);

    NfcApp* instance = context;
    const LegicPrimePollerEvent* legic_event = event.event_data;

    if(legic_event->type == LegicPrimePollerEventTypeRequestMode) {
        legic_event->data->mode_request.mode = LegicPrimePollerModeRead;
    } else if(legic_event->type == LegicPrimePollerEventTypeSuccess) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolLegicPrime, nfc_poller_get_data(instance->poller));
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventPollerSuccess);
        return NfcCommandStop;
    }
    // On Failure (logged by the poller) the poller starts over with a new
    // handshake: the scene keeps reading until the user leaves it.
    return NfcCommandContinue;
}

static void nfc_scene_read_on_enter_legic_prime(NfcApp* instance) {
    nfc_poller_start(instance->poller, nfc_scene_read_poller_callback_legic_prime, instance);
}

static void nfc_scene_read_success_on_enter_legic_prime(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const LegicPrimeData* data = nfc_device_get_data(device, NfcProtocolLegicPrime);

    FuriString* temp_str = furi_string_alloc();
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_legic_prime_info(data, NfcProtocolFormatTypeShort, temp_str);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));

    furi_string_free(temp_str);
}

static NfcCommand
    nfc_scene_emulate_listener_callback_legic_prime(NfcGenericEvent event, void* context) {
    UNUSED(event);
    UNUSED(context);
    return NfcCommandContinue;
}

static void nfc_scene_emulate_on_enter_legic_prime(NfcApp* instance) {
    const LegicPrimeData* data = nfc_device_get_data(instance->nfc_device, NfcProtocolLegicPrime);

    instance->listener = nfc_listener_alloc(instance->nfc, NfcProtocolLegicPrime, data);
    nfc_listener_start(
        instance->listener, nfc_scene_emulate_listener_callback_legic_prime, instance);
}

const NfcProtocolSupportBase nfc_protocol_support_legic_prime = {
    .features = NfcProtocolFeatureEmulateFull | NfcProtocolFeatureMoreInfo,

    .scene_info =
        {
            .on_enter = nfc_scene_info_on_enter_legic_prime,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_more_info =
        {
            .on_enter = nfc_scene_more_info_on_enter_legic_prime,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read =
        {
            .on_enter = nfc_scene_read_on_enter_legic_prime,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read_menu =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read_success =
        {
            .on_enter = nfc_scene_read_success_on_enter_legic_prime,
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
    .scene_emulate =
        {
            .on_enter = nfc_scene_emulate_on_enter_legic_prime,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_write =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
};
