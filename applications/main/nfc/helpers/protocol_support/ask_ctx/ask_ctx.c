#include "ask_ctx.h"

#include <nfc/protocols/ask_ctx/ask_ctx.h>
#include <nfc/protocols/ask_ctx/ask_ctx_poller.h>

#include "nfc/nfc_app_i.h"

#include "../nfc_protocol_support_common.h"
#include "../nfc_protocol_support_gui_common.h"
#include "../nfc_protocol_support_render_common.h"

static void nfc_render_ask_ctx_info(
    const AskCtxData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    furi_string_cat_printf(
        str,
        "UID: %02X %02X %02X %02X\nProduct code: %02X\nFab code: %02X",
        data->uid[0],
        data->uid[1],
        data->uid[2],
        data->uid[3],
        data->product_code,
        data->fab_code);

    if(format_type == NfcProtocolFormatTypeFull && data->block_count > 0) {
        furi_string_cat_printf(str, "\n::::::::::::::::::::::[Blocks]::::::::::::::::::::::");
        for(uint8_t block = 0; block < data->block_count; block += 4) {
            furi_string_cat_printf(str, "\n%02X", block);
            for(uint8_t j = 0; j < 4 && (block + j) < data->block_count; j++) {
                furi_string_cat_printf(
                    str, " %02X%02X", data->blocks[block + j][0], data->blocks[block + j][1]);
            }
        }
    }
}

static void nfc_scene_info_on_enter_ask_ctx(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const AskCtxData* data = nfc_device_get_data(device, NfcProtocolAskCtx);
    FuriString* str = furi_string_alloc();

    nfc_append_filename_string_when_present(instance, str);
    furi_string_cat_printf(str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_ask_ctx_info(data, NfcProtocolFormatTypeFull, str);
    widget_add_text_scroll_element(instance->widget, 0, 0, 128, 64, furi_string_get_cstr(str));

    furi_string_free(str);
}

static NfcCommand nfc_scene_read_poller_callback_ask_ctx(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolAskCtx);

    NfcApp* instance = context;
    const AskCtxPollerEvent* ask_ctx_event = event.event_data;
    if(ask_ctx_event->type == AskCtxPollerEventTypeReadSuccess) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolAskCtx, nfc_poller_get_data(instance->poller));
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventPollerSuccess);
    } else {
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventPollerFailure);
    }
    return NfcCommandStop;
}

static void nfc_scene_read_on_enter_ask_ctx(NfcApp* instance) {
    nfc_poller_start(instance->poller, nfc_scene_read_poller_callback_ask_ctx, instance);
}

static void nfc_scene_read_success_on_enter_ask_ctx(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const AskCtxData* data = nfc_device_get_data(device, NfcProtocolAskCtx);
    FuriString* str = furi_string_alloc();

    furi_string_cat_printf(str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_ask_ctx_info(data, NfcProtocolFormatTypeShort, str);
    widget_add_text_scroll_element(instance->widget, 0, 0, 128, 52, furi_string_get_cstr(str));

    furi_string_free(str);
}

const NfcProtocolSupportBase nfc_protocol_support_ask_ctx = {
    .features = NfcProtocolFeatureNone,
    .scene_info =
        {
            .on_enter = nfc_scene_info_on_enter_ask_ctx,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read =
        {
            .on_enter = nfc_scene_read_on_enter_ask_ctx,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read_menu =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read_success =
        {
            .on_enter = nfc_scene_read_success_on_enter_ask_ctx,
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
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
};
