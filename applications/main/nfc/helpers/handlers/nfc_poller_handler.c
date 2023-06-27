#include "nfc_poller_handler.h"

#include "../../nfc_app_i.h"

typedef NfcCustomEvent (*NfcPollerReadHandler)(NfcaPollerEvent* event, NfcApp* nfc_app);

static NfcCustomEvent
    nfc_poller_handler_read_iso14443_3a(NfcaPollerEvent* event, NfcApp* nfc_app) {
    UNUSED(nfc_app);
    NfcCustomEvent custom_event = NfcCustomEventReadHandlerIgnore;

    if(event->type == NfcaPollerEventTypeReady) {
        custom_event = NfcCustomEventReadHandlerSuccess;
    }

    return custom_event;
}

static NfcCustomEvent
    nfc_poller_handler_read_iso14443_4a(Iso14443_4aPollerEvent* event, NfcApp* nfc_app) {
    UNUSED(nfc_app);
    NfcCustomEvent custom_event = NfcCustomEventReadHandlerIgnore;

    if(event->type == Iso14443_4aPollerEventTypeReady) {
        custom_event = NfcCustomEventReadHandlerSuccess;
    }

    return custom_event;
}

static NfcCustomEvent
    nfc_poller_handler_read_mf_ultralight(MfUltralightPollerEvent* event, NfcApp* nfc_app) {
    NfcCustomEvent custom_event = NfcCustomEventReadHandlerIgnore;

    if(event->type == MfUltralightPollerEventTypeReadSuccess) {
        custom_event = NfcCustomEventReadHandlerSuccess;
    } else if(event->type == MfUltralightPollerEventTypeAuthRequest) {
        nfc_dev_set_protocol_data(
            nfc_app->nfc_dev, NfcProtocolTypeMfUltralight, nfc_poller_get_data(nfc_app->poller));
        const MfUltralightData* data =
            nfc_dev_get_protocol_data(nfc_app->nfc_dev, NfcProtocolTypeMfUltralight);
        if(nfc_app->mf_ul_auth->type == MfUltralightAuthTypeXiaomii) {
            if(mf_ultralight_generate_xiaomi_pass(
                   nfc_app->mf_ul_auth, data->nfca_data->uid, data->nfca_data->uid_len)) {
                event->data->auth_context.skip_auth = false;
            }
        } else if(nfc_app->mf_ul_auth->type == MfUltralightAuthTypeAmiibo) {
            if(mf_ultralight_generate_amiibo_pass(
                   nfc_app->mf_ul_auth, data->nfca_data->uid, data->nfca_data->uid_len)) {
                event->data->auth_context.skip_auth = false;
            }
        } else if(nfc_app->mf_ul_auth->type == MfUltralightAuthTypeManual) {
            event->data->auth_context.skip_auth = false;
        } else {
            event->data->auth_context.skip_auth = true;
        }
        if(!event->data->auth_context.skip_auth) {
            event->data->auth_context.password = nfc_app->mf_ul_auth->password;
        }
    } else if(event->type == MfUltralightPollerEventTypeAuthSuccess) {
        nfc_app->mf_ul_auth->pack = event->data->auth_context.pack;
    }

    return custom_event;
}

static NfcCustomEvent
    nfc_poller_handler_read_mf_classic(MfClassicPollerEvent* event, NfcApp* nfc_app) {
    UNUSED(event);
    UNUSED(nfc_app);

    NfcCustomEvent custom_event = NfcCustomEventReadHandlerIgnore;
    // TODO: Implement read mf_classic using key cache
    return custom_event;
}

static NfcCustomEvent
    nfc_poller_handler_read_mf_desfire(MfDesfirePollerEvent* event, NfcApp* nfc_app) {
    UNUSED(nfc_app);
    NfcCustomEvent custom_event = NfcCustomEventReadHandlerIgnore;

    if(event->type == MfDesfirePollerEventTypeReadSuccess) {
        custom_event = NfcCustomEventReadHandlerSuccess;
    }

    return custom_event;
}

static const NfcPollerReadHandler nfc_poller_handlers_read[] = {
    [NfcProtocolTypeIso14443_3a] = (NfcPollerReadHandler)nfc_poller_handler_read_iso14443_3a,
    [NfcProtocolTypeIso14443_4a] = (NfcPollerReadHandler)nfc_poller_handler_read_iso14443_4a,
    [NfcProtocolTypeMfUltralight] = (NfcPollerReadHandler)nfc_poller_handler_read_mf_ultralight,
    [NfcProtocolTypeMfClassic] = (NfcPollerReadHandler)nfc_poller_handler_read_mf_classic,
    [NfcProtocolTypeMfDesfire] = (NfcPollerReadHandler)nfc_poller_handler_read_mf_desfire,
};

NfcCustomEvent nfc_poller_handler_read(NfcPollerEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.poller);
    furi_assert(event.data);
    furi_assert(event.protocol_type < COUNT_OF(nfc_poller_handlers_read));

    NfcApp* nfc_app = context;

    NfcCustomEvent custom_event =
        nfc_poller_handlers_read[event.protocol_type](event.data, context);
    if(custom_event == NfcCustomEventReadHandlerSuccess) {
        nfc_dev_set_protocol_data(
            nfc_app->nfc_dev, event.protocol_type, nfc_poller_get_data(nfc_app->poller));
    }

    return custom_event;
}
